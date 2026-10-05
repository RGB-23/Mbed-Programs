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

// used for o-scope
#define OSCOPE (uint8_t)27

// base address for PWM0
#define PWM0 (NRF_PWM_Type *)(0x4001C000)

// base address for PWM0
#define PWM1 (NRF_PWM_Type *)(0x4001C000)

#define PWM3 (NRF_PWM_Type *)(0x4002D000)

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
// pwm period
#define EVENTS_PWMPERIODEND (uint32_t *)(0x4001C118)
#define EVENTS_SEQSTARTED0 (uint32_t *)(0x4001C108)
#define MODE (uint32_t *)(0x4001C504)
#define LOOP (uint32_t *)(0x4001C514)
#define DECODER (uint32_t *)(0x4001C510)
#define EVENTS_STOPPED (uint32_t *)(0x4001C104)


// number to determine how far up the duty cycle array the
// producer should go for determining brightnes
#define DUTY_CYCLE_NUM 101

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

// create thread for producer
Thread prod;

// create thread used for vanilla and chocolate
Thread cons;

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
    if(ticker_count <= duty_cycle_frequency) {

        // turn LED on
        *CLEAR = (0x1 << GREEN);

        // o-scope pin
        //*CLEAR = (0x1 << OSCOPE);

    }

    // turn and keep LED off after the duty cycle limit is reached
    else if(ticker_count > duty_cycle_frequency) {

        // turn LED off
        *SET = (0x1 << GREEN);

        // o-scope pin
        //*SET = (0x1 << OSCOPE);

    }
    
}


// producer thread for pushing new duty cycles onto the queue
// commented out right now so it doesn't conflict with part 3
/*
void producer() {
    
    // array of all the duty cycles
    int duty_cycles[DUTY_CYCLE_NUM] = {0};

    // put 0-100 into duty_cycles array
    for(int i = 0; i < DUTY_CYCLE_NUM; i++) {

        duty_cycles[i] = i;

    }

    while (true) {
    
        // get the LED to glow up
        for(int i = 0; i < DUTY_CYCLE_NUM; i++) {

            // create space in mempool for each duty cycle
            int *duty = mempool1.try_alloc();

            // check that duty actually points at something
            if(duty != nullptr) {
                
                // the value of duty is equal to the value at the array
                *duty = duty_cycles[i];

                // put the value of each pointer for the duty cycle onto the queue
                queue1.try_put(duty);

            }

            ThisThread::sleep_for(10ms);

        }

        // get the LED to diminsih glow
        for(int i = (DUTY_CYCLE_NUM - 1); i >= 0; i--) {

            // create space in mempool for each duty cycle
            int *duty = mempool1.try_alloc();

            // check that duty actually points at something
            if(duty != nullptr) {
                
                // the value of duty is equal to the value at the array
                *duty = duty_cycles[i];

                // put the value of each pointer for the duty cycle onto the queue
                queue1.try_put(duty);

            }

            ThisThread::sleep_for(10ms);

        }

    }

} */




//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

// PART 2A

/* this comment out connect to line 253

// VANILLA CONSUMER
void vanilla_consumer() {

    while(true) {
            
        // pointer used get the last element in the queue
        int *pwm_ptr;

        // point pwm_ptr to the last element in the queue
        if(queue1.try_get_for(1ms, &pwm_ptr)) {

            
            // set the duty cycle. period is 500
            // so multiplt everything by 5
            // divide by 3 to only keep on 1/3 brightness
            duty_cycle_frequency = (*pwm_ptr) * 5 / 3;

            // free the pointer so mempool doesn't get overrun
            mempool1.free(pwm_ptr);

        }
        ThisThread::sleep_for(1ms);

    }

}



// Part 2A: main for vanilla consumer
int main() {

    // set all the bits so they are turned off
    setbit(SET, GREEN);
    setbit(SET, BLUE);
    setbit(SET, RED);
    //setbit(SET, OSCOPE);

    // set the direction of all the colors
    setbit(DIRSET, GREEN);
    setbit(DIRSET, BLUE);
    setbit(DIRSET, RED);
    setbit(DIRSET, OSCOPE);

    prod.start(producer);
    cons.start(vanilla_consumer);
    ticker.attach(interrupt, 50us); // this is frequency for each square wave

    while (true) {

        //serial.printf("ticker_count %d\r\n", ticker_count);

        ThisThread::sleep_for(1ms);

    }

}


*/ //this comment out connect to line 188

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

// PART 2B: chocolate

/* this comment out connects to line 326


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
            // have to convert 0-100 duty cycles into 0-375 cause
            // 375 is 75% brightness of 500 total period
            led.pulsewidth_us((*pwm_ptr) * 375 / 100);

            // free the pointer so mempool doesn't get overrun
            mempool1.free(pwm_ptr);

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

*/ //this comment out connects to line 271


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

/* // this comment out connects to line 435

// PART 2C

//STRAWBERRY CONSUMER
void strawberry_consumer() {

    // pwm_sequence used for getting the LED to glow
    uint16_t pwm_sequence[1];

    // nrf_pwm_sequence_t is a struct for sequence
    // values within the sequence struct are for setting the sequence
    nrf_pwm_sequence_t sequence;  

    // address of pwm_sequence
    sequence.values.p_common = pwm_sequence;

    // take only one pwm value at a time
    sequence.length = 1;

    // don't let the cycle repeat
    sequence.repeats = 0;

    // no delay for the pwm
    sequence.end_delay = 0;



    // conncects the red LED to PWM0
    uint32_t out_pins[4] = {RED, NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED};

    // set the sequence
    nrf_pwm_sequence_set(PWM0, 0, &sequence);


    // set up the pins to the PWM0 channel
    nrf_pwm_pins_set(PWM0, out_pins);

    // select PWM unit, speed of counting, direction of counting, and what to count to 
    nrf_pwm_configure(PWM0, NRF_PWM_CLK_125kHz, NRF_PWM_MODE_UP, 500);

    // set the decoder to interpret the values in the sequence array
    nrf_pwm_decoder_set(PWM0, NRF_PWM_LOAD_COMMON, NRF_PWM_STEP_AUTO);

    // enable PWM0 so it's on
    nrf_pwm_enable(PWM0);

    // LED off to start (duty cycle = 0)
    pwm_sequence[0] = 0;

    // set the sequence
    nrf_pwm_sequence_set(PWM0, 0, &sequence);

    // clear pwm to reset it
    nrf_pwm_event_clear(PWM0, NRF_PWM_EVENT_SEQEND0);

    // activate PWM0 and start the sequence again
    nrf_pwm_task_trigger(PWM0, NRF_PWM_TASK_SEQSTART0);


    while (true) {

        int *pwm_ptr;

        // point pwm_ptr to the last element in the queue
        if(queue1.try_get_for(1ms, &pwm_ptr)) {

            // set the duty cycle and scale to fit the 500us period
            // and 0-100 duty cycle range
            duty_cycle_frequency = (*pwm_ptr) * 250 / 100;

            // update the duty cycle value with new value
            pwm_sequence[0] = duty_cycle_frequency;

            // set the sequence with the new duty cycle value
            nrf_pwm_sequence_set(PWM0, 0, &sequence);

            // activate PWM0 and start the sequence again
            // with new duty cycle value
            nrf_pwm_task_trigger(PWM0, NRF_PWM_TASK_SEQSTART0);

            // free the pointer so mempool doesn't get overrun
            mempool1.free(pwm_ptr);
        }

        ThisThread::sleep_for(1ms);
        
    }

}


//PART 2C: main for strawberry consumer
int main() {

    // clear all the LEDs
    setbit(SET, GREEN);
    setbit(SET, BLUE);
    setbit(SET, RED);


    prod.start(producer);
    cons.start(strawberry_consumer);
    

    while (true) {

        ThisThread::sleep_for(1ms);
    }

}

*/ //this comment out connects to line 312

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

// PART 3


// used for chocolate
PwmOut led(P0_6);

Queue<int, 9> vanilla_queue;
Queue<int, 9> chocolate_queue;
Queue<int, 9> strawberry_queue;

MemoryPool<int, 9> vanilla_mempool;
MemoryPool<int, 9> chocolate_mempool;
MemoryPool<int, 9> strawberry_mempool;


Thread produ;
Thread vanilla;
Thread chocolate;
Thread strawberry;


void producer() {
    
    // array of all the duty cycles
    int duty_cycles[DUTY_CYCLE_NUM] = {0};

    // put 0-100 into duty_cycles array
    for(int i = 0; i < DUTY_CYCLE_NUM; i++) {

        duty_cycles[i] = i;

    }

    while (true) {
    
        // get the LED to glow up
        for(int i = 0; i < DUTY_CYCLE_NUM; i++) {

            // create space in mempool for each duty cycle
            int *vanilla_duty = vanilla_mempool.try_alloc();

            // check to see if vanilla points to smth
            if(vanilla_duty != nullptr) {

                // vanilla duty cycle is equal to the new duty cycle value added
                *vanilla_duty = duty_cycles[i];

                // check that we can put smth on the queue
                if(!vanilla_queue.try_put(vanilla_duty)) {
                    
                    // free the memory for the next duty cycle
                    vanilla_mempool.free(vanilla_duty);

                }
            }

            int *chocolate_duty = chocolate_mempool.try_alloc();

            // check to see if chocolate points to smth
            if(chocolate_duty != nullptr) {

                // chocolate duty cycle is equal to the new duty cycle value added
                *chocolate_duty = duty_cycles[i];

                // check that we can put smth on the queue
                if(!chocolate_queue.try_put(chocolate_duty)) {
                    
                    // free the memory for the next duty cycle
                    chocolate_mempool.free(chocolate_duty);

                }
            }


            int *strawberry_duty = strawberry_mempool.try_alloc();

            // check to see if strawberry points to smth
            if(strawberry_duty != nullptr) {

                // strawberry duty cycle is equal to the new duty cycle value added
                *strawberry_duty = duty_cycles[i];

                // check that we can put smth on the queue
                if(!strawberry_queue.try_put(strawberry_duty)) {
                    
                    // free the memory for the next duty cycle
                    strawberry_mempool.free(strawberry_duty);

                }
            }

            //serial.printf("PRODUCER (1st sleep) i = %d\r\n", i);
            ThisThread::sleep_for(10ms);

        }

        // get the LED to diminsih glow
        for(int i = (DUTY_CYCLE_NUM - 1); i >= 0; i--) {

            // create space in mempool for each duty cycle
            int *vanilla_duty = vanilla_mempool.try_alloc();

            // check to see if vanilla points to smth
            if(vanilla_duty != nullptr) {

                // vanilla duty cycle is equal to the new duty cycle value added
                *vanilla_duty = duty_cycles[i];

                // check that we can put smth on the queue
                if(!vanilla_queue.try_put(vanilla_duty)) {
                    
                    // free the memory for the next duty cycle
                    vanilla_mempool.free(vanilla_duty);

                }
            }

            int *chocolate_duty = chocolate_mempool.try_alloc();

            // check to see if chocolate points to smth
            if(chocolate_duty != nullptr) {

                // chocolate duty cycle is equal to the new duty cycle value added
                *chocolate_duty = duty_cycles[i];

                // check that we can put smth on the queue
                if(!chocolate_queue.try_put(chocolate_duty)) {
                    
                    // free the memory for the next duty cycle
                    chocolate_mempool.free(chocolate_duty);

                }
            }


            int *strawberry_duty = strawberry_mempool.try_alloc();

            // check to see if strawberry points to smth
            if(strawberry_duty != nullptr) {

                // strawberry duty cycle is equal to the new duty cycle value added
                *strawberry_duty = duty_cycles[i];

                // check that we can put smth on the queue
                if(!strawberry_queue.try_put(strawberry_duty)) {
                    
                    // free the memory for the next duty cycle
                    strawberry_mempool.free(strawberry_duty);

                }
            }

            //serial.printf("PRODUCER (2nd sleep) i = %d\r\n", i);
            ThisThread::sleep_for(10ms);

        }

    }

}


void vanilla_consumer() {

    while(true) {
            
        // pointer used get the last element in the queue
        int *pwm_ptr;

        // point pwm_ptr to the last element in the queue
        if(vanilla_queue.try_get_for(1ms, &pwm_ptr)) {

           //serial.printf("vanilla value = %d\r\n", *pwm_ptr);

            
            // set the duty cycle. period is 500
            // so multiplt everything by 5
            // divide by 3 to only keep on 1/3 brightness
            duty_cycle_frequency = (*pwm_ptr) * 5 / 3;

            //serial.printf("vanilla value = %d, pwm = %d\r\n", *pwm_ptr, duty_cycle_frequency);

            // free the pointer so mempool doesn't get overrun
            vanilla_mempool.free(pwm_ptr);

        }
        ThisThread::sleep_for(1ms);

    }

}

void chocolate_consumer() {

    // 500 microsecond period
    led.period_us(500);

    while(true) {

        // pointer used get the last element in the queue
        int *pwm_ptr;

        //serial.printf("HELLO2\r\n");        

        // point pwm_ptr to the last element in the queue
        if(chocolate_queue.try_get_for(1ms, &pwm_ptr)) {
            
            // set the duty cycle frequency
            // have to convert 0-100 duty cycles into 0-375 cause
            // 375 is 75% brightness of 500 total period
            led.pulsewidth_us((*pwm_ptr) * 375 / 100);
            //led.write(1.0f - ((*pwm_ptr) * 75.0f / 100.0f));

            // free the pointer so mempool doesn't get overrun
            chocolate_mempool.free(pwm_ptr);

        }
        ThisThread::sleep_for(1ms);

    }

}

void strawberry_consumer() {

    // pwm_sequence used for getting the LED to glow
    uint16_t pwm_sequence[1];

    // nrf_pwm_sequence_t is a struct for sequence
    // values within the sequence struct are for setting the sequence
    nrf_pwm_sequence_t sequence;  

    // address of pwm_sequence
    sequence.values.p_common = pwm_sequence;

    // take only one pwm value at a time
    sequence.length = 1;

    // don't let the cycle repeat
    sequence.repeats = 0;

    // no delay for the pwm
    sequence.end_delay = 0;

    // conncects the red LED to PWM3
    uint32_t out_pins[4] = {RED, NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED};

    // set the sequence
    nrf_pwm_sequence_set(PWM3, 0, &sequence);


    // set up the pins to the PWM3 channel
    nrf_pwm_pins_set(PWM3, out_pins);

    // select PWM unit, speed of counting, direction of counting, and what to count to 
    nrf_pwm_configure(PWM3, NRF_PWM_CLK_125kHz, NRF_PWM_MODE_UP, 500);

    // set the decoder to interpret the values in the sequence array
    nrf_pwm_decoder_set(PWM3, NRF_PWM_LOAD_COMMON, NRF_PWM_STEP_AUTO);

    // enable PWM3 so it's on
    nrf_pwm_enable(PWM3);

    // LED off to start (duty cycle = 0)
    pwm_sequence[0] = 0;

    // set the sequence
    nrf_pwm_sequence_set(PWM3, 0, &sequence);

    // clear pwm to reset it
    nrf_pwm_event_clear(PWM3, NRF_PWM_EVENT_SEQEND0);

    // activate PWM3 and start the sequence again
    nrf_pwm_task_trigger(PWM3, NRF_PWM_TASK_SEQSTART0);


    while (true) {

        int *pwm_ptr;

        // point pwm_ptr to the last element in the queue
        if(strawberry_queue.try_get_for(1ms, &pwm_ptr)) {

            //serial.printf("strawberry value = %d\r\n", *pwm_ptr);

            // set the duty cycle and scale to fit the 500us period
            // and 0-100 duty cycle range
            duty_cycle_frequency = (*pwm_ptr) * 250 / 100;

            //serial.printf("strawberry value = %d, pwm = %d\r\n", *pwm_ptr, duty_cycle_frequency);

            // update the duty cycle value with new value
            pwm_sequence[0] = duty_cycle_frequency;

            // set the sequence with the new duty cycle value
            nrf_pwm_sequence_set(PWM3, 0, &sequence);

            // activate PWM3 and start the sequence again
            // with new duty cycle value
            nrf_pwm_task_trigger(PWM3, NRF_PWM_TASK_SEQSTART0);

            // free the pointer so mempool doesn't get overrun
            strawberry_mempool.free(pwm_ptr);
        }

        ThisThread::sleep_for(1ms);
        
    }

}



int main() {

    // set the direction of the LEDs
    setbit(DIRSET, GREEN);
    setbit(DIRSET, BLUE);
    setbit(DIRSET, RED);

    // clear all the LEDs
    setbit(SET, GREEN);
    setbit(SET, BLUE);
    setbit(SET, RED);

    produ.start(producer);
    vanilla.start(vanilla_consumer);
    chocolate.start(chocolate_consumer);
    strawberry.start(strawberry_consumer);
    ticker.attach(interrupt, 50us);
    

    while (true) {

        ThisThread::sleep_for(1ms);
    }

}