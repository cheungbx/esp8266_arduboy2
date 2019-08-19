/*
*/
int frameRate = 30;
#include <Arduboy2.h>

// make an instance of arduboy used for many functions
Arduboy2 arduboy;


unsigned long lastSetFrameTime = 0;  // the last time the LCD display happened.
const unsigned long SetFrameInterval = 1000;    // the display delay time in ms; increase if the output flickers


// This function runs once in your game.
// use it for anything that needs to be set only once in your game.
void setup() {
  // initiate arduboy instance
  arduboy.begin();

  // here we set the framerate to 15, we do not need to run at
  // default 60 and it saves us battery life
  arduboy.setFrameRate(frameRate);

}


// our main game loop, this runs once every cycle/frame.
// this is where our game logic goes.
void loop() {
  static uint8_t fcnt = 0;
  static uint8_t dtime = 0;

  // pause render until it's time for the next frame
  if (!(arduboy.nextFrame()))
    return;

  // first we clear our screen to black
  arduboy.clear();

  // we set our cursor 5 pixels to the right and 10 down from the top
  // (positions start at 0, 0)
  arduboy.setCursor(4, 9);

  
  // then we print to screen what is in the Quotation marks ""
  arduboy.print(F("VidTest: "));
  arduboy.print(fcnt>>4);

  arduboy.print(F("\nms/frame: "));
  arduboy.print(dtime);

  arduboy.print(F("\nfps: "));
  arduboy.print(frameRate);
  
  fcnt++;
  if(fcnt & 0x0f == 0x0f) {
    arduboy.invert((fcnt & 0x10));   
    arduboy.allPixelsOn((fcnt & 0x20));   
    arduboy.flipVertical((fcnt & 0x40));   
    arduboy.flipHorizontal((fcnt & 0x80));   
 
 }

  
  // then we finaly we tell the arduboy to display what we just wrote to the display
  arduboy.display();

  
if (abs((millis() - lastSetFrameTime) > SetFrameInterval)) { 
  lastSetFrameTime =   millis();
 frameRate = min(60, frameRate +1);
  arduboy.setFrameRate(frameRate);
}
 
  
 
    
}
