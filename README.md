2019 Aug 17   cheungbx

Many people have discussed and shared videos running Arduboy games on ESP8266. However, I have not seen any source codes shared by anyone for games running on  a ESP8266 with working Aruboy2 library that support push buttons. Hence I try to make one myself and share it.
Here is my youtube video that demonstrate the games that are working so far.

https://youtu.be/CinJb7gifH8


ESP8266 has pros and cons as a portable game console compared to the original Arduboy using ATmega32U4 (Pro Micro).

con
===
- heavy power consumption, will only run for 1-2 hours max on a tiny LIPO battery.
- limited GPIO pins hence cannot support other features in the original Arduboy, e.g. RGB LED, two audio output pins.
- due to  the above, restricted to using I2C for OLED display which may slow down some complex games
- sound support is minimal, only through the beeping function.
- support on background music and other sound related library that require timer interrupts is a challenge. I have not see a working library that support this yet. 

Pro
===
+ faster CPU 80Mh/160Mhz may compensate for some of the slowless mentioned above except for the limitation of I2C speed.
+ A big memory. Comes with 4MB, and can be upgraded  to 16MB using the same memory chip that @Mr.Blinky used for the flash cart.   This allow for more complex games, and also storing of games on the memory (there is a file system for ESP called SPIFFS that is a simplified version of SD CARD file system with only one flat directory, no subdirectory)
+ WIFI capable, opens up possibility for online games, posting of top scores from individual players to the web dashboard for competition, download and refresh of games directly from a web site, like that of an Android playstore.
+ Simpler wiring if you use the Node MCU D1 mini to make the board.
+ Cheaper overall cost.

Arduboy2 library for ESP8266 with push button supports I2C OLED and single pin sound
=====================================================================================

Thanks to hartmann1301 who created the Arduboy2 library based on the Arduboy2 library for the Slim boy version of Arduboy that runs on the Adruino Nano and I2C OLED dipslay. This ARduboy2 library for ESP8266 allow  Arduboy games to be run on ESP8266 with  a slower I2C SSD1306 OLED and an external button function for PS2 joysticks.  refer to https://github.com/hartmann1301/Arduboy2

However, I found that library not supporting push buttons  for ESP8266 directly. The game need to be programmed with extra external functions to read the push buttons. This add extra efforts to port Arduboy games over to an ESP8266 Arduboy. 

I modified that a bit to define the mapping of GPIO push buttons for ESP8266.
As ESP8266 has limited pins, so I have to use both the GPIO 0 and RX for A and B buttons.
Hence make sure you do not accidentally push these two buttons when you are flashing the firmware.
Otherwise, the flash may fail and you may need to redo the flash again.

Here is th elink to my github
* https://github.com/cheungbx/esp8266_arduboy2

As the ESP8266 already have 4MB memory, it should be able to hold many games on the go and self-flash it using a boot loader like what @Mr.Blinky did for the original Arduboy flash cartridge.
I know ESP8266 can be partitioned into two parts and self-flash over the area to the second partition that is not actively running a program. Then switch over to boot and run from the second partition.
But my programming skill is not at that level yet. So, do not know how to do that.

Appreciate help from anyone who's doing  something similar.

Here are the list of the original Arduboy games that have been  successfully ported to ESP8266 Arduboy/ The source code files modified for ESP8266_Arduboy  can be found in the examples folder of this library  with original licensing comments are kept in the modified source codes. Credits to the original creators of these Arduboy games.

TESTED on ESP8266 Arduboy
=========================
* evade - no background music
* ArduBOYING
* ArduBreakout
* breakout-v
* Sirene
* Mystic Balloon
* PicoVaders (space invader)
* Shadow Runner

Here are the pins used and wiring diagram. I am using a Node MCI D1 Mini on a bread board and then created a perf board version. Next, I I will try to build a tiny one using a mini-joystick (as small as  button) and the bare ESP12E module. Programming will be done through a ESP_USB programmer.


https://github.com/cheungbx/esp8266_arduboy2/blob/master/ESP8266%20Arduboy.jpg

PARTS
========
*NODE MCU Mini D1 (ESP8266)

* if you want to build a custom design board that is  super small, you can use the ESP8266 12E module directly, and get a USB_to_ESP programmer  board to program it.
* The ESP8266 12E is a bare board and you need to tie certain pins to ground and other pins to VCC to make it run firmware, or put it in the a programming mode.
* Connect the following pins to their own 1K resistors then to VCC (3.3V) : 
* Reset 
* EN  (chip enable)
* Connect the following pins to their own 1K resistors then to GND :
* D8 (GPIO15)


*I2C SSD1306 OLED 128x64 

*six push buttons

*extra push button for rest (for perf board version only)

*power on/off switch (or perf board version only)

*3.7V LIPO Battery

*Perf Board or breadboard

*Wires

*Pins for buttons

*GPIO13 D7—— Rightbutton----- .GND

*GPIO12 D6—— Left button----- .GND

*GPIO14 D5—— UP button----- .GND

*GPIO2 D4—— Down button----- .GND

*GPIO0 D3—— A button----- .GND // ** WARNING ** DO NOT press this button when flashing firmware.

*GPIO3 RX—— B button----- .GND

*GPIO15 D8——Piezo Speaker/headphone--GND

*ESP8266 i2c SSD1306 Oled

*=============================

*3.3V ------------VCC

*GND -------------GND

*GPIO5 D1 --------SCL

*GPIO4 D2—--------SDA

* Extra Libraries required to work with the ESP8266_Arduboy2 library:

* https://github.com/ThingPulse/esp8266-oled-ssd1306 version 4.0.0

* Brzo I2C library to drive SSD 1306 display at the max. I2C. speed.

* https://github.com/pasko-zh/brzo_i2c

Simple Steps to convert Arduboy games to run on ESP8266 Arduboy.
=======================================================================
* change "#include arduboy.h" to "#include arduboy2.h"
* add "BeepPin1 beep;"
* add "sound() function"
* comment to remove // #include <ArduboyTones.h>
* comment to remove // ArduboyTones sound(arduboy.audio.enabled);
* change all "sound.tone" to "playTone"
* some games use a function pointer arrary instead of a switch function to pass control to different part of the codes as the game state changes. In ATMEGA32U4 the memory address are 2 bytes (single word) long, in ESP8266, the memory addresses are 4 bytyes ( doube word) long, So you need to 
 change all "pgm_read_word" to "pgm_read_dword"
* add beep.begin() after arduboy.begin();
* if EEPROM is used by the game to keep configs/high scores,
* refer to the notes in my github on how to add the eeprom.update function to your eeprom library in the ESP8266 library for your adruino ide. 

* add EEPROM.begin(100) at setup() // 100 is just a rough max no. need to check the size 
* add EEPROM.commit() after the last EEPROM.put(), EEPORM.write() and EEPROM.update() of each blocks of code.

Caveats 
===============

** remove any reference to the Arduboy audio library that require timers to play back ground musics.
* that part of the library has not yet been ported
* remove any reference to the ATMlib for  complex sound output 
* games that directly control the SPI bus to write to OLED display need much more work to port instead of the simple steps above.

