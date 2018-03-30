#include "LCD.h"
#include <Arduino.h>
#include <inttypes.h>
#include "Print.h"

Parallel_4bit_lcd::Parallel_4bit_lcd(uint8_t rs, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t backlight_pin) 
{
    _rs_pin = rs;
    _enable_pin = enable;
    _data_pins[0] = d0;
    _data_pins[1] = d1;
    _data_pins[2] = d2;
    _data_pins[3] = d3; 

    _backlight_pin = backlight_pin;

    pinMode(_rs_pin, OUTPUT);
    digitalWrite(_rs_pin, LOW);

    pinMode(_enable_pin, OUTPUT);
    digitalWrite(_enable_pin, LOW);    

    for (int i = 0; i < 4; i++) {
      pinMode(_data_pins[i], OUTPUT);
      digitalWrite(_data_pins[i], 0);
    }

    if (_backlight_pin > 0) {
        pinMode(_backlight_pin, OUTPUT);
    }
}


void Parallel_4bit_lcd::setBacklight(uint8_t status) 
{   
    if (_backlight_pin > 0) {
        digitalWrite(_backlight_pin,  (status) ? HIGH : LOW);
    }
}

inline size_t Parallel_4bit_lcd::write(uint8_t value) {
  send(value, HIGH);
  return 1;
}
/************ low level data pushing commands **********/

// write either command or data
void Parallel_4bit_lcd::send(uint8_t value, boolean mode) 
{
    digitalWrite(_rs_pin, mode); 
    write4bits(value>>4);
    write4bits(value);
}

void Parallel_4bit_lcd::pulseEnable(void)
{
  digitalWrite(_enable_pin, LOW);
  delayMicroseconds(1);    
  digitalWrite(_enable_pin, HIGH);
  delayMicroseconds(1);    // enable pulse must be >450ns
  digitalWrite(_enable_pin, LOW);
  delayMicroseconds(100);   // commands need > 37us to settle
}

void Parallel_4bit_lcd::write4bits(uint8_t value) 
{
    for (int i = 0; i < 4; i++) {
//      _pinMode(_data_pins[i], OUTPUT);
      digitalWrite(_data_pins[i], (value >> i) & 0x01);
    }
    pulseEnable();
}
