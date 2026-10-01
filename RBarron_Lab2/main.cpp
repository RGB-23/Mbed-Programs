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

#define DUTY_CYCLE_NUM 3

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
static float frequency = 100; // in microseconds

// duty cycle associated with the square wave
float duty_cycle_frequency = 0;

int ticker_count = 0;

// interrupt class for ticker
void interrupt() {

    // count up every time ticker fires
    ticker_count = ticker_count + 1;


    // reset the ticker if it reaches 100 ticks (100 microseconds)
    // so that it resents for the sake of the duty cycle
    if (ticker_count >= frequency) {

        ticker_count = 0;

    }

    // turn and keep LED on for however long the duty cycle is
    else if(ticker_count < duty_cycle_frequency) {

        // turn LED on
        *CLEAR = (0x1 << GREEN);

    }

    // turn and keep LED off after the duty cycle limit is reached
    else if(ticker_count > duty_cycle_frequency) {

        // turn LED off
        *SET = (0x1 << GREEN);

    }
    
}


// producer thread for pushing new duty cycles onto the queue
void producer() {
    
    // array of all the duty cycles
    int duty_cycles[9] = {10, 20, 30, 40, 50, 60, 70, 80, 90};

    // for vanilla we only need 1/3 rate so only add the first 3
    for(int i = 0; i < DUTY_CYCLE_NUM; i++) {

        // create space in mempool for each duty cycle
        int *duty = mempool1.try_alloc();

        // the address of duty is equal to the duty cycle number
        *duty = duty_cycles[i];

        // put each pointer of the duty cycle onto the queue
        queue1.try_put(duty);

    }

}

void vanilla_consumer() {

    while(true) {
            
        // pointer used get the last element in the queue
        int *pwm_ptr;

        // point pwm_ptr to the last element in the queue
        queue1.try_get(&pwm_ptr);

        // pwm is the value at the location of pwm_ptr
        int pwm = *pwm_ptr;

        // duty_cycle_frequency is the duty cycle related to the overall
        // frequency of the pulse width modulation
        // multiple by 0.01 to make the duty cycles into percents
        // example frequency = 100. pwm = 30, so 30 * 0.01 * 100 = 30
        duty_cycle_frequency = (pwm * 0.01) * frequency;

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
    ticker.attach(&interrupt, 100us); // this is frequency for each square wave

    while (true) {

        ThisThread::sleep_for(1s);

    }

}