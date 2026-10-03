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

// number to determine how far up the duty cycle array the
// producer should go for determining brightnes
#define DUTY_CYCLE_NUM 5

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
static int period = 500; // in microseconds

// duty cycle associated with the square wave
int duty_cycle_frequency = 0;

int ticker_count = 0;

// interrupt class for ticker
void interrupt() {

    // count up every time ticker fires
    ticker_count = ticker_count + 1;


    // reset the ticker if it reaches 100 ticks (100 microseconds)
    // so that it resents for the sake of the duty cycle
    if (ticker_count >= period) {

        ticker_count = 0;

    }

    // turn and keep LED on for however long the duty cycle is
    else if(ticker_count <= duty_cycle_frequency) {

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
    float duty_cycles[9] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};

    // for vanilla we only need 1/3 rate so only add the first 3
    for(int i = 0; i < DUTY_CYCLE_NUM; i++) {

        // create space in mempool for each duty cycle
        int *duty = mempool1.try_alloc();

        // the address of duty is equal to the duty cycle number
        *duty = duty_cycles[i];

        // put the value of each pointer for the duty cycle onto the queue
        queue1.try_put(duty);

    }

    ThisThread::sleep_for(1ms);

}

/* VANILLA CONSUMER
void vanilla_consumer() {

    while(true) {
            
        // pointer used get the last element in the queue
        int *pwm_ptr;

        // point pwm_ptr to the last element in the queue
        if(queue1.try_get_for(1ms, &pwm_ptr)) {

            //serial.printf("*pwm_ptr: %d\r\n", *pwm_ptr);

            // set the duty cycle
            duty_cycle_frequency = (*pwm_ptr);

            //serial.printf("*duty_cycle_frequency: %d\r\n", duty_cycle_frequency);

        }
        ThisThread::sleep_for(1ms);

    }

}*/

// create thread for producer
Thread prod;

// create thread used for vanilla and chocolate
Thread cons;

// Part 2A: main for vanilla consumer
/*int main() {

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
    ticker.attach(interrupt, 50us); // this is frequency for each square wave

    while (true) {

        //serial.printf("ticker_count %d\r\n", ticker_count);

        ThisThread::sleep_for(1ms);

    }

}*/

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

// create led that is connected to the blue pin
PwmOut led(P0_6);

// CHOCOLATE CONSUMER
void chocolate_consumer() {

    // 500 microsecond period
    led.period_us(500);

    while(true) {

        // pointer used get the last element in the queue
        int *pwm_ptr;

        //serial.printf("HELLO2\r\n");        

        // point pwm_ptr to the last element in the queue
        if(queue1.try_get_for(1ms, &pwm_ptr)) {

            //serial.printf("*pwm_ptr: %d\r\n", *pwm_ptr);
            
            // set the duty cycle frequency
            led.pulsewidth_us(*pwm_ptr);

            //serial.printf("*duty_cycle_frequency: %d\r\n", duty_cycle_frequency);

        }
        ThisThread::sleep_for(1ms);

    }

}

// PART 2B: main for chocolate consumer
int main() {

    prod.start(producer);
    cons.start(chocolate_consumer);


    while (true) {

        ThisThread::sleep_for(1ms);

    }

}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
/*
//STRAWBERRY CONSUMER
void strawberry_consumer() {

    while(true) {
            
        // pointer used get the last element in the queue
        int *pwm_ptr;

        // point pwm_ptr to the last element in the queue
        if(queue1.try_get_for(1ms, &pwm_ptr)) {

            //serial.printf("*pwm_ptr: %d\r\n", *pwm_ptr);

            // set the duty cycle
            duty_cycle_frequency = (*pwm_ptr);

            //serial.printf("*duty_cycle_frequency: %d\r\n", duty_cycle_frequency);

        }
        ThisThread::sleep_for(1ms);

    }

}


//PART 2C: main for strawberry consumer
int main() {

    prod.start(producer);

    nrf_pwm_enable();

    if(nrf_pwm_enable_check()) {


        serial.printf("success");


    }

    else{

        serial.printf("fail");
        
    }




    while (true) {

        ThisThread::sleep_for(1ms);

    }

}
*/