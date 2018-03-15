
#include "serial_595_lcd.h"

#include <Arduino.h>

/**
* Performs the signalling required to write to a LCD port
* Write the raw data to the LCD port shift register.
*/
void lcd_port_write(serial_lcd *lcd) {
  digitalWrite(lcd->pin_rclk, LOW);
  shiftOut(lcd->pin_ser, lcd->pin_srclk, LSBFIRST, lcd->data);
  digitalWrite(lcd->pin_rclk, HIGH);
  digitalWrite(lcd->pin_rclk, LOW);
  digitalWrite(lcd->pin_ser, LOW);
}

void lcd_on(serial_lcd *lcd) {
  set_bit(&lcd->data, LCD_POWER);
  lcd_port_write(lcd);
  lcd_initialize(lcd);
  lcd_backlight_on(lcd);
}

void lcd_off(serial_lcd *lcd) {
  //backlight_off();
  //clear_bit(&port_b, LCD_POWER);
  // really we need to just put every pin low on the LCD port
  lcd->data = 0;
  lcd_port_write(lcd);
}

void lcd_backlight_on(serial_lcd *lcd) {
  set_bit(&lcd->data, LCD_BACKLIGHT);
  lcd_port_write(lcd);
}

void lcd_backlight_off(serial_lcd *lcd) {
  clear_bit(&lcd->data, LCD_BACKLIGHT);
  lcd_port_write(lcd);
}

void lcd_toggle_backlight(serial_lcd *lcd) {
  if (lcd->data & LCD_BACKLIGHT) {
      lcd_backlight_off(lcd);
  }
  else {
      lcd_backlight_on(lcd);
  }
}

// writes 4 bits
void lcd_write_nibble(serial_lcd *lcd, int RS, int data) {
  // see the write timing diagram

  // set up RS, Make sure E is LOW
  clear_bit(&lcd->data, LCD_E | LCD_RS);
  if (RS) {
    set_bit(&lcd->data, LCD_RS);
  }
  lcd_port_write(lcd);

  // toggle E to HIGH
  set_bit(&lcd->data, LCD_E);
  lcd_port_write(lcd);

  // set the data. Make sure just the lower 8 bits get changed.
  clear_bit(&lcd->data, LCD_DATA);
  lcd->data |= (data & LCD_DATA);
  lcd_port_write(lcd);

  // toggle E to LOW. This causes the data to be written to the LCD.
  clear_bit(&lcd->data, LCD_E);
  lcd_port_write(lcd);
}

void lcd_write(serial_lcd *lcd, int RS, int data) {
  lcd_write_nibble(lcd, RS, (data & 0xF0) >> 4);
  lcd_write_nibble(lcd, RS, (data & 0x0F) );
}

void lcd_port_toggle_e(serial_lcd *lcd) {
  // toggle E to HIGH
  set_bit(&lcd->data, LCD_E);
  lcd_port_write(lcd);
  delay(1);

  // toggle E to LOW.
  clear_bit(&lcd->data, LCD_E);
  lcd_port_write(lcd);
  delay(1);
}

void lcd_initialize(serial_lcd *lcd) {
  // see: https://cdn-shop.adafruit.com/datasheets/TC2004A-01.pdf
  delay(20);  // wait time > 40ms afte VDD > 2.7V
  lcd_write_nibble(lcd, 0, 0x3);  // function set [ 0 0 1 DL N 0 * * ] ; DL=1 for 8 bit mode, 0 for 4 bit mode ; N=1 for 16:1 mux, 8 for 8:1
  delay(10);
  lcd_port_toggle_e(lcd); // toggle E 2 more times (pass the function set value again twice more basically)
  delay(1);
  lcd_port_toggle_e(lcd);
  delay(1);
  lcd_write_nibble(lcd, 0, 0x2); // Function set (Set interface to 4 bits)
  delay(4);
  lcd_write(lcd, 0, 0x2C); // function set (2 lines, 5x11 font)
  lcd_write(lcd, 0, 0x08); // display off, cursor off, blink off
  lcd_write(lcd, 0, 0x01); // clear screen & return cursor home
  lcd_write(lcd, 0, 0x06); // inc cursor to the right when writing and don't shift the screen

  lcd_write(lcd, 0, 0x0C); // turn display on

  clear_bit (&lcd->data, LCD_DATA); // turn off data pins
  lcd_port_write(lcd);
}

void lcd_puts(serial_lcd *lcd, char *str) {
  unsigned int i = 0;
  for (; str[i] != 0; i++) {
    if (i > 15) {
      lcd_write(lcd, 0, 0x18); // shift the display
    }
    lcd_write(lcd, 1, str[i]);
  }
  clear_bit (&lcd->data, LCD_DATA); // turn off data pins
  clear_bit (&lcd->data, LCD_RS);
  lcd_port_write(lcd);
}

void lcd_clear(serial_lcd *lcd) {
  lcd_write(lcd, 0, 0x01);
  delay(1);
}

// Set DDRAM address in address counter
// this function takes 37 us
void lcd_set_dram_address(serial_lcd *lcd, int address) {
   lcd_write(lcd, 0, 0x80 + (address & 0x7F));
}


// The devices provide a 7 bit DRAM address
// Internally there are 40 character columns, even if the display does not physically have this many columns
// The row offsets are not consecutive starting points.
// e.g. for 40 characters, this only uses 0x00-0x27 (0-39)for the first line
// but the second line in the dram address starts at 0x40-0x67
// what about those 25 (0x19) bytes from 0x28-0x39?

// to make it more complicated, 4 line display modules that have less than 40 characters per line
// e.g. like these 20x4 modles I have http://www.newhavendisplay.com/specs/NHD-0420DZ-NSW-BBW.pdf
// use the not visible DRAM address positions from line 1 are mapped to display line 3.
// I guess this means we can not do the scroll display

// The set dram address command is 0x80, plus the memory offset.
// so we want to convert the input of (x,y) to the linear value
// that satisfies this mess.
void lcd_gotoxy(serial_lcd *lcd, int x, int y) {
  int offset = 0;
  if (x > 19) {
    // error
    return;
  }
  if (y > 2) {
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
  lcd_set_dram_address(lcd, offset);
}

// Set DDRAM address to "00H" from AC and return cursor to its original position if shifted.
// The contents of DDRAM are not changed.
// This function takes 1.52ms
void lcd_home(serial_lcd *lcd) {
  lcd_write(lcd, 0, 0x02); // go to home position
}
