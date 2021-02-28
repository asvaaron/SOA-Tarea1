//
// Created by aaron on 22/2/21.
//

#include "../include/car.h"


struct Road init(int roadSize, int bridgeSize) {
    struct Road road;
    road.left_index = 0;
    road.right_index = roadSize + bridgeSize;
    road.main_bridge.count = 0;
    road.main_bridge.right_index = roadSize/2;
    road.main_bridge.left_index = roadSize/2 + bridgeSize;
    road.road_left = malloc(road.right_index * sizeof(int));
    road.road_right = malloc(road.right_index * sizeof(int));
    leftMutex = malloc(road.right_index * sizeof(pthread_mutex_t));
    rightMutex = malloc(road.right_index * sizeof(pthread_mutex_t));
    for (int i = 0; i < roadSize; i++) {
        road.road_left[i] = -1;
        road.road_right[i] = -1;
        rightMutex[i] = PTHREAD_MUTEX_INITIALIZER;
        leftMutex[i] = PTHREAD_MUTEX_INITIALIZER;
    }
    return road;
}

void leftCars(struct Road road, int count) {
    while (count--) {
        pthread_t thread;
        car_args_t args;
        args.road = road;
        args.dir = RIGHT_DIRECTION;
        args.pos = -1;
        args.id = count + 1;
        pthread_create(&thread, NULL, carStart, &args);
        pthread_join(thread, NULL);
        sleep(1);
    }
}

void rightCars(struct Road road, int count) {
    while (count--) {
        pthread_t thread;
        car_args_t args;
        args.road = road;
        args.dir = LEFT_DIRECTION;
        args.pos = road.right_index;
        args.id = count + 1;
        pthread_create(&thread, NULL, carStart, &args);
        pthread_join(thread, NULL);
        sleep(1);
    }
}

void *carStart(void* args) {
    struct Car car;
    car.pos = ((car_args_t*)args)->pos;
    car.car_name = ((car_args_t*)args)->id;
    car.dir = ((car_args_t*)args)->dir;
    while(car.pos >= road.left_index && car.pos < road.right_index) {
        updateCar(((car_args_t*)args)->road, car);
    }
}

void updateCar(struct Road road, struct Car car) {
    int next_pos = carNextPosition(car);
    if (next_pos >= road.main_bridge.left_index && next_pos <= road.main_bridge.right_index) {
        moveOnBridge(road, car, next_pos);
    } else {
        moveOnTrack(road, car, next_pos);
    }
}

int carNextPosition(struct Car car) {
    return car.pos + car.dir;
}

void moveOnBridge(struct Road road, struct Car car, int next_pos) {
    if (getBridgeDirection(road.main_bridge) == NONE_DIRECTION) {
        if (pthread_mutex_lock(&bridgeLock) != 0) {
            printf("Error locking bridge");
        }
        addCarToBridge(road.main_bridge, car); //give direction to the bridge.
        moveOnTrack(road, car, next_pos);
    } else {
        if (car.dir == getBridgeDirection(road.main_bridge)) { // same direction
            moveOnTrack(road, car, next_pos);
            if (next_pos < road.main_bridge.left_index || next_pos > road.main_bridge.right_index) {
                removeCarFromBridge(road.main_bridge, car);
                if (getBridgeDirection(road.main_bridge) == NONE_DIRECTION) {
                    pthread_mutex_unlock(&bridgeLock);
                }
            }
        } else { // opposite direction
            if (pthread_mutex_lock(&bridgeLock) != 0) {
                printf("Error locking bridge");
            }
            moveOnBridge(road, car, next_pos);
        }
    }
}

int getBridgeDirection(struct Bridge bridge) {
    return bridge.count < 0 ? LEFT_DIRECTION : RIGHT_DIRECTION;
}

void addCarToBridge(struct Bridge bridge, Car car) {
    bridge.count += car.dir;
}

void removeCarFromBridge(struct Bridge bridge, Car car) {
    bridge.count -= car.dir;
}

void moveOnTrack(struct Road road, struct Car car, int next_pos) {
    if (car.dir == LEFT_DIRECTION) {
        if (pthread_mutex_lock(&leftMutex[next_pos]) != 0) {
            printf("Error locking L <- R position %d for car %d", next_pos, car.car_name);
        }
        updatePosition(road, car, next_pos);
        pthread_mutex_unlock(&leftMutex[next_pos]);
    } else {
        if (pthread_mutex_lock(&rightMutex[next_pos]) != 0) {
            printf("Error locking L -> R position %d for car %d", next_pos, car.car_name);
        }
        updatePosition(road, car, next_pos);
        pthread_mutex_unlock(&rightMutex[next_pos]);
    }
    //<- <- <- LEFT_DIRECTION dir = -1   start_pos = max
    //-> -> -> RIGHT_DIRECTION dir = 1   start_pos = 0
}

void updatePosition(struct Road road, struct Car car, int next_pos) {
    if (car.dir == LEFT_DIRECTION) {
        road.road_left[car.pos] = -1;
        car.pos = next_pos;
        road.road_left[car.pos] = car.car_name;
    } else {
        road.road_right[car.pos] = -1;
        car.pos = next_pos;
        road.road_right[car.pos] = car.car_name;
    }
}

void print_card(struct Car car){
    printf("Car name %d\n" , car.car_name);
    printf("Car pos %d\n" , car.pos);
    printf("Car direction %d\n" , car.dir);
}

void print_roads(int a [], int size_a, int b [], int size_b){
//    int size_a = sizeof(a)/sizeof(int);
//    int size_a = sizeof(b)/sizeof(int);
    for (int i = 0; i < size_a ; i++){
        if (a[i] < 0) {
            printf(" ");
        } else {
            printf("%d", a[i]);
        }
    }
    printf("\n");
    for (int i = 0; i< size_b; i++){
        if (b[i] < 0) {
            printf(" ");
        } else {
            printf("%d", b[i]);
        }
    }
    printf("\n");
}


