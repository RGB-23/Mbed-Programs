#include "mbed.h"
#include "USBSerial.h"
#include "RBarron_binaryutils.hpp"
#include "queue"


#define SET (uint32_t *)(0x50000508)
#define CLEAR (uint32_t *)(0x5000050C)
#define DIRSET (uint32_t *)(0x50000518)
#define GREEN (uint8_t)16
#define BLUE (uint8_t)6
#define RED (uint8_t)24

USBSerial serial;


int rob = 0;

Thread coop_thread;
Ticker coop_tick;

void rob_ticker() {

    rob = rob + 1;
    
}

void rob_thread() {

    while(true) {

        // check to see if ticker has run
        if(rob < 33) {

            // turn LED on
            *CLEAR = (0x1 << GREEN);

        }

        if(rob >= 33) {

            // turn LED off
            *SET = (0x1 << GREEN);


        }

        if (rob == 100) {

            rob = 0;
        }
    }
}

// main() runs in its own thread in the OS
int main()
{

    // clear all RGBs
    setbit(SET, GREEN);
    setbit(SET, BLUE);
    setbit(SET, RED);

    // set the direction of green
    setbit(DIRSET, GREEN);

    coop_thread.start(rob_thread);
    coop_tick.attach(&rob_ticker, 10s); // this is frequency

    while (true) {

        ThisThread::sleep_for(10);

    }
}

