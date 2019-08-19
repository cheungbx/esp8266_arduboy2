 /*
 Author: Billy Cheung
 
Traffic LIght Demonstration 

*/

#define red   D6    // D6   LED output pin - left
#define yellow  D5    // D5 - LED output pin - up
#define green   D7    // D6   LED output pin - Right

void light ( bool R, bool Y, bool G, int delaySecond)
{
if (R) digitalWrite(red, LOW); else digitalWrite(red, HIGH);
if (Y) digitalWrite(yellow, LOW); else digitalWrite(yellow, HIGH);
if (G) digitalWrite(green, LOW); else digitalWrite(green, HIGH);
delay ( delaySecond * 1000);
}


void setup() {
Serial.begin(115200);
Serial.println();
Serial.print("Traffic LIght Demo Started");

 pinMode(red, OUTPUT);
 pinMode(yellow, OUTPUT);
 pinMode(green, OUTPUT);

 light (0,0,0, 1 );

}




void loop() {

//     R Y G  Seconds
light (0,0,1, 5);
light (0,1,0, 2);
light (1,0,0, 5);
light (1,1,0, 2);


}
