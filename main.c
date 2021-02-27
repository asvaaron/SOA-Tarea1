#include <stdio.h>
#include "include/hello.h"
#include "include/validator.h"
#include "include/car.h"
#include <ctype.h>
#include <pthread.h>

void *myThreadFun(void *vargp)
{
    sleep(1);
    printf("Printing GeeksQuiz from Thread \n");
    return NULL;
}

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

        pthread_t thread_id;
        printf("Before Thread\n");
        pthread_create(&thread_id, NULL, myThreadFun, NULL);
        pthread_join(thread_id, NULL);
        printf("After Thread\n");
        exit(0);

    }


    return 0;
}
