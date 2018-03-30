/**
 * Demonstrates using a an I2C LCD backpack
 * 
 * LCD Module | Arduino Pin
 * DATA          A4 / SDA
 * CLOCK         A5 / SCL
 */

#include "LCD.h"
#include "Wire.h"
// Connect via i2c, default address #0 (A0-A2 not jumpered)
Adafruit_I2c_lcd lcd(0);

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
