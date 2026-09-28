#include "mbed.h"
#include "USBSerial.h"
#include "RBarron_binaryutils.hpp"

USBSerial serial;

// main() runs in its own thread in the OS
int main()
{

    ThisThread::sleep_for(2s);

    // 32 bit number to use for setting and clearing bit
    uint32_t solo = 0;

     // need to go to the address of solo
    setbit(&solo, 24); // set the 24th bit
    setbit(&solo, 16); // set the 16th bit
    setbit(&solo, 17); // set the 17th bit
    setbits(&solo, 4095); // 2047 is equivalent to the first 11 bits being set to 1
    clearbit(&solo, 11); // clear bit 11
    clearbits(&solo, (127 & ~7)); // set bits 4-7 but not 0-3

    serial.printf("Binary Solo\n\r");

    // print out solo after manipulating its bits
    serial.printf("Number: %s\n\r", display_binary(solo));

}

