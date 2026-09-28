/*
Ronan Barron
9/21
This code plays around with turning LEDs on and off
Mailbox class is used along with threads to cycle through
different colors and blinking rates

*/

#include "mbed.h"
#include "USBSerial.h"
#include "RBarron_binaryutils.hpp"
#include "Mail.h"

// set all the bits necessary for LEDs
#define SET (uint32_t *)(0x50000508)
#define CLEAR (uint32_t *)(0x5000050C)
#define DIRSET (uint32_t *)(0x50000518)
#define GREEN (uint8_t)16
#define BLUE (uint8_t)6
#define RED (uint8_t)24

USBSerial serial;

// enums for each state of LED blinking
enum Urgency {

    NO_ERROR,    // P16
    ATTN_REQ,   // P06
    FATAL_ERROR // P24

};

// create the mailbox for switching between states
// make the type "Urgency" so we can use the enums: BIG BRAIN move
Mail<Urgency, 10> mailbox;


/* This thread changes the state of the the blinking
   after receiving a new envelope from the mailbox
   that indicates to switch the state
   if the mailbox has nothing in it i.e the envelope points
   to nothing then get rid of the envelope by freeing it
   so a a new state can be switched to
*/
void led_diag_handler() {

    // initialize with no error
    Urgency state = NO_ERROR;

    serial.printf("LED Handler Started\r\n");

    while (true) {

        serial.printf("LED state: %d\r\n", state);

        // get the envelope from the mailbox
        Urgency *envelope = mailbox.try_get();

        // in case nothing was put into the mailbox this
        // will exit out and go back to diag_tester
        // since the next if statement won't be reached
        if (envelope != nullptr) {

            // the new stateis what the new envelope is
            // since it has been updated
            state = *envelope;

            // get rid of the envelope to not overfill mailbox
            mailbox.free(envelope);

        }


        switch(state) {

            case NO_ERROR:

                serial.printf("No Error. Rate: 1000ms\r\n");

                // turn off blue and red LEDs
                setbit(SET, BLUE);
                setbit(SET, RED);

                // blink green until state is changed
                while(state == NO_ERROR) {
            
                    ThisThread::sleep_for(1000ms);

                    // turn LED on
                    *CLEAR = (0x1 << GREEN);

                    ThisThread::sleep_for(1000ms);

                    // turn LED off
                    *SET = (0x1 << GREEN);

                    // check if state has changed
                    envelope = mailbox.try_get();

                    // try_get returns nullptr if nothing in mailbox
                    // switch state if nothing in mailbox
                    if (envelope != nullptr) {

                        // the new stateis what the new envelope is
                        // since it has been updated
                        state = *envelope;

                        // get rid of the envelope to not overfill mailbox
                        mailbox.free(envelope);

                    }

                }

                break;


            case ATTN_REQ:

                serial.printf("Attnetion Required. Rate: 250ms\r\n");

                // turn off green and red LEDs
                setbit(SET, GREEN);
                setbit(SET, RED);

                // blink blue until state is changed
                while(state == ATTN_REQ) {

                    ThisThread::sleep_for(250ms);
                    
                    // turn LED on
                    *CLEAR = (0x1 << BLUE);

                    ThisThread::sleep_for(250ms);

                    // turn LED off
                    *SET = (0x1 << BLUE);

                    // check if state has changed
                    envelope = mailbox.try_get();


                    // try_get returns nullptr if nothing in mailbox
                    // switch state if nothing in mailbox
                    if (envelope != nullptr) {

                        // the new stateis what the new envelope is
                        // since it has been updated
                        state = *envelope;

                        // get rid of the envelope to not overfill mailbox
                        mailbox.free(envelope);

                    }
                    
                }

                break;

                
        

            case FATAL_ERROR:

                serial.printf("Fatal Error. Rate: 100ms\r\n");

                // turn off green and blue LEDs
                setbit(SET, GREEN);
                setbit(SET, BLUE);

                // blink red until state is changed
                while(state == FATAL_ERROR) {
        
                    ThisThread::sleep_for(100ms);
                    
                    // turn LED on
                    *CLEAR = (0x1 << RED);

                    ThisThread::sleep_for(100ms);

                    // turn LED off
                    *SET = (0x1 << RED);

                    // try to get a message from the mailbox
                    envelope = mailbox.try_get();

                    // try_get returns nullptr if nothing in mailbox
                    // switch state if nothing in mailbox
                    if (envelope != nullptr) {

                        // the new stateis what the new envelope is
                        // since it has been updated
                        state = *envelope;

                        // get rid of the envelope to not overfill mailbox
                        mailbox.free(envelope);

                    }

                }

                break;

        }

        ThisThread::sleep_for(100ms);

    }
}

/* This thread sleeps for 5 seconds allowing each state to have 5 seconds to run
   it changes the state once it wakes up. It also allocates space
   in the mailbox for the envelope and sets the envelope equal to the new state
   so when the other thread cycles through it sees the envelope has changed states

*/
void diag_tester() {

    // initialize the first state to no error
    // outside while statement so it doesn't get reset every 5 seconds
    Urgency state = NO_ERROR;


    while (true) {

        ThisThread::sleep_for(5s);

        // switch from no error to attention required
        // dereference to get the value at the pointer
        if (state == NO_ERROR) {
            
            state = ATTN_REQ;

        }

        // switch from attention required to fatal error
        // dereference to get the value at the pointer
        else if (state == ATTN_REQ) {
            
            state = FATAL_ERROR;

        }

        // switch from fatal error to no error
        // dereference to get the value at the pointer
        else if (state == FATAL_ERROR) {
            
            state = NO_ERROR;

        }

        // allocate space for mail
        Urgency *envelope = mailbox.try_alloc();

        // make the envelope equal to the state
        // so the other thread can know what it is
        // not a gloabl variable
        *envelope = state;

        // put envelope in the mailbox
        mailbox.put(envelope);

        
    }
}

// create the threads to run in the main program
Thread led;
Thread diag;


// main() runs in its own thread in the OS
int main() {

// PART 3

    // set all the bits so they are turned off
    setbit(SET, GREEN);
    setbit(SET, BLUE);
    setbit(SET, RED);

    // set the direction of all the colors
    setbit(DIRSET, GREEN);
    setbit(DIRSET, BLUE);
    setbit(DIRSET, RED);

    diag.start(diag_tester);
    led.start(led_diag_handler);


    while (true) {

        ThisThread::sleep_for(1s);


/* PART 1

        setbit(SET, GREEN);
        setbit(SET, BLUE);
        setbit(SET, RED);


        ThisThread::sleep_for(2s);

        setbit(SET, 13);
        setbit(DIRSET, 13);

        ThisThread::sleep_for(2s);

        clearbit(SET, 13);
        setbit(CLEAR, 13);


        ThisThread::sleep_for(2s);

*/

/* PART 2

        setbit(SET, GREEN);
        setbit(SET, BLUE);
        setbit(SET, RED);
        setbit(DIRSET, GREEN);
        setbit(DIRSET, BLUE);
        setbit(DIRSET, RED);


        // i < 1 to ensure we get 2 seconds
        for(int i = 0; i < 1; i++) {
            
            ThisThread::sleep_for(1000ms);

            *CLEAR = (0x1 << GREEN);

            ThisThread::sleep_for(1000ms);

            *SET = (0x1 << GREEN);

        }

        ThisThread::sleep_for(1000ms);

        // i < 2 to ensure we get 2 seconds
        for(int i = 0; i < 4; i++) {

            // set the attention required blink

            ThisThread::sleep_for(250ms);
            
            *CLEAR = (0x1 << BLUE);

            // clear the set bit and set the clear bit to turn LED off
            ThisThread::sleep_for(250ms);

            *SET = (0x1 << BLUE);
            
        }

        // i < 10 to ensure we get 2 seconds
        for(int i = 0; i < 10; i++) {

        
            ThisThread::sleep_for(100ms);
            
            *CLEAR = (0x1 << RED);

            // clear the set bit and set the clear bit to turn LED off

            ThisThread::sleep_for(100ms);

            *SET = (0x1 << RED);

        }

*/

    }

}




        


