#ifndef ITEMS_H
#define ITEMS_H

#include <Arduino.h>
#include "globals.h"

#define ITEM_STONE_ONE              0
#define ITEM_STONE_TWO              1
#define ITEM_BIRD_ONE               2
#define ITEM_BIRD_TWO               3
#define ITEM_EXTRA_LIFE             4

#define STONES_Y                    36
#define BIRDS_Y                     16
#define HEART_Y                     4


byte showitems =  B00000000;        // this byte holds all the items the player runs into during the game
//                   | ||||
//                   | |||└->  stone1
//                   | ||└-->  stone2
//                   | |└--->  bird1
//                   | └---->  bird2
//                   |
//                   └------>  extra life

byte birdFrame = 0;
boolean heartFrame = 0;

int itemX[5] = { -64, 96, 48, 128, 128};


void checkItems()
{
  if (arduboy.everyXFrames(1))
  {
    itemX[ITEM_STONE_ONE] -= 2;
    if (itemX[ITEM_STONE_ONE] < -127)
    {
      // random set bit 0 to 0 or 1 (false or true)
      bitWrite(showitems, ITEM_STONE_ONE, random(0, 2));
      itemX[ITEM_STONE_ONE] = 128;
    }


    itemX[ITEM_STONE_TWO] -= 2;
    if (itemX[ITEM_STONE_TWO] < -127)
    {
      // random set bit 1 to 0 or 1 (false or true)
      bitWrite(showitems, ITEM_STONE_TWO, random(0, 2));
      itemX[ITEM_STONE_TWO] = 128;
    }


    itemX[ITEM_BIRD_ONE] -= 2;
    if (itemX[ITEM_BIRD_ONE] < -127)
    {
      // random set bit 2 to 0 or 1 (false or true)
      bitWrite(showitems, ITEM_BIRD_ONE, random(0, 2));
      itemX[ITEM_BIRD_ONE] = 128;
    }


    itemX[ITEM_BIRD_TWO] -= 2;
    if (itemX[ITEM_BIRD_TWO] < -127)
    {
      // random set bit 3 to 0 or 1 (false or true)
      bitWrite(showitems, ITEM_BIRD_TWO, random(0, 2));
      if ((showitems & B00001010) == B00001010) showitems ^= B00001000; // if stone2 no Bird2
      itemX[ITEM_BIRD_TWO] = 128;
    }


    if (showitems & B00100000)
    {
      itemX[ITEM_EXTRA_LIFE] -= 2;
      if (itemX[ITEM_EXTRA_LIFE] < -24)
      {
        showitems ^= B00100000;
        itemX[ITEM_EXTRA_LIFE] = 128;
      }
    }
  }

  if (arduboy.everyXFrames(6))
  {
    birdFrame++;
    heartFrame = !heartFrame;
  }
  if (birdFrame > 7) birdFrame = 0;
}



void drawItems()
{
  if (showitems & B00000001) sprites.drawPlusMask(itemX[ITEM_STONE_ONE], 36, stone_plus_mask, 0);
  if (showitems & B00000010) sprites.drawPlusMask(itemX[ITEM_STONE_TWO], 36, stone_plus_mask, 0);
  if (showitems & B00000100) sprites.drawErase(itemX[ITEM_BIRD_ONE], 16, bird, birdFrame);
  if (showitems & B00001000) sprites.drawErase(itemX[ITEM_BIRD_TWO], 16, bird, birdFrame);
  if (showitems & B00100000) sprites.drawErase(itemX[ITEM_EXTRA_LIFE], 4, heart, heartFrame);
}


#endif
