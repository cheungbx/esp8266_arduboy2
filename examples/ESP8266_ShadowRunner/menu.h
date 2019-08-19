#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include "globals.h"
#include "runner.h"

byte flameid = 0;
boolean showRunner = false;

void drawCandle (byte x, byte y)
{
  if (arduboy.everyXFrames(4))
  {
    flameid = random(0, 24);
  }
  sprites.drawSelfMasked(x, y + 34, candleTip, 0);
  sprites.drawSelfMasked(x + 4, y + 18, candleFlame, flameid);

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
};

void stateMenuMain()
{
  runnerY = 0;
  if (arduboy.everyXFrames(4)) runnerX += 4;
  if (runnerX > 127)
  {
    runnerX = -127;
    showRunner = !showRunner;
  }
  sprites.drawSelfMasked(16, 0, menuTitle, 0);
  sprites.drawSelfMasked(49, 26, menuItems, 0);
  sprites.drawSelfMasked(89, 50, menuYesNo, arduboy.audio.enabled());
  drawCandle(29, (menuSelection - 2) * 8);
  for (byte i = 2; i < 6; i++)
  {
    if (menuSelection != i) {
      sprites.drawErase(49, (i * 8) + 10, menuShade, 0);
      sprites.drawErase(82, (i * 8) + 10, menuShade, 0);
    }
  }
  if (showRunner)
  {
    sprites.drawSelfMasked(runnerX, -2, spotLight, 0);
    drawRunner();
  }
  if (arduboy.justPressed(UP_BUTTON) && (menuSelection > 2)) menuSelection--;
  else if (arduboy.justPressed(DOWN_BUTTON) && (menuSelection < 5)) menuSelection++;
  else if (arduboy.justPressed(A_BUTTON | B_BUTTON))
  {
    if (menuSelection != 5) gameState = menuSelection;
    else
    {
      if (arduboy.audio.enabled()) arduboy.audio.off();
      else arduboy.audio.on();
      arduboy.audio.saveOnOff();
    }
  }

}

void stateMenuHelp()
{
  sprites.drawSelfMasked(32, 0, qrcode, 0);
  if (arduboy.justPressed(A_BUTTON | B_BUTTON)) gameState = STATE_MENU_MAIN;
}

void stateMenuInfo()
{
  sprites.drawSelfMasked(16, 0, menuTitle, 0);
  sprites.drawSelfMasked(15, 25, menuInfo, 0);
  if (arduboy.justPressed(A_BUTTON | B_BUTTON)) gameState = STATE_MENU_MAIN;
}

void stateMenuSoundfx()
{
  // placeHolder
}


void stateMenuPlay()
{
  gameState = STATE_GAME_INIT_LEVEL;
}


#endif
