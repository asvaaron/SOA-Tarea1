#include <stdio.h>
#include "include/hello.h"
#include "include/validator.h"
#include "include/car.h"
#include <ctype.h>


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


        printf("Cars left: %d, Cars right: %d \n", count_cars_left, count_cars_right);

        myPrintHelloMake();

        int days[] = {1,2,3,4,5};
        int days2[] = {1,2,3,4,5};
        int *ptr = days;
        int size_days = sizeof(days)/sizeof(int);
        int size_days2 = sizeof(days2)/sizeof(int);

        print_roads(days, size_days, days2, size_days2);


    }


    return 0;
}
