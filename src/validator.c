//
// Created by aaron on 21/2/21.
//

#include "../include/validator.h"
#include <ctype.h>
#include <stdlib.h>


int all_are_numbers(char *s){
    for( ; *s ; ++s)
        if(!isdigit(*s))
        {
            return 0;
        }
    return 1;
}