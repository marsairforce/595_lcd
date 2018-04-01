/**
 * Demonstrates using a 1602-lcd-keypad-shield with our LCD library.
 */

#include "LCD.h"


/* http://www.thomasclausen.net/en/walking-through-the-1602-lcd-keypad-shield-for-arduino/
 *
 * LCD Module | Arduino Pin
 * D7 (3)       7
 * D6 (2)       6
 * D5 (1)       5
 * D4 (0)       4
 * RS           8
 * E            9
 * (backlight?) 10
 *
 * A0 : buttons input.
 */
#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5
#define btnUNKNOWN 6

class LCDKeypadShield {

    public:
    HD44780_LCD *lcd;

    int readkeypad() {
        int adc_key_in = analogRead(0);
        int ret = btnUNKNOWN;

        if (adc_key_in < 50) ret = btnRIGHT;
        if ((adc_key_in > 500) && (adc_key_in < 1150)) ret = btnNONE;
        if ( (adc_key_in > 120) && (adc_key_in < 150) ) ret = btnUP;
        if ( (adc_key_in > 250) && (adc_key_in < 350) ) ret = btnDOWN;
        if ( (adc_key_in > 450) && (adc_key_in < 550) ) ret = btnLEFT;
        if ( (adc_key_in > 700) && (adc_key_in < 750) ) ret = btnSELECT;
        return ret;
    }

    LCDKeypadShield() {
        lcd = new Parallel_4bit_lcd(8, 9, 4, 5, 6, 7, 10);
        lcd->begin(16,2);
        lcd->backlight();
    }
};

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