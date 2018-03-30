#include "LCD.h"
#include <Arduino.h>
#include <inttypes.h>
#include "Print.h"
#include <inttypes.h>

Adafruit_I2C_lcd::Adafruit_I2C_lcd(uint8_t i2cAddr) {
    _i2cAddr = i2cAddr;

    _i2c.begin(_i2cAddr);

    _i2c.pinMode(7, OUTPUT);    // backlight
    _i2c.digitalWrite(7, HIGH); // backlight

    // the I/O expander pinout
    _i2c.pinMode(1, OUTPUT);    // rs
    _i2c.pinMode(2, OUTPUT);    // enable
    _i2c.pinMode(3, OUTPUT);        // d4
    _i2c.pinMode(4, OUTPUT);        // d5
    _i2c.pinMode(5, OUTPUT);        // d6
    _i2c.pinMode(6, OUTPUT);        // d7
}

inline size_t Adafruit_I2C_lcd::write(uint8_t value) {
  send(value, HIGH);
  return 1;
}

void Adafruit_I2C_lcd::setBacklight(uint8_t status) {
    _i2c.digitalWrite(7, status); // backlight is on pin 7
}

void Adafruit_I2C_lcd::send(uint8_t value, boolean mode) {
    _i2c.digitalWrite(1, mode); // RS
    write4bits(value>>4);
    write4bits(value);    
}

void Adafruit_I2C_lcd::write4bits(uint8_t value) {
    uint8_t out = 0;

    out = _i2c.readGPIO();

    // speed up for i2c since its sluggish
    for (int i = 0, j=3; i < 4; i++, j++) {
        out &= ~_BV(j);
        out |= ((value >> i) & 0x1) << j;
    }

    // make sure enable is low
    out &= ~ _BV(2);

    _i2c.writeGPIO(out);

    // pulse enable
    delayMicroseconds(1);
    out |= _BV(2); // enable pin
    _i2c.writeGPIO(out);
    delayMicroseconds(1);
    out &= ~_BV(2); // enable pin
    _i2c.writeGPIO(out);   
    delayMicroseconds(100);
  
}