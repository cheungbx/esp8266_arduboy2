
/*
 * 
 * 
  Ported from ATmega32u4 Arduboy to ESP8266 Arduboy by cheungbx using the ESP8266 Arduboy2 library ported  by hartmann1301 and modified ported by by cheungbx to suit the 
  game board designed by cheungbx using 1 buzzer, 6 push buttons and 1 I2C OLED.

  list of modificaitons to port to ESP8266

* change "#include arduboy.h" to "#include arduboy2.h"
* chnage all "pgm_read_word" to "pgm_read_dword"
* comment to remove // #include <ArduboyTones.h>
* comment to remove // ArduboyTones sound(arduboy.audio.enabled);
* change all "sound.tone" to "playTone"
* add "BeepPin1 beep;" 
* add  "sound() function"
* void sound(byte tone, byte duration) {
*  if (arduboy.audio.enabled()) {
*    beep.tone(beep.freq(tone), duration/3);
*    beep.noTone();
*  }
* }
*
*  if EEPROM is used by the game to keep configs/high scores,
*    add EEPROM.begin(100) at setup()
*    add EEPROM.commit()  after the last EEPROM.put(), 
*    EEPORM.write()  and EEPROM.update() of each blocks of code.
*/
/*
  SHADOW RUNNER: http://www.team-arg.org/shrun-manual.html

  Arduboy version 1.6 : http://www.team-arg.org/more-about.html

  MADE by TEAM a.r.g. : http://www.team-arg.org/more-about.html

  2015 - JO3RI - Trodoss

  Special thanks to Dreamer3 for helping with the conversion from Gamby

  Game License: MIT : https://opensource.org/licenses/MIT

*/

//determine the game
#define GAME_ID 15

#include "globals.h"
#include "runner.h"
#include "menu.h"
#include "game.h"
#include "items.h"
#include "playfield.h"


typedef void (*FunctionPointer) ();

const FunctionPointer PROGMEM mainGameLoop[] = {
  stateMenuIntro,
  stateMenuMain,
  stateMenuHelp,
  stateMenuPlay,
  stateMenuInfo,
  stateMenuSoundfx,
  stateGameInitLevel,
  stateGamePlaying,
  stateGamePause,
  stateGameOver,
};

void setup () {
  arduboy.begin();
  beep.begin();
  arduboy.setFrameRate(FRAME_RATE);
  arduboy.initRandomSeed();
  arduboy.audio.on();
 
}

void loop() {
  if (!(arduboy.nextFrame())) return;
  arduboy.pollButtons();
  arduboy.clear();


  //  ((FunctionPointer) pgm_read_word (&mainGameLoop[gameState]))();

  switch (gameState) {

    case 0:
      stateMenuIntro();
      break;

    case 1:
      stateMenuMain();
      break;
    case 2:
      stateMenuHelp();
      break;
    case 3:
      stateMenuPlay();
      break;
    case 4:
      stateMenuInfo();
      break;
    case 5:
      stateMenuSoundfx();
      break;
    case 6:
      stateGameInitLevel();
      break;
    case 7:
      stateGamePlaying();
      break;
    case 8:
      stateGamePause();
      break;
    case 9:
      stateGameOver();
      break;
  }


  arduboy.display();
}
