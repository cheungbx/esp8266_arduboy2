#ifndef STAGES_H
#define STAGES_H

#include <Arduino.h>
#include "globals.h"
#include "enemies.h"

#define TOTAL_AMOUNT_OF_STAGES  9
#define TOTAL_AMOUNT_OF_WAVES   25

boolean checkStartWave()
{
  if (currentWave != previousWave)
  {
    previousWave = currentWave;
    return true;
  }
  return false;
}

boolean checkEndWave()
{
  byte test = 0;
  for (byte i = 0; i < MAX_ONSCREEN_ENEMIES; i++)
  {
    test += bitRead(enemy[i].characteristics, 7);
  }
  if (test < 1) currentWave++;
}

void wait()
{
  if (arduboy.everyXFrames(4)) globalCounter++;
  if (globalCounter > 16)
  {
    if (gameState == STATE_GAME_PLAYING) currentWave++;
    else gameOverAndStageFase++;
    globalCounter = 0;
  }
}

void wave000()
{
  wait();
}

void wave001()
{
  if (checkStartWave())
  {
    enemySetInLine(ENEMY_FISHY, 0, 3, 128, 12, 192, 0);
    enemySetInLine(ENEMY_FISHY, 3, 6, 256, 32, 192, 0);
    enemySetInLine(ENEMY_FISHY, 5, 9, 192, 48, 192, 0);
  }
  enemySwimRightLeft(0, 9, 3);
  checkEndWave();
}

void wave002()
{
  if (checkStartWave())
  {
    enemySetInLine(ENEMY_FISHY, 0, 4, 128, 12, 96, 0);
    enemySetInLine(ENEMY_FISHY, 4, 8, 176, 52, 96, 0);
  }
  enemySwimToMiddle(0, 8, 3);
  checkEndWave();
}

void wave003()
{
  if (checkStartWave())
  {
    enemySetInLine(ENEMY_FISHY, 0, 9, 128, 32, 36, 0);
  }
  enemySwimSine(0, 9, 3);
  checkEndWave();
}

void wave004()
{
  if (checkStartWave())
  {
    enemySetInLine(ENEMY_FISHY, 0, 8, 128, 12, 40, 6);
  }
  enemySwimRightLeft(0, 8, 3);
  checkEndWave();
}

void wave005()
{
  if (checkStartWave())
  {
    enemySetInLine(ENEMY_FISH, 0, 3, 128, 12, 24, 0);
    enemySetInLine(ENEMY_FISH, 3, 6, 128, 30, 24, 0);
    enemySetInLine(ENEMY_FISH, 6, 9, 128, 48, 24, 0);
  }
  enemySwimRightLeft(0, 9, 1);
  checkEndWave();
}

void wave006()
{
  if (checkStartWave())
  {
    enemySetInLine(ENEMY_FISH, 0, 3, 128, 12, 288, 0);
    enemySetInLine(ENEMY_FISH, 3, 6, 320, 32, 288, 0);
    enemySetInLine(ENEMY_FISH, 6, 9, 224, 48, 288, 0);
  }
  enemySwimRightLeft(0, 9, 3);
  checkEndWave();
}

void wave007()
{
  if (checkStartWave())
  {
    enemySetInLine(ENEMY_FISHY, 0, 3, 128, 12, 192, 0);
    enemySetInLine(ENEMY_FISHY, 3, 6, 128, 48, 192, 0);
    enemySetInLine(ENEMY_FISH, 6, 9, 192, 26, 256, 0);
  }
  enemySwimRightLeft(0, 6, 2);
  enemySwimRightLeft(6, 9, 3);
  checkEndWave();
}

void wave008()
{
  if (checkStartWave())
  {
    enemySetInLine(ENEMY_FISH, 0, 3, 128, 12, 256, 0);
    enemySetInLine(ENEMY_FISH, 3, 6, 256, 48, 256, 0);
    enemySetInLine(ENEMY_FISH, 6, 9, 192, 31, 256, 0);
  }
  enemySwimToMiddle(0, 9, 2);
  checkEndWave();
}

void wave009()
{
  if (checkStartWave())
  {
    enemySetInLine(ENEMY_FISH, 0, 2, 128, 12, 144, 0);
    enemySetInLine(ENEMY_FISH, 2, 6, 128, 30, 72, 0);
    enemySetInLine(ENEMY_FISH, 6, 8, 200, 48, 144, 0);
  }
  enemySwimRightLeft(0, 9, 2);
  checkEndWave();
}

void wave010()
{
  if (checkStartWave())
  {
    enemySetInLine(ENEMY_FISH, 0, 3, 128, 16, 144, 0);
    enemySetInLine(ENEMY_FISH, 3, 6, 128, 44, 144, 0);
    enemySetInLine(ENEMY_FISH, 6, 9, 200, 30, 144, 0);
  }
  enemySwimSine(6, 9, 1);
  enemySwimRightLeft(0, 6, 1);
  checkEndWave();
}

void wave011()
{
  if (checkStartWave())
  {
    enemySetInLine(ENEMY_FISH, 0, 2, 128, 31, 256, 0);
    enemySetInLine(ENEMY_FISH, 2, 4, 192, 10, 256, 0);
    enemySetInLine(ENEMY_FISH, 4, 6, 320, 48, 256, 0);
    enemySetInLine(ENEMY_FISH, 6, 8, 256, 31, 256, 0);
  }
  enemySwimRightLeft(0, 2, 2);
  enemySwimToMiddle(2, 6, 2);
  enemySwimSine(6, 8, 2);

  checkEndWave();
}

void wave012()
{
  if (checkStartWave())
  {
    enemySetInLine(ENEMY_FISH, 0, 3, 128, 30, 96, 0);
    enemySetInLine(ENEMY_FISH, 3, 6, 256, 12, 256, 0);
    enemySetInLine(ENEMY_FISH, 6, 9, 256, 48, 256, 0);
  }
  enemySwimSine(0, 3, 1);
  enemySwimRightLeft(3, 9, 2);
  checkEndWave();
}

void wave013()
{
  if (checkStartWave())
  {
    enemySetInLine(ENEMY_JELLYFISH, 0, 3, 16, 64, 40, 16);
    enemySetInLine(ENEMY_JELLYFISH, 3, 5, 32, 172, 48, 0);
    enemySetInLine(ENEMY_JELLYFISH, 5, 8, 16, 256, 40, -16);
  }
  enemySwimDownUp(0, 8, 1);
  checkEndWave();
}

void wave014()
{
  if (checkStartWave())
  {
    enemySetInLine(ENEMY_JELLYFISH, 0, 9, 128, 64, 40, 16);
  }
  enemySwimDownUp(0, 9, 1);
  enemySwimRightLeft(0, 9, 2);
  checkEndWave();
}

void wave015()
{
  if (checkStartWave())
  {
    enemySetInLine(ENEMY_JELLYFISH, 0, 6, 128, 64, 80, 32);
    enemySetInLine(ENEMY_FISH, 6, 9, 288, 12, 96, 0);
  }
  enemySwimDownUp(0, 6, 1);
  enemySwimRightLeft(0, 6, 2);
  enemySwimSine(6, 9, 2);
  checkEndWave();
}

void wave016()
{
  if (checkStartWave())
  {
    enemySetInLine(ENEMY_OCTOPUS, 0, 4, 128, 12, 128, 0);
    enemySetInLine(ENEMY_OCTOPUS, 4, 8, 176, 36, 128, 0);
  }
  enemySwimSine(0, 8, 1);
  checkEndWave();
}

void wave017()
{
  if (checkStartWave())
  {
    enemySetInLine(ENEMY_OCTOPUS, 0, 4, 128, 10, 192, 0);
    enemySetInLine(ENEMY_OCTOPUS, 4, 8, 192, 48, 192, 0);
  }
  enemySwimRightLeft(0, 8, 2);
  enemyShoot(0, 8);
  checkEndWave();
}

void wave018()
{
  if (checkStartWave())
  {
    enemySetInLine(ENEMY_OCTOPUS, 0, 3, 128, 10, 192, 0);
    enemySetInLine(ENEMY_OCTOPUS, 3, 6, 128, 48, 192, 0);
    enemySetInLine(ENEMY_FISH, 6, 9, 288, 29, 192, 0);
  }
  enemySwimRightLeft(0, 6, 2);
  enemySwimRightLeft(6, 9, 3);
  enemyShoot(0, 6);
  checkEndWave();
}

// POWER UPS
////////////
void wave100()
{
  powerUPSet(POWER_UP_HEART);
  currentWave++;
}

void wave101()
{
  powerUPSet(POWER_UP_SHIELD);
  currentWave++;
}

void wave102()
{
  powerUPSet(POWER_UP_TRIDENT);
  currentWave++;
}

void wave103()
{
  powerUPSet(POWER_UP_BUBBLE);
  currentWave++;
}

void wave104()
{
  powerUPSet(POWER_UP_SEASHELL);
  currentWave++;
}

void wave105()
{
  powerUPSet(POWER_UP_MAGIC);
  currentWave++;
}

void wave106()
{
  powerUPSet(POWER_UP_STAR);
  currentWave++;
}



// BOSS ATTACKS
///////////////
void wave250()
{
  //Shark attack
  if (checkStartWave())setEndBoss();
  ((FunctionPointer) pgm_read_dword (&sharkAttackFases[endBoss.attackFase]))();
  if (!bitRead(endBoss.characteristics, 7)) currentWave++;
}

void wave251()
{
  //seahorse attack
  if (checkStartWave())setEndBoss();
  ((FunctionPointer) pgm_read_dword (&seahorseAttackFases[endBoss.attackFase]))();
  if (!bitRead(endBoss.characteristics, 7)) currentWave++;
}

void wave252()
{
  //pirateShip attack
  if (checkStartWave())setEndBoss();
  ((FunctionPointer) pgm_read_dword (&pirateShipAttackFases[endBoss.attackFase]))();
  if (!bitRead(endBoss.characteristics, 7)) currentWave++;
}

void wave253()
{
  //Explosions
  checkExplosions();
}


// END WAVES
///////////////
void wave254()
{
  gameState = STATE_GAME_NEXT_STAGE;
}

void wave255()
{
  mermaid.x = 16;
  mermaid.y = 32;
  gameState = STATE_GAME_ENDED;
}


typedef void (*FunctionPointer) ();
const FunctionPointer PROGMEM stages[TOTAL_AMOUNT_OF_STAGES][TOTAL_AMOUNT_OF_WAVES] =
{
  //PART 1
  { //STAGE 1
    wave001,
    wave001,
    wave002,
    wave101, //POWER_UP_SHIELD
    wave003,
    wave005,
    wave005,
    wave106, //POWER_UP_STAR
    wave001,
    wave002,
    wave001,
    wave103, //POWER_UP_BUBBLE
    wave002,
    wave003,
    wave004,
    wave106, //POWER_UP_STAR
    wave004,
    wave001,
    wave005,
    wave100, //POWER_UP_HEART
    wave001,
    wave005,
    wave007,
    wave000,
    wave254,
  },
  { //STAGE 2
    wave001,
    wave001,
    wave002,
    wave104, //POWER_UP_SEASHELL
    wave002,
    wave003,
    wave001,
    wave106, //POWER_UP_STAR
    wave002,
    wave002,
    wave003,
    wave101, //POWER_UP_SHIELD
    wave003,
    wave004,
    wave007,
    wave106, //POWER_UP_STAR
    wave007,
    wave005,
    wave006,
    wave100, //POWER_UP_HEART
    wave005,
    wave006,
    wave007,
    wave000,
    wave254,
  },
  { //STAGE 3
    wave010,
    wave009,
    wave008,
    wave101, //POWER_UP_SHIELD
    wave006,
    wave005,
    wave011,
    wave106, //POWER_UP_STAR
    wave011,
    wave005,
    wave006,
    wave100, //POWER_UP_HEART
    wave008,
    wave009,
    wave010,
    wave106, //POWER_UP_STAR
    wave009,
    wave006,
    wave005,
    wave103, //POWER_UP_BUBBLE
    wave005,
    wave011,
    wave250,
    wave253,
    wave254,
  },

  //PART 2
  { //STAGE 4
    wave012,
    wave001,
    wave012,
    wave100, //POWER_UP_HEART
    wave001,
    wave013,
    wave002,
    wave106, //POWER_UP_STAR
    wave013,
    wave001,
    wave012,
    wave105, //POWER_UP_MAGIC
    wave001,
    wave013,
    wave007,
    wave106, //POWER_UP_STAR
    wave011,
    wave010,
    wave013,
    wave102, //POWER_UP_TRIDENT
    wave007,
    wave012,
    wave001,
    wave000,
    wave254,
  },
  { //STAGE 5
    wave015,
    wave015,
    wave014,
    wave104, //POWER_UP_SEASHELL
    wave013,
    wave006,
    wave005,
    wave106, //POWER_UP_STAR
    wave006,
    wave010,
    wave015,
    wave101, //POWER_UP_SHIELD
    wave014,
    wave013,
    wave013,
    wave106, //POWER_UP_STAR
    wave009,
    wave008,
    wave006,
    wave100, //POWER_UP_HEART
    wave015,
    wave014,
    wave013,
    wave000,
    wave254,
  },
  { //STAGE 6
    wave001,
    wave002,
    wave003,
    wave100, //POWER_UP_HEART
    wave004,
    wave005,
    wave006,
    wave106, //POWER_UP_STAR
    wave007,
    wave008,
    wave009,
    wave103, //POWER_UP_BUBBLE
    wave010,
    wave011,
    wave012,
    wave106, //POWER_UP_STAR
    wave013,
    wave014,
    wave015,
    wave102, //POWER_UP_TRIDENT
    wave014,
    wave013,
    wave251,
    wave253,
    wave254,
  },

  //PART 3
  { //STAGE 7
    wave012,
    wave013,
    wave012,
    wave100, //POWER_UP_HEART
    wave010,
    wave016,
    wave013,
    wave106, //POWER_UP_STAR
    wave012,
    wave010,
    wave011,
    wave105, //POWER_UP_MAGIC
    wave011,
    wave007,
    wave016,
    wave106, //POWER_UP_STAR
    wave011,
    wave010,
    wave013,
    wave101, //POWER_UP_SHIELD
    wave007,
    wave012,
    wave001,
    wave000,
    wave254,
  },
  { //STAGE 8
    wave007,
    wave014,
    wave017,
    wave104, //POWER_UP_SEASHELL
    wave016,
    wave003,
    wave001,
    wave106, //POWER_UP_STAR
    wave013,
    wave012,
    wave003,
    wave100, //POWER_UP_HEART
    wave016,
    wave017,
    wave016,
    wave106, //POWER_UP_STAR
    wave007,
    wave005,
    wave010,
    wave105, //POWER_UP_MAGIC
    wave009,
    wave006,
    wave007,
    wave000,
    wave254,
  },
  { //STAGE 9
    wave016,
    wave017,
    wave018,
    wave102, //POWER_UP_TRIDENT
    wave004,
    wave005,
    wave006,
    wave106, //POWER_UP_STAR
    wave007,
    wave008,
    wave016,
    wave100, //POWER_UP_HEART
    wave017,
    wave018,
    wave012,
    wave106, //POWER_UP_STAR
    wave014,
    wave015,
    wave016,
    wave100, //POWER_UP_HEART
    wave017,
    wave018,
    wave252,
    wave253,
    wave255,
  },
};

void checkCollisions()
{
  Rect mermaidRect = {.x = mermaid.x + MERMAID_COLLISION_OFFSET, .y = mermaid.y + MERMAID_COLLISION_OFFSET, .width = MERMAID_COLLISION_SIZE, .height = MERMAID_COLLISION_SIZE};
  Rect endBossRect = {.x = endBoss.x, .y = endBoss.y + 2, .width = 32, .height = 12};

  ////// Check collision bullets with enemies and bosses /////
  ////////////////////////////////////////////////////////////
  for (byte k = 0; k < MAX_ONSCREEN_BULLETS; k++)
  {
    if (bullet[k].isVisible)
    {
      Rect bulletsRect {.x = bullet[k].x + bulletCollisionOffset[bullet[k].type], .y = bullet[k].y, .width = 8, .height = 8};
      for (byte i = 0; i < MAX_ONSCREEN_ENEMIES; i++)
      {
        Rect enemyRect = {.x = enemy[i].x, .y = enemy[i].y, .width = enemyCollisionWidth[enemy[i].type], .height = enemyCollisionHeight[enemy[i].type]};
        if (bitRead(enemy[i].characteristics, 4) && !bitRead(enemy[i].characteristics, 5) && arduboy.collide(bulletsRect, enemyRect))
        {
          if (!bitRead(enemy[i].characteristics, 6))
          {
            sound(523, 10);
            enemy[i].HP -= bullet[k].damage;
            bitSet(enemy[i].characteristics, 6);
            if (enemy[i].HP < 1) giveBonus(enemiesPoints[enemy[i].type], enemy[i].x, enemy[i].y);
          }
          if (bullet[k].type != WEAPON_TYPE_MAGIC)
          {
            bullet[k].isVisible = false;
          }
        }
      }
      if (bitRead(endBoss.characteristics, 4) && !bitRead(endBoss.characteristics, 5) && !bitRead(endBoss.characteristics, 6))
      {
        switch (endBoss.type)
        {
          case ENDBOSS_SHARK:
            endBossRect = {.x = endBoss.x, .y = endBoss.y + 2, .width = 32, .height = 12};
            break;
          case ENDBOSS_SEAHORSE:
            endBossRect = {.x = endBoss.x + 2, .y = endBoss.y + 2, .width = 6, .height = 6};
            break;
          case ENDBOSS_PIRATESHIP:
            endBossRect = {.x = endBoss.x + 38, .y = endBoss.y + 15, .width = 5, .height = 5};
            break;
        }
        if (arduboy.collide(bulletsRect, endBossRect))
        {
          sound(523, 10);
          bitSet(endBoss.characteristics, 6);
          bullet[k].isVisible = false;
          endBoss.HP -= bullet[k].damage;
          giveBonus(10, endBossRect.x, endBossRect.y);
          if (endBoss.HP < 1) giveBonus(5000, endBossRect.x, endBossRect.y);
        }
      }
    }
  }


  ////// Check collision mermaid with enemies and bosses /////
  ////////////////////////////////////////////////////////////

  for (byte i = 0; i < MAX_ONSCREEN_ENEMIES; i++)
  {
    Rect enemyRect = {.x = enemy[i].x, .y = enemy[i].y, .width = enemyCollisionWidth[enemy[i].type], .height = enemyCollisionHeight[enemy[i].type]};
    if (bitRead(enemy[i].characteristics, 4) && !bitRead(enemy[i].characteristics, 5) && arduboy.collide(mermaidRect, enemyRect))
    {
      if (!mermaid.isImune && !mermaid.hasShield)
      {
        mermaid.isImune = true;
        mermaid.HP -= 1;
      }
      if (!bitRead(enemy[i].characteristics, 6))
      {
        bitSet(enemy[i].characteristics, 5);
        sound(2349, 15);
      }
    }
  }
  for (byte i = 0; i < MAX_ONSCREEN_ENEMY_BULLETS; i++)
  {
    Rect enemyBulletRect = {.x = enemyBullet[i].x + 1, .y = enemyBullet[i].y + 1, .width = 6, .height = 6};
    if (enemyBullet[i].isVisible && arduboy.collide(mermaidRect, enemyBulletRect))
    {
      if (!mermaid.isImune && !mermaid.hasShield)
      {
        sound(2349, 15);
        mermaid.isImune = true;
        mermaid.HP -= BULLET_DAMAGE;
        enemyBullet[i].isVisible = false;
      }
    }
  }

  if (bitRead(endBoss.characteristics, 4) && !bitRead(endBoss.characteristics, 5))
  {
    switch (endBoss.type)
    {
      case ENDBOSS_SHARK:
        endBossRect = {.x = endBoss.x, .y = endBoss.y + 2, .width = 32, .height = 12};
        break;
      case ENDBOSS_SEAHORSE:
        endBossRect = {.x = endBoss.x + 2, .y = endBoss.y + 2, .width = 12, .height = 28};
        break;
      case ENDBOSS_PIRATESHIP:
        endBossRect = {.x = endBoss.x + 19, .y = endBoss.y + 10, .width = 46, .height = 40};
        break;
    }
    if (arduboy.collide(mermaidRect, endBossRect))
    {
      if (!bitRead(endBoss.characteristics, 6))
      {
        sound(2349, 15);
        bitSet(endBoss.characteristics, 6);
        endBoss.HP--;
      }
      if (!mermaid.isImune && !mermaid.hasShield)
      {
        mermaid.isImune = true;
        mermaid.HP -= 1;
      }
    }
  }

  if (powerUP.isActive)
  {
    Rect powerUPRect = {.x = powerUP.x, .y = powerUP.y, .width = 8, .height = 8};
    if (arduboy.collide(mermaidRect, powerUPRect))
    {
      switch (powerUP.type)
      {
        case POWER_UP_HEART:
          if (mermaid.HP < MAX_HP_MERMAID) mermaid.HP++;
          else giveBonus(1000, powerUP.x, powerUP.y);
          break;
        case POWER_UP_SHIELD:
          mermaid.hasShield = true;
          break;
        case POWER_UP_TRIDENT:
          mermaid.weaponType = WEAPON_TYPE_TRIDENT;
          break;
        case POWER_UP_BUBBLE:
          mermaid.weaponType = WEAPON_TYPE_BUBBLES;
          break;
        case POWER_UP_SEASHELL:
          mermaid.weaponType = WEAPON_TYPE_SEASHELL;
          break;
        case POWER_UP_MAGIC:
          mermaid.weaponType = WEAPON_TYPE_MAGIC;
          break;
        case POWER_UP_STAR:
          giveBonus(2500, powerUP.x, powerUP.y);
          break;
      }
      powerUP.isActive = false;
      sound(2093, 15);
      delay (20);
      sound(4186, 30);
    }
  }
}


void drawScore(byte fontType)
{
  char buf[10];
  //scorePlayer = arduboy.cpuLoad();
  ltoa(scorePlayer, buf, 10);
  char charLen = strlen(buf);
  char pad = 7 - charLen;

  //draw 0 padding
  for (byte i = 0; i < pad; i++)
  {
    switch (fontType)
    {
      case SCORE_SMALL_FONT:
        sprites.drawSelfMasked(95 + (5 * i), 0, numbersSmall, 0);
        break;
      case SCORE_BIG_FONT:
        sprites.drawSelfMasked(40 + (7 * i), 40, numbersBig, 0);
        break;
    }
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
    switch (fontType)
    {
      case SCORE_SMALL_FONT:
        sprites.drawSelfMasked(95 + (pad * 5) + (5 * i), 0, numbersSmall, digit);
        break;
      case SCORE_BIG_FONT:
        sprites.drawSelfMasked(40 + (pad * 7) + (7 * i), 40, numbersBig, digit);
        break;
    }
  }
}


#endif
