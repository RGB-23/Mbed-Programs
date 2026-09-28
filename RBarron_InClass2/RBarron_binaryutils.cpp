/*
Ronan Barron
9/23
This file implements the binaryutls.hpp functions
*/

#include <cstdint>
#include <stdlib.h>

void setbit(uint32_t *addr, uint8_t whichbit) {

    *addr = (*addr) | (0x1 << whichbit);

}

void clearbit(uint32_t *addr, uint8_t whichbit) {

    *addr = (*addr) & ~(0x1 << whichbit);

}

void setbits(uint32_t *addr, uint32_t bitmask) {

    *addr = (*addr) | (bitmask);

}

void clearbits(uint32_t *addr, uint32_t bitmask) {

    *addr = (*addr) & ~(bitmask);

}


char *display_binary(uint32_t num) {

    // used to store each digit from num to print out or else array is lost
    char *num_array = (char *)calloc(33, sizeof(char));

    int digit; // used to get the last digit in each number

    num_array[32] = '\0'; // null terminator at the end of the array

    for(int i = 31; i >= 0; i--) {

        digit = num % 2; // get the last digit in num by getting the remainder

        if(digit == 1) {

            num_array[i] = '1'; // add '1' to the last spot in the char array

        }

        else {

            num_array[i] = '0'; // add '0' to the last spot in the char array
        }
        

        num = num >> 1; // right shift the num by 1 to get a new LSB

    }

    return num_array;

}