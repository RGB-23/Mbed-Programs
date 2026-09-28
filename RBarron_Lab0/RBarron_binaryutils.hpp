/*
Ronan Barron
9/1
This header file contains all the functions that are implemented in RBarron_binaryutils.cpp
The functions here implement seting and clearing either 1 or multiple bits. There is also a functions
that returns a character array of a 32 bit binary digit
*/

#include <cstdint>
// set a bit given the address of a 32 bit number and which bit should be set
// *addr is the address of a 32 bit binary number and whichbit is the specific bit to set
void setbit(uint32_t *addr, uint8_t whichbit);

// clear a bit given the address of a 32 bit number and which bit should be cleared
// *addr is the address of a 32 bit binary number and whichbit is the specific bit to clear
void clearbit(uint32_t *addr, uint8_t whichbit);

/* set multiple bits given the address of a 32 bit number and a bitmask for which bits to set
* @param: *addr is the address of a 32 bit binary number
* @param: bitmask is a 32 bit number for which bits to set
*/
void setbits(uint32_t *addr, uint32_t bitmask);

// clear multiple bits given the address of a 32 bit number and a bitmask for which bits to clear
// *addr is the address of a 32 bit binary number and bitmask is a 32 bit number for which bits to clear
void clearbits(uint32_t *addr, uint32_t bitmask);

// take a given 32 bit number and represent the same number in a char array
// num is a 32 bit number that will be turned into a char array. The char array of this number is returned
char display_binary(uint32_t num);
