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

// base address for PWM0
#define PWM0 (NRF_PWM_Type *)(0x4001C000)

// output pin for PWM0 channel
#define OUTPUT_PIN (uint32_t *)(0x4001C560)
// the enable register for PWM0
#define ENABLE (uint32_t *)(0x4001C500)
// period of the square wave
#define COUNTERTOP (uint32_t *)(0x4001C508)
// period of the square wave
#define PRESCALER (uint32_t *)(0x4001C50C)
// start the sequence (start runing)
#define SEQSTART0 (uint32_t *)(0x4001C008)
// duty cycles in the sequence
#define SEQCNT (uint32_t *)(0x4001C524)
// stores pointer to the duty cycle value
#define SEQPTR (uint32_t *)(0x4001C520)
// stores pointer to the duty cycle value
#define PSELOUT0 (uint32_t *)(0x4001C560)


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


//PART 2B

/*

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

*/


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

// PART 2C

//STRAWBERRY CONSUMER
/*void strawberry_consumer() {

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



//PART 2C: main for strawberry consumer
int main() {


    uint16_t pwm_value = 500;
    serial.printf("pwm_value = %u\r\n", pwm_value);

    // nrf_pwm_sequence_t is a struct for sequence
    // values within the sequence struct are for setting the sequence
    nrf_pwm_sequence_t sequence;

    // address of pwm value
    sequence.values.p_raw = &pwm_value;
    // how many pwm values
    sequence.length = 1;
    // how 
    sequence.repeats = 0;
    sequence.end_delay = 0;

    //prod.start(producer);

    // enable the PWM0 unit
    //nrf_pwm_enable(PWM0);

    // set the direction of all the colors
    setbit(DIRSET, GREEN);
    setbit(DIRSET, BLUE);
    setbit(DIRSET, RED);

    // Turn red LED on
    setbit(SET, RED);
    setbit(SET, GREEN);
    setbit(SET, BLUE);


    // put 24 into the output pin register
    // 24 is the pin number for the red LED
    //*OUTPUT_PIN = 24;

    //*SEQCNT = 1;

    //*SEQPTR = (uint32_t)&pwm_value;

    
    // how fast countertop should count
    // 7 corresponds to 125 kHz
    //*PRESCALER = 7; // done with configure

    // period
    //*COUNTERTOP = 1000; // done with

 

    // conncects the red LED to PWM0
    uint32_t out_pins[4] = {RED, NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED};



    nrf_pwm_pins_set(PWM0, out_pins);



    // select pWM unit, speed of counting, direction of counting, and what to count to 
    nrf_pwm_configure(PWM0, NRF_PWM_CLK_125kHz, NRF_PWM_MODE_UP, 1000);
    


    nrf_pwm_sequence_set(PWM0, 0, &sequence);

    serial.printf("space\r\n");

    serial.printf("Starting PWM\r\n");
    serial.printf("pwm_value = %u\r\n", pwm_value);

    nrf_pwm_enable(PWM0);
    serial.printf("ENABLE = %lu\r\n", *ENABLE);

    // starts the the sequence
    nrf_pwm_task_trigger(PWM0, NRF_PWM_TASK_SEQSTART0);
    serial.printf("PTR = %lu\r\n", *SEQPTR);
    serial.printf("CNT = %lu\r\n", *SEQCNT);
    serial.printf("PSEL = %lu\r\n", *PSELOUT0);

    // start the pwm
    //*SEQSTART0  = 1;

    while (true) {

        ThisThread::sleep_for(1ms);

    }

}
