//
// Created by aaron on 22/2/21.
//

#include "../include/car.h"


void initRoad(int roadSize, int bridgeSize) {
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
        road.road_left[i] = '_';
        road.road_right[i] = '_';
        if (i >= road.main_bridge.left_index && i <= road.main_bridge.right_index) {
            road.road_left[i] = '=';
            road.road_right[i] = '=';
        }
        pthread_mutex_init(&rightMutex[i], NULL);
        pthread_mutex_init(&leftMutex[i], NULL);
    }
    return road;
}

void generateCars(int left, int right, double leftLambda, double rightLambda) {
    pthread_t left_thread;
    pthread_t right_thread;

    left_cars_lambda = leftLambda;
    left_cars_count = left;

    right_cars_lambda = rightLambda;
    right_cars_count = right;

    left_car_args = malloc(left_cars_count * sizeof(left_cars_count));
    right_car_args = malloc(right_cars_count * sizeof(right_cars_count));

    pthread_create(&left_thread, NULL, leftCars, NULL);
    pthread_create(&right_thread, NULL, rightCars, NULL);
    if (pthread_join(right_thread, NULL) != 0) {
        fprintf(stdout, "Error join thread generating right cars.");
    }
    if (pthread_join(left_thread, NULL) != 0) {
        fprintf(stdout, "Error join thread generating left cars.");
    }
}

void* leftCars(void* args) {
    pthread_t* threads = malloc(left_cars_count * sizeof(pthread_t));
    for (int i = 0; i < left_cars_count; i++) {
        left_car_args[i].dir = RIGHT_DIRECTION;
        left_car_args[i].pos = -1;
        left_car_args[i].id = i + 1;
        pthread_create(&threads[i], NULL, carStart, &left_car_args[i]);
        double exp = randExp(left_cars_lambda);
        usleep(exp*4000000);
    }
    fprintf(stdout, "Termino generar los de la izquierda.\n");
    for (int i = left_cars_count - 1; i >= 0; i--) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stdout, "Error join left car %c.", left_cars_count - 1 + 'a');
        }
    }
    pthread_exit(NULL);
}

void* rightCars(void* args) {
    pthread_t* threads = malloc(right_cars_count * sizeof(pthread_t));
    for (int i = 0; i < right_cars_count; i++) {
        right_car_args[i].dir = LEFT_DIRECTION;
        right_car_args[i].pos = road.right_index;
        right_car_args[i].id = i + 1;
        pthread_create(&threads[i], NULL, carStart, &right_car_args[i]);
        double exp = randExp(right_cars_lambda);
        usleep(exp*4000000);
    }
    for (int i = right_cars_count - 1; i >= 0; i--) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stdout, "Error join right car %c.", right_cars_count - 1 + 'a');
        }
    }
    pthread_exit(NULL);
}

double randExp(double lambda) {
    double p;
    p = rand() / (RAND_MAX + 1.0);
    return (-log(1.0- p) / lambda);
}

void *carStart(void* args) {
    struct Car car;
    car.pos = ((car_args_t*)args)->pos;
    car.car_name = ((car_args_t*)args)->id;
    car.dir = ((car_args_t*)args)->dir;
    do {
        updateCar(&car);
        print_roads();
        usleep(1000000);
    } while(car.pos <= road.right_index && car.pos >= road.left_index);
    fprintf(stdout, "Car: %c completed.\n", car.car_name - 1 + 'a');
    pthread_exit(NULL);
}

void updateCar(struct Car* car) {
    int next_pos = carNextPosition(car);
    
    // moveOnBridge if car is going to enter or it is already in the bridge
    if (next_pos == road.main_bridge.left_index || next_pos == road.main_bridge.right_index ||
        (car -> pos >= road.main_bridge.left_index && car -> pos <= road.main_bridge.right_index)) {
        moveOnBridge(car, next_pos);
    } else {
        moveOnTrack(car, next_pos);
    }
}

int carNextPosition(struct Car* car) {
    return car -> pos + car -> dir;
}

void moveOnBridge(struct Car* car, int next_pos) {
    if (getBridgeDirection(road.main_bridge) == NONE_DIRECTION) {
        pthread_mutex_lock(&bridgeLock);
        while (getBridgeDirection(road.main_bridge) != NONE_DIRECTION) {
            fprintf(stdout,"Car %d waiting to enter the bridge.\n", car -> car_name - 1 + 'a');
            pthread_cond_wait(&bridgeCond, &bridgeLock);
            fprintf(stdout,"Car %d awakes and tries to enter the bridge.\n", car -> car_name - 1 + 'a');
        }
        fprintf(stdout,"Car %d enters the bridge.\n", car->car_name - 1 + 'a');
        moveOnTrack(car, next_pos);
        addCarToBridge(&(road.main_bridge), car); //give direction to the bridge.
        fprintf(stdout,"Bridge locked.\n");
        pthread_mutex_unlock(&bridgeLock);
    } else {
        if (car -> dir == getBridgeDirection(road.main_bridge)) { // same direction
            // Try to move the car first, before update the bridge count. 
            // It may be blocked if there is another car in front.
            moveOnTrack(car, next_pos);
            // Is the car entering to the brigde?
            if ((car -> dir == RIGHT_DIRECTION && next_pos == road.main_bridge.left_index) ||
                (car -> dir == LEFT_DIRECTION && next_pos == road.main_bridge.right_index)) {
                addCarToBridge(&(road.main_bridge), car); // update the bridge count once the car actualy moved.
                fprintf(stdout,"Car %d enters the bridge.\n", car -> car_name - 1 + 'a');
            }
            // Is the car exiting the bridge?
            if ((car -> dir == LEFT_DIRECTION && next_pos < road.main_bridge.left_index) ||
                (car -> dir == RIGHT_DIRECTION && next_pos > road.main_bridge.right_index)) {
                removeCarFromBridge(&(road.main_bridge), car); // update the bridge count once the car actualy moved.
                fprintf(stdout,"Car %d exits the bridge.\n", car -> car_name - 1 + 'a');
                if (getBridgeDirection(road.main_bridge) == NONE_DIRECTION) {
                    // Bridge is empty unlock the bridge
                    pthread_cond_signal(&bridgeCond);
                    fprintf(stdout,"Bridge unlock.\n");
                }
            }
        } else { // opposite direction
            //wait until the bridge is empty and try to move again.
            pthread_mutex_lock(&bridgeLock);
            while (getBridgeDirection(road.main_bridge) != NONE_DIRECTION) {
                fprintf(stdout,"Car %d waiting to enter the bridge.\n", car -> car_name - 1 + 'a');
                pthread_cond_wait(&bridgeCond, &bridgeLock);
                fprintf(stdout,"Car %d awakes and tries to enter the bridge.\n", car -> car_name - 1 + 'a');
            }
            fprintf(stdout,"Car %d enters the bridge.\n", car->car_name - 1 + 'a');
            moveOnTrack(car, next_pos);
            addCarToBridge(&(road.main_bridge), car); //give direction to the bridge.
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

void moveOnTrack(struct Car* car, int next_pos) {
    if (car -> dir == LEFT_DIRECTION) {
        if (next_pos > -1 && next_pos < road.right_index) {
            if (pthread_mutex_lock(&leftMutex[next_pos]) != 0) {
                fprintf(stdout,"Error locking L <- R position %d for car %d", next_pos, car->car_name);
            }
        }
        int previous_pos = car -> pos; 
        updatePosition(car, next_pos);
        if (previous_pos > -1 && previous_pos < road.right_index) {
            pthread_mutex_unlock(&leftMutex[previous_pos]);
        }
    } else {
        if (next_pos > -1 && next_pos < road.right_index) {
            if (pthread_mutex_lock(&rightMutex[next_pos]) != 0) {
                fprintf(stdout,"Error locking L -> R position %d for car %d", next_pos, car->car_name);
            }
        }
        int previous_pos = car -> pos; 
        updatePosition(car, next_pos);
        if (previous_pos > -1 && previous_pos < road.right_index) {
            pthread_mutex_unlock(&rightMutex[previous_pos]);
        }
    }
    //<- <- <- LEFT_DIRECTION dir = -1   start_pos = max
    //-> -> -> RIGHT_DIRECTION dir = 1   start_pos = 0
}

void updatePosition(struct Car* car, int next_pos) {
    if (car -> dir == LEFT_DIRECTION) {
        if (car -> pos > -1 && car -> pos < road.right_index) {
            if (car -> pos >= road.main_bridge.left_index &&
            car -> pos <= road.main_bridge.right_index) {
                road.road_left[car -> pos] = '=';
            } else {
                road.road_left[car -> pos] = '_';
            }
        }
        car -> pos = next_pos;
        if (car -> pos > -1 && car -> pos < road.right_index) {
            road.road_left[car->pos] = car->car_name - 1 + 'a';
        }
    } else {
        if (car -> pos > -1 && car -> pos < road.right_index) {
            if (car -> pos >= road.main_bridge.left_index &&
                car -> pos <= road.main_bridge.right_index) {
                road.road_right[car->pos] = '=';
            } else {
                road.road_right[car->pos] = '_';
            }

        }
        car -> pos = next_pos;
        if (car -> pos > -1 && car -> pos < road.right_index) {
            road.road_right[car->pos] = car->car_name - 1 + 'a';
        }
    }
}

void print_card(struct Car* car){
    fprintf(stdout,"Car name %d\n" , car -> car_name);
    fprintf(stdout,"Car pos %d\n" , car -> pos);
    fprintf(stdout,"Car direction %d\n" , car -> dir);
}

void print_roads(){
    fprintf(stdout, "%s\n%s\n\n", road.road_left, road.road_right);
}


