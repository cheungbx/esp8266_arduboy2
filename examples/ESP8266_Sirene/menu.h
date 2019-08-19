#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include "globals.h"
byte hairFrame = 0;
byte eyesFrame = 0;
byte eyesSequence[] = {0, 1, 2, 3, 3, 3, 1};

void drawTitleScreen()
{
  byte eyesFrame2;
  if (arduboy.everyXFrames(10)) hairFrame++;
  if (arduboy.everyXFrames(2)) eyesFrame++;
  if (eyesFrame > 60)eyesFrame = 0;
  eyesFrame2 = eyesFrame;
  if (eyesFrame2 > 6) eyesFrame2 = 0;
  if (hairFrame > 3) hairFrame = 0;
  sprites.drawSelfMasked(36, 1, mermaidTitle, 0);
  sprites.drawSelfMasked(59, 21, mermaidTrident, 0);
  sprites.drawSelfMasked(62, 48, mermaidFin, 0);
  sprites.drawSelfMasked(4, 46, mermaidBody, 0);
  sprites.drawSelfMasked(3, 14, mermaidHair, hairFrame);
  sprites.drawSelfMasked(10, 24, mermaidBlink, eyesSequence[eyesFrame2]);
}

void stateMenuIntro()
{
  globalCounter++;
  sprites.drawSelfMasked(34, 4, T_arg, 0);
  if (globalCounter > 180)
  {
    globalCounter = 0;
    gameState = STATE_MENU_MAIN;
  }
}

void stateMenuMain()
{
  drawTitleScreen();
  for (byte i = 0; i < 4; i++)
  {
    sprites.drawSelfMasked(108, 32 +(i*8), menuText, i);
  }
  sprites.drawSelfMasked(92, 32 + 8*(menuSelection-2), trident, 0);
  if (arduboy.justPressed(DOWN_BUTTON) && (menuSelection < 5)) menuSelection++;
  if (arduboy.justPressed(UP_BUTTON) && (menuSelection > 2)) menuSelection--;
  if (arduboy.justPressed(B_BUTTON)) gameState = menuSelection;
}

void stateMenuHelp()
{
  sprites.drawSelfMasked(32, 0, qrcode, 0);
  if (arduboy.justPressed(A_BUTTON | B_BUTTON)) gameState = STATE_MENU_MAIN;
}


void stateMenuInfo()
{
  sprites.drawSelfMasked(33, 10, mermaidTitle, 0);
  sprites.drawSelfMasked(36, 31, madeBy, 0);
  if (arduboy.justPressed(A_BUTTON | B_BUTTON)) gameState = STATE_MENU_MAIN;
}

void stateMenuSoundfx()
{
  drawTitleScreen();
  for (byte i = 0; i < 3; i++)
  {
    sprites.drawSelfMasked(108, 40 +(i*8), menuText, i+4);
  }
  sprites.drawSelfMasked(92, 48 + 8*arduboy.audio.enabled(), trident, 0);
  if (arduboy.justPressed(DOWN_BUTTON)) arduboy.audio.on();
  if (arduboy.justPressed(UP_BUTTON)) arduboy.audio.off();
  if (arduboy.justPressed(A_BUTTON | B_BUTTON))
  {
    arduboy.audio.saveOnOff();
    gameState = STATE_MENU_MAIN;
  }
}

#endif
