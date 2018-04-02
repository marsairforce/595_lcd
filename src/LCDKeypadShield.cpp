
#include "LCD.h"
#include <Arduino.h>

LCDKeypadShield::LCDKeypadShield() {
    lcd = new Parallel_4bit_lcd(8, 9, 4, 5, 6, 7, 10);
    lcd->begin(16,2);
    lcd->backlight();
}

int LCDKeypadShield::readkeypad() {
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
