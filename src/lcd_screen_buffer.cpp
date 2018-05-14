// member functions for lcd_screen_buffer

#include "LCD.h"
#include <Arduino.h>

// https://gist.github.com/tuttlem/0b413109c8d5592b9065

lcd_screen_buffer::lcd_screen_buffer(HD44780_LCD *lcd) {
  _lcd = lcd;
}

// we assume lcd.begin() has been called before this is called.
void lcd_screen_buffer::begin(uint8_t cols, uint8_t rows) {
  // if (this->_buffer != NULL) {
  //   return;
  // }

  // if (cols > 0) {
  //   _cols = cols;
  // }
  // else {
  //   _cols = _lcd->getCols();
  // }

  // if (rows > 0) {
  //   _rows = rows;
  // }
  // else {
  //   _rows = _lcd->getRows();
  // }

  //_size = _rows * _cols;

    // allocate buffer area
  //_buffer = new char[_size+1];
  clear();

  // _win_x = 0;
  // _win_y = 0;

  // _cursor_x = 0;
  // _cursor_y = 0;

  _vcursor_x = 0;
  _vcursor_y = 0;
}

lcd_screen_buffer::~lcd_screen_buffer() {
  //delete[] this->_buffer;
}

void lcd_screen_buffer::clear() {
  memset(this->_buffer, ' ', this->_size);
}

size_t lcd_screen_buffer::write(uint8_t value) {
  // use the current cursor position to compute the linear address into the buffer
  this->_buffer[ _vcursor_x + (_vcursor_y * this->_cols)] = (char) value;

  // and then adjust the position following number of characters written
  _vcursor_x ++;
  if (_vcursor_x >= _cols) {
    _vcursor_x = 0;
    _vcursor_y++;
    if (_vcursor_y >= _rows) {
      _vcursor_y = 0;
    }
  }
  return 1;
}


void lcd_screen_buffer::writeToLcd() {
	for (int y = 0; y < this->_rows; y ++) {
		this->_lcd->setCursor(0, y);
		for (int x = 0; x < this->_cols; x ++) {
			this->_lcd->print((char) this->_buffer[x + (y * this->_cols)]);
		}
	}
  // if (_cursor_visible) {
  //   _lcd->cursor();
  // }
  // else {
  //   _lcd->noCursor();
  // }
  // if (_cursor_blink) {
  //   _lcd->blink();
  // }
  // else {
  //   _lcd->noBlink();
  // }
}