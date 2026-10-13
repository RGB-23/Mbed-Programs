/*
Ronan Barron
10/18

ADD COMMENTS

*/

#include "mbed.h"
#include "USBSerial.h"
#include "RBarron_binaryutils.hpp"

#define SET (uint32_t *)(0x50000508)
#define CLEAR (uint32_t *)(0x5000050C)
#define DIRSET (uint32_t *)(0x50000518)
#define GREEN (uint8_t)16
#define BLUE (uint8_t)6
#define RED (uint8_t)24

// pin 22 for voltage
#define VDD (uint8_t)22

#define ZERO (uint8_t)(0x00)

// set up the programming modes to calibrate the sensor
#define PROGRAMMING_MODE_1 (uint8_t)(0xA0)
#define PROGRAMMING_MODE_2 (uint8_t)(0x00)
#define PROGRAMMING_MODE_3 (uint8_t)(0x00)

// go back to normal sensor operations
#define NORMAL (uint8_t)(0x80)

// memory registers for reading/writing the humidity/temps
#define HUMIDITY_SENSOR_READ (uint8_t)(0x06)
#define HUMIDITY_SENSOR_WRITE (uint8_t)(0x46)
#define TEMP_SENSOR_READ (uint8_t)(0x11)
#define TEMP_SENSOR_WRITE (uint8_t)(0x51)

// upper and lower bits for temp and humidity readings
#define UPPER (uint8_t)(0x1E)
#define LOWER (uint8_t)(0x1F)


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

        // turn LED on
        *CLEAR = (0x1 << GREEN);

        serial_mutex.lock();

        serial.printf("Reading HUMIDITY \r\n");

        serial_mutex.unlock();

        ThisThread::sleep_for(2s);

        // turn LED off
        *SET = (0x1 << GREEN);

    }

}


void read_temperature() {

    while (true) {

        event_flags.wait_all(TEMPERATURE);

        // turn LED on
        *CLEAR = (0x1 << BLUE);

        serial_mutex.lock();

        serial.printf("Reading TEMPERATURE \r\n");

        serial_mutex.unlock();

        ThisThread::sleep_for(2s);

        // turn LED off
        *SET = (0x1 << BLUE);

    }

}


// main() runs in its own thread in the OS
int main()
{

    // set the direction of the LEDs
    setbit(DIRSET, GREEN);
    setbit(DIRSET, BLUE);
    setbit(DIRSET, RED);

    // clear all the LEDs
    setbit(SET, GREEN);
    setbit(SET, BLUE);
    setbit(SET, RED);

    //humidity.start(read_humidity);
    //temperature.start(read_temperature);
    //weather.attach(weather_interrupt, 2s);

    // PROVIDE VOLTAGE
    setbit(DIRSET, VDD);
    setbit(CLEAR, VDD);
    

    int check = 0;

    // used for doing commands
    char cmd[3];

    cmd[0] = PROGRAMMING_MODE_1;
    cmd[1] = PROGRAMMING_MODE_2;
    cmd[2] = PROGRAMMING_MODE_3;

    // last number is the number of bytes that I am sending as a command (same # as commands I am sending)
    // enter programming mode to be able to use temp and humid sensors
    // check to see it is actually talking to write address
    int val = i2c.write(addr8_write, cmd, 3);
    serial.printf("Value of val: %d\r\n", val);
  

    cmd[0] = NORMAL;
    cmd[1] = ZERO;
    cmd[2] = ZERO;

    if (i2c.write(addr8_write, cmd, 3)) {

        check = 2;
        serial.printf("Value of check: %d\r\n", check);

    }

    // write and read the humidity info
    cmd[0] = HUMIDITY_SENSOR_READ;
    cmd[1] = ZERO;
    cmd[2] = ZERO;

    // write the register address (step 1 from figure 19)
    if (i2c.write(addr8_write, cmd, 3)) {

        check = 3;
        serial.printf("Value of check: %d\r\n", check);


    }


    cmd[0] = ZERO;
    cmd[1] = ZERO;
    cmd[2] = ZERO;

    // read the register cntents
    if (i2c.read(addr8_read, cmd, 3)) {

        check = 4;
        serial.printf("Value of check: %d\r\n", check);

    }

    // bitmask and set bits 10 and 11 to get a resolution of 14
    cmd[1] = (cmd[1] & 0b11110011) | 0b00001100;

    cmd[0] = HUMIDITY_SENSOR_WRITE;

    // write the register contents back
    if (i2c.write(addr8_write, cmd, 3)) {

        check = 5;
        serial.printf("Value of check: %d\r\n", check);

    }

    cmd[0] = UPPER;
    cmd[1] = LOWER;

    // write the upper address
    if (i2c.write(addr8_write, cmd, 2)) {

        check = 6;
        serial.printf("Value of check: %d\r\n", check);

    }

    // read from upper address
    if (i2c.read(addr8_read, cmd, 2)) {

        check = 7;
        serial.printf("Value of check: %d\r\n", check);

    }

    while (true) {

        ThisThread::sleep_for(1ms);

    }
}

