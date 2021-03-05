
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
#include <math.h>
#include <pthread.h>
#include <unistd.h>
//#include "color.h"

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
    char * road_left;
    char * road_right;
    struct Bridge main_bridge;
};

typedef struct start_car_args {
    struct Road* road;
    int pos;
    int id;
    int dir;
} car_args_t;

typedef struct generate_car_args {
    struct Road* road;
    int count;
    int lambda;
} gencar_args_t;

pthread_mutex_t bridgeLock;
pthread_cond_t  bridgeCond;
pthread_mutex_t printLock;

pthread_mutex_t* rightMutex;
pthread_mutex_t* leftMutex;

struct Road init(int roadSize, int bridgeSize);

void generateCars(struct Road* road, int left, int right, double leftLambda, double rightLambda);

void* leftCars(void*);

void* rightCars(void*);

double randExp(double lambda);

void *carStart(void*);

int carNextPosition(struct Car*);

void updateCar(struct Road*, struct Car*);

void moveOnTrack(struct Road*, struct Car*, int next_pos);

void moveOnBridge(struct Road*, struct Car*, int next_pos);

int getBridgeDirection(struct Bridge);

void addCarToBridge(struct Bridge*, struct Car*);

void removeCarFromBridge(struct Bridge*, struct Car*);

void updatePosition(struct Road*, struct Car*, int next_pos);

void print_card(struct Car* car);

void print_roads(struct Road*);

#endif //SOA_TAREA1_CAR_H
