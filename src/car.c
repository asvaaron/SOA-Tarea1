//
// Created by aaron on 22/2/21.
//

#include "../include/car.h"

void carStart(struct Road road, int pos, int id, int dir) {
    struct Car car;
    car.pos = pos;
    car.car_name = id;
    car.dir = dir;
    while(car.pos >= road.left_index && car.pos <= road.right_index) {
        updateCar(road, car);
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
        addCarToBridge(road.main_bridge); //give direction to the bridge.
        moveOnTrack(road, car, next_pos);
    } else {
        if (car.dir == getBridgeDirection(road.main_bridge)) { // same direction
            moveOnTrack(road, car, next_pos);
            if (next_pos < road.main_bridge.left_index || next_pos > road.main_bridge.right_index) {
                removeCarFromBridge(road.main_bridge);
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
        pthread_mutex_unlock(&&rightMutex[next_pos]);
    }
    //<- <- <- LEFT_DIRECTION dir = -1   start_pos = max
    //-> -> -> RIGHT_DIRECTION dir = 1   start_pos = 0
}

void updatePosition(struct Road road, struct Car car, int next_pos) {
    if (car.dir == LEFT_DIRECTION) {
        road.left_index[car.pos] = -1;
        car.pos = next_pos;
        road.left_index[car.pos] = car.car_name;
    } else {
        road.right_index[car.pos] = -1;
        car.pos = next_pos;
        road.right_index[car.pos] = car.car_name;
    }
}

void print_card(struct Car car){
    printf("Car name %d\n" , car.car_name);
    printf("Car pos %d\n" , car.pos);
    printf("Car direction %d\n" , car.dir);
}

void print_roads(int a [], int size_a, int b [], int size_b){;
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


