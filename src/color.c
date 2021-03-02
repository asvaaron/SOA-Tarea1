# include "../include/color.h"

void red_print() {
    printf("\033[1;31m");
}

void yellow_print() {
    printf("\033[1;33m");
}

void green_print() {
    printf("\033[0;32m");
}

void blue_print() {
    printf("\033[0;34m");
}

void purple_print() {
    printf("\033[0;35m");
}

void reset_color_print(){
    printf("\033[0m");
}
