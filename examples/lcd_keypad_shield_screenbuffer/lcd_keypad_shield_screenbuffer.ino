/**
 * Demonstrates using a 1602-lcd-keypad-shield with our LCD library.
 * Here we write things to a screen buffer, and then write the screen buffer to the LCD.
 */

#include "LCD.h"

LCDKeypadShield s;
lcd_screen_buffer b(s.lcd);

void setup() {
    b.begin();
}

// Just for ease of displaying
const char *button[7] ={"Right ", "Up ", "Down ", "Left ", "Select ", "       ", "??????"};

void loop() {
    b.setCursor(0,0);
    b.print("Button Pressed:");
    b.setCursor(0, 1);
    b.print(button[s.readkeypad()]);
    b.setHCursor(12, 1);
    b.cursor();
    b.blink();
    b.writeToLcd();
    delay(10);
}