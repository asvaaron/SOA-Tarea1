//
// Created by aaron on 21/2/21.
//

#ifndef SOA_TAREA1_CAR_H
#define SOA_TAREA1_CAR_H

// Define directions
#define LEFT_DIRECTION "L"
#define RIGHT_DIRECTION "R"
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

struct Road{
    int left_index;
    BOOL bridge; // Boolean flag
    int right_index;
    int cars_total;
    int * road;
};




#endif //SOA_TAREA1_CAR_H
