
#ifndef _serial_595_lcd_h_
#define _serial_595_lcd_h_

#ifndef byte
typedef unsigned char byte;
#endif

#define LCD_E           0x10
#define LCD_RS          0x20
#define LCD_BACKLIGHT   0x40
#define LCD_POWER       0x80
#define LCD_DATA        0x0F

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
  byte cursor; // the index into the DRAM buffer where the cursor is

  // bit fields
  byte cursor_visible : 1;
  byte cursor_blink : 1;
public:
  void clear_screen();
};

/**
 * This structure represents a LCD display as it is connected behind a 74HC595 shift register.
 * Fields then store the specific digital IO pin numbers, a byte register for the value on the shift register.
 * Functions are for doing all the IO work to drive the LCD.
 */
struct serial_lcd {
  private:
    int _m_ser;        // the pin number on the Arduino that connects to SER (pin 14 of the 74HC595)
    int _m_srclk;      // the pin number on the Arduino that connects to the SRCLK (pin 11 on the 74HC595)
    int _m_rclk;       // the pin number on the Arduino that connects to the RCLK (pin 12 on the 74HC595)
    volatile unsigned int _m_data;  // the byte value representing the pin state on the 74HC595

    /**
    * Performs the signalling required to write to a LCD port
    * Write the raw data to the LCD port shift register.
    */
    void port_write();
    void set_e();
    void clear_e();
    void toggle_e();
    /**
    * writes 4 bits. Value in loser 4 bits.
    */
    void write_nibble(unsigned char data);

    void write_byte(unsigned char RS, unsigned char data);

    void reset_sequence();


  public:

    serial_lcd(); // TODO: remove this once we remove v1 api
    serial_lcd(int pin_ser, int pin_srclk, int pin_rclk);

    /**
     * Perform the initialization of the LCD following power on state.
     * See the data sheets. There is a few magic bytes of chatter we need to do, and a timing sequence that is also magic.
     */
    void initialize();

    /**
     * Set DDRAM address in address counter
     * this function takes 37 us
     */
    void set_dram_address(int address);

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

    /**
     * Turn the backlight LED on
     */
    void backlight_on();

    /**
     * Turn the backlight LED off
     */
    void backlight_off();

    /**
     * Toggle the state of the backlight
     */
    void toggle_backlight();

    /**
     * Set DDRAM address to "00H" from AC and return cursor to its original position if shifted.
     * The contents of DDRAM are not changed.
     * This function takes 1.52ms
     */
    void home();

    /**
     * The devices provide a 7 bit DRAM address
     * Internally there are 40 character columns, even if the display does not physically have this many columns
     * The row offsets are not consecutive starting points.
     * e.g. for 40 characters, this only uses 0x00-0x27 (0-39)for the first line
     * but the second line in the dram address starts at 0x40-0x67
     * what about those 25 (0x19) bytes from 0x28-0x39?
     *
     * To make it more complicated, 4 line display modules that have less than 40 characters per line
     * e.g. like these 20x4 modles I have http://www.newhavendisplay.com/specs/NHD-0420DZ-NSW-BBW.pdf
     * use the not visible DRAM address positions from line 1 are mapped to display line 3.
     * I guess this means we can not do the scroll display
     *
     * The set dram address command is 0x80, plus the memory offset.
     * so we want to convert the input of (x,y) to the linear value
     * that satisfies this mess.
     */
    void gotoxy(int x, int y);

    /**
     * Write a string to the LCD at the current memory location.
     */
    void puts(char *str);

    /**
     * Clear the screen
     */
    void clear();

    /**
     * Causes the contents of the screen to be displayed onto the LCD.
     * This will clear the entire contents if any on the LCD,
     * and update the cursor position, cursor visible, and cursor blink, as to what ever they were in the screen structure.
     */
    void display_screen(lcd_screen_buffer *screen);
};


#endif
