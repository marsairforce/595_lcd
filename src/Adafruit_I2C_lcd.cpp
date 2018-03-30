#include "LCD.h"
#include <Arduino.h>
#include <inttypes.h>
#include "Print.h"
#include <inttypes.h>
#include "Adafruit_MCP23008.h"
Adafruit_I2C_lcd::Adafruit_I2C_lcd(uint8_t i2cAddr) {
    _i2cAddr = i2cAddr;
    // the I/O expander pinout
    _rs_pin = 1;
    _enable_pin = 2;
    _data_pins[0] = 3;  // really d4
    _data_pins[1] = 4;  // really d5
    _data_pins[2] = 5;  // really d6
    _data_pins[3] = 6;  // really d7
    _backlight_pin = 7;

    // the i2c begin stuff has to be called from inside a function, such as setup(),
    // so we don't do that stuff here. Instead we override begin() below
}

// we can only do this i2c bein inside setup() function, where the lcd.begin() is called.
// usually we declare the instance variable in the file right.
void Adafruit_I2C_lcd::begin(uint8_t cols, uint8_t rows, uint8_t charsize)
{
    // from the begin()
    _i2c.begin(_i2cAddr);

    _i2c.pinMode(_backlight_pin, OUTPUT);    // backlight
    _i2c.digitalWrite(_backlight_pin, HIGH); // backlight

    for (uint8_t i=0; i<4; i++) {
        _i2c.pinMode(_data_pins[i], OUTPUT);
    }
    _i2c.pinMode(_rs_pin, OUTPUT);    // rs
    _i2c.pinMode(_enable_pin, OUTPUT);    // enable

    _i2c.digitalWrite(_rs_pin, LOW);
    _i2c.digitalWrite(_enable_pin, LOW);

    HD44780_LCD::begin(cols, rows, charsize);
}

inline size_t Adafruit_I2C_lcd::write(uint8_t value) {
  send(value, HIGH);
  return 1;
}

void Adafruit_I2C_lcd::setBacklight(uint8_t status) {
    _i2c.digitalWrite(7, status); // backlight is on pin 7
}

void Adafruit_I2C_lcd::send(uint8_t value, boolean mode) {
    _i2c.digitalWrite(_rs_pin, mode); // RS
    write4bits(value>>4);
    write4bits(value);
}

void Adafruit_I2C_lcd::write4bits(uint8_t value) {
    uint8_t out = 0;
    out = _i2c.readGPIO();

    // speed up for i2c since its sluggish
    for (int i = 0; i < 4; i++) {
        out &= ~_BV(_data_pins[i]);
        out |= ((value >> i) & 0x1) << _data_pins[i];
    }

    // make sure enable is low
    out &= ~ _BV(_enable_pin);

    _i2c.writeGPIO(out);

    // pulse enable
    delayMicroseconds(1);
    out |= _BV(_enable_pin);
    _i2c.writeGPIO(out);
    delayMicroseconds(1);
    out &= ~_BV(_enable_pin);
    _i2c.writeGPIO(out);
    delayMicroseconds(100);

}