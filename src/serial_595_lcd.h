
#ifndef _serial_595_lcd_h_
#define _serial_595_lcd_h_

#define LCD_E           0x10
#define LCD_RS          0x20
#define LCD_BACKLIGHT   0x40
#define LCD_POWER       0x80
#define LCD_DATA        0x0F


/**
 * This structure represents a LCD display
 * We store these together so we can have more than one display.
 */
struct serial_lcd {
    int pin_ser;        // the pin number on the Arduino that connects to SER (pin 14 of the 74HC595)
    int pin_srclk;      // the pin number on the Arduino that connects to the SRCLK (pin 11 on the 74HC595)
    int pin_rclk;       // the pin number on the Arduino that connects to the RCLK (pin 12 on the 74HC595)
    volatile int data;  // the byte value representing the pin state on the 74HC595
};

// Turns the LCD on. The power pin of the LCD is wired to an output of the 74HC595 in this case.
void lcd_on(serial_lcd *lcd);

// Turns off the LCD. Note here we need to turn off all the data and control inputs to the LCD,
// since the LCD will usually be able to derive power from the input pins.
void lcd_off(serial_lcd *lcd);

// Turn the backlight LED on
void lcd_backlight_on(serial_lcd *lcd);

// Turn the backlight LED off
void lcd_backlight_off(serial_lcd *lcd);

// Toggle the state of the backlight
void lcd_toggle_backlight(serial_lcd *lcd);

// Perform the initialization of the LCD
void lcd_initialize(serial_lcd *lcd);

// Write a string to the LCD at the current memory location.
void lcd_puts(serial_lcd *lcd, char *str);

// Move the cursor to the specified coordinate.
void lcd_gotoxy(serial_lcd *lcd, int x, int y) ;

// Clear the screen
void lcd_clear(serial_lcd *lcd);

// Move the cursor to the top left of the display.
void lcd_home(serial_lcd *lcd);


#ifndef _bit_macros_
#define _bit_macros_
// TODO: these macros do not really belong here, but we don't have a standard shift register libary yet

// macros for setting or clearing a bit or bit mask of bits.
static inline void set_bit(int *x, int mask) {
  *x |= mask;
}
static inline void clear_bit(int *x, int mask) {
  *x &= ~mask;
}
#endif

#endif
