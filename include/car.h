//
// Created by aaron on 21/2/21.
//

#ifndef SOA_TAREA1_CAR_H
#define SOA_TAREA1_CAR_H

// Define directions
#define LEFT_DIRECTION -1
#define RIGHT_DIRECTION 1
#define NONE_DIRECTION 0
#define TRUE 1
#define FALSE 0
#define BOOL unsigned int

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct Car{
    unsigned int car_name;
    int pos; // start row, end row
    int dir; // -1 left, 1 right
};

struct Bridge {
    int left_index;
    int right_index;
    int count;
};

struct Road{
    int left_index;
    int right_index;
    int cars_total;
    int * road_left;
    int * road_right;
    struct Bridge main_bridge;
};

pthread_mutex_t bridgeLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  emptyBridge = PTHREAD_COND_INITIALIZER;

pthread_mutex_t* rightMutex;
pthread_mutex_t* leftMutex;

void carStart(struct Road, int pos, int id, int dir);

int carNextPosition(struct Car);

void updateCar(struct Road, struct Car);

void moveOnTrack(struct Road, struct Car, int next_pos);

void moveOnBridge(struct Road, struct Car, int next_pos);

int getBridgeDirection(struct Bridge);

void addCarToBridge(struct Bridge, struct Car);

void removeCarFromBridge(struct Bridge, struct Car);

void updatePosition(struct Road, struct Car, int next_pos);

void print_card(struct Car car);

void print_roads(int a [], int size_a, int b [], int size_b);

#endif //SOA_TAREA1_CAR_H
