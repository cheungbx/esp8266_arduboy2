#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include <Arduino.h>
#include "globals.h"
#include "runner.h"
#include "items.h"

int background1step = 0;
int background2step = 128;
byte background1id = 0;
byte background2id = 1;
int fence1step = 0;
int fence2step = 128;
byte fence1id = 0;
byte fence2id = 1;
int forgroundstep = 128;
byte forgroundid = 0;


void drawScore(int scoreX, int scoreY)
{
  sprites.drawSelfMasked(scoreX, scoreY, score, 0);
  char buf[8];
  ltoa(scorePlayer, buf, 10); // Numerical base used to represent the value as a string, between 2 and 36, where 10 means decimal base
  char charLen = strlen(buf);
  char pad = 8 - charLen;

  //draw 0 padding
  for (byte i = 0; i < pad; i++)
  {
    sprites.drawSelfMasked(scoreX + 28 + (5 * i), scoreY + 1, numbers, 0);
  }

  for (byte i = 0; i < charLen; i++)
  {
    char digit = buf[i];
    byte j;
    if (digit <= 48)
    {
      digit = 0;
    }
    else {
      digit -= 48;
      if (digit > 9) digit = 0;
    }

    for (byte z = 0; z < 10; z++)
    {
      if (digit == z) j = z;
    }
    sprites.drawSelfMasked(scoreX + 28 + (pad * 5) + (5 * i), scoreY + 1, numbers, digit);
  }
}

void drawBackGround()
{
  if (arduboy.everyXFrames(3))
  {
    background1step -= 1;
    background2step -= 1;
  }
  if (background1step < -127)
  {
    background1step = 128;
    background1id = random(0, 2);
  }
  if (background2step < -127)
  {
    background2step = 128;
    background2id = random(1, 5);
  }
  sprites.drawSelfMasked(background1step, 0, backGrounds, 2 * background1id);
  sprites.drawSelfMasked(background1step + 64, 0, backGrounds, (2 * background1id) + 1);
  sprites.drawSelfMasked(background2step, 0, backGrounds, 2 * background2id);
  sprites.drawSelfMasked(background2step + 64, 0, backGrounds, 2 * (background2id) + 1);
}

void drawFence()
{
  if (arduboy.everyXFrames(1))
  {
    fence1step -= 1;
    fence2step -= 1;
  }
  sprites.drawPlusMask(fence1step, 36, fences_plus_mask, 2 * fence1id);
  sprites.drawPlusMask(fence1step + 64, 36, fences_plus_mask, (2 * fence1id) + 1);
  sprites.drawPlusMask(fence2step, 36, fences_plus_mask, 2 * fence2id);
  sprites.drawPlusMask(fence2step + 64, 36, fences_plus_mask, (2 * fence2id) + 1);
  if (fence1step < -127)
  {
    fence1step = 128;
    fence1id = random(0, 3);
  }
  if (fence2step < -127)
  {
    fence2step = 128;
    fence2id = random(2, 5);
  }
}

void drawForGround()
{
  if (forgroundstep == 128) forgroundid = random(0, 3);
  sprites.drawErase(forgroundstep, -4, forgroundTrees, forgroundid);
  if (arduboy.everyXFrames(2))
  {
    forgroundstep -= 4;
  }
  if (forgroundstep < -255) forgroundstep = 128;
}

void drawScoreAndLive()
{
  if (arduboy.everyXFrames(16 - 2 * level))
  {
    lifePlayer--;
  }
  if (lifePlayer < 64)
  {
    showitems = showitems | B00100000;
  }
  sprites.drawSelfMasked(2, 52, life, 0);
  for (byte i = 0; i < lifePlayer + 1; i++) sprites.drawSelfMasked(i, 61, lifeBar, 0);
  drawScore(59, 52);
}

void checkScoreAndLevel()
{
  if (nextLevelAt < scorePlayer)
  {
    level += 1;
    nextLevelAt += 1000;
    if (level > 7) level = 7;
  }
  scorePlayer++;
}

void checkInputs()
{
  if (arduboy.justPressed(B_BUTTON))
  {
    if (!jumping)
    {
      jumping = true;
      runnerFrame = RUNNER_JUMPING;
    }
  }
  else if (arduboy.justPressed(A_BUTTON | LEFT_BUTTON))
  {
    if (!ducking && !jumping)
    {
      ducking = true;
      runnerFrame = RUNNER_DUCKING;
    }
  }
  else if (arduboy.justPressed(UP_BUTTON | DOWN_BUTTON | RIGHT_BUTTON)) gameState = STATE_GAME_PAUSE;
}

void checkCollisions()
{
  Rect runnerRect = {.x = runnerX + 8, .y = runnerY + 2, .width = 10, .height = 20};

  Rect stoneOneRect = {.x = itemX[ITEM_STONE_ONE] + 2, .y = STONES_Y + 4, .width = 11, .height = 12};
  Rect stoneTwoRect = {.x = itemX[ITEM_STONE_TWO] + 2, .y = STONES_Y + 4, .width = 11, .height = 12};
  Rect birdOneRect = {.x = itemX[ITEM_BIRD_ONE] + 2, .y = BIRDS_Y + 4, .width = 12, .height = 20};
  Rect birdTwoRect = {.x = itemX[ITEM_BIRD_TWO] + 2, .y = BIRDS_Y + 4, .width = 12, .height = 20};
  Rect extraLifeRect = {.x = itemX[ITEM_EXTRA_LIFE] + 2, .y = HEART_Y + 2, .width = 12, .height = 12};

  if ((showitems & B00000001) && arduboy.collide(runnerRect, stoneOneRect))
  {
    lifePlayer -= 4;
    sound(175, 1);
  }

  if ((showitems & B00000010) && arduboy.collide(runnerRect, stoneTwoRect))
  {
    lifePlayer -= 4;
    sound(175, 1);
  }

  if ((showitems & B00000100) && arduboy.collide(runnerRect, birdOneRect))
  {
    lifePlayer -= 2;
    sound(523, 1);
  }

  if ((showitems & B00001000) && arduboy.collide(runnerRect, birdTwoRect))
  {
    lifePlayer -= 2;
    sound(523, 1);
  }

  if ((showitems & B00100000) && arduboy.collide(runnerRect, extraLifeRect))
  {
    showitems ^= B00100000;
    itemX[ITEM_EXTRA_LIFE] = 128;
    lifePlayer = 128;
    scorePlayer += 500;
    sound(750, 1);
  }
}

#endif
