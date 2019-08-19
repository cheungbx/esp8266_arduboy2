/*
 * 
 * 
  Ported from ATmega32u4 Arduboy to ESP8266 Arduboy by cheungbx using the ESP8266 Arduboy2 library ported  by hartmann1301 and modified ported by by cheungbx to suit the 
  game board designed by cheungbx using 1 buzzer, 6 push buttons and 1 I2C OLED.

  list of modificaitons to port to ESP8266

* change "#include arduboy.h" to "#include arduboy2.h"
* add "BeepPin1 beep;" 
* add  "sound() function"
* comment to remove // #include <ArduboyTones.h>
* comment to remove // ArduboyTones sound(arduboy.audio.enabled);
* change all "sound.tone" to "playTone"
* chnage all "pgm_read_word" to "pgm_read_dword"
*  add beep.begin() after arduboy.begin();
*  if EEPROM is used by the game to keep configs/high scores,
*    add EEPROM.begin(100) at setup()
*    add EEPROM.commit()  after the last EEPROM.put(), 
*    EEPORM.write()  and EEPROM.update() of each blocks of code.
*    
  *** original comments of the author below ******
  * 
  * 
  * 
  *
  
  SIRÈNE: http://www.team-arg.org/srn-manual.html

  Arduboy version 1.2.2:  http://www.team-arg.org/srn-downloads.html

  MADE by TEAM a.r.g. : http://www.team-arg.org/more-about.html

  2016 - JO3RI - JUSTIN_CYR

  License: MIT : https://opensource.org/licenses/MIT

*/

//determine the game
#define GAME_ID 42

#include "globals.h"
#include "menu.h"
#include "game.h"
#include "inputs.h"
#include "player.h"
#include "enemies.h"
#include "elements.h"
#include "stages.h"

typedef void (*FunctionPointer) ();

const FunctionPointer PROGMEM  mainGameLoop[] = {
  stateMenuIntro,
  stateMenuMain,
  stateMenuHelp,
  stateMenuPlay,
  stateMenuInfo,
  stateMenuSoundfx,
  stateGameNextStage,
  stateGamePlaying,
  stateGamePause,
  stateGameOver,
  stateGameEnded,
};


void setup() {
  arduboy.begin();
  arduboy.setFrameRate(FRAME_RATE);
  arduboy.initRandomSeed();
  beep.begin();
  arduboy.audio.on();
 
}


void loop() {
  if (!(arduboy.nextFrame())) return;
  arduboy.pollButtons();
  arduboy.clear();
 
 ((FunctionPointer) pgm_read_dword (&mainGameLoop[gameState]))();

 
  arduboy.display();


}
