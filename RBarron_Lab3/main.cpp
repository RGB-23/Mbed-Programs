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


#define HUMIDITY (uint32_t)10
#define TEMPERATURE (uint32_t)20

// SDA (P0_14) and SCL (P0_15) pin numbers
I2C i2c(P0_14, P0_15)

// 7 bit address for the temperature/humidity sensor
const int temp_hum_addr = 0x44;

// shift 7 bit address to get it into one byte 0x88
const int 8bit_addr = 0x44 << 1;


USBSerial serial;

Mutex serial_mutex;

EventFlags event_flags;
Ticker weather;

Thread humidity;
Thread temperature;

I2C i2c(I2C_SDA, I2C_SCL)


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

    humidity.start(read_humidity);
    temperature.start(read_temperature);
    weather.attach(weather_interrupt, 2s);

    while (true) {

        ThisThread::sleep_for(1ms);

    }
}

