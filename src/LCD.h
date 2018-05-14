
#ifndef _LCD_h_
#define _LCD_h_

#include "Arduino.h"
#include <inttypes.h>
#include "Adafruit_MCP23008.h"



#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

/**
 * A class compatible with the Arduino LiquidCrystal library interface.
 * Specific implementations (I2C, 595 SPI, Parallel) can extend this.
 */
class HD44780_LCD : public Print {
  public:
  virtual void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);

  // performs the reset of the display after power on. Called from begin and powerOn();
  void initialize();
  void clear();
  void home();

  void noDisplay();
  void display();
  void noBlink();
  void blink();
  void noCursor();
  void cursor();
  void scrollDisplayLeft();
  void scrollDisplayRight();
  void leftToRight();
  void rightToLeft();
  void autoscroll();
  void noAutoscroll();
  void backlight();
  void noBacklight();
  void toggleBacklight() {
    if (isBacklight()) {
        noBacklight();
    }
    else {
        backlight();
    }
  }
  // I guess this was created to support variable levels of backlight from software.
  // Currently none of the hardware we have can do this.
  virtual boolean isBacklight() = 0;
  virtual void setBacklight(uint8_t status) = 0;
  void createChar(uint8_t, uint8_t[]);
  virtual size_t write(uint8_t) = 0;
  void setCursor(uint8_t, uint8_t);
  uint8_t getCols() { return _cols; }
  uint8_t getRows() { return _rows; }
  void command(uint8_t);
  private:
  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;
  uint8_t _cols = 20;
  uint8_t _rows = 4;

  // This is going to be the interface specific method to communicate to the LCD
  virtual void send(uint8_t value, boolean mode) = 0;
  virtual void write4bits(uint8_t);
};

// The DRAM in the display is actually 40 characters wide and 2 lines
// See also pg 34: http://www.farnell.com/datasheets/50586.pdf
// we use this constant to initialize memory for screen buffering
#define LCD_DRAM_MAX_COLUMN  40
#define LCD_DRAM_MAX_ROW     2


// bit masks for flags in the screen buffer structure
#define SCREEN_BUFFER_FLAG_CURSOR_VISIBLE 0x02
#define SCREEN_BUFFER_FLAG_CURSOR_BLINK   0x01

/**
 * A model for the contents and visible state of a LCD screen.
 * This allows for our application to preserve the state of the screen, such as when we power down the LCD,
 * or to have more than one logical screen.
 * Note we need to perform the operation of doing all our writes to this structure, and then
 * invoke the LCD class functions to transpose these to the LCD display. We are not able to read from the LCD at this time.
 * due to the hardware interface being write only.
 */
class lcd_screen_buffer : public Print {
  public:
  lcd_screen_buffer(HD44780_LCD *lcd); // use screen buffer the same size as the LCD
  ~lcd_screen_buffer();
  void begin(uint8_t cols = 0, uint8_t rows = 0);  // optionally specify our own virtual screen size. If zero, then use the LCD display size.

  void clear();
  // will write character into the current virtual cursor position into the virtual buffer.
  virtual size_t write(uint8_t);
  void setCursor(uint8_t col, uint8_t row) {
    _vcursor_x = col;
    _vcursor_y = row;
  }

  // the position of the cursor on the display itself.
  // void setHCursor(uint8_t col, uint8_t row) {
  //   _cursor_x = col;
  //   _cursor_y = row;
  // }
  // uint8_t getHCursorX() { return _cursor_x; }
  // uint8_t getHCursorY() { return _cursor_y; }

  // sets the offset of the virtual buffer to be presented to the hardware viewport
  // void setWindow(uint8_t col, uint8_t row) {
  //   _win_x = col;
  //   _win_y = row;
  // }
  // uint8_t getWindowX() { return _win_x; }
  // uint8_t getWindowY() { return _win_y; }

  // the state we should set the hardware cursor on the device.
  void noBlink() { _cursor_blink = 0; }
  void blink() { _cursor_blink = 1; }
  void noCursor() { _cursor_visible = 0; }
  void cursor() { _cursor_visible = 1; }

  void writeToLcd();

  private:
  HD44780_LCD *_lcd;
  char _buffer[LCD_DRAM_MAX_COLUMN * LCD_DRAM_MAX_ROW]; // +1 for the null terminator
  const uint8_t _size = LCD_DRAM_MAX_COLUMN * LCD_DRAM_MAX_ROW ;
  const uint8_t _cols = 20; // the virtual viewport columns
  const uint8_t _rows = 4;  // the virtual viewport rows

  // the virtual viewport coordinates that map to the physical screen.
  // uint8_t _win_x;
  // uint8_t _win_y;

  // uint8_t _cursor_x;
  // uint8_t _cursor_y;
  //uint8_t _cursor; // the index into the DRAM buffer where the cursor is

  // the current position for character entry into the virtual buffer space.
  uint8_t _vcursor_x;
  uint8_t _vcursor_y;

  // bit fields
  uint8_t _cursor_visible : 1;
  uint8_t _cursor_blink : 1;

  //void printToBuffer(const uint8_t x, const uint8_t y, const char *s);
};



class Parallel_4bit_lcd : public HD44780_LCD {
  public:
  Parallel_4bit_lcd(uint8_t rs, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t backlight = 0);
  virtual void setBacklight(uint8_t status);
  virtual boolean isBacklight();
  virtual size_t write(uint8_t); // for Print class

  private:
  uint8_t _rs_pin; // LOW: command.  HIGH: character.
  uint8_t _enable_pin; // activated by a HIGH pulse.
  uint8_t _data_pins[4];
  uint8_t _backlight_pin;

  union {
    struct {
      uint8_t backlight:1;
    } field;
    uint8_t raw;
  } _data;

  virtual void send(uint8_t value, boolean mode);
  virtual void write4bits(uint8_t);
  void pulseEnable();
};


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

    int readkeypad();
    LCDKeypadShield();
};

class Adafruit_I2C_lcd : public HD44780_LCD {
  public:
  Adafruit_I2C_lcd(uint8_t i2cAddr);
  virtual void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);
  virtual void setBacklight(uint8_t status);
  virtual boolean isBacklight();
  virtual size_t write(uint8_t); // for Print class

  private:
  uint8_t _i2cAddr;
  Adafruit_MCP23008 _i2c;
  union {
    struct {
      uint8_t backlight:1;
    } field;
    uint8_t raw;
  } _data;

  virtual void send(uint8_t value, boolean mode);
  virtual void write4bits(uint8_t);
};

//#define ADAFRUIT_SPI_USE_FIELD_STRUCT true
class Adafruit_SPI_lcd : public HD44780_LCD {
  public:
  Adafruit_SPI_lcd(uint8_t data, uint8_t clock, uint8_t latch);
  virtual void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);
  virtual void setBacklight(uint8_t status);
  virtual boolean isBacklight();
  virtual size_t write(uint8_t); // for Print class

  private:
  virtual void send(uint8_t value, boolean mode);
  virtual void write4bits(uint8_t);

  uint8_t _data_pin;
  uint8_t _clock_pin;
  uint8_t _latch_pin;

  // the SPI expander pinout on the Adafruit LCD backpack
  // _rs_pin = 1;
  // _rw_pin = 255;
  // _enable_pin = 2;
  // _data_pins[0] = 6;  // really d4
  // _data_pins[1] = 5;  // really d5
  // _data_pins[2] = 4;  // really d6
  // _data_pins[3] = 3;  // really d7

  // // Fields map to the SPI expander pinout, LSB first
  // We can't conveniently map to a struct here, because I think the data pins are backwards.
  // And they serialize MSBFIRST.

#ifdef ADAFRUIT_SPI_USE_FIELD_STRUCT
  union {
     struct {
       uint8_t unused:1;      // P0
       uint8_t rs:1;          // P1
       uint8_t en:1;          // P2
       uint8_t data:4;        // P3..P6
       uint8_t backlight:1;   // P7
     } field;
     uint8_t raw;
   } _data;
   void port_write();
#else
  uint8_t _SPIbuff;
  uint8_t _rs_pin;
  uint8_t _enable_pin;
  uint8_t _data_pins[4];
  void _digitalWrite(uint8_t p, uint8_t d);
  void pulseEnable();
#endif
};

class PCF8574_lcd : public HD44780_LCD {
  public:
  PCF8574_lcd(uint8_t addr);
  virtual void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);
  virtual void setBacklight(uint8_t status);
  virtual boolean isBacklight();
  virtual size_t write(uint8_t); // for Print class

  private:
  uint8_t _addr;

  // bit fields and flags for the state of the parallel port data on the PCF8574 as it is connected to the LCD.
  // your device might be built differently. Check your datasheet.
  // Define the LSB first
  union {
    struct {
      uint8_t rs:1;         // P0
      uint8_t rw:1;         // P1
      uint8_t en:1;         // P2
      uint8_t backlight:1;  // P3
      uint8_t data:4;       // P4..P7
    } field;
    uint8_t raw;
  } _data;

  virtual void send(uint8_t value, boolean mode);
  virtual void write4bits(uint8_t);
  void port_write();
};

/**
 * This structure represents a LCD display as it is connected behind a 74HC595 shift register.
 * Fields then store the specific digital IO pin numbers, a byte register for the value on the shift register.
 */
class Serial_595_lcd : public HD44780_LCD {
  public:

    Serial_595_lcd(uint8_t pin_ser, uint8_t pin_srclk, uint8_t pin_rclk);
    virtual void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);
    virtual void setBacklight(uint8_t status);
    virtual boolean isBacklight() {   return _data.field.backlight; }
    virtual size_t write(uint8_t); // for Print class

    /**
     * Turns the LCD on. The power pin of the LCD is wired to an output of the 74HC595 in this case.
     * Also turns on the backlight.
     * And then initializes the display.
     */
    void powerOn();

    /**
     * Turns off the LCD. Note here we need to turn off all the data and control inputs to the LCD,
     * since the LCD will usually be able to derive power from the input pins.
     */
    void powerOff();

    boolean isOn() { return _data.field.power; }

    void togglePower() {
      if (isOn()) {
        powerOff();
      }
      else {
        powerOn();
      }
    }

    /**
     * Perform the initialization of the LCD following power on state.
     * See the data sheets. There is a few magic bytes of chatter we need to do, and a timing sequence that is also magic.
     */
     // this is just begin()
    //void initialize();
    //void reset_sequence();
    //void set_e();
    //void clear_e();
    //void toggle_e();

    /**
     * Set DDRAM address in address counter
     * this function takes 37 us
     */
    //void set_dram_address(unsigned char address);

//    void backlight_on();
//    void backlight_off();
//    void toggle_backlight();


    //this is just setCursor()
    //void gotoxy(unsigned char x, unsigned char y);

    // this is just print()
    //void puts(char *str);

  private:
    uint8_t _m_ser;        // the pin number on the Arduino that connects to SER (pin 14 of the 74HC595)
    uint8_t _m_srclk;      // the pin number on the Arduino that connects to the SRCLK (pin 11 on the 74HC595)
    uint8_t _m_rclk;       // the pin number on the Arduino that connects to the RCLK (pin 12 on the 74HC595)

    // the byte value representing the pin state on the 74HC595
    // mapped into the fields LSB first
    union {
      struct {
        uint8_t data:4;       // 0x0F D0..D3 to the LCD
        uint8_t en:1;          // 0x10
        uint8_t rs:1;         // 0x20
        uint8_t backlight:1;  // 0x40
        uint8_t power:1;      // 0x80
      } field;
      uint8_t raw;
    } _data;

    virtual void send(uint8_t value, boolean mode);
    virtual void write4bits(uint8_t);

    /**
    * Performs the signalling required to write to a LCD port
    * Write the raw data to the LCD port shift register.
    */
    void port_write();

};

#endif
