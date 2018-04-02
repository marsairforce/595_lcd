/**
 * Demonstrates using a 1602-lcd-keypad-shield with our LCD library.
 * 
 * LCD Module | Arduino Pin
 * D7 (3)       7
 * D6 (2)       6
 * D5 (1)       5
 * D4 (0)       4
 * RS           8
 * E            9
 * (backlight?) 10
 */

#include "LCD.h"

Parallel_4bit_lcd lcd(8, 9, 4, 5, 6, 7,10);

void setup() {
    lcd.begin(16,2);
    lcd.setBacklight(1);
}

int i = 0;
void loop() {
    lcd.setCursor(0, 0);
    lcd.print("Hello World");
    lcd.setCursor(0, 1);
    lcd.print(i);
    delay(1000);
    i++;
}