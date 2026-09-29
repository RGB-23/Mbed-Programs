/*
Ronan Barron
10/6

ADD COMMENTS


*/
#include "mbed.h"
#include "USBSerial.h"
#include "RBarron_binaryutils.hpp"
#include "queue"

// set all the bits necessary for LEDs
#define SET (uint32_t *)(0x50000508)
#define CLEAR (uint32_t *)(0x5000050C)
#define DIRSET (uint32_t *)(0x50000518)
#define GREEN (uint8_t)16
#define BLUE (uint8_t)6
#define RED (uint8_t)24

USBSerial serial;


// queue with 9 spaces
Queue<int, 9> queue1;

// memory pool with 9 spaces
MemoryPool<int, 9> mempool1;

// ticker for determining frequency (duty cycle is within frequency)
// frequency does not change but the duty cycle does
// duty cycle changes within the frequency
Ticker ticker;

// frequency for the square wave
static float frequency = 1; // in seconds

// interrupt class for ticker
void interrupt() {

    // turn LED on
    *CLEAR = (0x1 << GREEN);

    // turn LED off
    *SET = (0x1 << GREEN);

}


// producer thread for pushing new duty cycles onto the queue
void producer() {
    
    // array of all the duty cycles
    float duty_cycles[9] = {.1, .2, .3, .4, .5, .6, .7, .8, .9};

    // for vanilla we only need 1/3 rate so only add the first 3
    for(int i = 0; i < 3; i++) {

        // create space in mempool for each duty cycle
        int *duty = mempool1.try_alloc();

        // the address of duty is equal to the duty cycle number
        *duty = duty_cycles[i];

        // put each pointer of the duty cycle onto the queue
        queue1.try_put(duty);

    }

}


void vanilla_consumer() {

    // set the blue and red bits so they are turned off
    setbit(SET, BLUE);
    setbit(SET, RED);

    while(true) {
        
        int *pwm_ptr;

        // get the last element in the queue
        queue1.try_get(&pwm_ptr);

        // pwm is the value at the location of pwm_ptr
        int pwm = *pwm_ptr;

        // duty cycle
        float time = pwm * frequency;

        // multiply pwm by ticker for certain % duty cycle
        //ThisThread::sleep_for(std::chrono::milliseconds(int(time)));

    }

}


Thread prod;
Thread cons;


int main() {

    // set all the bits so they are turned off
    setbit(SET, GREEN);
    setbit(SET, BLUE);
    setbit(SET, RED);

    // set the direction of all the colors
    setbit(DIRSET, GREEN);
    setbit(DIRSET, BLUE);
    setbit(DIRSET, RED);

    prod.start(producer);
    cons.start(vanilla_consumer);
    ticker.attach(&interrupt, 1us); // this is frequency for each square wave

    while (true) {

        //ThisThread::sleep_for(1s);

    }

}