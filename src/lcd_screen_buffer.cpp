// member functions for lcd_screen_buffer

#include "LCD.h"
#include <Arduino.h>

lcd_screen_buffer::lcd_screen_buffer(HD44780_LCD *lcd) {
  _lcd = lcd;
  _rows = _lcd->getRows();
  _cols = _lcd->getCols();
  _size = _rows * _cols;
  _cursor = 0;

  // allocate buffer area
  _buffer = new char[_size];
}

void lcd_screen_buffer::clear() {
  // just write out space characters to the buffer
  memset(this->_buffer, ' ', sizeof(char) * this->_size);
}

size_t lcd_screen_buffer::write(uint8_t value) {

  return 1;
}

void lcd_screen_buffer::noBlink() {
  _cursor_blink = 0;
}

void lcd_screen_buffer::blink() {
  _cursor_blink = 1;
}

void lcd_screen_buffer::noCursor() {
  _cursor_visible = 0;
}

void lcd_screen_buffer::cursor() {
  _cursor_visible = 1;
}

void lcd_screen_buffer::setCursor(uint8_t row, uint8_t col) {
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if ( row > _rows ) {
    row = _rows-1;    // we count rows starting w/0
  }

  _cursor = (col + row_offsets[row]);
}

void lcd_screen_buffer::writeToLcd() {

}