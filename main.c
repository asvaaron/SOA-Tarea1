#include <stdio.h>
#include "include/hello.h"

int main(int argc, char **argv) {

    if ((argc - 1) != 2) {
        printf("Not enough cars both west and east \n");
    } else {
        // Arguments starts at 1
        int cars_west = atoi(argv[1]);
        int cars_east = atoi(argv[2]);

        printf("Cars West: %d, Cars East: %d \n", cars_east, cars_west);


        myPrintHelloMake();

    }


    return 0;
}
