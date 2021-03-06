#include <stdio.h>
#include "include/validator.h"
#include "include/car.h"
#include <ctype.h>
#include <pthread.h>

int main(int argc, char **argv) {

    if ((argc - 1) != 2) {
        printf("Not valid number of cars both left and right \n");
        return 0;
    } else {
        if(!all_are_numbers(argv[1]) || !all_are_numbers(argv[2])){
            printf("All the arguments are not integers \n");
            return 0;
        }
        // Arguments starts at 1
        int count_cars_left = atoi(argv[1]);
        int count_cars_right = atoi(argv[2]);

        initRoad(30, 10);
        generateCars(count_cars_left, count_cars_right, 1.002, 1.005);

        //printf("Cars left: %d, Cars right: %d \n", count_cars_left, count_cars_right);
        exit(0);
    }
}
