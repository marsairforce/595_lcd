/*
   - 20x4 LCD display, 4 bit mode, driven by 74HC595 shift register
*/


// LCD Display on a 74HC595, 4 bit data mode
#define PIN_SER       5
#define PIN_SRCLK     6
#define PIN_RCLK      7

#include <serial_595_lcd.h>

serial_lcd lcd;

void setup() {

  pinMode(PIN_SER,   OUTPUT);
  pinMode(PIN_SRCLK, OUTPUT);
  pinMode(PIN_RCLK,  OUTPUT);

  lcd.pin_ser   = PIN_SER;
  lcd.pin_srclk = PIN_SRCLK;
  lcd.pin_rclk  = PIN_RCLK;
  lcd_on(&lcd);
}

int offset = 0;
int dir = +1;
void loop() {

  char msg[] = "-= Hello =-";
  if (offset + strlen(msg) + 1 > 20) {
    dir = -1;
  }
  if (offset -1 < 0) {
    dir = +1;
  }
  lcd_clear(&lcd);
  lcd_gotoxy(&lcd, offset, 1);
  lcd_puts(&lcd, msg);
  offset += dir;
  
  delay(500);

}
