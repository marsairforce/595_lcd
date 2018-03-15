# LCD driven by a 74HC595

* Using the 4 bit mode of the standard HD44780 type LCD controller.

![schematic](doc/v1_schematic.png)
# Theory
* 4 data pins
* RS pin
* E pin
* Variable resistor to adjust contrast.
* Backlight control pin. Optional variable resistor to adjust brightness.
* Power pin

Which is a lot less pins than just using the pins on the device.

We can use software to turn the display off entirely.

There is an initialization chatter that we need to do to set up the display.

This hardware is write-only. We can not read from the LCD in this configuration.


# Why
I have and appreciate the I2C LCD backpack. But sometimes I am playing with ATTiny devices,
or something else that does ot have an I2C. I guess I could use the SPI mode on that.

But then sometimes I don't feel like spending $10 USD, which when converted to CDN, plus taxes and shipping
usually comes out to about double.

There are a lot of low price LCD backpacks on Amazon, or AliExpress.
But then I am impatient and don't have them at the moment when I feel like wiring something up on a breadboard,
and it turns out I do have a bunch of 74HC595's. So there is that.

I also thought it is a good learning experience to build something yourself sometimes.
