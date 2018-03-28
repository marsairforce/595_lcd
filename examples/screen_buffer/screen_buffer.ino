/*
   - 20x4 LCD display, 4 bit mode, driven by 74HC595 shift register

   Demonstration of the screen buffer class.
*/


// LCD Display on a 74HC595, 4 bit data mode
#define PIN_SER       5
#define PIN_SRCLK     6
#define PIN_RCLK      7

#include <serial_595_lcd.h>
serial_lcd lcd(PIN_SER, PIN_SRCLK, PIN_RCLK);

void setup() {
    lcd.on();
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

  lcd.clear();
  lcd.gotoxy(offset, 1);
  lcd.puts(msg);
  offset += dir;


  delay(500);

}
