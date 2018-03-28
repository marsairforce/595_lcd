// member functions for lcd_screen_buffer

#include "serial_595_lcd.h"
#include "bit_macros.h"
#include <Arduino.h>


void lcd_screen_buffer::clear_screen() {
  // just write out space characters to the buffer
  printf("%*s", LCD_DRAM_MAX_COLUMN * LCD_DRAM_MAX_ROW, ' ');
}

void lcd_screen_buffer::puts(char *str) {

}
