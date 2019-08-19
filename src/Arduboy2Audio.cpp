/**
 * @file Arduboy2Audio.cpp
 * \brief
 * The Arduboy2Audio class for speaker and sound control.
 */

#include "Arduboy2.h"
#include "Arduboy2Audio.h"

bool Arduboy2Audio::audio_enabled = false;

void Arduboy2Audio::on()
{
#ifndef ESP8266
  // fire up audio pins by seting them as outputs
#ifdef ARDUBOY_10
  bitSet(SPEAKER_1_DDR, SPEAKER_1_BIT);
  bitSet(SPEAKER_2_DDR, SPEAKER_2_BIT);
#else
  bitSet(SPEAKER_1_DDR, SPEAKER_1_BIT);
#endif
#endif
  audio_enabled = true;
}

void Arduboy2Audio::off()
{
  audio_enabled = false;
  // shut off audio pins by setting them as inputs
#ifndef ESP8266
#ifdef ARDUBOY_10
  bitClear(SPEAKER_1_DDR, SPEAKER_1_BIT);
  bitClear(SPEAKER_2_DDR, SPEAKER_2_BIT);
#else
  bitClear(SPEAKER_1_DDR, SPEAKER_1_BIT);
#endif
#endif
}

void Arduboy2Audio::toggle()
{
  if (audio_enabled)
    off();
  else
    on();
}

void Arduboy2Audio::saveOnOff()
{
  EEPROM.update(EEPROM_AUDIO_ON_OFF, audio_enabled);
#ifndef ESP8266
  EEPROM.committ();
#endif
}

void Arduboy2Audio::begin()
{
  if (EEPROM.read(EEPROM_AUDIO_ON_OFF))
  if (true)
    on();
  else
    off();
}

bool Arduboy2Audio::enabled()
{
  return audio_enabled;
}
