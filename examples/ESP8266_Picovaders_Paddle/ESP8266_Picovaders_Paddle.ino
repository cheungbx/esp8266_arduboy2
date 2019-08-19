// picovaders-on-paddle
//
// based on the original source created by  boochow
// Please refer to the original source on github https://github.com/boochow/picovaders 
// 

// modified by cheungbx 2019 02 06 to add support for Paddle controller
// Sound will always be on now, 
// When the game starts:
// Press Button A will select to control the cannon movement using Left and Right buttons 
// Press Button B will select to control the cannon movement using the paddel (10K Analog potentiomenter)

/*
#define paddle1X A3
#define paddle2X A1
#define minPaddle 550
#define maxPaddle 1010
*/

#define paddle1X A0
#define minPaddle 1
#define maxPaddle 1024

bool usePaddle = false;

#include "Arduboy2.h"

#define MAXINT16 65535





static Arduboy2 arduboy;
BeepPin1 beep;
#define DRAW_PIXEL(x,y,c)    arduboy.drawPixel((x), (y), (c));
#define GET_PIXEL(x,y)      arduboy.getPixel((x), (y))
#define DRAW_BITMAP(x,y,b,w,h,c) arduboy.drawBitmap((x), (y), (b), (w), (h), (c))
#define DRAW_RECT(x,y,w,h,c)  arduboy.drawRect((x), (y), (w), (h), (c))
#define FILL_RECT(x,y,w,h,c)  arduboy.fillRect((x), (y), (w), (h), (c))
#define DRAW_H_LINE(x,y,l,c)  arduboy.drawFastHLine((x),(y),(l),(c))
#define DRAW_V_LINE(x,y,l,c)  arduboy.drawFastVLine((x),(y),(l),(c))
#define G_TXT_CURSOR(c,r)   arduboy.setCursor((c),(r))
#define G_PRINT(s)        arduboy.print((s))

#define KEY_PRESSED(k)    arduboy.pressed((k))
#define KEY_NOT_PRESSED(k)  arduboy.notPressed((k))


#define SOUND_ON      arduboy.audio.on()
#define SOUND_OFF     arduboy.audio.off()

#define SCRN_LEFT  19
#define SCRN_RIGHT 107
#define SCRN_BOTTOM 64
#define SCRN_FPS 60


// 3x6 pixel digit font
const uint8_t digit[10][3] PROGMEM = {
  { 0x3f, 0x21, 0x3f },
  { 0x00, 0x3f, 0x00 },
  { 0x3d, 0x25, 0x27 },
  { 0x25, 0x25, 0x3f },
  { 0x07, 0x04, 0x3f },
  { 0x27, 0x25, 0x3d },
  { 0x3f, 0x25, 0x3d },
  { 0x01, 0x01, 0x3f },
  { 0x3f, 0x25, 0x3f },
  { 0x27, 0x25, 0x3f },
};

void draw_digit(int16_t x, int16_t y, uint8_t n) {
  FILL_RECT(x, y, 3, 6, BLACK);
  DRAW_BITMAP(x, y, digit[n % 10], 3, 5, WHITE);
}

void print_int(int16_t x, int16_t y, uint16_t s, uint8_t digit) {
  x += 4 * (digit - 1);
  for (; digit > 0; digit--) {
    draw_digit(x, y, s % 10);
    x -= 4;
    s /= 10;
  }
}

// Sound utilities

#include "pitches.h"
#define TICKS2MSECS(t) ((t) * (1000 / SCRN_FPS))

enum SoundState {
  SoundReady,
  SoundPlaying,
  SoundDone
};

struct sound_fx_t {
  int16_t *data;    // array of frequencies, use -1 as end marker
  uint8_t gate_time;  // note gate time in msec
  uint8_t clk;    // ticks per note
  boolean loop;   // true for loop play
  uint8_t idx;
  uint8_t clk_cnt;
  enum SoundState status;
};

void sound_start(struct sound_fx_t *s) {
  if (s->status == SoundReady) {
    s->status = SoundPlaying;
    s->clk_cnt = 0;
    s->idx = 0;
  }
}

void sound_stop(struct sound_fx_t *s) {
  if (s->status == SoundPlaying)
    s->status = SoundDone;
}

void sound_restart(struct sound_fx_t *s) {
  if (s->status == SoundDone)
    s->status = SoundReady;
}

boolean sound_ready(struct sound_fx_t *s) {
  return (s->status == SoundReady);
}

void sound_set_tempo(struct sound_fx_t *s, uint8_t t) {
  s->clk = t;
}


void sound_play(struct sound_fx_t *s) {
  if (s->status == SoundPlaying) {
    int16_t t = s->gate_time - TICKS2MSECS(s->clk_cnt++);
    if (t > 0) {
    if (arduboy.audio.enabled())
        beep.tone(beep.freq((s->data[s->idx])), t/4 );
//      beep.tone(beep.freq((s->data[s->idx])), t/ (1000 / FRAME_RATE));
//      TONE(s->data[s->idx], t);
    }
    if (s->clk_cnt > s->clk) {
      s->clk_cnt = 0;
      if (s->data[++(s->idx)] < 0) {
        if (!s->loop)
//         beep.noTone();
          s->status = SoundDone;
        s->idx = 0;
      }
    }
  }
}



// true when a button pressed and released
boolean button_pressed(uint8_t button, boolean *b_state) {
  if (*b_state && KEY_NOT_PRESSED(button)) {
    *b_state = false;
    return true;
  }
  else if (!*b_state && KEY_PRESSED(button))
    *b_state = true;
  return false;
}


/////////////////////////////////////////////////////////
//  Common status definition
/////////////////////////////////////////////////////////

#define OBJ_ACTIVE 0  // object is visible and moving
#define OBJ_READY -1  // object is not active but ready to be active
#define OBJ_RECYCLE 1 // object is in the end of explosion, going to be ready


/////////////////////////////////////////////////////////
//  Invaders
/////////////////////////////////////////////////////////

#define ALN_ROW 5
#define ALN_COLUMN 10
#define ALN_NUM (ALN_ROW * ALN_COLUMN)
#define ALN_TOP 7
#define ALN_W 5
#define ALN_H 3
#define ALN_HSPACING 2
#define ALN_VSPACING 3
#define ALN_VMOVE 3
#define ALN_EXPLOSION 24

#define ALN_W_OF_ALIENS (ALN_COLUMN * (ALN_W + ALN_HSPACING) - ALN_HSPACING)
#define ALN_H_OF_ALIENS (ALN_ROW * (ALN_H + ALN_VSPACING) - ALN_VSPACING)

const uint8_t aln1_img[2][ALN_W] PROGMEM = {
  { 0x00, 0x03, 0x06, 0x03, 0x00 },
  { 0x00, 0x06, 0x03, 0x06, 0x00 }
};

const uint8_t aln2_img[2][ALN_W] PROGMEM = {
  { 0x03, 0x06, 0x03, 0x06, 0x03 },
  { 0x06, 0x03, 0x06, 0x03, 0x06 }
};

const uint8_t aln3_img[2][ALN_W] PROGMEM = {
  { 0x03, 0x05, 0x06, 0x05, 0x03 },
  { 0x06, 0x05, 0x02, 0x05, 0x06 }
};

const uint8_t ALN_hit_img[ALN_W] PROGMEM =
{ 0x05, 0x00, 0x05, 0x00, 0x05 };

struct aliens_t {
  boolean exist[ALN_NUM];
  uint8_t alive;    // number of aliens alive
  int16_t cur_left;
  int16_t cur_top;
  int16_t nxt_left;
  int16_t nxt_top;
  int16_t v;      // left move or right move
  uint8_t nxt_update; // which invader will be moved at the next tick
  uint8_t pose;   // index num for iv?_img[]
  int16_t bottom;   // be updated in aliens_draw
  boolean move_down;  // down-move flag (updated in aliens_draw)
  boolean touch_down; // an alien touched bottom line flag
  int8_t status;
  uint8_t hit_idx;  // which invader is been hit
} g_aliens;

void aliens_init(struct aliens_t *a, uint8_t stg) {
  const int8_t top[9] = { 0, 2, 4, 5, 5, 5, 6, 6, 6 };
  int16_t y = ALN_TOP + top[stg % 9] * ALN_H;
  for (uint8_t i = 0; i < ALN_NUM; i++)
    a->exist[i] = true;
  a->cur_left = SCRN_LEFT + 9;
  a->v = 1;
  a->nxt_left = a->cur_left + a->v;
  a->cur_top = y;
  a->nxt_top = y;
  a->bottom = 0;
  a->nxt_update = 0;
  a->pose = 0;
  a->move_down = false;
  a->touch_down = false;
  a->status = 0;
  a->hit_idx = 0;
  a->alive = ALN_NUM;
}

#define A_ROW(a) (ALN_ROW - 1 - (a) / ALN_COLUMN)
#define A_COL(a) ((a) % ALN_COLUMN)
#define A_IDX(r,c) ((c) + ALN_COLUMN * (ALN_ROW - (r) - 1))
#define A_XOFS(c) (c * (ALN_W + ALN_HSPACING))
#define A_YOFS(r) (r * (ALN_H + ALN_VSPACING))

void aliens_erase(struct aliens_t *a) {
  uint8_t row = A_ROW(a->nxt_update);
  uint8_t col = A_COL(a->nxt_update);
  int16_t x = a->cur_left + A_XOFS(col);
  int16_t y = a->cur_top + A_YOFS(row);
  FILL_RECT(x, y, ALN_W, ALN_H, BLACK);
}

void aliens_draw(struct aliens_t *a) {
  if (!a->exist[a->nxt_update])
    return;

  uint8_t row = A_ROW(a->nxt_update);
  uint8_t col = A_COL(a->nxt_update);
  int16_t x = a->nxt_left + col * (ALN_W + ALN_HSPACING);
  int16_t y = a->nxt_top + row * (ALN_H + ALN_VSPACING);
  uint8_t *img;

  switch (row) {
  case 0: img = (uint8_t *)aln1_img[a->pose]; break;
  case 1:
  case 2: img = (uint8_t *)aln2_img[a->pose]; break;
  case 3:
  case 4: img = (uint8_t *)aln3_img[a->pose]; break;
  }
  DRAW_BITMAP(x, y, img, ALN_W, ALN_H, WHITE);

  if (((x <= SCRN_LEFT) && (a->v < 0)) ||
    ((x >= SCRN_RIGHT - ALN_W) && (a->v > 0)))
    a->move_down = true;

  a->bottom = max(a->bottom, y + ALN_H);
}

void aliens_hitimg_erase_alien(struct aliens_t *a) {
  uint8_t row = A_ROW(a->hit_idx);
  uint8_t col = A_COL(a->hit_idx);
  int16_t x;
  int16_t y;

  if (a->hit_idx >= a->nxt_update) {
    x = a->cur_left;
    y = a->cur_top;
  }
  else {
    x = a->nxt_left;
    y = a->nxt_top;
  }
  x += A_XOFS(col);
  y += A_YOFS(row);;
  FILL_RECT(x, y, ALN_W, ALN_H, BLACK);
}

void aliens_hitimg_erase(struct aliens_t *a) {
  uint8_t row = A_ROW(a->hit_idx);
  uint8_t col = A_COL(a->hit_idx);

  int16_t x = a->nxt_left + A_XOFS(col);
  int16_t y = a->nxt_top + A_YOFS(row);
  FILL_RECT(x, y, ALN_W, ALN_H, BLACK);
}

void aliens_hitimg_draw(struct aliens_t *a) {
  uint8_t row = A_ROW(a->hit_idx);
  uint8_t col = A_COL(a->hit_idx);
  int16_t x = a->nxt_left + A_XOFS(col);
  int16_t y = a->nxt_top + A_YOFS(row);
  DRAW_BITMAP(x, y, ALN_hit_img, ALN_W, ALN_H, WHITE);
}

void aliens_move(struct aliens_t *a) {
  uint8_t u = a->nxt_update;
  do {
    u++;
  } while ((u < ALN_NUM) && (!a->exist[u]));

  if (u < ALN_NUM) {
    a->nxt_update = u;
  }
  else {  // all aliens have moved
    if (a->bottom >= SCRN_BOTTOM)
      a->touch_down = true;
    for (u = 0; (u < ALN_NUM) && (!a->exist[u]); u++);
    a->nxt_update = u;
    a->pose = (a->pose + 1) % 2;
    a->cur_left = a->nxt_left;
    a->nxt_left += a->v;
    a->cur_top = a->nxt_top;
    if (a->move_down) { // this flag had been set in aliens_draw()
      a->move_down = false;
      a->v = -a->v;
      a->nxt_top = a->cur_top + ALN_VMOVE;
      a->nxt_left = a->cur_left + a->v;
    }
  }
}

void aliens_hit(struct aliens_t *a, uint8_t idx) {
  a->exist[idx] = false;
  a->hit_idx = idx;
  a->status = ALN_EXPLOSION;
  a->alive--;
}

int8_t aliens_hit_test(struct aliens_t *a, int16_t x, int16_t y) { // returns alien index
  int16_t lmin = min(a->cur_left, a->nxt_left);
  int16_t tmin = min(a->cur_top, a->nxt_top);

  if ((x > lmin) && (x < lmin + ALN_W_OF_ALIENS) &&
    (y > tmin) && (y < tmin + ALN_H_OF_ALIENS)) {
    for (uint8_t i = 0; i < a->nxt_update; i++)
      if (a->exist[i]) {
        int16_t ax = a->nxt_left + A_XOFS(A_COL(i));
        int16_t ay = a->nxt_top + A_YOFS(A_ROW(i));
        if ((x >= ax) && (x < ax + ALN_W) && (y >= ay) && (y < ay + ALN_H))
          return i;
      }

    for (uint8_t i = a->nxt_update; i < ALN_NUM; i++)
      if (a->exist[i]) {
        int16_t ax = a->cur_left + A_XOFS(A_COL(i));
        int16_t ay = a->cur_top + A_YOFS(A_ROW(i));
        if ((x >= ax) && (x < ax + ALN_W) && (y >= ay) && (y < ay + ALN_H))
          return i;
      }
  }
  return -1; // no aliens hits
}

void aliens_update(struct aliens_t *a) {
  switch (a->status) {
  case OBJ_ACTIVE:
    aliens_erase(a);
    aliens_draw(a);
    aliens_move(a);
    break;
  case ALN_EXPLOSION:
    aliens_hitimg_erase_alien(a);
    aliens_hitimg_draw(a);
    a->status--;
    break;
  case OBJ_RECYCLE:
    aliens_hitimg_erase(a);
    a->status = OBJ_ACTIVE;
    break;
  default:
    a->status--;
  }
}

/////////////////////////////////////////////////////////
//  UFO
/////////////////////////////////////////////////////////

#define UFO_TOP 0
#define UFO_BOTTOM 3
#define UFO_W 7
#define UFO_H 3
#define UFO_INTERVAL (25 * SCRN_FPS)
#define UFO_WAIT (SCRN_FPS / 30)
#define UFO_EXPLOSION (2 * SCRN_FPS)

const uint8_t ufo_img[UFO_W] PROGMEM =
{ 0x06, 0x05, 0x07, 0x03, 0x07, 0x05, 0x06 };

const uint8_t ufo_score_img[4][UFO_W] PROGMEM = {
  { 0x00, 0x05, 0x03, 0x00, 0x06, 0x06, 0x00 },
  { 0x07, 0x00, 0x06, 0x06, 0x00, 0x06, 0x06 },
  { 0x07, 0x00, 0x05, 0x03, 0x00, 0x06, 0x06 },
  { 0x05, 0x07, 0x00, 0x06, 0x06, 0x06, 0x06 }
};

struct ufo_t {
  uint16_t wait_ctr;
  uint16_t x;
  int8_t v;
  int8_t l_or_r;
  int8_t status;
  uint8_t img_idx;  // index for ufo_score_img
} g_ufo;

void ufo_init(struct ufo_t *u) {
  u->wait_ctr = 0;
  u->status = OBJ_READY;
}

void ufo_appear(struct ufo_t *u) {

  if ((g_aliens.alive > 7) && (u->wait_ctr++ > UFO_INTERVAL)) {

    u->status = OBJ_ACTIVE;
    u->wait_ctr = 0;

    switch (u->l_or_r) {
    case 0:
      u->v = 1;
      u->x = SCRN_LEFT;
      break;
    default:
      u->v = -1;
      u->x = SCRN_RIGHT - UFO_W;
    }
  }
}

void ufo_draw(struct ufo_t *u) {
  DRAW_BITMAP(u->x, UFO_TOP, ufo_img, UFO_W, UFO_H, WHITE);
}

void ufo_erase(struct ufo_t *u) {
  FILL_RECT(u->x, UFO_TOP, UFO_W, UFO_H, BLACK);
}

void ufo_move(struct ufo_t *u) {
  ufo_erase(u);
  u->x += u->v;
  if ((u->x <= SCRN_LEFT) || (u->x >= SCRN_RIGHT - UFO_W))
    u->status = OBJ_RECYCLE;
  ufo_draw(u);
}

void ufo_hit(struct ufo_t *u, uint8_t idx) {
  u->status = UFO_EXPLOSION;
  ufo_erase(u);
  u->img_idx = idx;
}

boolean ufo_hit_test(struct ufo_t *u, uint16_t x, uint16_t y) {
  if ((u->status == OBJ_ACTIVE) &&
    (y < UFO_BOTTOM) &&
    (x >= u->x) && (x < u->x + UFO_W)) {

    u->status = UFO_EXPLOSION;
    ufo_erase(u);
    return true;
  }
  else
    return false;
}

void ufo_update(struct ufo_t *u) {
  switch (u->status) {
  case OBJ_ACTIVE:
    if (u->wait_ctr++ > UFO_WAIT) {
      u->wait_ctr = 0;
      ufo_move(u);
    }
    break;
  case OBJ_READY:
    ufo_appear(u);
    break;
  case OBJ_RECYCLE:
    ufo_erase(u);
    u->status = OBJ_READY;
    break;
  case UFO_EXPLOSION:
    DRAW_BITMAP(u->x, UFO_TOP, ufo_score_img[u->img_idx], UFO_W, UFO_H, WHITE);
  default:
    u->status--;
  }
}

/////////////////////////////////////////////////////////
//  Bunkers
/////////////////////////////////////////////////////////

#define BUNKER_TOP 52
#define BUNKER_BOTTOM (BUNKER_TOP + BUNKER_H)
#define BUNKER_W 8
#define BUNKER_H 6
#define BUNKER_LEFT 13
#define BUNKER_SPACE 10
#define BUNKER_NUM 4

const uint8_t bunker_img[BUNKER_W] PROGMEM =
{ 0x3c, 0x3e, 0x1f, 0x0f, 0x0f, 0x1f, 0x3e, 0x3c };

void draw_bunkers() {
  uint16_t x = BUNKER_LEFT + SCRN_LEFT;

  for (uint8_t i = 0; i < BUNKER_NUM; i++) {
    DRAW_BITMAP(x, BUNKER_TOP, bunker_img, BUNKER_W, BUNKER_H, WHITE);
    x += BUNKER_W + BUNKER_SPACE;
  }
}

/////////////////////////////////////////////////////////
//  Bombs
/////////////////////////////////////////////////////////

#define BOMB_NUM 3
#define BOMB_WAIT (SCRN_FPS / 20)
#define BOMB_V 2
#define BOMB_EXPLOSION (SCRN_FPS / 3)
#define BOMB_SHOT_INTERVAL SCRN_FPS

struct bomb_t {
  int16_t x;
  int16_t y;
  int8_t wait_ctr;
  int8_t status;
  void(*draw_func)(struct bomb_t *b, uint8_t color);
  void(*shot_func)(struct bomb_t *b);
} bombs[BOMB_NUM];

void bomb_draw(struct bomb_t *b, uint8_t color) {
  b->draw_func(b, color);
}

void bomb_draw_a(struct bomb_t *b, uint8_t color) {
  DRAW_PIXEL(b->x + ((1 + b->y / BOMB_V) % 2), b->y - 3, color);
  DRAW_PIXEL(b->x + (b->y / BOMB_V % 2), b->y - 2, color);
  DRAW_PIXEL(b->x + ((1 + b->y / BOMB_V) % 2), b->y - 1, color);
  DRAW_PIXEL(b->x + (b->y / BOMB_V % 2), b->y, color);
}

void bomb_draw_b(struct bomb_t *b, uint8_t color) {
  DRAW_PIXEL(b->x + 1, b->y - 1 - (b->y / BOMB_V % 2), color);
  DRAW_PIXEL(b->x, b->y - 1 - (b->y / BOMB_V % 2), color);
  DRAW_PIXEL(b->x + 1, b->y, color);
  DRAW_PIXEL(b->x, b->y, color);
}

void bomb_draw_c(struct bomb_t *b, uint8_t color) {
  DRAW_PIXEL(b->x + ((1 + b->y) / BOMB_V % 2), b->y - 2, color);
  DRAW_PIXEL(b->x + 1, b->y - 1, color);
  DRAW_PIXEL(b->x, b->y - 1, color);
  DRAW_PIXEL(b->x + (b->y / BOMB_V % 2), b->y, color);
}

int8_t search_alien(struct aliens_t *a, int8_t x) {
  int8_t a_idx = -1;
  int8_t y = a->bottom - ALN_H;

  for (uint8_t i = 0; i < ALN_ROW; i++) {
    a_idx = aliens_hit_test(a, x + ALN_W / 2, y);
    if (a_idx >= 0)
      break;
    else
      y -= ALN_VSPACING + ALN_H;
  }
  return a_idx;
}

void bomb_shot_from(struct bomb_t *b, uint8_t a_idx) {
  int8_t i = a_idx;
  while ((i -= ALN_COLUMN) >= 0)
    if (g_aliens.exist[i])
      a_idx = i;
  b->status = OBJ_ACTIVE;
  b->x = g_aliens.nxt_left + A_XOFS(A_COL(a_idx)) + ALN_W / 2 - 1;
  b->y = g_aliens.nxt_top + A_YOFS(A_ROW(a_idx)) + ALN_H + ALN_VSPACING;
}

void bomb_shot_a(struct bomb_t *b) {
  bomb_shot_from(b, g_aliens.nxt_update);
}

void bomb_shot_c(struct bomb_t *b) {
  int8_t a_idx = (g_aliens.nxt_update + ALN_COLUMN + ALN_NUM) % ALN_NUM;
  if (!g_aliens.exist[a_idx])
    return;
  bomb_shot_from(b, a_idx);
}

void bomb_init(struct bomb_t *b) {
  b->wait_ctr = 0;
  b->status = BOMB_EXPLOSION - 1;
  b->y = SCRN_BOTTOM;
  b->draw_func = bomb_draw_a;
  b->shot_func = bomb_shot_a;
}

void bomb_hitimg_draw(struct bomb_t *b, uint8_t color) {
  DRAW_PIXEL(b->x, b->y, color);
  DRAW_PIXEL(b->x + 1, b->y, color);
  DRAW_PIXEL(b->x, b->y - 1, color);
  DRAW_PIXEL(b->x + 1, b->y - 1, color);
}

boolean bomb_bunker_test(struct bomb_t *b) {
  if ((b->y < BUNKER_BOTTOM) && (b->y > BUNKER_TOP))
    if (GET_PIXEL(b->x, b->y) ||
      GET_PIXEL(b->x + 1, b->y) ||
      GET_PIXEL(b->x, b->y - 1) ||
      GET_PIXEL(b->x + 1, b->y - 1)) {
      return true;
    }
  return false;
}

void bomb_move(struct bomb_t *b) {
  if (b->wait_ctr++ > BOMB_WAIT) {
    b->wait_ctr = 0;

    bomb_draw(b, BLACK);
    b->y += BOMB_V;
    if (b->y > SCRN_BOTTOM)
      b->status = BOMB_EXPLOSION - 1;
    else if (bomb_bunker_test(b))
      b->status = BOMB_EXPLOSION;
    else
      bomb_draw(b, WHITE);
  }
}

void bomb_shot(struct bomb_t *b) {
  static uint8_t last_shot = 0;

  if ((++b->wait_ctr > BOMB_SHOT_INTERVAL) && (++last_shot > BOMB_SHOT_INTERVAL / BOMB_NUM)) {
    b->shot_func(b);
    last_shot = 0;
  }
}

/////////////////////////////////////////////////////////
//  Laser
/////////////////////////////////////////////////////////

#define LASER_V 2
#define LASER_WAIT 0
#define LASER_CNTR_INIT 14  // to adjust points of ufo 
#define LASER_CNTR_MOD 15
#define LASER_EXPLOSION ALN_EXPLOSION

const uint8_t laser_explosion_img[3] PROGMEM = { 0x05, 0x02, 0x05 };

struct laser_t {
  int16_t x;
  int16_t y;
  uint8_t shot_ctr; // for calculating ufo score
  int8_t status;
};

void laser_init(struct laser_t *l) {
  l->status = OBJ_READY;
  l->shot_ctr = LASER_CNTR_INIT;
}

void laser_draw_explosion(struct laser_t *l, uint8_t color) {
  DRAW_BITMAP(l->x - 1, l->y, laser_explosion_img, 3, 3, color);
}

boolean laser_bunker_test(struct laser_t *l) {
  if ((l->y < BUNKER_BOTTOM) && (l->y > BUNKER_TOP) && (l->y > g_aliens.bottom))
    if (GET_PIXEL(l->x, l->y) || (GET_PIXEL(l->x, l->y + 1)))
      return true;
  return false;
}

void laser_move(struct laser_t *l) {
  if (l->status == OBJ_ACTIVE) {
    l->y -= LASER_V;
    if (l->y <= 0)
      l->status = LASER_EXPLOSION;
  }
}

void laser_draw(struct laser_t *l, uint8_t color) {
  DRAW_PIXEL(l->x, l->y, color);
  DRAW_PIXEL(l->x, l->y + 1, color);
}

void laser_shoot(struct laser_t *l, uint16_t x, uint16_t y) {
  if ((l->status == OBJ_READY) && (g_aliens.status == OBJ_ACTIVE))
    if (KEY_PRESSED(A_BUTTON) || KEY_PRESSED(B_BUTTON)) {
      l->status = OBJ_ACTIVE;
      l->x = x;
      l->y = y;
      l->shot_ctr = (l->shot_ctr + 1) % LASER_CNTR_MOD;
      g_ufo.l_or_r = l->shot_ctr & 0x1;
    }
}

uint8_t laser_ufo_point(struct laser_t *l) {
  uint8_t idx;

  switch (l->shot_ctr) {
  case 0:
  case 1:
  case 6:
  case 11:
    idx = 0;
    break;

  case 3:
  case 12:
    idx = 2;
    break;

  case 7:
    idx = 3;
    break;

  default:
    idx = 1;
    break;
  }
  return idx;
}

uint8_t laser_update(struct laser_t *l) { // returns point if laser hits
  const uint8_t alien_points[5] = { 3, 2, 2, 1, 1 };
  const uint8_t ufo_points[4] = { 5, 10, 15, 30 };
  int8_t hit;
  uint8_t result = 0;

  switch (l->status) {
  case OBJ_READY: // laser shot in cannon_update()
    break;
  case OBJ_ACTIVE:
    laser_draw(l, BLACK);
    laser_move(l);

    hit = aliens_hit_test(&g_aliens, l->x, l->y);
    if (hit >= 0) {
      aliens_hit(&g_aliens, hit);
      result = alien_points[A_ROW(hit)];
      l->status = OBJ_READY;
    }
    else if (ufo_hit_test(&g_ufo, l->x, l->y)) {
      uint8_t idx = laser_ufo_point(l);
      ufo_hit(&g_ufo, idx);
      result = ufo_points[idx];
      l->status = OBJ_READY;
    }
    else if (laser_bunker_test(l)) {
      l->status = LASER_EXPLOSION;
    }
    else {
      for (uint8_t i = 0; i < BOMB_NUM; i++) {
        if ((bombs[i].status == OBJ_ACTIVE) &&
          ((l->x == bombs[i].x) || (l->x == 1 + bombs[i].x)) &&
          ((l->y == bombs[i].y) || (l->y == 1 + bombs[i].y)))
          l->status = LASER_EXPLOSION;
      }
    }
    if (l->status == OBJ_ACTIVE)
      laser_draw(l, WHITE);
    break;
  case LASER_EXPLOSION:
    laser_draw(l, BLACK);
    laser_draw_explosion(l, WHITE);
    l->status--;
    break;
  case OBJ_RECYCLE:
    laser_draw_explosion(l, BLACK);
    l->status = OBJ_READY;
    break;
  default:
    l->status--;
  }

  return result;
}

/////////////////////////////////////////////////////////
//  Cannon
/////////////////////////////////////////////////////////

#define CANNON_W 5
#define CANNON_H 3
#define CANNON_V 1
#define CANNON_C_OFS 2
#define CANNON_WAIT 0
#define CANNON_TOP 61
#define CANNON_EXPLOSION 127

const uint8_t cannon_img[CANNON_W] PROGMEM =
{ 0x06, 0x06, 0x07, 0x06, 0x06 };

const uint8_t cannon_hit_img[2][CANNON_W] PROGMEM = {
  { 0x01, 0x05, 0x06, 0x06, 0x05 },
  { 0x04, 0x07, 0x04, 0x05, 0x02 }
};

struct cannon_t {
  uint16_t x;
  uint8_t left;
  struct laser_t laser;
  uint8_t wait_ctr;
  int8_t status;
} g_cannon;

void cannon_init(struct cannon_t *c) {
  c->x = 32;
  c->wait_ctr = 0;
  laser_init(&c->laser);
  c->status = OBJ_ACTIVE;
}

void cannon_erase(struct cannon_t *c) {
  FILL_RECT(c->x, CANNON_TOP, CANNON_W, CANNON_H, BLACK);
}

void cannon_draw(struct cannon_t *c) {
  DRAW_BITMAP(c->x, CANNON_TOP, cannon_img, CANNON_W, CANNON_H, WHITE);
}

void cannon_move(struct cannon_t *c) {
  cannon_erase(c);
  if (KEY_PRESSED(RIGHT_BUTTON)) {
    c->x += CANNON_V;
  }
  if (KEY_PRESSED(LEFT_BUTTON)) {
    c->x -= CANNON_V;
  }
  
// changed to control cannon by paddle - cheungbx
// modified by cheungbx 2019 02 06 to add support for Paddle controller
if (usePaddle)
    c->x  = map (constrain (analogRead(paddle1X), minPaddle, maxPaddle), minPaddle, maxPaddle,SCRN_LEFT, SCRN_RIGHT - CANNON_W);
else
    c->x = constrain(c->x, SCRN_LEFT, SCRN_RIGHT - CANNON_W);

  cannon_draw(c);
}

void cannon_hit(struct cannon_t *c) {
  c->status = CANNON_EXPLOSION;
  switch (c->laser.status) {
  case OBJ_ACTIVE: laser_draw(&c->laser, BLACK); break;
  case OBJ_READY: break;
  default:
    laser_draw_explosion(&c->laser, BLACK);
  }
  c->laser.status = OBJ_RECYCLE;
}

boolean cannon_hit_test(struct cannon_t *c, uint16_t x, uint16_t y) {
  if ((y > CANNON_TOP) && (y < CANNON_TOP + CANNON_H) &&
    (x >= c->x) && (x <= c->x + CANNON_W)) {
    return true;
  }
  else
    return false;
}

void cannon_update(struct cannon_t *c) {

  switch (c->status) {
  case OBJ_ACTIVE:
    if (c->wait_ctr++ > CANNON_WAIT) {
      c->wait_ctr = 0;
      cannon_move(c);
      laser_shoot(&c->laser, c->x + CANNON_C_OFS, CANNON_TOP - 1);
    }
    break;
  case OBJ_RECYCLE:
    laser_draw(&c->laser, BLACK);
    c->status = OBJ_READY;
    break;
  default: // CANNON_EXPLOSION
    cannon_erase(c);
    uint8_t *img = (uint8_t *)cannon_hit_img[(c->status >> 2) % 2];
    DRAW_BITMAP(c->x, CANNON_TOP, img, CANNON_W, CANNON_H, WHITE);
    c->status--;
    break;
  }
}

void bomb_shot_b(struct bomb_t *b) {
  int8_t a_idx = search_alien(&g_aliens, g_cannon.x + 2);
  if (a_idx >= 0)
    bomb_shot_from(b, a_idx);
}

void bomb_update(struct bomb_t *b) {
  switch (b->status) {
  case OBJ_READY:
    bomb_shot(b);
    if (b->status == OBJ_ACTIVE) {
      bomb_draw(b, WHITE);
    }
    break;
  case OBJ_ACTIVE:
    bomb_move(b);
    if (cannon_hit_test(&g_cannon, b->x + 1, b->y)) {
      cannon_hit(&g_cannon);
    }
    break;
  case OBJ_RECYCLE:
    bomb_hitimg_draw(b, BLACK);
    b->status = OBJ_READY;
    b->wait_ctr = 0;
    break;
  case BOMB_EXPLOSION:
    bomb_hitimg_draw(b, WHITE);
  default:
    b->status--;
  }
}

/////////////////////////////////////////////////////////
//  Game
/////////////////////////////////////////////////////////

enum game_status_t {
  GameLost,
  GameOnGoing,
  GameRestart,
  GameOver,
  GameTitle
};

struct game_t {
  int16_t score;
  int16_t high_score;
  uint8_t stage;
  uint8_t left;
  game_status_t status;
}g_game;

#define SC_X 109  // score display location
#define SC_Y 20
#define HSC_Y 30  // high-score display location

void print_score() {
  print_int(SC_X, SC_Y, g_game.score, 4);
}

void print_high_score() {
  print_int(SC_X, HSC_Y, g_game.high_score, 4);
}

void print_cannon_left(uint8_t n) {
  draw_digit(13, 20, n);
}

#define TITLE_LEFT 31
#define TITLE_TOP 28
#define TITLE_W 64
#define TITLE_H 8

void game_title_draw() {
  static const uint8_t title_img[TITLE_W] PROGMEM = {
    0xf8, 0xfc, 0x24, 0x24, 0x3c, 0x18, 0,
    0x3d, 0x3d, 0, 0,
    0x18, 0x3c, 0x24, 0x24, 0x24, 0,
    0x18, 0x3c, 0x24, 0x24, 0x3c, 0x18, 0,
    0x04, 0x1c, 0x38, 0x20, 0x38, 0x1c, 0x04,
    0x10, 0x38, 0x2a, 0x2a, 0x2e, 0x3c, 0x20,
    0x18, 0x3c, 0x24, 0x24, 0x27, 0x3f, 0x20, 0,
    0x1c, 0x3e, 0x2a, 0x2a, 0x2e, 0x0c, 0,
    0x04, 0x3c, 0x3e, 0x02, 0x02,
    0x24, 0x2e, 0x2a, 0x2a, 0x3a, 0x12
  };
  DRAW_BITMAP(TITLE_LEFT, TITLE_TOP, title_img, TITLE_W, TITLE_H, WHITE);
  
}

void game_title_alien_draw(int16_t y0, uint8_t c) {
  for (uint8_t i = 0; i < 8; i++) {
    int16_t x = TITLE_LEFT - 4 + ALN_W * i;
    int16_t y = y0 - 20 + ALN_H * i;
    switch (i / 3) {
    case 0: DRAW_BITMAP(x, y, aln3_img[y0 / 8 % 2], ALN_W, ALN_H, c); break;
    case 1: DRAW_BITMAP(x, y, aln2_img[y0 / 8 % 2], ALN_W, ALN_H, c); break;
    default: DRAW_BITMAP(x, y, aln1_img[y0 / 8 % 2], ALN_W, ALN_H, c);
    }
  }
  for (uint8_t i = 8; i < 15; i++) {
    int16_t x = TITLE_LEFT - 4 + ALN_W * i;
    int16_t y = y0 - 20 + ALN_H * (14 - i);
    switch ((14 - i) / 3) {
    case 0: DRAW_BITMAP(x, y, aln3_img[y0 / 8 % 2], ALN_W, ALN_H, c); break;
    case 1: DRAW_BITMAP(x, y, aln2_img[y0 / 8 % 2], ALN_W, ALN_H, c); break;
    default: DRAW_BITMAP(x, y, aln1_img[y0 / 8 % 2], ALN_W, ALN_H, c);
    }
  }
  int16_t x1 = UFO_W * 23 - y0;
  int16_t x2 = y0 - UFO_W * 6;
  DRAW_BITMAP(x1, TITLE_TOP, ufo_img, UFO_W, UFO_H, c);
  DRAW_BITMAP(x2, TITLE_TOP + UFO_H, ufo_img, UFO_W, UFO_H, c);
}

#define SND_LASER 0
#define SND_ALIEN 1
#define SND_UFOFLY 2
#define SND_UFOHIT 3
#define SND_CANNON 4
#define SND_1UP 5

#define SOUND_FX_NUM 6

int16_t snd1[5] = {   // laser
  NOTE_B6, NOTE_C7, 0, NOTE_B6, -1
};

int16_t snd2[25] = {  // alien explosion
  NOTE_G5, NOTE_FS5, NOTE_F5, NOTE_E5, 0,
  NOTE_A6, NOTE_GS6, NOTE_G6, NOTE_FS6, NOTE_F6,
  NOTE_E6, NOTE_DS6, NOTE_D6, NOTE_CS6, NOTE_C6,
  NOTE_B5, NOTE_AS5,
  NOTE_E6, NOTE_DS6, NOTE_D6, NOTE_CS6,-1
};

int16_t snd3[13] = {  // ufo flying
  120, 241, 570, 1020, 1470, 1800, 1920, 1800, 1470, 1020, 570, 241, -1
};

int16_t snd4[13] = {  // ufo explosion
  120, 180, 345, 570, 795, 960, 1020, 960, 795, 570, 345, 180, -1
};

int16_t snd5[3] = {   // cannon explosion
  NOTE_B2, 0, -1
};

int16_t snd6[18] = {   // cannon explosion
  1000, 0, 1000, 0, 1000, 0, 1000, 0, 1000, 0, 1000, 0, 1000, 0, 1000, 0, 1000, -1
};

struct sound_fx_t snd_fx[SOUND_FX_NUM] = {
  { snd1, 20, 0, false, 0, 0, SoundReady }, // data, gate_time, clk, repeat, (internal vars)
  { snd2, 20, 0, false, 0, 0, SoundReady },
  { snd3, 20, 0, true, 0, 0, SoundReady },
  { snd4, 20, 0, true, 0, 0, SoundReady },
  { snd5, 20, 0, true, 0, 0, SoundReady },
  { snd6, 100, 6, false, 0, 0, SoundReady }
};

int16_t bgm_s[5] = {
  NOTE_E3, NOTE_D3, NOTE_C3, NOTE_B2, -1
};

struct sound_fx_t bgm = { bgm_s, 70, 52, true, 0, false };

#define BGM_STEP_NUM 10

uint8_t bgm_speed[BGM_STEP_NUM] = { 52, 44, 36, 28, 23, 18, 13, 10, 7, 5 };
uint8_t bgm_alien_num[BGM_STEP_NUM] = { 50, 40, 30, 22, 16, 12,  8,  4, 2, 1 };

void game_sound_main() {
  switch (g_aliens.status) {
  case OBJ_ACTIVE:
    for (uint8_t i = 0; i < BGM_STEP_NUM; i++)
      if (g_aliens.alive == bgm_alien_num[i])
        sound_set_tempo(&bgm, bgm_speed[i]);
    sound_start(&bgm);
    break;
  case ALN_EXPLOSION - 1:
    if (sound_ready(&snd_fx[SND_ALIEN]))
      sound_start(&snd_fx[SND_ALIEN]);
    break;
  default:
    sound_restart(&snd_fx[SND_ALIEN]);
    break;
  }

  switch (g_ufo.status) {
  case OBJ_ACTIVE: sound_start(&snd_fx[SND_UFOFLY]); break;
  case UFO_EXPLOSION - 1:
    sound_stop(&snd_fx[SND_UFOFLY]);
    sound_start(&snd_fx[SND_UFOHIT]);
    break;
  case OBJ_RECYCLE:
    sound_stop(&snd_fx[SND_UFOFLY]);
    sound_restart(&snd_fx[SND_UFOFLY]);
    sound_stop(&snd_fx[SND_UFOHIT]);
    sound_restart(&snd_fx[SND_UFOHIT]);
    break;
  default:
    break;
  }

  switch (g_cannon.status) {
  case CANNON_EXPLOSION - 1: sound_start(&snd_fx[SND_CANNON]); break;
  case OBJ_RECYCLE:
    sound_stop(&snd_fx[SND_CANNON]);
    sound_restart(&snd_fx[SND_CANNON]);
    break;
  default:
    break;
  }

  switch (g_cannon.laser.status) {
  case OBJ_ACTIVE:
    if (sound_ready(&snd_fx[SND_LASER]))
      sound_start(&snd_fx[SND_LASER]);
    break;
  case OBJ_RECYCLE: sound_stop(&snd_fx[SND_LASER]); break;
  case OBJ_READY: sound_restart(&snd_fx[SND_LASER]); break;
  default: break;
  }

  switch (g_game.status) {
  case GameOnGoing:
    sound_play(&bgm);
    for (uint8_t i = 0; i < SOUND_FX_NUM; i++)
      sound_play(&snd_fx[i]);
    break;
  default:
    sound_stop(&bgm);
    sound_restart(&bgm);
    for (uint8_t i = 0; i < SOUND_FX_NUM; i++) {
      sound_stop(&snd_fx[i]);
      sound_restart(&snd_fx[i]);
    }
  }
}

void draw_game_field() {
  DRAW_V_LINE(SCRN_LEFT - 1, 0, 64, WHITE);
  DRAW_V_LINE(SCRN_RIGHT, 0, 64, WHITE);

  print_int(SC_X + 16, SC_Y, 0, 1);
  print_int(SC_X + 16, HSC_Y, 0, 1);
  print_high_score();

  DRAW_BITMAP(4, 22, cannon_img, CANNON_W, CANNON_H, WHITE);
  print_cannon_left(0);
}

void game_initialize() {
  g_game.score = 0;
  g_game.high_score = 0;
  g_game.stage = 0;
  g_game.left = 3;
  arduboy.clear(); //cheungbx - clear screen for game to begin.  
}

boolean game_new_game() {
  static boolean pressed_A = false;
  static boolean pressed_B = false;

//cheungbx - display instructions to select button/paddle/demo
   arduboy.setCursor(0, 0);
   arduboy.print("Picovaders on Paddle");
   arduboy.setCursor(0, 10);
   arduboy.print("cheungbx 2019Feb06");
   arduboy.setCursor(20, 40);
   arduboy.print("B for Paddle");   
   arduboy.setCursor(20, 55);
   arduboy.print("A for Buttons");
   arduboy.display();
//cheungbx - display instructions to select button/paddle/demo  

// modified by cheungbx 2019 02 06 to add support for Paddle controller
// Sound will always be on now, 
// When the game starts:
// Press Button A will select to control the cannon movement using Left and Right buttons 
// Press Button B will select to control the cannon movement using the paddel (10K Analog potentiomenter)
  if (button_pressed(A_BUTTON, &pressed_A)) {
// cheungbx - now A button means play the game using D-PAD
      usePaddle = false;
      SOUND_ON;
      return true;

  }
  else if (button_pressed(B_BUTTON, &pressed_B)) {
// cheungbx - now B button means play the game using Paddles, sound will always be ON.
//    SOUND_OFF;
      SOUND_ON;  // sound will always be ON.
      usePaddle = true;
// enable ADC for paddeleX which is powered down by default by Arduboy library - cheungbx
//      power_adc_enable();
      return true;
  }
  else
    return false;
}

void bomb_init_all() {
  bomb_init(&bombs[0]);
  bombs[1].draw_func = bomb_draw_a;
  bombs[0].draw_func = bomb_draw_a;

  bomb_init(&bombs[1]);
  bombs[1].draw_func = bomb_draw_b;
  bombs[1].shot_func = bomb_shot_b;

  bomb_init(&bombs[2]);
  bombs[2].draw_func = bomb_draw_c;
  bombs[2].shot_func = bomb_shot_c;
}

void game_restart() {
  FILL_RECT(SCRN_LEFT, 0, SCRN_RIGHT - SCRN_LEFT, SCRN_BOTTOM, BLACK);

  cannon_init(&g_cannon);
  aliens_init(&g_aliens, g_game.stage);
  ufo_init(&g_ufo);
  draw_bunkers();

  bomb_init_all();
}

#define SCORE_1UP 150

void game_main() {
  cannon_update(&g_cannon);
  if (g_cannon.status == OBJ_ACTIVE) {
    uint8_t sc = laser_update(&g_cannon.laser);
    if (sc > 0) {
      if ((g_game.score < SCORE_1UP) && (g_game.score + sc > SCORE_1UP)) {
        print_cannon_left(++g_game.left);
        sound_start(&snd_fx[SND_1UP]);
      }
      g_game.score += sc;
      print_score();
    }

    for (uint8_t i = 0; i < BOMB_NUM; i++)
      bomb_update(&bombs[i]);
    ufo_update(&g_ufo);
    aliens_update(&g_aliens);
  }
}

void print_game_over() {
  FILL_RECT(33, 23, 60, 37, BLACK);
  G_TXT_CURSOR(37, 28);
  G_PRINT(F("GAME OVER"));
}

void setup() {
  arduboy.begin();
  arduboy.clear();
  arduboy.setFrameRate(SCRN_FPS);
  beep.begin();
  g_game.status = GameTitle;


}

#define TITLE_TIME 200

void loop() {
  if (!(arduboy.nextFrame()))
    return;

  static int16_t passed_time = 0;

  switch (g_game.status) {
  case GameRestart:
    game_restart();
    g_game.status = GameOnGoing;
    break;

  case GameOnGoing:
    game_main();
    if (g_cannon.status == OBJ_READY) {
      g_game.status = GameLost;
    }
    if ((g_aliens.alive == 0) && g_aliens.status == OBJ_ACTIVE) {
      g_game.stage++;
      g_game.status = GameRestart;
    }
    if (g_aliens.touch_down) {
      print_game_over();
      g_game.status = GameOver;
    }
    game_sound_main();
    break;

  case GameLost:
    print_cannon_left(--g_game.left);
    cannon_erase(&g_cannon);
    cannon_init(&g_cannon);
    for (uint8_t i = 0; i < BOMB_NUM; i++) {
      bomb_draw(&bombs[i], BLACK);
    }
    bomb_init_all();

    if (g_game.left > 0)
      g_game.status = GameOnGoing;
    else {
      print_game_over();
      if (g_game.score > g_game.high_score)
        g_game.high_score = g_game.score;
      g_game.status = GameOver;
    }
    break;

  case GameOver:
    if (passed_time++ > TITLE_TIME) {
      passed_time = 0;
      g_game.status = GameTitle;
    }
    else if (game_new_game()) { // true if A or B button is pushed
      game_initialize();
      print_cannon_left(g_game.left);
      print_score();
      g_game.status = GameRestart;
      passed_time = 0;
    }
    break;
  case GameTitle:
    switch (passed_time) {
    case 1:
    case 20:
    case 40:
      FILL_RECT(0, 0, 128, 64, BLACK);
      break;
    case 10:
    case 30:
    case 50:
      game_title_draw();
      break;
    case 600:
      passed_time = 0;
      break;
    default:
      if ((passed_time > 60) && (passed_time % 3 == 0)) {
        game_title_alien_draw(passed_time / 3 - 21, BLACK);
        game_title_alien_draw(passed_time / 3 - 20, WHITE);
      }
      break;
    }
    passed_time++;
    if (game_new_game()) { // true if A or B button is pushed
      FILL_RECT(0, 0, 128, 64, BLACK);
      draw_game_field();
      game_initialize();
      print_cannon_left(g_game.left);
      print_score();
      g_game.status = GameRestart;
      passed_time = 0;
    }
  }
  arduboy.display();
}
