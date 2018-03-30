// member functions for serial_lcd

#include "LCD.h"
#include <inttypes.h>
#include "bit_macros.h"
#include <Arduino.h>

serial_595_lcd::serial_595_lcd(uint8_t pin_ser, uint8_t pin_srclk, uint8_t pin_rclk) {
  _m_ser   = pin_ser;
  _m_srclk = pin_srclk;
  _m_rclk  = pin_rclk;

  pinMode(_m_ser,   OUTPUT);
  pinMode(_m_srclk, OUTPUT);
  pinMode(_m_rclk,  OUTPUT);
  digitalWrite(_m_rclk, LOW);
}

void serial_595_lcd::port_write() {
  shiftOut(_m_ser, _m_srclk, LSBFIRST, _m_data);

  digitalWrite(_m_rclk, HIGH);
  digitalWrite(_m_ser, LOW);
  digitalWrite(_m_rclk, LOW);
}

void serial_595_lcd::set_e() {
  // set E to high
  set_bit(&_m_data, LCD_E);
  port_write();
}

void serial_595_lcd::clear_e() {
  clear_bit(&_m_data, LCD_E);
  port_write();
}

void serial_595_lcd::toggle_e() {
  set_bit(&_m_data, LCD_E);
  port_write();

  delayMicroseconds(50);

  clear_bit(&_m_data, LCD_E);
  port_write();
}

void serial_595_lcd::write_nibble(unsigned char data) {
  // see the write timing diagram
  clear_bit(&_m_data, LCD_RS);  // rs is always 0. we only write nibbles for reset sequence.
  clear_bit(&_m_data, LCD_DATA);
  set_e();

  // set the data. Make sure just the lower 8 bits get changed.
  _m_data |= (data & LCD_DATA);
  port_write();

  // toggle E. This causes the data to be written to the LCD.
  clear_e();
}

void serial_595_lcd::write_byte(unsigned char RS, unsigned char d_in) {
  // set up RS
  if (RS) {
    set_bit(&_m_data, LCD_RS);
  } else {
    clear_bit(&_m_data, LCD_RS);
  }
  clear_bit(&_m_data, LCD_DATA);
  port_write();

  set_e();

  // upper 4 bits
  _m_data |= ((d_in  >> 4) & LCD_DATA);
  port_write();

  clear_e();

  set_e();

  // lower 4 bits
  clear_bit(&_m_data, LCD_DATA);
  _m_data |= (d_in & LCD_DATA);
  port_write();

  clear_e();

  clear_bit (&_m_data, LCD_DATA | LCD_RS ); // turn off data and control pins
  port_write();
}

void serial_595_lcd::on() {
  set_bit(&_m_data, LCD_POWER);
  port_write();
  backlight_on();

  delay(20); // we do require a bit more time after power up before we start to initialize this.
  initialize();
}

void serial_595_lcd::off() {
  _m_data = 0;
  port_write();
}

void serial_595_lcd::power_on() {
  set_bit(&_m_data, LCD_POWER);
  port_write();
}

void serial_595_lcd::power_off() {
  clear_bit(&_m_data, LCD_POWER);
  port_write();
}

void serial_595_lcd::backlight_on() {
  set_bit(&_m_data, LCD_BACKLIGHT);
  port_write();
}

void serial_595_lcd::backlight_off() {
  clear_bit(&_m_data, LCD_BACKLIGHT);
  port_write();
}

void serial_595_lcd::toggle_backlight() {
  if (_m_data & LCD_BACKLIGHT) {
      backlight_off();
  }
  else {
      backlight_on();
  }
}

void serial_595_lcd::reset_sequence() {
  // https://electronics.stackexchange.com/questions/102245/hd44780-initialization-for-4-bit-mode
  // http://web.alfredstate.edu/faculty/weimandn/lcd/lcd_initialization/lcd_initialization_index.html
  // http://www.farnell.com/datasheets/50586.pdf

  write_nibble(0x3);  // function set [ 0 0 1 DL N 0 * * ] ; DL=1 for 8 bit mode, 0 for 4 bit mode ; N=1 for 16:1 mux, 8 for 8:1
//  delayMicroseconds(5000); // was 4500
  ;
  toggle_e(); // toggle E 2 more times (pass the function set value again twice more basically)
//  delayMicroseconds(100); // was 100

  toggle_e();
//  delayMicroseconds(100);

  write_nibble(0x2); // Function set (Set interface to 4 bits)
//  delayMicroseconds(100);

  clear_bit (&_m_data, LCD_DATA ); // turn off data and control pins
  port_write();
}

void serial_595_lcd::initialize() {
  reset_sequence();
  write_byte(0, 0x2C); // function set (2 lines, 5x11 font)
  write_byte(0, 0x08); // display off, cursor off, blink off
  write_byte(0, 0x01); // clear screen & return cursor home
  write_byte(0, 0x06); // inc cursor to the right when writing and don't shift the screen
  write_byte(0, 0x0C); // turn display on
}

void serial_595_lcd::set_size(int cols, int rows) {
  _m_cols = cols;
  _m_rows = rows;
}

void serial_595_lcd::home() {
  write_byte(0, 0x02); // go to home position
}

void serial_595_lcd::set_dram_address(unsigned char address) {
  write_byte(0, 0x80 + (address & 0x7F));
}

void serial_595_lcd::gotoxy(unsigned char x, unsigned char y) {
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

void serial_595_lcd::puts(char *str) {
  for (int i = 0; str[i] != 0; i++) {
    // TODO: define auto shift ability?
    // if (i > 15) {
    //   this->write_byte(0, 0x18); // shift the display
    // }
    write_byte(1, str[i]);
  }
  clear_bit (&_m_data, LCD_DATA); // turn off data pins
  clear_bit (&_m_data, LCD_RS);
  port_write();
}

void serial_595_lcd::clear() {
  write_byte(0, 0x01);
  delay(1); // TODO: determine if we can use smaller value here.
}

void serial_595_lcd::display_screen(lcd_screen_buffer *screen) {
    // TODO
}
