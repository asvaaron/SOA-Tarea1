//
// Created by aaron on 22/2/21.
//

#include "../include/car.h"

void print_card(struct Car car){
    printf("Car name %d\n" , car.car_name);
    printf("Car pos %d\n" , car.pos);
    printf("Car direction %d\n" , car.dir);
}

void print_roads(int a [], int size_a, int b [], int size_b){;
//    int size_a = sizeof(a)/sizeof(int);
//    int size_a = sizeof(b)/sizeof(int);
    for (int i = 0; i< size_a ; i++){
        printf(" %d " , a[i]);
    }
    printf("\n");
    for (int i = 0; i< size_b; i++){
        printf(" %d " , b[i]);
    }
    printf("\n");
}


