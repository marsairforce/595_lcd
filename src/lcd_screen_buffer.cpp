// member functions for lcd_screen_buffer

#include "serial_595_lcd.h"
#include "bit_macros.h"
#include <Arduino.h>

// screen_buffer member functions
void lcd_screen_buffer::clear_screen() {
  printf("%*s", LCD_DRAM_MAX_COLUMN * LCD_DRAM_MAX_ROW, ' ');
}
