#include "LCD.h"
#include <Arduino.h>
#include <inttypes.h>
#include "Print.h"
#include <inttypes.h>
#include <Wire.h>



PCF8574_lcd::PCF8574_lcd(uint8_t addr)
{
    _addr = addr;
}

void PCF8574_lcd::begin(uint8_t cols, uint8_t rows, uint8_t charsize) {
    Wire.begin();
    _data.field.data=0;
    _data.field.en=0;
    _data.field.rs=0;
    _data.field.backlight=1;
    port_write();
    HD44780_LCD::begin(cols, rows, charsize);
}

inline size_t PCF8574_lcd::write(uint8_t value) {
  send(value, HIGH);
  return 1;
}

void PCF8574_lcd::setBacklight(uint8_t status) {
    _data.field.backlight = status;
    port_write();
}

boolean PCF8574_lcd::isBacklight() {
    return _data.field.backlight;
}

void PCF8574_lcd::send(uint8_t value, boolean mode) {
    _data.field.rs = mode;
    port_write();
    write4bits(value>>4);
    write4bits(value);
}

void PCF8574_lcd::write4bits(uint8_t value) {
    _data.field.data = value;
//    _data.fields.en = 0;  // make sure enable is low
    port_write();

    // pulse enable
    delayMicroseconds(1);
    _data.field.en = 1;
    port_write();
    delayMicroseconds(1);
    _data.field.en = 0;
    port_write();
    delayMicroseconds(100);
}

// private function to change the PCF8674 pins to the given value
// it looks like this device is like a 74HC595 shift register, where we need to send the state of each bit with every write operation.
// So we define member variables for the state pins like we do in the serial_595_lcd.
void PCF8574_lcd::port_write() {
  Wire.beginTransmission(_addr);
  Wire.write(_data.raw);
  Wire.endTransmission();
}
