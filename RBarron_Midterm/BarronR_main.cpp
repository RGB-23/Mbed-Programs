/*
Ronan Barron
10/20 Midterm

See very bottom for details on how I would complete parts that I did 
not finish in the lab time

*/


#include "mbed.h"
#include "USBSerial.h"
#include "RBarron_binaryutils.hpp"
#include "math.h"

#define SET (uint32_t *)(0x50000508)
#define CLEAR (uint32_t *)(0x5000050C)
#define DIRSET (uint32_t *)(0x50000518)
#define GREEN (uint8_t)16
#define BLUE (uint8_t)6
#define RED (uint8_t)24

// define SET and DIRSET for Port 1
#define PORT1 (uint32_t *)(0x50000300)
#define SET_PORT1 (uint32_t *)(0x50000808)
#define DIRSET_PORT1 (uint32_t *)(0x50000818)

// PIN0 as related to port 1 (pin 0 uses the offset of port 1)
#define PIN0 (uint8_t)0

// zero is used for commands within for reading/writing
#define ZERO (uint8_t)(0x00)

// identification number as defined in the data sheet
#define ID (uint8_t)(0x92)


// address for reading in RGBC
#define low_clear (uint8_t)(0x94)
#define high_clear (uint8_t)(0x95)
#define low_red (uint8_t)(0x96)
#define high_red (uint8_t)(0x97)
#define low_green (uint8_t)(0x98)
#define high_green (uint8_t)(0x99)
#define low_blue (uint8_t)(0x9A)
#define high_blue (uint8_t)(0x9B)

USBSerial serial;

// 7 bit address for the proximity sensor
const int addr7 = 0x39;

// shift 7 bit address
// shifting to the left allows for reading and writing operations
// 0 as the last bit means it writes
const int addr8_write = 0x39 << 1;

// 1 as the last bit means it reads
const int addr8_read = addr8_write + 0x1;

// SDA (P0_14) and SCL (P0_15) pin numbers
I2C i2c(P0_14, P0_15);

// The enable register 0x80 is used for turning the power on/off
// how do I send power to a register or do I have to set that register
// equal to 1?
// this register says it is for voltage on the data sheet
#define VDD (uint8_t)(0x80)


// used for ID of sensor
Thread handshake;

// used for getting measurements every 100ms
Thread measurements;

// used for averaging the 10 color measurements
Thread average;



// there is no pin for voltage on the pinout so I am assuming that the fact that the sensor
// is plugged into my computer means it is receiving 3V already??
// this thread is used to verify that I am connecting to the correct sensor
// 171 is the ID number and 171 prints out successfully LETS GOOO
// ok i am testing it at the end of this lab and it doesn't work anymore
// i literally don't know how this is even possible. i did not alter this thread at all
// 
void colorimetry() {

    while(true) {


        serial.printf("In colorimetry\r\n");        

        // used to check if ACK was received
        int check = 0;

        // used for writing/reading commands and obtaining values from registers
        char cmd_colorimetry[1];

        cmd_colorimetry[0] = ID;

        if (i2c.write(addr8_write, cmd_colorimetry, 1) == 0) {

            check = 2;
            serial.printf("Value of check: %d\r\n", check);
        }

        if (i2c.read(addr8_read, cmd_colorimetry, 1) == 0) {

            check = 3;
            serial.printf("Value of check: %d\r\n", check);

            serial.printf("ID number: %d\r\n", cmd_colorimetry[0]);
        }        

        // long sleep for so it doesn't keep showing up
        ThisThread::sleep_for(500s);
    
    }
}


// PART 2: every 100ms or faster obtain RGBC values
// For some reason it doesn't read values all the time
// I jus don't understand why it's not reading color values
// when i use i2c read commands. Not sure I have to execute some write command before
// reading all the value but I have ben scouring for information
// on reading color data from the sensor on the data sheet and nothing
// point me in the right direction. I know pages 12 and 13 like the back of my hand

void getvalues() {

    while (true) {

        serial.printf("In thread\r\n");
    
        int check = 0;

        char cmd[8];

        //read in the colors
        cmd[0] = low_clear;
        cmd[1] = high_clear;    
        cmd[2] = low_red;
        cmd[3] = high_red;
        cmd[4] = low_green;
        cmd[5] = high_green;
        cmd[6] = low_blue;
        cmd[7] = high_blue;

        // if (i2c.write(addr8_write, cmd, 8)) {

        //     check = 3;
        //     serial.printf("Value of check: %d\r\n", check);

        //     serial.printf("Value of cmd[0]: %d\r\n", cmd[0]);
        //     serial.printf("Value of cmd[1]: %d\r\n", cmd[1]);
        //     serial.printf("Value of cmd[2]: %d\r\n", cmd[2]);
        //     serial.printf("Value of cmd[3]: %d\r\n", cmd[3]);
        //     serial.printf("Value of cmd[4]: %d\r\n", cmd[4]);
        //     serial.printf("Value of cmd[5]: %d\r\n", cmd[5]);
        //     serial.printf("Value of cmd[6]: %d\r\n", cmd[6]);
        //     serial.printf("Value of cmd[7]: %d\r\n", cmd[7]);

        
        // }

    
        if (i2c.read(addr8_read, cmd, 8) == 0) {

            check = 4;
            serial.printf("Value of check: %d\r\n", check);

            serial.printf("Value of cmd[0]: %d\r\n", cmd[0]);
            serial.printf("Value of cmd[1]: %d\r\n", cmd[1]);
            serial.printf("Value of cmd[2]: %d\r\n", cmd[2]);
            serial.printf("Value of cmd[3]: %d\r\n", cmd[3]);
            serial.printf("Value of cmd[4]: %d\r\n", cmd[4]);
            serial.printf("Value of cmd[5]: %d\r\n", cmd[5]);
            serial.printf("Value of cmd[6]: %d\r\n", cmd[6]);
            serial.printf("Value of cmd[7]: %d\r\n", cmd[7]);

        }
    



        ThisThread::sleep_for(100ms);

    }


}

void separateThread() {

    // I would use PWM to glow the LED that is closest to the
    // would use if statements to determine whatever value is read
    // from the sensor to make it into a glowing LEDs




}


// main() runs in its own thread in the OS
int main() {

    // set up pull up for PIN0
    setbit(DIRSET_PORT1, PIN0);
    setbit(SET_PORT1, PIN0);

    ThisThread::sleep_for(1s);

    handshake.start(colorimetry);
    //measurements.start(getvalues);
    
    while (true) {
        ThisThread::sleep_for(100ms);
    }

}

    //from data sheet: "The read sequence must read byte pairs (low followed by high)"
    // read low first
    // cmd[0] = low_red;
    
    // // read high second
    // cmd[1] = high_red;

    
    // if (i2c.write(addr8_write, cmd, 2) == 0) {

    // check = 1;
    // serial.printf("Value of check: %d\r\n", check);

    // }

    // if (i2c.read(addr8_read, cmd, 2) == 0) {

    // check = 2;
    // serial.printf("Value of check: %d\r\n", check);

    // serial.printf("Value of cmd[0]: %d\r\n", cmd[0]);
    // serial.printf("Value of cmd[1]: %d\r\n", cmd[1]);

    // }

    // ENABLE ALS (im assuming that this enables power on when I write to it)
    // cmd[0] = 0x80;

    // if (i2c.write(addr8_write, cmd, 1) == 0) {

    //     check = 1;
    //     serial.printf("Value of check: %d\r\n", check);
    // }


    // trying to read in the red light but I think I need to enable sometime
    // not sure if ALS and color sensing are the same/require the same enabling: answer is yes


//     while (true) {
//         ThisThread::sleep_for(100ms);
//     }


// }

// for PART 2
/*
I would make a thread that runs every 100ms and continuously
collect data from the registers that pick up the RGBC values
I would implement reading commands (which I have tried to do
and they just don't work) to be able to pick up what the values
are that are being read into the registers. I even tried putting a write
command before hand to see if that changed anything but no avail.
No combination that I tried has worked. I even tried only reading 2 registers at
a time but nothing changed: my output was still always 0 for cmd[0] through cmd[

*/

// for PART 3
/*
I would put all the values that are read into cmd[] into an array and then
calculate the average between all of them. Whatever RGB value is closest to
red green or blue i would use pwm to dimly light that LED

*/


// for PART 4
/*
I would hold up the sensor to each of the color swatches and read the values
associates with those colors. Then based on the values of the readings for these
colors I would print out the color name. For each of the colors I would give them
a range of values to be in (like there is a certain range of bits that correspond
to the color red, the color blue, etc). Depending on what the value of the bits is,
I would then print out that estimated color name. It would be a series of if statements
for the range (ie  if(bits > 10 and bits < 50) {printf("RED")}  )

*/

// for PART 5
/*
I would hold up the sensor to the swatch for each of the colors. Using the values in the
registers that are obtained from the sensors I would print out what he register values are
I would then explain the significance of these values and their correspondence with
the RGB colors
*/