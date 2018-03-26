# Serial 595 LCD Revisions

## 1.1 ( in progres )
* Define a screen buffer structure. Idea is to allow writes to this, and then have the LCD display this buffer. Makes it easier to have different logical displays, such as menus or modes of operation in your project.
* Start to define class member functions for serial_lcd structure. Eventually we should make this a nice C++ class probably. More thought is needed to determine if there is ever any use case for having a C style function and pointer to structure type of interface.
* depends on now separate bit_macros library.

## 1.0 (2018-03-18)
* Created the library by extracting functions from a project.
* Set up the GitHub project.
* Rename the project because did not know we could not start it with a number.
