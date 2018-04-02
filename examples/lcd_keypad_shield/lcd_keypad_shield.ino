/**
 * Demonstrates using a 1602-lcd-keypad-shield with our LCD library.
 */

#include "LCD.h"

LCDKeypadShield s;

void setup() {
}

// Just for ease of displaying
const char *button[7] ={"Right ", "Up ", "Down ", "Left ", "Select ", "       ", "??????"};

void loop() {
    s.lcd->setCursor(0, 0);
    s.lcd->print("Button Pressed:");
    s.lcd->setCursor(0, 1);
    s.lcd->print(button[s.readkeypad()]);
    delay(10);
}