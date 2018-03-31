# Serial 595 LCD Revisions

## 2.0
* Removed the v1 API.
* Created a general purpose abstract base LCD class. Allows for implementations of different kinds of hardware to use the same library features.
* Added support for Adafruit I2C LCD Backpack.
* Added support for PCF8574 I2C LCD Backack.
* Added support for parallel mode LCDs. But only ironically. Because this is a serial LCD library of course.
* Refactory out the need for bit macros library, favouring instead the union of struct of bit fields, which gives us a named reference to access the bit i a more convenient way that does not clutter the code.

## 1.1 ( 2018-03-25 )
* Define a screen buffer structure. Idea is to allow writes to this, and then have the LCD display this buffer. Makes it easier to have different logical displays, such as menus or modes of operation in your project.
* Start to define class member functions for serial_lcd structure. Eventually we should make this a nice C++ class probably. More thought is needed to determine if there is ever any use case for having a C style function and pointer to structure type of interface.
* depends on now separate bit_macros library. https://github.com/marsairforce/bit_macros

## 1.0 (2018-03-18)
* Created the library by extracting functions from a project.
* Set up the GitHub project.
* Rename the project because did not know we could not start it with a number.
