//
// Created by aaron on 21/2/21.
//

#include "../include/validator.h"

#include <ctype.h>
#include <stdlib.h>


int all_are_numbers(char *s)
{
    int b = 1;
    for( ; *s ; ++s)
        if(!isdigit(*s))
        {
            b = 0;
            return b;
        }

    return b;
}