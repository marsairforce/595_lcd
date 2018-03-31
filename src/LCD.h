
#ifndef _LCD_h_
#define _LCD_h_

#include "Arduino.h"
#include <inttypes.h>
#include "Adafruit_MCP23008.h"




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
struct lcd_screen_buffer {
  char buf[ LCD_DRAM_MAX_COLUMN * LCD_DRAM_MAX_ROW + 1]; // +1 for the null terminator
  uint8_t cursor; // the index into the DRAM buffer where the cursor is

  // bit fields
  uint8_t cursor_visible : 1;
  uint8_t cursor_blink : 1;
public:
  void clear_screen();
  void puts(char *str);
  void set_dram_address(unsigned char address);
};


#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

/**
 * A class compatible with the Arduino LiquidCrystal library interface.
 * Specific implementations (I2C, 595 SPI, Parallel) can extend this.
 */
class HD44780_LCD {
  protected:
    uint8_t _m_cols = 20;
    uint8_t _m_rows = 4;

  public:
  virtual void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);

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
  virtual void setBacklight(uint8_t status) = 0;
  void createChar(uint8_t, uint8_t[]);
  virtual size_t write(uint8_t) = 0;
  void setCursor(uint8_t, uint8_t);

  // sends a control command to the LCD
  void command(uint8_t);

  private:
  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;
  uint8_t _numrows;
  uint8_t _currrow;

  // This is going to be the interface specific method to communicate to the LCD
  virtual void send(uint8_t value, boolean mode) = 0;
  virtual void write4bits(uint8_t);

};


class Parallel_4bit_lcd : public HD44780_LCD, public Print {
  public:
  Parallel_4bit_lcd(uint8_t rs, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t backlight = 0);
  virtual void setBacklight(uint8_t status);
  virtual size_t write(uint8_t); // for Print class

  private:
  uint8_t _rs_pin; // LOW: command.  HIGH: character.
  uint8_t _enable_pin; // activated by a HIGH pulse.
  uint8_t _data_pins[4];
  uint8_t _backlight_pin;

  virtual void send(uint8_t value, boolean mode);
  virtual void write4bits(uint8_t);
  void pulseEnable();
};


class Adafruit_I2C_lcd : public HD44780_LCD, public Print {
  public:
  Adafruit_I2C_lcd(uint8_t i2cAddr);
  virtual void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);
  virtual void setBacklight(uint8_t status);
  virtual size_t write(uint8_t); // for Print class

  private:
  uint8_t _i2cAddr;
  Adafruit_MCP23008 _i2c;

  virtual void send(uint8_t value, boolean mode);
  virtual void write4bits(uint8_t);
};


class PCF8574_lcd : public HD44780_LCD, public Print {
  public:
  PCF8574_lcd(uint8_t addr);
  virtual void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);
  virtual void setBacklight(uint8_t status);
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

  // The raw write the state to the i2c to parallel output port.
  void write2Wire();
};

/**
 * This structure represents a LCD display as it is connected behind a 74HC595 shift register.
 * Fields then store the specific digital IO pin numbers, a byte register for the value on the shift register.
 */
class Serial_595_lcd : public HD44780_LCD, public Print {
  public:

    Serial_595_lcd(uint8_t pin_ser, uint8_t pin_srclk, uint8_t pin_rclk);
    virtual void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);
    virtual void setBacklight(uint8_t status);
    virtual size_t write(uint8_t); // for Print class

    void set_e();
    void clear_e();
    void toggle_e();

    /**
    * writes 4 bits. Value in loser 4 bits.
    * @Deprecated, should be write4bits
    */
    //void write_nibble(uint8_t data);

    // this is now send()
    //void write_byte(uint8_t RS, uint8_t data);

    void reset_sequence();


    /**
     * Perform the initialization of the LCD following power on state.
     * See the data sheets. There is a few magic bytes of chatter we need to do, and a timing sequence that is also magic.
     */
    void initialize();

    /**
     * Set DDRAM address in address counter
     * this function takes 37 us
     */
    void set_dram_address(unsigned char address);

    /**
     * Turns the LCD on. The power pin of the LCD is wired to an output of the 74HC595 in this case.
     * Also turns on the backlight.
     * And then initializes the display.
     */
    void on();

    /**
     * Turns off the LCD. Note here we need to turn off all the data and control inputs to the LCD,
     * since the LCD will usually be able to derive power from the input pins.
     */
    void off();

    void power_on();
    void power_off();
    void backlight_on();
    void backlight_off();
    void toggle_backlight();


    void gotoxy(unsigned char x, unsigned char y);

    /**
     * Write a string to the LCD at the current memory location.
     */
    void puts(char *str);

    /**
     * Causes the contents of the screen to be displayed onto the LCD.
     * This will clear the entire contents if any on the LCD,
     * and update the cursor position, cursor visible, and cursor blink, as to what ever they were in the screen structure.
     */
    void display_screen(lcd_screen_buffer *screen);

  private:
    uint8_t _m_ser;        // the pin number on the Arduino that connects to SER (pin 14 of the 74HC595)
    uint8_t _m_srclk;      // the pin number on the Arduino that connects to the SRCLK (pin 11 on the 74HC595)
    uint8_t _m_rclk;       // the pin number on the Arduino that connects to the RCLK (pin 12 on the 74HC595)

    // the byte value representing the pin state on the 74HC595
    // mapped into the fields LSB first
    union {
      struct {
        uint8_t data:4;       // 0x0F D0..D3 to the LCD
        uint8_t e:1;          // 0x10
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
