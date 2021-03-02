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
    pthread_mutex_init(&bridgeLock, NULL);
    pthread_cond_init(&bridgeCond, NULL);
    pthread_mutex_init(&printLock, NULL);
    for (int i = 0; i < road.right_index; i++) {
        road.road_left[i] = 'x';
        road.road_right[i] = 'x';
        if (i >= road.main_bridge.left_index && i <= road.main_bridge.right_index) {
            road.road_left[i] = 'B';
            road.road_right[i] = 'B';
        }
        pthread_mutex_init(&rightMutex[i], NULL);
        pthread_mutex_init(&leftMutex[i], NULL);
    }
    return road;
}

void generateCars(struct Road* road, int left, int right, int leftLambda, int rightLambda) {
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
    pthread_create(&right_thread, NULL, rightCars, &right_cars_args);
    pthread_join(right_thread, NULL);
    pthread_join(left_thread, NULL);
}

void* leftCars(void* args) {
    int count = ((gencar_args_t*)args) -> count;
    pthread_t* threads = malloc(count* sizeof(pthread_t));
    car_args_t* car_args = malloc(count * sizeof(car_args));
    for (int i = 0; i < count; i++) {
        car_args[i].road = ((gencar_args_t*)args) -> road;
        car_args[i].dir = RIGHT_DIRECTION;
        car_args[i].pos = -1;
        car_args[i].id = i + 1;
        pthread_create(&threads[i], NULL, carStart, &car_args[i]);
        usleep(randExp(((gencar_args_t*)args) -> lambda)*1000000);
    }
    for (int i = count-1; i >= 0; i--) {
        pthread_join(threads[i], NULL);
    }
    pthread_exit(NULL);
}

void* rightCars(void* args) {
    int count = ((gencar_args_t*)args) -> count;
    pthread_t* threads = malloc(count * sizeof(pthread_t));
    car_args_t* car_args = malloc(count * sizeof(car_args));
    for (int i = 0; i < count; i++) {
        car_args[i].road = ((gencar_args_t*)args) -> road;
        car_args[i].dir = LEFT_DIRECTION;
        car_args[i].pos = ((gencar_args_t*)args) -> road -> right_index;
        car_args[i].id = i + 1;
        pthread_create(&threads[i], NULL, carStart, &car_args[i]);
        usleep(randExp(((gencar_args_t*)args) -> lambda)*1000000);
    }
    for (int i = count-1; i >= 0; i--) {
        pthread_join(threads[i], NULL);
    }
    pthread_exit(NULL);
}

double randExp(double lambda) {
    double u;
    u = rand() / (RAND_MAX + 1.0);
    return (-log(1.0- u) / lambda);
}

void *carStart(void* args) {
    struct Car car;
    struct Road* road = ((car_args_t*)args) -> road;
    car.pos = ((car_args_t*)args)->pos;
    car.car_name = ((car_args_t*)args)->id;
    car.dir = ((car_args_t*)args)->dir;
    do {
        updateCar(road, &car);
        usleep(1000000);
        print_roads(road);
    } while(car.pos >= road -> left_index && car.pos < road -> right_index);
    pthread_exit(NULL);
}

void updateCar(struct Road* road, struct Car* car) {
    int next_pos = carNextPosition(car);
    
    // moveOnBridge if car is going to enter or it is already in the bridge
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
        pthread_mutex_lock(&bridgeLock);
        while (getBridgeDirection(road -> main_bridge) != NONE_DIRECTION) {
            fprintf(stdout,"Car %d waiting to enter the bridge.\n", car -> car_name);
            pthread_cond_wait(&bridgeCond, &bridgeLock);
            fprintf(stdout,"Car %d awakes and tries to enter the bridge.\n", car -> car_name);
        }
        fprintf(stdout,"Car %d enters the bridge.\n", car->car_name);
        moveOnTrack(road, car, next_pos);
        addCarToBridge(&(road -> main_bridge), car); //give direction to the bridge.
        fprintf(stdout,"Bridge locked.\n");
        pthread_mutex_unlock(&bridgeLock);
    } else {
        if (car -> dir == getBridgeDirection(road -> main_bridge)) { // same direction
            // Try to move the car first, before update the bridge count. 
            // It may be blocked if there is another car in front.
            moveOnTrack(road, car, next_pos); 
            // Is the car entering to the brigde?
            if ((car -> dir == RIGHT_DIRECTION && next_pos == road -> main_bridge.left_index) || 
                (car -> dir == LEFT_DIRECTION && next_pos == road -> main_bridge.right_index)) {
                addCarToBridge(&(road -> main_bridge), car); // update the bridge count once the car actualy moved.
                fprintf(stdout,"Car %d enters the bridge.\n", car -> car_name);
            }
            // Is the car exiting the bridge?
            if ((car -> dir == LEFT_DIRECTION && next_pos < road -> main_bridge.left_index) || 
                (car -> dir == RIGHT_DIRECTION && next_pos > road -> main_bridge.right_index)) {
                removeCarFromBridge(&(road -> main_bridge), car); // update the bridge count once the car actualy moved.
                fprintf(stdout,"Car %d exits the bridge.\n", car -> car_name);
                if (getBridgeDirection(road -> main_bridge) == NONE_DIRECTION) {
                    // Bridge is empty unlock the bridge
                    pthread_cond_signal(&bridgeCond);
                    fprintf(stdout,"Bridge unlock.\n");
                }
            }
        } else { // opposite direction
            //wait until the bridge is empty and try to move again.
            pthread_mutex_lock(&bridgeLock);
            while (getBridgeDirection(road->main_bridge) != NONE_DIRECTION) {
                fprintf(stdout,"Car %d waiting to enter the bridge.\n", car -> car_name);
                pthread_cond_wait(&bridgeCond, &bridgeLock);
                fprintf(stdout,"Car %d awakes and tries to enter the bridge.\n", car -> car_name);
            }
            fprintf(stdout,"Car %d enters the bridge.\n", car->car_name);
            moveOnTrack(road, car, next_pos);
            addCarToBridge(&(road -> main_bridge), car); //give direction to the bridge.
            fprintf(stdout,"Bridge locked.\n");
            pthread_mutex_unlock(&bridgeLock);
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
                fprintf(stdout,"Error locking L <- R position %d for car %d", next_pos, car->car_name);
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
                fprintf(stdout,"Error locking L -> R position %d for car %d", next_pos, car->car_name);
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
            if (car -> pos >= road -> main_bridge.left_index &&
            car -> pos <= road -> main_bridge.right_index) {
                road -> road_left[car -> pos] = 'B';
            } else {
                road -> road_left[car -> pos] = 'x';
            }
        }
        car -> pos = next_pos;
        if (car -> pos > -1 && car -> pos < road -> right_index) {
            road->road_left[car->pos] = car->car_name + '0';
        }
    } else {
        if (car -> pos > -1 && car -> pos < road -> right_index) {
            if (car -> pos >= road -> main_bridge.left_index &&
                car -> pos <= road -> main_bridge.right_index) {
                road->road_right[car->pos] = 'B';
            } else {
                road->road_right[car->pos] = 'x';
            }

        }
        car -> pos = next_pos;
        if (car -> pos > -1 && car -> pos < road -> right_index) {
            road->road_right[car->pos] = car->car_name + '0';
        }
    }
}

void print_card(struct Car* car){
    fprintf(stdout,"Car name %d\n" , car -> car_name);
    fprintf(stdout,"Car pos %d\n" , car -> pos);
    fprintf(stdout,"Car direction %d\n" , car -> dir);
}

void print_roads(struct Road* road){
    fprintf(stdout, "%s\n%s\n\n", road->road_left, road->road_right);
}


