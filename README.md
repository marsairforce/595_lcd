# LCD driven by a 74HC595

This was inspired by those I2C or SPI LCD backpacks from Adafruit.

![schematic](doc/v1_schematic.png)

Only here, well, we just use a 74HC595.

The library requires (3) digital IO pins on your Arduino.

There are some assumptions for how the LCD is connected to the 74HC595, see the
schematic in the doc folder.

For more information about this library please visit us at
https://github.com/marsairforce/595_lcd

# General Serial LCD Operation
After building a serial to parallel converter, and then owning a couple different I2C kinds:
* Adafruit LCD backpack, driven by MCP23008: [Adafruit_LiquidCrystal](https://github.com/adafruit/Adafruit_LiquidCrystal)
  * This extends the Arduino community [LiquidCrystal library](https://github.com/arduino-libraries/LiquidCrystal/blob/master/src/LiquidCrystal.h) ([doc](https://www.arduino.cc/en/Reference/LiquidCrystal))
  * Kind of not sure why they did not just subclass the LiquidCrystal one. They just added a couple functions. But left the parallel operation things, which are never going to be used in this mode.
* Generic LCD backpack, driven by PCF8574: "I2C LCD1602/2004 Adapter Plate" from Ali Express.
  * Another library here: [LiquidCrystal_PCF8574](https://github.com/mathertel/LiquidCrystal_PCF8574)

* (and also my own library evolving here)

I started to have the general theory, that I would like the same software interface (using a display buffer), but to support the specific lcd backend technology.

This facilitated extracting an interface of the functions that exist in each library and making a common parent class, that is implemented by each specific library. If there are any specific messaging needed that is.

I chose to not subclass the Arduino LiquidCrystal class, since it has a bunch of fields that only make sense if you are parallel mode.  I guess really for completeness I should define a class to support 4 bit parallel mode here as well.


# 74HC595 LCD Interface
# Theory
* Using the 4 bit mode of the standard HD44780 type LCD controller.
* RS pin
* E pin
* Variable resistor to adjust contrast.
* Backlight control pin. Optional variable resistor to adjust brightness.
* Power pin

Which is a lot less pins than just using the pins on the device.

We can use software to turn the display off entirely.

There is an initialization chatter that we need to do to set up the display.

This hardware is write-only. We can not read from the LCD in this configuration.


# Operation

All we want to do here is write bits to the LCD.

Where we do this, according to the data sheet,
* Load the value of the data onto the data pins.
* LCD_E is LOW by default. We strobe it to HIGH to make read/write operations. The effect is applied on the *falling* edge of E signal.
* LCD RS is LOW for command operation, HIGH for data operation.

![lcd write sequence](doc/lcd_write_timing.png)

(Wavedrom source for image above)
```
{signal: [
  {name: 'RS', wave: 'x2...x'},
  {name: 'R/W', wave: '20...2'},
  {name: 'E', wave: '0.1.0.'},
  {name: 'DB7-0', wave: '2..2.2', data: ['','Valid Data']}
]}
```

Where for 4 bit operation,
* Load the upper nibble of the data or command byte to the [D4..D7] pins.
* Strobe `E` pin.
* Load the lower nibble of the data or command byte to the [D4..D7] pins.
* Strobe 'E' pin.


## Pin mapping

| 74HC595  | Display  |
|----------|:---------|
| Q7 | D4 |
| Q6 | D5 |
| Q5 | D6 |
| Q4 | D7 |
| Q3 | E |
| Q2 | RS |
| Q1 | Backlight |
| Q0 | LCD VDD |

This works out well, so that we can use shift out with the least significant bit first,
```
shiftOut(lcd->pin_ser, lcd->pin_srclk, LSBFIRST, lcd->data);
```
so in our software if we wanted to write, e.g. a `0x03`, it will be the value on [D4..D7] as it should be.

However, because the data pins are the last bits (higher number) in the shift register,
and the control pins are also in this 8 bit register, the lower down, the data when we view the waveform appears backwards.
![timing diagram](doc/signal_timings.png)

Where we can see in the frame ther, the `3B` and then '3C' value, corresponds to
the activity of writing the value `0x0C` to the display, where we have the `E` pin first high,
and then low.

The timing diagram above only shows one nibble, or half of a character or command sequence.

### A Use case: Clearing the screen

Lets have a look at the operations that are involved by a single command.

![clear screen timing](doc/clear_screen_command.png)

The library provides  a function to clear the display, `void lcd_clear(serial_lcd *lcd)`:
```
void lcd_clear(serial_lcd *lcd) {
  lcd_write(lcd, 0, 0x01);
  delay(2);
}
```

Where lcd_write invokes two nibble write operations. But we have refactored the lcd_write_nibble to remove one cycle of loading the shift register.
So this is now done in 5 cycles. The original was 8.


(Where this low level port write is just digitalWrite and shiftOut opertions):
```
void lcd_port_write(serial_lcd *lcd) {
  shiftOut(lcd->pin_ser, lcd->pin_srclk, LSBFIRST, lcd->data);
  digitalWrite(lcd->pin_rclk, HIGH);
  digitalWrite(lcd->pin_rclk, LOW);
}
```

### A note about 8 bit parallel mode
Here we are using the 4 bit parallel mode, because it requires fewer pins from the shift register. We require at least of 2 control pins, so if we used 8 bit data mode we
would need 2 shift registers to get at least 10 bits.

But when we operate the display in 8 bit mode we just send out the value on the 8 pins and strobe `E` once. So there is perhaps a compelling reason to use 8 bit mode if we wanted to save some timing requirements.

Measurement in my implementation, the time required to write one nibble is about 275 microseconds. So an entire command byte is then the order of 550 microseconds.
In my application this is fine. The LCD display has a hysteresis effect, so updating or moving characters faster appear to be blurry anyway.

# Software Features
I created a structure to hold the state for a given LCD
```
struct serial_lcd {
    int pin_ser;        // the pin number on the Arduino that connects to SER (pin 14 of the 74HC595)
    int pin_srclk;      // the pin number on the Arduino that connects to the SRCLK (pin 11 on the 74HC595)
    int pin_rclk;       // the pin number on the Arduino that connects to the RCLK (pin 12 on the 74HC595)
    volatile int data;  // the byte value representing the pin state on the 74HC595
};
```

This allows us to have more than one LCD attached to the Arduino and in use at the same time.

This does require initialization at start up. Where we need to define an instance of this and set the properties for pin_ser, pin_srclk, and pin_rclk before we can use this.

Then all the functions (see ser_595_lcd.h) require a pointer to this serial_lcd structure.

The data property should generally not be mutated by your application. This represents the state that is written on the pins of the 74HC595. The functions update various bits as required to manipulate the control pins (like `E` and `RS`) as required. They also contain the bit mapping for the power, backlight, and the [D4..D7] data bus pins into the display.

See the table above.

All functions in the libary have the sensible prefix `lcd_`. Hopefully to not collide with functions in your Sketch or other Arduino libraries that happen to be in use.

There is no reason you could not also use the Adafruit LCD library at the same time, for example.

## Example Sketch
Have a look in the examples folder for sample use of the library.

* display_text : Prints "Hello" and moves the text back and forth on the screen.  Here I have a 20x4 LCD, so I assume there are 20 visible columns on the display.

# Hardware

A Kicad project for a circuit board to hold the 74HC595 shift register and the trimmer resistors is in the `backpack` sub folder here.

<img src="doc/v1_board_3d_model.png" alt="3D model" width="400"/>
<img src="doc/v1_board_layout.png" alt="board layout" width="200"/>

My board design is on OSHPark here: https://oshpark.com/projects/P1p8G9Nj


# Why
I have and appreciate the I2C LCD backpack. But sometimes I am playing with ATTiny devices,
or something else that does ot have an I2C. I guess I could use the SPI mode on that.

But then sometimes I don't feel like spending $10 USD, which when converted to CDN, plus taxes and shipping
usually comes out to about double.

There are a lot of low price LCD backpacks on Amazon, or AliExpress.
But then I am impatient and don't have them at the moment when I feel like wiring something up on a breadboard,
and it turns out I do have a bunch of 74HC595's. So there is that.

I also thought it is a good learning experience to build something yourself sometimes.

# Other hardwares
I felt like making this library work for the other types of serial LCD backpacks I had, so I didn't need to have a bunch of libraries to maintain.

# PCF8574
You can get these on Ali Epress in a bunch of them in a lot for a couple dollars a piece. Really it is cheaper than me buying even a 74HC595 device. I don't know how they do it.

![PCF8574 pinout](doc/PCF8574T.png)

Mapping to the LCD pins

| PCF8574T | LCD |
| :------- | :-- |
| P0 | RS |
| P1 | RW |
| P2 | E  |
| P3 | BACKLIGHT |
| P4 | D4 (D0) |
| P5 | D5 (D1) |
| P6 | D6 (D2) |
| P7 | D7 (D3) |