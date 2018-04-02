// member functions for lcd_screen_buffer

#include "LCD.h"
#include <Arduino.h>

// https://gist.github.com/tuttlem/0b413109c8d5592b9065

lcd_screen_buffer::lcd_screen_buffer(HD44780_LCD *lcd) {
  _lcd = lcd;
}

// we assume lcd.begin() has been called before this is called.
void lcd_screen_buffer::begin(uint8_t cols, uint8_t rows) {
  if (this->_buffer != NULL) {
    return;
  }

  if (cols > 0) {
    _cols = cols;
  }
  else {
    _cols = _lcd->getCols();
  }

  if (rows > 0) {
    _rows = rows;
  }
  else {
    _rows = _lcd->getRows();
  }

  _size = _rows * _cols;
  //_cursor = 0;

  // allocate buffer area
  _buffer = new char[_size];
  clear();
}

lcd_screen_buffer::~lcd_screen_buffer() {
  delete[] this->_buffer;
}

void lcd_screen_buffer::clear() {
  // just write out space characters to the buffer
  memset(this->_buffer, ' ', sizeof(char) * this->_size);
}

size_t lcd_screen_buffer::write(uint8_t value) {
  // use the current cursor position to compute the linear address into the buffer
  char buf[2];
  sprintf(buf, "%c", value);
  printToBuffer(_vcursor_x, _vcursor_y, buf);
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

void lcd_screen_buffer::printToBuffer(const uint8_t x, const uint8_t y, const char *s) {
	char *o = this->_buffer + (x + (y * this->_cols));
	memcpy(o, s, strlen(s)); // Note: we should have some boundary checking here.
}

// void lcd_screen_buffer::setCursor(uint8_t row, uint8_t col) {
//   int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
//   if ( row > _rows ) {
//     row = _rows-1;    // we count rows starting w/0
//   }

//   _cursor = (col + row_offsets[row]);
// }

void lcd_screen_buffer::writeToLcd() {
  int buf_y = this->_win_y;
  int lcdRows = this->_lcd->getRows();
  int lcdCols = this->_lcd->getCols();
	for (int y = 0; y < lcdRows; y ++) {
		int buf_x = this->_win_x;
		for (int x = 0; x < lcdCols; x ++) {
			this->_lcd->setCursor(x, y);
			if (buf_y >= 0 && buf_y < this->_rows && buf_x >= 0 && buf_x < this->_cols) {
				int ofs = buf_x + (buf_y * this->_cols);
				this->_lcd->print(this->_buffer[ofs]);
			} else {
				this->_lcd->print(" ");
			}
			buf_x ++;
		}
		buf_y ++;
	}
  if (_cursor_visible) {
    _lcd->cursor();
  }
  else {
    _lcd->noCursor();
  }
  if (_cursor_blink) {
    _lcd->blink();
  }
  else {
    _lcd->noBlink();
  }
}