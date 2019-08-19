# ArduBOYNG
A simple game with two paddles and a ball BOYINGing around...

Written for the awesome Arduboy by Kevin Bates.

## Installation
Download, compile in Arduino, upload to Arduboy, enjoy.

## Instructions
###Menu
- Directional keys to navigate and select
- A or B to start the game

### Game
- Up/down to steer the left paddle 
- A/B to steer the right paddle (in 2 players mode)

### Settings
- Mode - who plays against whom
 - 1 player - left paddle controlled by a human, right paddle by Arduboy (default)
 - 2 players - both paddles controlled by humans
 - demo - both paddles controlled by Arduboy
- Speed - initial veolocity of the ball
 - slow, normal (default), fast, insane
- Points - points needed to win the game
 - 3, 5, 7, 9
- AI - strength of Arduboy's play
 - dumb - Arduboy misses the ball every now and then
 - normal - Arduboy plays quite well (default)
 - smart - Arduboy plays pretty good
 - 100% - Arduboy never misses
- Acceleration - speed increase at every paddle hit in % of initial speed
 - 0%, 1% (default), 3%, 5%
