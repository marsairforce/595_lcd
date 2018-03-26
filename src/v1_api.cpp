/**
* The C style functions from the first version of this library.
* These have been replaced by functions in a structure now, and should be deleted.
* Keeping for now to maintain backwards compatibility.
* @deprecated Will be removed in version 2.
*/

#include "serial_595_lcd.h"
#include "serial_lcd_v1.h"


void lcd_on(serial_lcd *lcd) {
  lcd->on();
}

void lcd_off(serial_lcd *lcd) {
  lcd->off();
}

void lcd_backlight_on(serial_lcd *lcd) {
  lcd->backlight_on();
}

void lcd_backlight_off(serial_lcd *lcd) {
  lcd->backlight_off();
}

void lcd_toggle_backlight(serial_lcd *lcd) {
  lcd->toggle_backlight();
}

void lcd_initialize(serial_lcd *lcd) {
  lcd->initialize();
}

void lcd_puts(serial_lcd *lcd, char *str) {
  lcd->puts(str);
}

void lcd_clear(serial_lcd *lcd) {
  lcd->clear();
}

void lcd_set_dram_address(serial_lcd *lcd, int address) {
   lcd->set_dram_address(address);
}

void lcd_gotoxy(serial_lcd *lcd, int x, int y) {
  lcd->gotoxy(x,y);
}

void lcd_home(serial_lcd *lcd) {
  lcd->home();
}

