
#include "serial_595_lcd.h"

#include <Arduino.h>

/**
* @deprecated
*/
void lcd_port_write(serial_lcd *lcd) {
  lcd->port_write();
}

// @deprecated: should use class member function
void lcd_on(serial_lcd *lcd) {
  lcd->lcd_on();
}

// @deprecated: should use class member function
void lcd_off(serial_lcd *lcd) {
  lcd->lcd_off();
}

// @deprecated
void lcd_backlight_on(serial_lcd *lcd) {
  lcd->backlight_on();
}

// @deprecated
void lcd_backlight_off(serial_lcd *lcd) {
  lcd->backlight_off();
}

// @deprecated
void lcd_toggle_backlight(serial_lcd *lcd) {
  lcd->toggle_backlight();
}

// @deprecated
void lcd_initialize(serial_lcd *lcd) {
  lcd->initialize();
}

// deprecated
void lcd_puts(serial_lcd *lcd, char *str) {
  lcd->puts(str);
}

// @deprecated
void lcd_clear(serial_lcd *lcd) {
  lcd->clear();
}

/**
 * @deprecated
 */
void lcd_set_dram_address(serial_lcd *lcd, int address) {
   lcd->set_dram_address(address);
}


/**
 * @deprecated
 */
void lcd_gotoxy(serial_lcd *lcd, int x, int y) {
  lcd->gotoxy(x,y);
}

/**
 * @deprecated
 */
void lcd_home(serial_lcd *lcd) {
  lcd->home();
}


// serial_lcd member functions

void serial_lcd::port_write() {
  shiftOut(this->pin_ser, this->pin_srclk, LSBFIRST, this->data);
  digitalWrite(this->pin_rclk, HIGH);
  digitalWrite(this->pin_ser, LOW);
  digitalWrite(this->pin_rclk, LOW);
}

void serial_lcd::port_toggle_e() {
  set_bit(&this->data, LCD_E);
  this->port_write();
  delayMicroseconds(5);
  clear_bit(&this->data, LCD_E);
  this->port_write();
}

void serial_lcd::write_nibble(int data) {
  // see the write timing diagram

  clear_bit(&this->data, LCD_RS);
  // set E to high
  set_bit(&this->data, LCD_E);
  this->port_write();

  // set the data. Make sure just the lower 8 bits get changed.
  clear_bit(&this->data, LCD_DATA);
  this->data |= (data & LCD_DATA);
  this->port_write();


  // toggle E to LOW. This causes the data to be written to the LCD.
  clear_bit(&this->data, LCD_E);
  this->port_write();
}

void serial_lcd::write_byte(int RS, int data) {
  // set up RS
  if (RS) {
    set_bit(&this->data, LCD_RS);
  } else {
    clear_bit(&this->data, LCD_RS);
  }
  clear_bit(&this->data, LCD_DATA);
  this->port_write();

  // set E to high
  set_bit(&this->data, LCD_E);
  this->port_write();

  // set the data. Make sure just the lower 8 bits get changed.
  clear_bit(&this->data, LCD_DATA);
  this->data |= (((data & 0xF0) >> 4) & LCD_DATA);
  this->port_write();

  // toggle E to LOW. This causes the data to be written to the LCD.
  clear_bit(&this->data, LCD_E);
  this->port_write();

  set_bit(&this->data, LCD_E);
  this->port_write();

  // set the data. Make sure just the lower 8 bits get changed.
  clear_bit(&this->data, LCD_DATA);
  this->data |= ((data & 0xF) & LCD_DATA);
  this->port_write();

  // toggle E to LOW. This causes the data to be written to the LCD.
  clear_bit(&this->data, LCD_E);
  this->port_write();

  delayMicroseconds(37); // TODO: this was 80. Make sure things still work.
}

void serial_lcd::lcd_on() {
  set_bit(&this->data, LCD_POWER);
  this->port_write();
  delay(20); // we do require a bit more time after power up before we start to initialize this.
  lcd_initialize(this);
  lcd_backlight_on(this);
}

void serial_lcd::lcd_off() {
  this->data = 0;
  this->port_write();
}

void serial_lcd::backlight_on() {
  set_bit(&this->data, LCD_BACKLIGHT);
  this->port_write();
}

void serial_lcd::backlight_off() {
  clear_bit(&this->data, LCD_BACKLIGHT);
  this->port_write();
}

void serial_lcd::toggle_backlight() {
  if (this->data & LCD_BACKLIGHT) {
      this->backlight_off();
  }
  else {
      this->backlight_on();
  }
}

void serial_lcd::initialize() {
  digitalWrite(this->pin_rclk, LOW); // pre-initialize this to low.

  // https://electronics.stackexchange.com/questions/102245/hd44780-initialization-for-4-bit-mode
  // http://web.alfredstate.edu/faculty/weimandn/lcd/lcd_initialization/lcd_initialization_index.html
  // http://www.farnell.com/datasheets/50586.pdf

  this->write_nibble(0x3);  // function set [ 0 0 1 DL N 0 * * ] ; DL=1 for 8 bit mode, 0 for 4 bit mode ; N=1 for 16:1 mux, 8 for 8:1
  delayMicroseconds(4500);
  this->port_toggle_e(); // toggle E 2 more times (pass the function set value again twice more basically)
  delayMicroseconds(100);
  this->port_toggle_e();
  delayMicroseconds(100);
  this->write_nibble(0x2); // Function set (Set interface to 4 bits)
  delayMicroseconds(100);

  this->write_byte(0, 0x2C); // function set (2 lines, 5x11 font)
  this->write_byte(0, 0x08); // display off, cursor off, blink off
  this->write_byte(0, 0x01); // clear screen & return cursor home
  this->write_byte(0, 0x06); // inc cursor to the right when writing and don't shift the screen
  this->write_byte(0, 0x0C); // turn display on

  clear_bit (&this->data, LCD_DATA); // turn off data pins
  this->port_write();
}

void serial_lcd::home() {
  this->write_byte(0, 0x02); // go to home position
}

void serial_lcd::set_dram_address(int address) {
   this->write_byte(0, 0x80 + (address & 0x7F));
}

void serial_lcd::gotoxy(int x, int y) {
  int offset = 0;
  if (x > 19) {
    // error
    return;
  }
  if (y > 3) {
    // error
    return;
  }
  if ( y == 1 || y == 3) {
    offset += 0x40;
  }
  if (y == 2 || y == 3 ) {
    offset += 20;
  }
  offset += x;
  this->set_dram_address(offset);
}

void serial_lcd::puts(char *str) {
  unsigned int i = 0;
  for (; str[i] != 0; i++) {
    // TODO: define auto shift ability?
    // if (i > 15) {
    //   this->write_byte(0, 0x18); // shift the display
    // }
    this->write_byte(1, str[i]);
  }
  clear_bit (&this->data, LCD_DATA); // turn off data pins
  clear_bit (&this->data, LCD_RS);
  this->port_write();
}

void serial_lcd::clear() {
  this->write_byte(0, 0x01);
  delay(1); // TODO: determine if we can use smaller value here.
}

void serial_lcd::display_screen(lcd_screen_buffer *screen) {
    // TODO
}

// screen_buffer member functions
void lcd_screen_buffer::clear_screen() {
  printf("%*s", LCD_DRAM_MAX_COLUMN * LCD_DRAM_MAX_ROW, ' ');
}
