#include <stdio.h>
#include "include/hello.h"
#include "include/validator.h"
#include <ctype.h>

int main(int argc, char **argv) {

    if ((argc - 1) != 2) {
        printf("Not valid number of cars both west and east \n");
        return 0;
    } else {
        if(! all_are_numbers(argv[1])  || ! all_are_numbers(argv[2])){
            printf("All the arguments are not integers \n");
            return 0;
        }
        // Arguments starts at 1
        int cars_west = atoi(argv[1]);
        int cars_east = atoi(argv[2]);

        printf("Cars West: %d, Cars East: %d \n", cars_west, cars_east);


        myPrintHelloMake();

    }


    return 0;
}
