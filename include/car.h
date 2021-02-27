//
// Created by aaron on 21/2/21.
//

#ifndef SOA_TAREA1_CAR_H
#define SOA_TAREA1_CAR_H

// Define directions
#define LEFT_DIRECTION -1
#define RIGHT_DIRECTION 1
#define TRUE 1
#define FALSE 0
#define BOOL unsigned int

#include <stdio.h>
#include <stdlib.h>

struct Car{
    unsigned int car_name;
    int pos; // -1 left, 0 bridge, 1 right
    int dir; // -1 left, 1 right
};

struct Bridge{
    int left_index;
    int right_index;
    int dir;
};

struct Road{
    int left_index;
    BOOL bridge; // Boolean flag
    int right_index;
    int cars_total;
    int * road;
    struct Bridge * main_bridge;
};

void print_card(struct Car car);


#endif //SOA_TAREA1_CAR_H
