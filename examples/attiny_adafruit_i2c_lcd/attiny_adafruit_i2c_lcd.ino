/**
 * Demonstrates using the Adafruit I2C LCD backpack Based on the MCP23008
 * But driven by an ATTiny85, where we use the USIWire instead.
 * According to the USI_TWI_Master.h in the USIWire library, We use pin B0 as the SDA and B2 as SCL
    #define PORT_USI_SDA PORTB0
    #define PORT_USI_SCL PORTB2
 * The circuit:
 * 5V to Arduino 5V pin
 * GND to Arduino GND pin
 * CLK to Analog #5 / SCL
 * DAT to Analog #4 / SDA
 */

// magically defining this causes the USIWire.h to be used instead of Wire.h
#define __AVR_ATtiny85__

#include "LCD.h"
// Connect via i2c, default address #0 (A0-A2 not jumpered)
Adafruit_I2C_lcd lcd(0);

void setup() {
  // set up the LCD's number of rows and columns:
  lcd.begin(16, 2);
  lcd.clear();
  // Print a message to the LCD.
  lcd.print("hello, world!");
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis()/1000);
  lcd.setBacklight(HIGH);
  delay(500);
  lcd.setBacklight(LOW);
  delay(500);
}
