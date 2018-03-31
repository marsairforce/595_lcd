// member functions for serial_lcd

#include "LCD.h"
#include <inttypes.h>
#include <Arduino.h>

Serial_595_lcd::Serial_595_lcd(uint8_t pin_ser, uint8_t pin_srclk, uint8_t pin_rclk) {
  _m_ser   = pin_ser;
  _m_srclk = pin_srclk;
  _m_rclk  = pin_rclk;

  pinMode(_m_ser,   OUTPUT);
  pinMode(_m_srclk, OUTPUT);
  pinMode(_m_rclk,  OUTPUT);
  digitalWrite(_m_rclk, LOW);
}

// class virtual functions

void Serial_595_lcd::begin(uint8_t cols, uint8_t rows, uint8_t charsize) {
  power_on();
  backlight_on();
  HD44780_LCD::begin(cols, rows, charsize);
}

void Serial_595_lcd::setBacklight(uint8_t status) {
  _data.field.backlight=status;
  port_write();
}

inline size_t Serial_595_lcd::write(uint8_t value) {
  send(value, HIGH);
  return 1;
}

void Serial_595_lcd::send(uint8_t value, boolean mode) {

  // set up RS
  _data.field.rs = mode;
  _data.field.data = 0;
  port_write();

  set_e();

  // upper 4 bits
  _data.field.data |= value  >> 4;
  port_write();

  clear_e();
  set_e();

  // lower 4 bits
  _data.field.data = 0;
  _data.field.data |= value;
  port_write();

  clear_e();

  _data.field.rs = 0; // turn off data and control pins
  port_write();
}

void Serial_595_lcd::write4bits(uint8_t data) {
  // see the write timing diagram
  _data.field.rs = 0; // rs is always 0. we only write nibbles for reset sequence.
  _data.field.data = 0;
  set_e();

  // set the data. Make sure just the lower 8 bits get changed.
  _data.field.data |= data ;
  port_write();

  // toggle E. This causes the data to be written to the LCD.
  clear_e();
}



// low level function
void Serial_595_lcd::port_write() {
  shiftOut(_m_ser, _m_srclk, LSBFIRST, _data.raw);
  digitalWrite(_m_rclk, HIGH);
  digitalWrite(_m_ser, LOW);
  digitalWrite(_m_rclk, LOW);
}

void Serial_595_lcd::set_e() {
  _data.field.e = 1;
  port_write();
}

void Serial_595_lcd::clear_e() {
  _data.field.e = 0;
  port_write();
}

void Serial_595_lcd::toggle_e() {
  _data.field.e = 1;
  port_write();
  delayMicroseconds(50);
  _data.field.e = 0;
  port_write();
}


void Serial_595_lcd::on() {
  power_on();
  backlight_on();

  delay(20); // we do require a bit more time after power up before we start to initialize this.
  initialize();
}

void Serial_595_lcd::off() {
  _data.raw = 0;
  port_write();
}

void Serial_595_lcd::power_on() {
  _data.field.power=1;
  port_write();
}

void Serial_595_lcd::power_off() {
  _data.field.power=0;
  port_write();
}

void Serial_595_lcd::backlight_on() {
  _data.field.backlight=1;
  port_write();
}

void Serial_595_lcd::backlight_off() {
  _data.field.backlight=0;
  port_write();
}

void Serial_595_lcd::toggle_backlight() {
  if (_data.field.backlight) {
      backlight_off();
  }
  else {
      backlight_on();
  }
}

void Serial_595_lcd::reset_sequence() {
  // https://electronics.stackexchange.com/questions/102245/hd44780-initialization-for-4-bit-mode
  // http://web.alfredstate.edu/faculty/weimandn/lcd/lcd_initialization/lcd_initialization_index.html
  // http://www.farnell.com/datasheets/50586.pdf

  write4bits(0x3);  // function set [ 0 0 1 DL N 0 * * ] ; DL=1 for 8 bit mode, 0 for 4 bit mode ; N=1 for 16:1 mux, 8 for 8:1
//  delayMicroseconds(5000); // was 4500
  ;
  toggle_e(); // toggle E 2 more times (pass the function set value again twice more basically)
//  delayMicroseconds(100); // was 100

  toggle_e();
//  delayMicroseconds(100);

  write4bits(0x2); // Function set (Set interface to 4 bits)
//  delayMicroseconds(100);

  _data.field.data = 0;
  port_write();
}

void Serial_595_lcd::initialize() {
  reset_sequence();
  send(0x2C, false); // function set (2 lines, 5x11 font)
  send(0x08, false); // display off, cursor off, blink off
  send(0x01, false); // clear screen & return cursor home
  send(0x06, false); // inc cursor to the right when writing and don't shift the screen
  send(0x0C, false); // turn display on
}

void Serial_595_lcd::set_dram_address(unsigned char address) {
  send(0x80 + (address & 0x7F), false);
}

/**
     * The devices provide a 7 bit DRAM address
     * Internally there are 40 character columns, even if the display does not physically have this many columns
     * The row offsets are not consecutive starting points.
     * e.g. for 40 characters, this only uses 0x00-0x27 (0-39)for the first line
     * but the second line in the dram address starts at 0x40-0x67
     * what about those 25 (0x19) bytes from 0x28-0x39?
     *
     * To make it more complicated, 4 line display modules that have less than 40 characters per line
     * e.g. like these 20x4 modles I have http://www.newhavendisplay.com/specs/NHD-0420DZ-NSW-BBW.pdf
     * use the not visible DRAM address positions from line 1 are mapped to display line 3.
     * I guess this means we can not do the scroll display
     *
     * The set dram address command is 0x80, plus the memory offset.
     * so we want to convert the input of (x,y) to the linear value
     * that satisfies this mess.
     */
void Serial_595_lcd::gotoxy(unsigned char x, unsigned char y) {
  int offset = 0;
  if (x > (_m_cols - 1)) {
    // error
    return;
  }
  if (y > (_m_rows - 1)) {
    // error
    return;
  }
  if ( y == 1 || y == 3 ) {
    offset += 0x40;
  }
  if (y == 2 || y == 3 ) {
    offset += 20;
  }
  offset += x;
  set_dram_address(offset);
}

void Serial_595_lcd::puts(char *str) {
  for (int i = 0; str[i] != 0; i++) {
    send(str[i], true);
  }
  _data.field.data = 0;
  _data.field.rs = 0;
  port_write();
}

void Serial_595_lcd::display_screen(lcd_screen_buffer *screen) {
    // TODO
}
