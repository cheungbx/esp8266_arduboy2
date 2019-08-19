#include "Arduboy2.h"
#include "glcdfont.c"

Arduboy2 arduboy;
BeepPin1 beep;

const unsigned int FRAME_RATE = 40; // Frame rate in frames per second

#define BLACK   0x0
#define WHITE   0x1

#define NOTE_A1  110
#define NOTE_C2  131
#define NOTE_G2  196
#define NOTE_C3  262
#define NOTE_G3  392
#define NOTE_C4  523

#define SCRNOFS_X 0
#define SCRNOFS_Y 2
#define FONTWIDTH 6
#define FONTHEIGHT 8
#define LEFTBALLS_R 2
#define LEFTBALLS_X 112
#define LEFTBALLS_Y 16

#define GAMEOVER_X 56
#define GAMEOVER_Y 5
#define SCORE_X 127
#define SCORE_Y 17

#define BALLS 5

#define RACKETSIZE 8
#define RACKETLINE 47
#define SCRNWIDTH 30
#define SCRNHEIGHT 50

#define BLOCKWIDTH 3
#define BLOCKHEIGHT 2
#define BLOCKTOP 3
#define BLOCKBOTTOM 12
#define BLOCKLINES ((BLOCKBOTTOM-BLOCKTOP+1)/BLOCKHEIGHT)
#define NBLOCKS (BLOCKLINES*SCRNWIDTH/BLOCKWIDTH)

// The coordination system of the game is SCRNWIDTH * SCRNHEIGHT pixels, top left is (0, 0).
// FillRectRotAndMag convert a rectangle from the game coordination to screen coordination.
void FillRectRotAndMag(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color, uint8_t shrink)
{
  arduboy.fillRect((SCRNHEIGHT - y) * 2, x * 2 + SCRNOFS_Y, h * 2 - shrink, w * 2 - shrink, color);
}

// modified version of drawChar() in Arduboy.cpp
void drawChar_Rot90
(int16_t x, int16_t y, unsigned char c, uint8_t color, uint8_t bg, uint8_t size)
{
  boolean draw_background = bg != color;

  if ((x > WIDTH) ||              // Clip right   ;this line is modified
    (y > (HEIGHT + 5 * size)) ||  // Clip bottom  ;this line is modified
    ((x - 8 * size - 1) < 0) ||   // Clip left    ;this line is modified
    ((y + 5 * size - 1) < 0)      // Clip top     ;this line is modified
  )
  {
    return;
  }

  for (int8_t i=0; i<6; i++ )
  {
    uint8_t line;
    if (i == 5)
    {
      line = 0x0;
    }
    else
    {
      line = pgm_read_byte(font+(c*5)+i);
    }

    for (int8_t j = 0; j<8; j++)
    {
      uint8_t draw_color = (line & 0x1) ? color : bg;

      if (draw_color || draw_background) {
        for (uint8_t a = 0; a < size; a++ ) {
          for (uint8_t b = 0; b < size; b++ ) {
            arduboy.drawPixel( x - (j * size) - b, y + (i * size) + a, draw_color); //  this line is modified
          }
        }
      }
      line >>= 1;
    }
  }
}

void print_score(int16_t x, int16_t y, int16_t score)
{
  drawChar_Rot90(x, y, '0' + score / 10000, WHITE, BLACK, 1);
  score = score % 10000;
  drawChar_Rot90(x, y += 6, '0' + score / 1000, WHITE, BLACK, 1);
  score = score % 1000;
  drawChar_Rot90(x, y += 6, '0' + score / 100, WHITE, BLACK, 1);
  score = score % 100;
  drawChar_Rot90(x, y += 6, '0' + score / 10, WHITE, BLACK, 1);
  score = score % 10;
  drawChar_Rot90(x, y += 6, '0' + score, WHITE, BLACK, 1);
}

void print_game_over()
{
  static char s[10] = "GAME OVER";
  int16_t y = GAMEOVER_Y;
  
  for(char *p = s; *p != NULL; p++) {
    drawChar_Rot90(GAMEOVER_X, y, *p, WHITE, BLACK, 1);
    y += FONTWIDTH;
  }
}

void erase_game_over()
{
  arduboy.fillRect(GAMEOVER_X - FONTHEIGHT + 1, GAMEOVER_Y - 1, FONTHEIGHT, 9 * FONTWIDTH, BLACK);
}

void print_balls_left(int16_t x, int16_t y, int8_t balls)
{
  arduboy.fillRect(x, y, LEFTBALLS_R * 2 + 1, LEFTBALLS_R * 3 * BALLS, BLACK);
  x += LEFTBALLS_R;
  y += LEFTBALLS_R;
  for(int8_t i = 0; i < balls; i++) {
    arduboy.fillCircle(x, y, LEFTBALLS_R, WHITE);
    y += LEFTBALLS_R * 3;
  }
}

boolean sound_on = true;

void ToneWithMute(unsigned int frequency, unsigned long duration) {
//  if (sound_on) 
 //   arduboy.tunes.tone(frequency, duration);
 
  if (sound_on)   beep.tone(beep.freq(frequency), duration / (1000 / FRAME_RATE));

}

boolean exist[NBLOCKS]; // exist[n] = true if the block n exists

void BlocksInit()
{
  int8_t i;
  
  for(i=0; i<NBLOCKS; i++) {
    exist[i] = true;
  }
}

uint8_t BlocksLeft()
{
  int8_t i;
  int8_t leftBlocks;
  
  leftBlocks = 0;
  for (i=0; i<NBLOCKS; i++){
    if (exist[i])
      leftBlocks++;
  }
  return leftBlocks;
}


void BlocksDrawAll(){
  int8_t x,y;
  int8_t w,h;
  int8_t i;
  
  w = BLOCKWIDTH;
  h = BLOCKHEIGHT;
  i = 0;
  for(y = BLOCKTOP; y <= BLOCKBOTTOM; y += BLOCKHEIGHT) {
    for(x=0; x < SCRNWIDTH; x += BLOCKWIDTH) {
      if (exist[i++]) {
        FillRectRotAndMag(x, y, w, h, WHITE, 1);
      }
    }
  }
}

void BlocksEraseOne(int8_t block) {
  int8_t x,y;
  int8_t w,h;
  int8_t rows;
  
  w = BLOCKWIDTH;
  h = BLOCKHEIGHT;
  rows = SCRNWIDTH / BLOCKWIDTH;
  x = (block % rows) * BLOCKWIDTH;
  y = (block / rows) * BLOCKHEIGHT + BLOCKTOP;
  FillRectRotAndMag(x, y, w, h, BLACK, 1);
}

int8_t BlocksFind(uint8_t x, uint8_t y)
{
  if (x > SCRNWIDTH) {
    return -1;
  }
  if ((y < BLOCKTOP) || (y > BLOCKBOTTOM)) {
    return -1;
  }
  
  uint8_t rows = SCRNWIDTH / BLOCKWIDTH;
  return (y - BLOCKTOP) / BLOCKHEIGHT * rows + x / BLOCKWIDTH;
}

int8_t BlocksHit(uint8_t x, uint8_t y, int8_t *vx, int8_t *vy)
{
  int8_t block;
  
  block = BlocksFind(x + *vx, y);
  if ((block >= 0) && exist[block]) {
    *vx = -*vx;
    return block;
  }
  
  block = BlocksFind(x, y + *vy);
  if ((block >= 0) && exist[block]) {
    *vy = -*vy;
    return block;
  }
  
  block = BlocksFind(x + *vx, y + *vy);
  if ((block >= 0) && exist[block]) {
    *vx = -*vx;
    *vy = -*vy;
    return block;
  }
  
  return -1;
}

struct Ball_t
{
  uint8_t x,y;
  int8_t vx,vy;
};

void BallInit(struct Ball_t *ball)
{
  ball->x = SCRNWIDTH - 1;
  ball->y = BLOCKBOTTOM + BLOCKHEIGHT;
  ball->vx = -1;
  ball->vy = 1;
}

void BallDraw(struct Ball_t *ball, uint8_t color)
{
  FillRectRotAndMag(ball->x, ball->y, 1, 1, color, 0);
}

void RacketDraw(int16_t racket, uint8_t color)
{
  FillRectRotAndMag(racket, (RACKETLINE+1), RACKETSIZE, 1, color, 1);
}

enum GameStatus_t {
  GAME_LOST,
  GAME_ONGOING,
  GAME_RESTART,
  GAME_OVER
} gameStatus;

int16_t racket=(SCRNWIDTH-RACKETSIZE)>>1;
struct Ball_t ball;
int16_t score;
int8_t balls;
boolean demo_mode;

void game_initialize() {
  score = 0;
  balls = BALLS;
  print_score(SCORE_X, SCORE_Y, score);
  print_balls_left(LEFTBALLS_X, LEFTBALLS_Y, balls);
}

boolean game_restart()
{
  if ((ball.y > BLOCKBOTTOM) && (ball.vy >0)) {
    BlocksInit();
    BlocksDrawAll();
    return true;
  } else {
    return false;
  }
}

boolean game_ongoing()
{
  int8_t block;
  boolean blockErased;

  blockErased = false;
  do {
    block=BlocksHit(ball.x,ball.y,&ball.vx,&ball.vy);
    if (block >= 0) {
      exist[block] = false;
      BlocksEraseOne(block);
      blockErased = true;
      score += BLOCKLINES - block / (SCRNWIDTH/BLOCKWIDTH);
    }
  } while (block > 0); 
  return blockErased;
}

boolean game_new_ball()
{
  static uint8_t count = 0;
  if (count == 0) {
    count = 30;
  } else if (--count == 0) {
    BallInit(&ball);
    balls--;
    print_balls_left(LEFTBALLS_X, LEFTBALLS_Y, balls);
  }
  return (count == 0);
}

boolean game_new_game()
{
  static boolean pressed_A = false;
  static boolean pressed_B = false;
  boolean result;

  demo_mode = arduboy.pressed(LEFT_BUTTON);
  if (arduboy.pressed(RIGHT_BUTTON)) {
    arduboy.setFrameRate(60);
  } else {
    arduboy.setFrameRate(30);
  }
  
  result = false;
  if (pressed_A && arduboy.notPressed(A_BUTTON)) {
    pressed_A = false;
    sound_on = true;
    result = true;
  } else if (!pressed_A && arduboy.pressed(A_BUTTON)) {
    pressed_A = true;
  }

  if(!result) {
    if (pressed_B && arduboy.notPressed(B_BUTTON)) {
      pressed_B = false;
      sound_on = false;
      result = true;
    } else if (!pressed_B && arduboy.pressed(B_BUTTON)) {
      pressed_B = true;
    }
  }
  
  return result;
}

boolean game_move_ball()
{
  boolean result = true;
  
  BallDraw(&ball, BLACK);
  ball.x += ball.vx;
  if (ball.x >= SCRNWIDTH - 1 || ball.x < 1) {
    ball.vx = -ball.vx;
    ToneWithMute(NOTE_G3,80);
  }
  ball.y += ball.vy;
  if (ball.y == RACKETLINE) {
    if (ball.x >= racket && ball.x <= racket+RACKETSIZE) {
      ball.vy = -ball.vy;
      ToneWithMute(NOTE_C2,80);
    }
  }
  if (ball.y == 0) {
    ball.vy = -ball.vy;
    ToneWithMute(NOTE_G2,80);
  }
  if (ball.y > SCRNHEIGHT) {
    ToneWithMute(NOTE_A1,200);
    result = false;
  }
  BallDraw(&ball, WHITE);

  return result;
}

void game_move_racket()
{
  RacketDraw(racket, BLACK);

  if (arduboy.pressed(DOWN_BUTTON)) {
    racket += 2;
    if (racket > SCRNWIDTH - RACKETSIZE) {
      racket = SCRNWIDTH - RACKETSIZE;
    }
  }
  if (arduboy.pressed(UP_BUTTON)) {
    if (racket > 0) {
      racket -= min(racket, 2);
    }
  }

  if(demo_mode) {
    racket = ball.x - (RACKETSIZE / 2);
    if (arduboy.pressed(A_BUTTON) || arduboy.pressed(B_BUTTON)) {
      demo_mode = false;
    }
  }
  
  racket=constrain(racket, 0, SCRNWIDTH-RACKETSIZE);
  RacketDraw(racket,WHITE);  
}

void setup(void) {
  arduboy.begin();
  arduboy.clear();
  arduboy.drawRect(-1, 0, (SCRNHEIGHT + 2) * 2 + SCRNOFS_X, SCRNWIDTH * 2 + SCRNOFS_Y + 1, WHITE);
  arduboy.display();
  arduboy.setFrameRate(30);
  beep.begin();

  gameStatus = GAME_OVER;
  BallInit(&ball);
}

void loop()
{
  if (!(arduboy.nextFrame()))
    return;
  
  
  switch (gameStatus) {
    case GAME_RESTART:
      if (game_restart()) { // true when initialization is done
        gameStatus = GAME_ONGOING;
      }
      break;
      
    case GAME_ONGOING:
      if (game_ongoing()) { // true if the ball hits a block
        print_score(SCORE_X, SCORE_Y, score);
        ToneWithMute(NOTE_C3,40);
      }
      if (BlocksLeft() == 0) {
        gameStatus=GAME_RESTART;
      }
      break;
      
    case GAME_LOST:
      if (game_new_ball()) {
        if (balls > 0) {
          gameStatus = GAME_ONGOING;
        } else {
          print_game_over();
          gameStatus = GAME_OVER;
        }
      }
      break;
      
    case GAME_OVER:
      if (game_new_game()) { // true if A or B button is pushed
        gameStatus = GAME_RESTART;
        erase_game_over();
        game_initialize();
      }
  }

  if ((gameStatus == GAME_ONGOING) || (gameStatus == GAME_RESTART)) 
    if (!game_move_ball())
      gameStatus = GAME_LOST;
  game_move_racket();
  
  arduboy.display();
}
