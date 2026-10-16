/*
Ronan Barron
10/18

ADD COMMENTS

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

// pin 22 for voltage
#define VDD (uint8_t)22

// define SET and DIRSET for Port 1
#define PORT1 (uint32_t *)(0x50000300)
#define SET_PORT1 (uint32_t *)(0x50000808)
#define DIRSET_PORT1 (uint32_t *)(0x50000818)

// PIN0 as related to port 1 (pin 0 uses the offset of port 1)
#define PIN0 (uint8_t)0

// zero is used for commands within for reading/writing
#define ZERO (uint8_t)(0x00)

// go back to normal sensor operations
#define NORMAL (uint8_t)(0x80)


#define HUMIDITY (uint32_t)10
#define TEMPERATURE (uint32_t)20

// SDA (P0_14) and SCL (P0_15) pin numbers
I2C i2c(P0_14, P0_15);

// 7 bit address for the temperature/humidity sensor
// address of the temp and humidity sensor
const int addr7 = 0x44;

// shift 7 bit address to get it into one byte 0x88
// shifting to the left allows for reading and writing operations
// 0 as the last bit means it writes
const int addr8_write = 0x44 << 1;

// 1 as the last bit means it reads
const int addr8_read = addr8_write + 0x1;




USBSerial serial;

Mutex serial_mutex;

EventFlags event_flags;
Ticker weather;

Thread humidity;
Thread temperature;
Thread info;

int counter = 0; // used to alternate between setting the flags

void weather_interrupt() {

    counter++;

    if (counter == 1) {

        // set the humidity
        event_flags.set(HUMIDITY);
        

    }

    else if (counter == 2) {

        // set the temperature
        event_flags.set(TEMPERATURE);

        // reset counter
        counter = 0;

    }

}


void read_humidity() {

    while (true) {

        event_flags.wait_all(HUMIDITY);

        // lock so calculation can occur
        serial_mutex.lock();


        int check = 0;

        char cmd[4];

        cmd[0] = ZERO;
        cmd[1] = ZERO;
        cmd[2] = ZERO;
        cmd[3] = ZERO;


        // start in normal mode
        if (i2c.write(addr8_write, cmd, 0) == 0) {

            check = 2;
            serial.printf("Value of check: %d\r\n", check);

        }

        ThisThread::sleep_for(50ms);


        if (i2c.read(addr8_read, cmd, 4) == 0) {

            check = 3;
            serial.printf("Value of check: %d\r\n", check);


        }

        // combine cmd[0] and cmd[1] into 1 16-bit number 
        // shift cmd[0] (upper)
        // all the way over to the left so we have space for 
        // 8 slots on the right for cmd[1] to enter in
        int humidity_calc = (cmd[0] << 8) | (cmd[1]);
        
        // keep only bits 13:0
        humidity_calc = humidity_calc & 0b0011111111111111;
    
        // calculate humidity and temperature based off equations
        // in the data sheet
        int percent_humidity = (humidity_calc/( ( pow(2, 14) ) - 1 ) ) * 100;

        // print out humidity and temp values
        serial.printf("humidity: %d\r\n", percent_humidity);

        //unlock so calculation can occur
        serial_mutex.unlock();

    }

}



void read_temperature() {

    while (true) {

        event_flags.wait_all(TEMPERATURE);

        // lock while calculations occur
        serial_mutex.lock();

        serial.printf("Reading TEMPERATURE \r\n");


        int check = 0;


        char cmd[4];

        cmd[0] = ZERO;
        cmd[1] = ZERO;
        cmd[2] = ZERO;
        cmd[3] = ZERO;


        // start in normal mode
        if (i2c.write(addr8_write, cmd, 0) == 0) {

            check = 2;
            serial.printf("Value of check: %d\r\n", check);

        }

        ThisThread::sleep_for(50ms);


        if (i2c.read(addr8_read, cmd, 4) == 0) {

            check = 3;
            serial.printf("Value of check: %d\r\n", check);


        }


        // combine cmd[0] and cmd[1] into 1 16-bit number 
        // shift cmd[0] (upper)
        // all the way over to the left so we have space for 
        // 8 slots on the right for cmd[1] to enter in

        // do the same for temperature
        int temp_calc = (cmd[2] << 8) | (cmd[3]);
        
        // keep only bits 15:2
        temp_calc = temp_calc & 0b1111111111111100;

        // shift so bit 2 is the LSB
        temp_calc = temp_calc >> 2;


        int celsius = ((temp_calc/( ( pow(2, 14) ) - 1 ) ) * 165) - 40;

        // print out temp value
        serial.printf("temp: %d\r\n", celsius);

        // unlock so humidity can run
        serial_mutex.unlock();

    }

}


// main() runs in its own thread in the OS
int main() {

    // set the direction of the LEDs
    //setbit(DIRSET, GREEN);
    //setbit(DIRSET, BLUE);

    // clear all the LEDs
    //setbit(SET, GREEN);
    //setbit(SET, BLUE);

    // PROVIDE VOLTAGE
    setbit(DIRSET, VDD);
    setbit(SET, VDD);


    // set up pull up for PIN0
    setbit(DIRSET_PORT1, PIN0);
    setbit(SET_PORT1, PIN0);

    ThisThread::sleep_for(100ms);


    humidity.start(read_humidity);
    temperature.start(read_temperature);
    weather.attach(weather_interrupt, 3s);


/*

    int check = 0;


    char cmd[4];

    cmd[0] = ZERO;
    cmd[1] = ZERO;
    cmd[2] = ZERO;
    cmd[3] = ZERO;


    // start in normal mode
    if (i2c.write(addr8_write, cmd, 0) == 0) {

        check = 2;
        serial.printf("Value of check: %d\r\n", check);

    }

    ThisThread::sleep_for(50ms);


    if (i2c.read(addr8_read, cmd, 4) == 0) {

        check = 3;
        serial.printf("Value of check: %d\r\n", check);


    }


    // combine cmd[0] and cmd[1] into 1 16-bit number 
    // shift cmd[0] (upper)
    // all the way over to the left so we have space for 
    // 8 slots on the right for cmd[1] to enter in
    int humidity_calc = (cmd[0] << 8) | (cmd[1]);
    
    // keep only bits 13:0
    humidity_calc = humidity_calc & 0b0011111111111111;

    // do the same for temperature
    int temp_calc = (cmd[2] << 8) | (cmd[3]);
    
    // keep only bits 15:2
    temp_calc = temp_calc & 0b1111111111111100;

    // shift so bit 2 is the LSB
    temp_calc = temp_calc >> 2;

  
    // calculate humidity and temperature based off equations
    // in the data sheet
    int percent_humidity = (humidity_calc/( ( pow(2, 14) ) - 1 ) ) * 100;
    int celsius = ((temp_calc/( ( pow(2, 14) ) - 1 ) ) * 165) - 40;

    // print out humidity and temp values
    serial.printf("humidity: %d\r\n", percent_humidity);
    serial.printf("temp: %d\r\n", celsius);


*/

    while (true) {

        ThisThread::sleep_for(1ms);

    }
}