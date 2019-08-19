#ifndef GLOBALS_H
#define GLOBALS_H

/*
 *  File: globals.h
 *  Purpose: Global artifacts for Evade game.
 *  Author: Modus Create
 */

// Button debounce
#define DEBOUNCE_DELAY 100

// Number of lives the player starts with
#define MAX_LIVES 4

// How many entries in high score table, and fake position used
// to determine player didn't get a new high score
#define NUM_HIGH_SCORES 3
#define NOT_NEW_HI_SCORE 5

// Number of stars comprising the starfield background
#define NUM_STARS 15

// Define limits that player movement is bounded by
#define MIN_PLAYER_Y 8
#define MAX_PLAYER_Y 48
#define MIN_PLAYER_X 0
#define MAX_PLAYER_X 27 // allows 16 for ship width
#define PLAYER_SIZE 16
#define MAX_GUN_CHARGE 40
#define GUN_SHOT_COST 12

// Define limits that enemy ship movement is bounded by
#define MIN_ENEMY_SHIP_X 92
#define MAX_ENEMY_SHIP_X 112

// Max bullets and enemies allowed in play
#define MAX_PLAYER_BULLETS 4
#define MAX_BOSS_BULLETS 2
#define MAX_ENEMIES 3

// Minimum number of enemies kills needed to spawn next box
#define BOSS1_MIN_KILLS 20
#define BOSS2_MIN_KILLS 25
#define BOSS3_MIN_KILLS 30

// Time before title screen flips to high score screen
#define ATTRACT_MODE_TIMEOUT 400000

// Title screen outcomes
#define TITLE_CREDITS 0
#define TITLE_PLAY_GAME 1
#define TITLE_SETTINGS 2
#define TITLE_TIMEOUT 3

// Settings screen outcomes
#define SETTINGS_SOUND 0
#define SETTINGS_RESET_HIGH_SCORE 1
#define SETTINGS_EXIT 2

// Arduboy library object
Arduboy2 arduboy;

// Used to count frames
unsigned long inGameFrame;

// Current player attributes
byte playerX,
     playerY,
     playerFrame,
     playerDying,
     playerGunCharge,
     livesRemaining = MAX_LIVES,
     currentKills = 0;  // Used to determine when bosses appear

// Star field attributes
float starX[NUM_STARS],
      starSpeed[NUM_STARS];
      
byte starY[NUM_STARS],
     starWidth[NUM_STARS];

// Initial high score table contents
char highScoreTable[] = "AAA000300BBB000200CCC000100";

// Used to print letters from bitmap font
const unsigned char *alphabet[29];

// Track button presses for debounce, frame last death was in, player score
unsigned long inGameAButtonLastPress, 
              inGameBButtonLastPress, 
              inGameLastDeath, 
              score;

// Game state booleans
bool isBossAlive,
     isInitialTitleScreen = true,
     soundOn = true;

// Which tune is currently playing
byte currentSong = 255;

// General purpose text buffer for string concatenation and read from progmem
char textBuf[23];


#endif
