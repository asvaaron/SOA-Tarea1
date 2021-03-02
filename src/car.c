//
// Created by aaron on 22/2/21.
//

#include "../include/car.h"


struct Road init(int roadSize, int bridgeSize) {
    struct Road road;
    road.left_index = 0;
    road.right_index = roadSize + bridgeSize;
    road.main_bridge.count = 0;
    road.main_bridge.left_index = roadSize/2;
    road.main_bridge.right_index = roadSize/2 + bridgeSize - 1; // bridge index are inclusive 
    road.road_left = malloc(road.right_index * sizeof(int));
    road.road_right = malloc(road.right_index * sizeof(int));
    leftMutex = malloc(road.right_index * sizeof(pthread_mutex_t));
    rightMutex = malloc(road.right_index * sizeof(pthread_mutex_t));
    for (int i = 0; i < road.right_index; i++) {
        road.road_left[i] = -1;
        road.road_right[i] = -1;
        pthread_mutex_init(&rightMutex[i], NULL);
        pthread_mutex_init(&leftMutex[i], NULL);
    }
    return road;
}

void generateCars(Road* road, int left, int right, int leftLambda, int rightLambda) {
    pthread_t left_thread;
    pthread_t right_thread;
    gencar_args_t left_cars_args;
    gencar_args_t right_cars_args;
    
    left_cars_args.road = road;
    left_cars_args.lambda = leftLambda;
    left_cars_args.count = left;
    
    right_cars_args.road = road;
    right_cars_args.lambda = rightLambda;
    right_cars_args.count = right;
    
    pthread_create(&left_thread, NULL, leftCars, &left_cars_args);
    pthread_join(left_thread, NULL);
    pthread_create(&right_thread, NULL, rightCars, &right_cars_args);
    pthread_join(right_thread, NULL);
}

void* leftCars(void* args) {
    int count = ((gencar_args_t*)args) -> count;
    while (count--) {
        pthread_t thread;
        car_args_t args;
        args.road = ((gencar_args_t*)args) -> road;
        args.dir = RIGHT_DIRECTION;
        args.pos = -1;
        args.id = count + 1;
        pthread_create(&thread, NULL, carStart, &args);
        pthread_join(thread, NULL);
        sleep(randExp(((gencar_args_t*)args) -> lambda));
    }
    pthread_exit(NULL);
}

void* rightCars(void* args) {
    int count = ((gencar_args_t*)args) -> count;
    while (count--) {
        pthread_t thread;
        car_args_t args;
        args.road = ((gencar_args_t*)args) -> road;
        args.dir = LEFT_DIRECTION;
        args.pos = ((gencar_args_t*)args) -> road -> right_index;
        args.id = count + 1;
        pthread_create(&thread, NULL, carStart, &args);
        pthread_join(thread, NULL);
        sleep(randExp(((gencar_args_t*)args) -> lambda));
    }
    pthread_exit(NULL);
}

double randExp(double lambda) {
    double u;
    u = rand() / (RAND_MAX + 1.0);
    return -log(1- u) / lambda;
}

void *carStart(void* args) {
    struct Car car;
    struct Road* road = ((car_args_t*)args) -> road;
    car.pos = ((car_args_t*)args)->pos;
    car.car_name = ((car_args_t*)args)->id;
    car.dir = ((car_args_t*)args)->dir;
    while(car.pos >= road -> left_index - 1 && car.pos < road -> right_index) {
        updateCar(road, &car);
        printf(" Car %d, new pos: %d\n", car.car_name, car.pos);
    }
    print_roads(road->road_left, road->right_index,road->road_right,road->right_index);
    pthread_exit(NULL);
}

void updateCar(struct Road* road, struct Car* car) {
    int next_pos = carNextPosition(car);
    
    // moveOnBridge if cars is going to enter or it is already in the bridge
    if (next_pos == road -> main_bridge.left_index || next_pos == road -> main_bridge.right_index ||
        (car -> pos >= road -> main_bridge.left_index && car -> pos <= road -> main_bridge.right_index)) {
        moveOnBridge(road, car, next_pos);
    } else {
        moveOnTrack(road, car, next_pos);
    }
}

int carNextPosition(struct Car* car) {
    return car -> pos + car -> dir;
}

void moveOnBridge(struct Road* road, struct Car* car, int next_pos) {
    if (getBridgeDirection(road -> main_bridge) == NONE_DIRECTION) {
        if (pthread_mutex_lock(&bridgeLock) != 0) {
            printf("Error locking bridge");
        }
        moveOnTrack(road, car, next_pos);
        addCarToBridge(&(road -> main_bridge), car); //give direction to the bridge.
    } else {
        if (car -> dir == getBridgeDirection(road -> main_bridge)) { // same direction
            // Try to move the car first, before update the bridge count. 
            // It may be blocked if there is another car in front.
            moveOnTrack(road, car, next_pos); 
            // Is the car entering to the brigde?
            if ((car -> dir == RIGHT_DIRECTION && next_pos == road -> main_bridge.left_index) || 
                (car -> dir == LEFT_DIRECTION && next_pos == road -> main_bridge.right_index) {
                addCarToBridge(&(road -> main_bridge), car); // update the bridge count once the car actualy moved.
            }
            // Is the car exiting the bridge?
            if ((car -> dir == LEFT_DIRECTION && next_pos < road -> main_bridge.left_index) || 
                (car -> dir == RIGHT_DIRECTION && next_pos > road -> main_bridge.right_index) {
                removeCarFromBridge(&(road -> main_bridge), car); // update the bridge count once the car actualy moved.
                if (getBridgeDirection(road -> main_bridge) == NONE_DIRECTION) { 
                    // Bridge is empty unlock the bridge
                    pthread_mutex_unlock(&bridgeLock);
                }
            }
        } else { // opposite direction
            //wait until the bridge is empty and try move again.
            if (pthread_mutex_lock(&bridgeLock) != 0) {
                printf("Error locking bridge");
            }
            moveOnBridge(road, car, next_pos);
        }
    }
}

int getBridgeDirection(struct Bridge bridge) {
    return bridge.count == 0 ? NONE_DIRECTION : (bridge.count < 0 ? LEFT_DIRECTION : RIGHT_DIRECTION);
}

void addCarToBridge(struct Bridge* bridge, struct Car* car) {
    bridge -> count += car -> dir;
}

void removeCarFromBridge(struct Bridge* bridge, struct Car* car) {
    bridge -> count -= car -> dir;
}

void moveOnTrack(struct Road* road, struct Car* car, int next_pos) {
    if (car -> dir == LEFT_DIRECTION) {
        if (next_pos > -1 && next_pos < road -> right_index) {
            if (pthread_mutex_lock(&leftMutex[next_pos]) != 0) {
                printf("Error locking L <- R position %d for car %d", next_pos, car->car_name);
            }
        }
        int previous_pos = car -> pos; 
        updatePosition(road, car, next_pos);
        if (previous_pos > -1 && previous_pos < road -> right_index) {
            pthread_mutex_unlock(&leftMutex[previous_pos]);
        }
    } else {
        if (next_pos > -1 && next_pos < road -> right_index) {
            if (pthread_mutex_lock(&rightMutex[next_pos]) != 0) {
                printf("Error locking L -> R position %d for car %d", next_pos, car->car_name);
            }
        }
        int previous_pos = car -> pos; 
        updatePosition(road, car, next_pos);
        if (previous_pos > -1 && previous_pos < road -> right_index) {
            pthread_mutex_unlock(&rightMutex[previous_pos]);
        }
    }
    //<- <- <- LEFT_DIRECTION dir = -1   start_pos = max
    //-> -> -> RIGHT_DIRECTION dir = 1   start_pos = 0
}

void updatePosition(struct Road* road, struct Car* car, int next_pos) {
    if (car -> dir == LEFT_DIRECTION) {
        if (car -> pos > -1 && car -> pos < road -> right_index) {
            road -> road_left[car -> pos] = -1;
        }
        car -> pos = next_pos;
        if (car -> pos > -1 && car -> pos < road -> right_index) {
            road->road_left[car->pos] = car->car_name;
        }
    } else {
        if (car -> pos > -1 && car -> pos < road -> right_index) {
            road->road_right[car->pos] = -1;
        }
        car -> pos = next_pos;
        if (car -> pos > -1 && car -> pos < road -> right_index) {
            road->road_right[car->pos] = car->car_name;
        }
    }
    //Print cars
    print_roads(road->road_left, road->right_index,road->road_right,road->right_index);
}

void print_card(struct Car* car){
    printf("Car name %d\n" , car -> car_name);
    printf("Car pos %d\n" , car -> pos);
    printf("Car direction %d\n" , car -> dir);
}

void print_roads(int a [], int size_a, int b [], int size_b){
//    int size_a = sizeof(a)/sizeof(int);
//    int size_a = sizeof(b)/sizeof(int);
    green_print();
    for (int i = 0; i < size_a ; i++){
        if (a[i] < 0) {
            printf("x");
        } else {
            printf("%d", a[i]);
        }
    }
    printf("\n");
    for (int i = 0; i< size_b; i++){
        if (b[i] < 0) {
            printf("x");
        } else {
            printf("%d", b[i]);
        }
    }
    reset_color_print();
    printf("\n");
}


