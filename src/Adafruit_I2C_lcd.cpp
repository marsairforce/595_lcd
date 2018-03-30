#include "LCD.h"
#include <Arduino.h>
#include <inttypes.h>
#include "Print.h"
#include <inttypes.h>
#include "Adafruit_MCP23008.h"

// the I/O expander pinout
#define ADAFRUIT_MPC_PIN_RS         1
#define ADAFRUIT_MPC_PIN_ENABLE     2
#define ADAFRUIT_MPC_PIN_D0         3
#define ADAFRUIT_MPC_PIN_D1         4
#define ADAFRUIT_MPC_PIN_D2         5
#define ADAFRUIT_MPC_PIN_D3         6
#define ADAFRUIT_MPC_PIN_BACKLIGHT  7

Adafruit_I2C_lcd::Adafruit_I2C_lcd(uint8_t i2cAddr) {
    _i2cAddr = i2cAddr;
    // the i2c begin stuff has to be called from inside a function, such as setup(),
    // so we don't do that stuff here. Instead we override begin() below
}

// we can only do this i2c bein inside setup() function, where the lcd.begin() is called.
// usually we declare the instance variable in the file right.
void Adafruit_I2C_lcd::begin(uint8_t cols, uint8_t rows, uint8_t charsize)
{
    // from the begin()
    _i2c.begin(_i2cAddr);

    _i2c.pinMode(ADAFRUIT_MPC_PIN_BACKLIGHT, OUTPUT);    // backlight
    _i2c.digitalWrite(ADAFRUIT_MPC_PIN_BACKLIGHT, HIGH); // backlight

    _i2c.pinMode(ADAFRUIT_MPC_PIN_D0, OUTPUT);
    _i2c.pinMode(ADAFRUIT_MPC_PIN_D1, OUTPUT);
    _i2c.pinMode(ADAFRUIT_MPC_PIN_D2, OUTPUT);
    _i2c.pinMode(ADAFRUIT_MPC_PIN_D3, OUTPUT);

    _i2c.pinMode(ADAFRUIT_MPC_PIN_RS, OUTPUT);    // rs
    _i2c.pinMode(ADAFRUIT_MPC_PIN_ENABLE, OUTPUT);    // enable

    _i2c.digitalWrite(ADAFRUIT_MPC_PIN_RS, LOW);
    _i2c.digitalWrite(ADAFRUIT_MPC_PIN_ENABLE, LOW);

    HD44780_LCD::begin(cols, rows, charsize);
}

inline size_t Adafruit_I2C_lcd::write(uint8_t value) {
  send(value, HIGH);
  return 1;
}

void Adafruit_I2C_lcd::setBacklight(uint8_t status) {
    _i2c.digitalWrite(ADAFRUIT_MPC_PIN_BACKLIGHT, status); // backlight is on pin 7
}

void Adafruit_I2C_lcd::send(uint8_t value, boolean mode) {
    _i2c.digitalWrite(ADAFRUIT_MPC_PIN_RS, mode); // RS
    write4bits(value>>4);
    write4bits(value);
}

void Adafruit_I2C_lcd::write4bits(uint8_t value) {
    uint8_t out = 0;
    out = _i2c.readGPIO();

    // speed up for i2c since its sluggish
    out &= ~_BV(ADAFRUIT_MPC_PIN_D0);
    out |= ((value >> 0) & 0x1) << ADAFRUIT_MPC_PIN_D0;

    out &= ~_BV(ADAFRUIT_MPC_PIN_D1);
    out |= ((value >> 1) & 0x1) << ADAFRUIT_MPC_PIN_D1;

    out &= ~_BV(ADAFRUIT_MPC_PIN_D2);
    out |= ((value >> 2) & 0x1) << ADAFRUIT_MPC_PIN_D2;

    out &= ~_BV(ADAFRUIT_MPC_PIN_D3);
    out |= ((value >> 3) & 0x1) << ADAFRUIT_MPC_PIN_D3;

    // make sure enable is low
    out &= ~ _BV(ADAFRUIT_MPC_PIN_ENABLE);

    _i2c.writeGPIO(out);

    // pulse enable
    delayMicroseconds(1);
    out |= _BV(ADAFRUIT_MPC_PIN_ENABLE);
    _i2c.writeGPIO(out);
    delayMicroseconds(1);
    out &= ~_BV(ADAFRUIT_MPC_PIN_ENABLE);
    _i2c.writeGPIO(out);
    delayMicroseconds(100);

}