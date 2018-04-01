#include "LCD.h"
#include <Arduino.h>
#include <inttypes.h>
#include "Print.h"
#include <inttypes.h>

// the I/O expander pinout
#define ADAFRUIT_MPC_PIN_RS         1
#define ADAFRUIT_MPC_PIN_ENABLE     2
#define ADAFRUIT_MPC_PIN_D0         3
#define ADAFRUIT_MPC_PIN_D1         4
#define ADAFRUIT_MPC_PIN_D2         5
#define ADAFRUIT_MPC_PIN_D3         6
#define ADAFRUIT_MPC_PIN_BACKLIGHT  7

Adafruit_SPI_lcd::Adafruit_SPI_lcd(uint8_t data, uint8_t clock, uint8_t latch) {
    _data_pin = data;
    _clock_pin = clock;
    _latch_pin = latch;

    pinMode(_data_pin, OUTPUT);
    pinMode(_clock_pin, OUTPUT);
    pinMode(_latch_pin, OUTPUT);
    digitalWrite(_latch_pin, LOW);

    // _data.field.backlight = 1;
    // _data.field.rs = 0;
    // _data.field.en = 0;
    // port_write();

    // the SPI expander pinout
    _rs_pin = 1;
    _enable_pin = 2;
    _data_pins[0] = 6;  // really d4
    _data_pins[1] = 5;  // really d5
    _data_pins[2] = 4;  // really d6
    _data_pins[3] = 3;  // really d7
    _SPIbuff = 0x80; // backlight
}

// we can only do this i2c bein inside setup() function, where the lcd.begin() is called.
// usually we declare the instance variable in the file right.
void Adafruit_SPI_lcd::begin(uint8_t cols, uint8_t rows, uint8_t charsize)
{
    // from the begin()
    HD44780_LCD::begin(cols, rows, charsize);
}

inline size_t Adafruit_SPI_lcd::write(uint8_t value) {
  send(value, HIGH);
  return 1;
}

void Adafruit_SPI_lcd::setBacklight(uint8_t status) {
//    _data.field.backlight = status;
//    port_write();
    _digitalWrite(7, status); // backlight is on pin 7
}

boolean Adafruit_SPI_lcd::isBacklight() {
    //return _data.field.backlight;
    return _SPIbuff & 0x80;
}

void Adafruit_SPI_lcd::send(uint8_t value, boolean mode) {
    // _data.field.rs = mode; // RS
    // port_write();
    _digitalWrite(_rs_pin, mode);
    write4bits(value>>4);
    write4bits(value);
}

void Adafruit_SPI_lcd::write4bits(uint8_t value) {
    // _data.field.data = value;
    // //_data.field.en = 0; // make sure enable is low
    // port_write();
    // // pulse enable
    // delayMicroseconds(1);
    // _data.field.en = 1;
    // port_write();
    // delayMicroseconds(1);
    // _data.field.en = 0;
    // delayMicroseconds(100);

    for (int i = 0; i < 4; i++) {
      _digitalWrite(_data_pins[i], (value >> i) & 0x01);
    }
    pulseEnable();
}

// low level functions

void  Adafruit_SPI_lcd::_digitalWrite(uint8_t p, uint8_t d) {
    if (d == HIGH) {
      _SPIbuff |= (1 << p);
    }
    else {
      _SPIbuff &= ~(1 << p);
    }

    digitalWrite(_latch_pin, LOW);
    shiftOut(_data_pin, _clock_pin, MSBFIRST,_SPIbuff);
    digitalWrite(_latch_pin, HIGH);
}

void Adafruit_SPI_lcd::pulseEnable() {
  _digitalWrite(_enable_pin, LOW);
  delayMicroseconds(1);
  _digitalWrite(_enable_pin, HIGH);
  delayMicroseconds(1);    // enable pulse must be >450ns
  _digitalWrite(_enable_pin, LOW);
  delayMicroseconds(100);   // commands need > 37us to settle
}

// void Adafruit_SPI_lcd::port_write() {
//   shiftOut(_data_pin, _clock_pin, LSBFIRST, _data.raw);
//   digitalWrite(_latch_pin, HIGH);
//   digitalWrite(_data_pin, LOW);
//   digitalWrite(_latch_pin, LOW);
// }