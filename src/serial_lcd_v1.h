#ifndef _serial_lcd_v1_h_
#define _serial_lcd_v1_h_
// @deprecated
void lcd_on(serial_lcd *lcd);

// @deprecated
void lcd_off(serial_lcd *lcd);

// @deprecated
void lcd_backlight_on(serial_lcd *lcd);

// @deprecated
void lcd_backlight_off(serial_lcd *lcd);

// @deprecated
void lcd_toggle_backlight(serial_lcd *lcd);

// @deprecated
void lcd_initialize(serial_lcd *lcd);

// @deprecated
void lcd_puts(serial_lcd *lcd, char *str);

// @deprecated
void lcd_gotoxy(serial_lcd *lcd, int x, int y) ;

// @deprecated
void lcd_clear(serial_lcd *lcd);

// @deprecated
void lcd_home(serial_lcd *lcd);
#endif
