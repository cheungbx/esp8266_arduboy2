/*
ArduTewst
2019 02 06 
Billy Cheung
This code tests the buttons and the sound output of  Arduboy

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
*/

#define paddle1X A0



#define minPaddle 1
#define maxPaddle 1024



#include <Arduboy2.h>

// Make an instance of arduboy used for many functions
Arduboy2 arduboy;
BeepPin1 beep; // Create a class instance for speaker pin 1


// Variables for your game go here.
char title[] = "*";
byte x;
byte y;



// Width of each charcter including inter-character space
#define CHAR_WIDTH 6

// Height of each charater
#define CHAR_HEIGHT 8

// To get the number of characters, we subtract 1 from the length of
// the array because there will be a NULL terminator at the end.
#define NUM_CHARS (sizeof(title) - 1)

// This is the highest value that x can be without the end of the text
// going farther than the right side of the screen. We add one because
// there will be a 1 pixel space at the end of the last character.
// WIDTH and HEIGHT are defined in the Arduboy library.
#define X_MAX (WIDTH - (NUM_CHARS * CHAR_WIDTH) + 1)

// This is the highest value that y can be without the text going below
// the bottom of the screen.
#define Y_MAX (HEIGHT - CHAR_HEIGHT)


// This function runs once in your game.
// use it for anything that needs to be set only once in your game.
void setup() {
  //initiate arduboy instance
  arduboy.begin();

  // here we set the framerate to 30, we do not need to run at default 60 and
  // it saves us battery life.
  arduboy.setFrameRate(30);

  arduboy.audio.on; 

  
}


// our main game loop, this runs once every cycle/frame.
// this is where our game logic goes.
void loop() {

 
  // pause render until it's time for the next frame
  if (!(arduboy.nextFrame()))
    return;

  // The timer() function is called once per frame, so duration values will be
  // the number of frames that the tone plays for.
  // At 25 frames per second each frame will be 40ms.
  beep.timer(); // handle tone duration

 // we clear our screen to black
  arduboy.clear();

  // the next couple of lines will deal with checking if the D-pad buttons
  // are pressed and print the text accordingly.
   arduboy.setCursor(0, 0);
   arduboy.print("ArduTest");
    
  // Read paddle 1 values from ADC
   arduboy.setCursor(0, 10);
   arduboy.print("X1=");
   arduboy.setCursor(20, 10);
   arduboy.print (analogRead(paddle1X));
   x = constrain (map (analogRead(paddle1X), minPaddle, maxPaddle, 0, X_MAX), 0, X_MAX);
   y= 20;
   
  
   
// print a "*" at the current position of  paddle1   
   arduboy.setCursor(x, y);
   arduboy.print("*");
   
     
   arduboy.setCursor(20, 30); 
   if(arduboy.pressed(UP_BUTTON)) {arduboy.print("U"); beep.tone(beep.freq(523.251), 5);} else arduboy.print ("O");
  
  arduboy.setCursor(20, 50);
  if(arduboy.pressed(DOWN_BUTTON)) { arduboy.print("D"); beep.tone(beep.freq(587.330), 5);} else arduboy.print ("O");

  arduboy.setCursor(10, 40); 
  if(arduboy.pressed(LEFT_BUTTON)) { arduboy.print("L"); beep.tone(beep.freq(659.255), 5);} else arduboy.print ("O");
     
  arduboy.setCursor(30, 40); 
  if(arduboy.pressed(RIGHT_BUTTON)) { arduboy.print("R"); beep.tone(beep.freq(698.456), 5);} else arduboy.print ("O");
  
  arduboy.setCursor(60, 45);
  if(arduboy.pressed(A_BUTTON)) { arduboy.print("A"); beep.tone(beep.freq(784.251), 5);} else arduboy.print ("O");

  arduboy.setCursor(75, 35);
  if(arduboy.pressed(B_BUTTON)) { arduboy.print("B"); beep.tone(beep.freq(880.251), 5);} else arduboy.print ("O");


  
    // then we finaly we tell the arduboy to display what we just wrote to the display.
  arduboy.display();
}
