/* 
  ArduboyNGOnPaddle 
  based on the original source created by Troubadixx
  Please refer to the original source on https://github.com/troubadixx/ArduBOYNG/releases/tag/v0.81

  modified by cheungbx 2019 02 06 to add support for Paddle controller 
   

 *   Press A to start game with button control
 *   Press B to start game with paddle control
 */
 
 /* Following are original comments from the author of ArduBOYNG that this fork is based on original creation by troubadixx
 * Refer to the original source code here
 * https://github.com/troubadixx/ArduBOYNG/releases/tag/v0.81* 
 * 
 * ArduBOYNG
 * 
 * Ein einfaches Spiel mit zwei Schlägern und einem Ball, der herumBOYNGt...
 * Geschrieben für den großartigen Arduboy von Kevin Bates
 * 
 * A simple game with two paddles and a ball BOYNGing around...
 * Written for the awesome Arduboy by Kevin Bates 
 * 
 * http://www.arduboy.com
 * 
 * October 2016
 */

/* Instructions
 *  
 * Menu
 *   - Directional keys to navigate and select
 *   - A or B to start the game
 *  
 * Game
 *   - up/down to steer the left paddle 
 *   - A/B to steer the right paddle (in 2 players mode)
 *  
 * Settings
 *   - Mode - who plays against whom:
 *     o 1 player - left paddle controlled by a human, right paddle by Arduboy (default)
 *     o 2 players - both paddles controlled by humans
 *     o demo - both paddles controlled by Arduboy
 *   - Speed - initial veolocity of the ball:
 *     o slow, normal (default), fast, insane
 *   - Points - points needed to win the game:
 *     o 3, 5, 7, 9
 *   - AI - strength of Arduboy's play:
 *     o dumb - Arduboy misses the ball every now and then
 *     o normal - Arduboy plays quite well (default)
 *     o smart - Arduboy plays pretty good
 *     o 100% - Arduboy never misses
 *   - Acceleration - speed increase at every paddle hit in % of initial speed
 *     o 0%, 1% (default), 3%, 5%
 */

 /* Anleitung
  *  
  * Tastenbelegung im Menü 
  *   - Navigation und Einstellung mit den Richtungstasten
  *   - Spielstart mit A oder B
  *   
  * Tastenbelegung im Spiel
  *   - Schläger links mit hoch/runter
  *   - Schläger rechts (im 2 players mode) A/B
  * 
  * Einstellungen
  *   - Mode - Spielmodus: 
  *     o 1 player - Mensch am linken Schläger gegen Arduboy (Voreinstellung)
  *     o 2 players - Zwei Menschen gegeneinander
  *     o demo - Beide Schäger vom Arduboy gesteuert
  *   - Speed - Geschwindigkeit:
  *     o slow - langsam
  *     o normal - normal (Voreinstellung)
  *     o fast - schnell
  *     o insane - verdammt schnell
  *   - Points - Notwendige Punkte für Spielgewinn
  *     o 3, 5 (Voreinstellung), 7, 9
  *   - Sound - Spiel- und Menügeräusche
  *     o on - an (Voreinstellung)
  *     o off - aus
  *   - AI - Spielstärke des Arduboy
  *     o dumb - Arduboy schlägt gerne mal daneben
  *     o normal - Arduboy spielt ganz ordentlich (Voreinstellung) 
  *     o smart - Arduboy spielt schon recht gut
  *     o 100% - Arduboy trifft immer
  *   - Acceleration - Geschwindigkeitszuwachs je Schägertreffer in % der Anfangsgeschwindigkeit
  *     o 0%, 1% (Voreintesllung), 3%, 5%
  */
#define paddle1X A0
#define minPaddle 1
#define maxPaddle 1024
bool usePaddle = false;

#include "Arduboy2.h" //ESP8266_Arduboy

Arduboy2 arduboy;  //
BeepPin1 beep;     //ESP8266_Arduboy
const boolean debug = false;

// Einige Grundeinstellungen
const String TITEL1 = "ArduBOYNG";                           // Titelbildschirm: Kleingedruckte erste Titelzeile
const String TITEL2 = "A=Button B=Paddle";                    // Titelbildschirm: Großgedruckte zweite Titelzeile
const String UNTERTITEL = "v0.81";                            // Titelbildschirm: Untertitel
const int BALL_DURCHMESSER = 3;                               // "Durchmesser" des Balls in Pixel - der Ball ist in Wirklichkeit ein Quadrat mit dieser Seitenlänge in Pixel.     
const int SCHLAEGER_BREITE = 15;                              // Breite der Schläger in Pixel.
const float TREFFERLAENGE_HALBE = ( SCHLAEGER_BREITE + BALL_DURCHMESSER ) / 2;   // Anschaulich die Hälfte der effektiven "Trefferlänge" des Schlägers. Dabei sind Treffer auch dann gegeben, wenn der Ball ober- oder unterhalb des Schlägers ist, aber den Schläger gerade noch berührt. Wird zur Berechung des Abprallwinkels benötigt.
const int FPS = 60;                                           // Bildschirmzeichnungen pro Sekunde, "frames per second".
const boolean SPIELFELDRAND = true;                           // Ob Spielfeldrand gezeichnet wird (true) oder nicht (true). Hinweis: Zur Zeit _mit_ Spielfeldrand programmiert. Für Variante ohne Spielfeldrand müsste (eigentlich) noch die Spielfeldgrenzen, Ball-/Schläger-Mini/Maxima umdefiniert werden.
const int COUNTDOWN_VERZOEGERUNG = 1000;                      // Wartezeit zwischen den Countdown-Zahlen in ms
const int GAMEOVER_VERZOEGERUNG = 5000;                       // Extra Wartezeit nach Spielende in ms (vor Zurückschalten zum Titel / Menü)
const int PUNKTGEWINN_VERZOEGERUNG = 2000;                    // Wartezeit nach Punktgewinn in mx (vor Rücksetzen von Schläger und BAll und nächstem Countdown)

// Menü: Einträge, Werte und Steuervariablen
int menueZeile = 0;                                           // Laufende Menüzeile. Anfangs auf Null gesetzt für den ersten Menüeintrag
int menueAuswahlen[] = {  0, 1, 1, 0, 1, 1 };                  // Der Index der ausgewählten Werte für jede Zeile. 0 bedeutet, dass der erste Wert in der jeweiligen Zeile ausgewählt ist. Die angegebenen Werte sind die Voreinstellungen, die beim Programmstart.
const String MENUE_TEXTE[][5] =                               // Menütexte Zeile für Zeile mit jeweils den auswählbaren Werten und der Zeilenbezeichnung
  {
      { "1 player", "2 players", "demo", "Mode: " }, 
    { "slow", "normal", "fast", "insane", "Speed: " }, 
    { "3", "5", "7", "9", "Points: " } , 
    { "on", "off", "Sound: " }, 
    { "dumb", "normal", "smart", "100%", "AI: " }, 
    { "0%", "1%", "3%", "5%", "Acceleration: "}
  };
const int MENUE_ZEILEN = 5;                                   // Anzahl Menüzeilen - 1 = Maximaler Wert der Variable menueZeile
const int MENUE_AUSWAHLMOEGLICHKEITEN[] = { 2, 3, 3, 1, 3, 3 };  // Anzahl auswählbarer Werte in jeder Menüzeile - 1
const boolean MENUE_MODE_WERTE[][2] = { {false, true}, {false, false}, {true, true} };  // Auswählbare Werte der Menüzeile Mode: {false, true} bedeutet linker Schläger vom Menschen gesteuert, rechter Schläger vom Computer gesteuert ("AI" = Artificial Intelligence). Usw.
const float MENUE_SPEED_WERTE[] = { 0.8, 1.2, 1.7, 2.5 };     // Auswählbare Werte der Menüzeile Speed: Geschwindigkeitsfaktoren
const int MENUE_POINTS_WERTE[] = { 3, 5, 7, 9 };              // Auswählbare Werte der Menüzeile Points: Das Spiel endet, wenn ein Spieler diese Anzahl Punkte erreicht.
const boolean MENUE_SOUND_WERTE[] = { true, false };          // Auswählbare Werte der Menüzeile Sound: true = on, false = off
const float MENUE_AI_WERTE[] = { 0.6, 0.8, 0.9, 1.0 };        // Auswählbare Werte der Menüzeile AI: Spielstärke der "AI", etwa in Prozent. z.B. 0.8 bedeutet, dass eine Trefferrate von ca. 80%. Usw.
const float MENUE_ACCELERATION_WERTE[] = { 0.0, 0.01, 0.03, 0.05 };  // Geschwindigkeitserhöhung (in Prozent der Initialgeschwindigkeit vInitial) je Richtungswechsel in x-Richtung (d.h. je Schlägertreffer)
const String MENUE_HAUPTZEILE_EINLEITUNG = ">> ";             // Text zur Markierung der Hauptzeile des Menüs (vor der Menüzeile), 3 Zeichen
const String MENUE_HAUPTZEILE_ABSCHLUSS = "<<";               // Text zur Markierung der Hauptzeile des Menüs (hinter der Menüzeile), 3 Zeichen
const int MENUE_X = 0;                                        // Menuedarstellung: x Koordinate. Hier fängt die Hauptzeile mit den Einleitungszeichen an.
const int MENUE_X_ABSCHLUSS = WIDTH-1 - 6*2;                  // Menuedarstellung: x Koordinate für die Abschlusszeichen
const int MENUE_Y = 48;                                       // Menuedarstellung: y Koordinate der Hauptzeile
const int MENUE_Y_DELTA = 8;                                  // Menuedarstellung: Abstand zur vorherigen und zur folgenden Zeile
const int MENUE_VERZOEGERUNG = 300;                           // Verzögerung (Wartezeit) zwischen Menueklicks in Millisekunden

// Ball-Variablen und -Konstanten
float xb;                                                     // xb = x-Koordinate des Balls (links oben am Ball). Wird gesetzt nach Menüauswahl und vor Anstößen und im Spiel laufend geändert via dxb.
float yb;                                                     // yb = y-Koordinate des Balls (links oben am Ball). Wird gesetzt nach Menüauswahl und vor Anstößen und im Spiel laufend geändert via dxy.
float ybMitte = yb + ( BALL_DURCHMESSER - 1 ) / 2;            // ybMitte = y-Koordinate der Ballmitte. Normalerweise wird der Ball mittels der linken oberen Ecke definiert (xb, yb). Für die Berechnung des Abprallwinkels wird jedoch diese Hilfsvariable benötigt.
float v;                                                      // Geschwindigkeit = Betrag des Geschwindigkeitsvektors (dxb, dyb). Anfangs auf vInitial gesetzt und ggf. bei Schlägertreffer um speedErhoehung erhöht.
float vInitial;                                               // Geschwindigkeit am Anfang beim Anstoß = Betrag des Geschwindigkeitsvektors (dxb, dyb) am Anfang beim Abstoß. Wird gesetzt nach Menüauswahl.
float phiDeg;                                                 // Richtung des Geschwindigkeitsvektors (dxb, dyb) als Winkel in Grad. Anfangs und vor Anstößen zufällig gewählt, bei Schlägertreffer aus Trefferposition berechnet. Innerhalb Bandbreite (-PHI_DEG_MAX, PHI_DEG_MAX).
float phi;                                                    // Richtung des Geschwindigkeitsvektors (dxb, dyb) als Winkel in Rad. Siehe oben    
float dxb;                                                    // Geschwindigkeit des Balles in x-Richtung. Wird gesetzt nach Menüauswahl und vor Anstößen und im Spiel geändert bei Schlägertreffer und ggf. Geschwindigkeitserhöhung.
float dyb;                                                    // Geschwindigkeit des Balles in y-Richtung. Wird gesetzt nach Menüauswahl und vor Anstößen und im Spiel geändert bei Abprall am Rand, Schälgertreffer und ggf. Geschwindigkeitserhöhung.
float dxbVorzeichen;                                          // Vorzeichen = Richtung des Balles in x-Richtung (wird benötigt beim Berechnen des Abpralls und beim Anstoß)
float speedErhoehung;                                         // Bei jedem Schlägertreffer wird die Geschwindigkeit um diesen Wert erhöht. Wird gesetzt nach Menüauswahl auf Menüzeile "Acceleration". speedErhoehung = MENUE_ACCELERATION_WERTE[menueAuswahlen[5]] * vInitial;
const int XB_MIN = 1;                                         // Minmale x-Koordinate des Balls mit Spielfeldrand. 0 ohne Spielfeldrand.
const int XB_MAX = WIDTH - BALL_DURCHMESSER -1;               // Maximale x-Koordinate des Ball mit Spielfeldrand. Ohne die "-1" ohne Spielfeldrand.
int YB_MIN = 1;                                               // Minmale y-Koordinate des Balls mit Spielfeldrand. 0 ohne Spielfeldrand.
int YB_MAX = HEIGHT - BALL_DURCHMESSER -1;                    // Maximale y-Koordinate des Ball mit Spielfeldrand. Ohne die "-1" ohne Spielfeldrand.
const int XB_ABSCHLAG = XB_MAX / 2;                           // Ballposition (x) bei Spielbeginn in der Spielfeldmitte.
const int XB_ABSCHLAG1 = 3;                                   // Ballposition (x) bei Abschlag des linken Spielers (1) nach Nichttreffen mit dem Schäger.
const int XB_ABSCHLAG2 = WIDTH-1 - 3 - BALL_DURCHMESSER;      // Ballposition (x) bei Abschlag des rechten Spielers (2) nach Nichttreffen mit dem Schäger.
const int YB_ABSCHLAG = YB_MAX / 2;                           // Ballposition (y) bei Spielbeginn und Abschlag nach Nichttreffen - in der Spielfeldmitte in y-Richtung.
const float PHI_DEG_MAX = 45;                                 // Maximaler Betrag von phi in Grad
const float PHI_MAX = PHI_DEG_MAX * PI / 180;                 // Maximaler Betrag von phi in Rad

// Schläger-Konstanten, Schläger-Variablen und initiale Werte
float ys1;                                                    // y-Koordinate Schläger 1 (oberes Pixel). Bei Spielbeginn und nach Punktgewinn in Mitte (in y-Richtung) gesetzt und während des Spiels geändert via DYS1.         
float ys2;                                                    // y-Koordinate Schläger 2 (oberes Pixel). Bei Spielbeginn und nach Punktgewinn in Mitte (in y-Richtung) gesetzt und während des Spiels geändert via DYS2.
float ys1Mitte = ys1 + ( SCHLAEGER_BREITE - 1 ) / 2;          // ys1Mitte = y-Koordinate der Schägermitte (Schläger 1). Normalerweise wird der Schläger mittels des oberen Pixel definiert (XS1, ys1). Für die Berechnung des Abprallwinkels wird jedoch diese Hilfsvariable benötigt.
float ys2Mitte = ys2 + ( SCHLAEGER_BREITE - 1 ) / 2;          // ys2Mitte = y-Koordinate der Schägermitte (Schläger 2). Normalerweise wird der Schläger mittels des oberen Pixel definiert (XS2, ys2). Für die Berechnung des Abprallwinkels wird jedoch diese Hilfsvariable benötigt.
const float DYS1 = 2.0;                                       // "Geschwindigkeit" der Schägerbewegung = Veränderung der y-Koordinate des Schlägers 1 je Knopfdruck bzw. je Frame
const float DYS2 = 2.0;                                       // "Geschwindigkeit" der Schägerbewegung = Veränderung der y-Koordinate des Schlägers 2 je Knopfdruck bzw. je Frame
const int XS1 = 1;                                            // x-Koordinate des Schägers 1 (linker Schläger) = 1. (0 ohne Bildschirmrand)
const int XS2 = WIDTH-1 -1;                                   // x-Koordinate des Schägers 2 (rechter Schläger) = WIDTH-2. (WIDTH-1 ohne Bildschirmrand) 
const float YS_MIN = - SCHLAEGER_BREITE / 2;                  // Minimale y-Koordinate der Schläger. So bleibt mindestens der halbe Schläger sichtbar.
const float YS_MAX = HEIGHT-1 - SCHLAEGER_BREITE / 2;         // Maximale y-Koordinate der Schläger. So bleibt mindestens der halbe Schläger sichtbar.
const float YS1_START = (HEIGHT - SCHLAEGER_BREITE) / 2 ;     // Initiale y-Koordinate von Schläger 1 in Spielfeldmitte.
const float YS2_START = (HEIGHT - SCHLAEGER_BREITE) / 2 ;     // Initiale y-Koordinate von Schläger 2 in Spielfeldmitte.
// Hilfsvariablen
float diff1Mitte;                                             // Hilfsvariable für Berechnung des Abpralls vom Schläger
float diff2Mitte;                                             // Hilfsvariable für Berechnung des Abpralls vom Schläger

// "AI"-Variable
boolean ai1;                                                  // false: linker Schläger wird von Mensch gespielt. true: linker Schläger wird von Computer gespielt. Wird gesetzt nach Menüauswahl aus Menüzeile "Mode".
boolean ai2;                                                  // false: rechter Schläger wird von Mensch gespielt. true: rechter Schläger wird von Computer gespielt. Wird gesetzt nach Menüauswahl aus Menüzeile "Mode".
float aiStaerke;                                              // Spielstärke der "AI". Wird gesetzt nach Menüauswahl aus Menüzeile "AI".
float AI_Intervallbreite_delta_einseitig_neu;                 // Hilfsvariable für "AI-Bewegungen" der Schläger. Wird nach Menüauswahl berechnet aus aiStaerke, BALL_DURCHMESSER und SCHLAEGER_BREITE. AI_Intervallbreite_delta_einseitig =  ( 1/aiStaerke - 1) * ( BALL_DURCHMESSER+SCHLAEGER_BREITE+1 ) / 2;

// Punkte-Variablen und -Konstanten
int punkte1;                                                  // Punkte Spieler 1 (links). Wird nach Menüauswahl auf 0 gesetzt.
int punkte2;                                                  // Punkte Spieler 2 (rechts). Wird nach Menüauswahl auf 0 gesetzt.
int punkteMax;                                                // Spiel endet, wenn ein Spieler punkteMax Punkte erreicht hat. Wird gesetzt nach Menüauswahl aus Menüzeile "Points".
const int PUNKTE1_X = 51;                                     // Position Punkteanzeige Spieler 1, x-Koordinate.
const int PUNKTE2_X = 71;                                     // Position Punkteanzeige Spieler 2, x-Koordinate.
const int PUNKTE1_Y = 3;                                      // Position Punkteanzeige Spieler 1, y-Koordinate.
const int PUNKTE2_Y = PUNKTE1_Y;                              // Position Punkteanzeige Spieler 2, y-Koordinate.

// Töne für verschiedene Vorkommnisse. Jeweils Frequenz und Dauer.
boolean sound;  
// Töne an (true) oder aus (false). Default = false. Wird auf Default gesetzt vor Menüauswahl und während Menüauswahl je nach Einstellung geändert.
//ESP8266_Arduboy , divde by T1 to adjust the duration as ESP8266 speed differs from  ATMega32U4.
#define T1 7   

const int TON_COUNTDOWN_FREQ = 880;                           // Countdown vor Anstoß
const int TON_COUNTDOWN_DAUER = 100/T1;
const int TON_SCHLAEGER_FREQ = 440;                           // Abprall am Schläger
const int TON_SCHLAEGER_DAUER = 200/T1;
const int TON_PUNKT_FREQ = 110;                               // Punktgewinn (bzw. Ball durchgelassen für den anderen Spieler)
const int TON_PUNKT_DAUER = 1000/T1;
const int TON_RAND_FREQ = 1320;                               // Abprall am Rand
const int TON_RAND_DAUER = 50/T1;
const int TON_GAMEOVER_FREQ = 220;                            // Spiel beendet  
const int TON_GAMEOVER_DAUER = 2000/T1;
const int TON_KICKOFF_FREQ = TON_RAND_FREQ;                   // Anstoß
const int TON_KICKOFF_DAUER = TON_RAND_DAUER/T1;
const int TON_MENUE_FREQ = 1320;                              // Klick im Menü
const int TON_MENUE_DAUER = 50/T1;

// Steuervariablen
boolean gameover = false;                                     // Steuervariable: Falls ein Spieler die Gewinnpunktzahl erreicht, wird gameover=true gesetzt und damit die Gameoversequenz und der Neustart initiiert.
boolean intro = true;                                         // Steuervariable: Ob der Titel- und Menüschirm angezeigt werden soll.
boolean anstoss = true;                                       // Steuervariable: Am Spielbeginn und nach jedem Punktgewinn erfolgt ein Anstoss

// Setze Variablen vor Spielstart
//   - Übersetze Menüauswahl in Variablenwerte
//   - Setze Anfangswerte für Schläger, Ball, Punkte, etc. 
void setze_variablen_vor_spielstart() {
  // Menüauswahl Mode 
  ai1 = MENUE_MODE_WERTE[menueAuswahlen[0]][0];
  ai2 = MENUE_MODE_WERTE[menueAuswahlen[0]][1];  
  // Menüauswahl Speed
  vInitial = MENUE_SPEED_WERTE[menueAuswahlen[1]];
  // Menüauswahl Points
  punkteMax = MENUE_POINTS_WERTE[menueAuswahlen[2]];
  // Menüauswahl Sound
  sound = MENUE_SOUND_WERTE[menueAuswahlen[3]];
  // Menüauswahl AI
  aiStaerke = MENUE_AI_WERTE[menueAuswahlen[4]];
  AI_Intervallbreite_delta_einseitig_neu =  ( 1/aiStaerke - 1) * ( BALL_DURCHMESSER+SCHLAEGER_BREITE+1 ) / 2;
  // Menüauswahl Acceleration
  speedErhoehung = MENUE_ACCELERATION_WERTE[menueAuswahlen[5]] * vInitial;
  // Punkte bei Spielbeginn auf 0
  punkte1 = 0;
  punkte2 = 0;
  // Schläger auf Startposition 
  ys1 = YS1_START;
  ys2 = YS2_START;
  // Ball auf Startposition in Spielfeldmitte für ersten Anstoß
  xb = XB_ABSCHLAG;
  yb = YB_ABSCHLAG;
  // Ball-Geschwindigkeit und -Richtung für Anstoß
  v = vInitial;
  phiDeg = random(-PHI_DEG_MAX, PHI_DEG_MAX);
  phi = phiDeg / 180 * PI;
  dxb = v / sqrt( 1 + tan(phi)*tan(phi) );
  // dx_sign = (random(0,2)*2-1);
  dxbVorzeichen = 1;
  if (random(0,2) == 0) dxbVorzeichen = -1;
  dxb = dxbVorzeichen * dxb;
  dyb = dxb * tan(phi);
  // Steuervariablen
  anstoss = true;
}

// Setze Variablen vor Ballstart, i. e. nach Punktgewinn
void setze_variablen_nach_punktgewinn() {
  // Schläger auf Startposition
  ys1 = YS1_START; 
  ys2 = YS2_START;
  // Ball auf Startposition 
  //   Keine Anweisungen hier.
  //   xb und yb werden bereits vor Aufruf dieser Funktion gesetzt, 
  //   da abhängig vom Punktgewinner der Abschlag in der linken bzw. rechten Spielfeldhälfte stattfindet.
  // Ball-Geschwindigkeit für Anstoß
  v = vInitial;
  phiDeg = random(-PHI_DEG_MAX, PHI_DEG_MAX);
  phi = phiDeg / 180 * PI;
  dxb = v / sqrt( 1 + tan(phi)*tan(phi) );
  dxb = dxbVorzeichen * dxb;                                  // dxbVorzeichen und damit die Richtung des Anstoßes wird bereits vor Aufruf dieser Funktion gesetzt, da abhängig vom Punktegewinner der Abschlag nach links bzw. nach rechts erfolgen muss.
  dyb = dxb * tan(phi);
  // Steuervariablen
  anstoss = true;
}

// Zeichne und zeige Titel und Menü
void zeichne_und_zeige_titel_und_menue() {
  arduboy.clear();                                            // Lösche Bildschirm
  zeichne_titel();                                            // Zeichne den oberen Teil des Titel-/Menü-Bildschirms, d.h. den Titel
  // Zeichne die Hauptzeile des Menüs
  arduboy.setCursor(MENUE_X,MENUE_Y);
  arduboy.print(MENUE_HAUPTZEILE_EINLEITUNG);
  arduboy.print(MENUE_TEXTE[menueZeile][MENUE_AUSWAHLMOEGLICHKEITEN[menueZeile]+1]);
  arduboy.print(MENUE_TEXTE[menueZeile][menueAuswahlen[menueZeile]]);
  arduboy.setCursor(MENUE_X_ABSCHLUSS, MENUE_Y);
  arduboy.print(MENUE_HAUPTZEILE_ABSCHLUSS);
  // Zeichne die Menüzeile darüber
  if (menueZeile > 0) {
    arduboy.setCursor(MENUE_X,MENUE_Y-MENUE_Y_DELTA);
    arduboy.print("   ");
    arduboy.print(MENUE_TEXTE[menueZeile-1][MENUE_AUSWAHLMOEGLICHKEITEN[menueZeile-1]+1]);
    arduboy.print(MENUE_TEXTE[menueZeile-1][menueAuswahlen[menueZeile-1]]);        
  }
  // Zeichne die Menüzeile darunter
  if (menueZeile < MENUE_ZEILEN) {
    arduboy.setCursor(MENUE_X,MENUE_Y+MENUE_Y_DELTA);
    arduboy.print("   ");
    arduboy.print(MENUE_TEXTE[menueZeile+1][MENUE_AUSWAHLMOEGLICHKEITEN[menueZeile+1]+1]);
    arduboy.print(MENUE_TEXTE[menueZeile+1][menueAuswahlen[menueZeile+1]]);        
  }
  arduboy.display();                                          // Zeige den Bildschirminhalt an
}

// Zeichne die obere Hälfte des Titel/Menü-Bildschirms
//   o wird aufgerufen aus zeichne_und_zeige_titel_und_menue()
//   o wird auch separat aufgerufen, daher aus zeichne_und_zeige_titel_und_menue() ausgelagert
void zeichne_titel() {
  arduboy.setTextSize(1);
  arduboy.setCursor( 30, 0 );
  arduboy.print(TITEL1);
  arduboy.setTextSize(1);
  arduboy.setCursor( 10, 13 );
  arduboy.print(TITEL2);
  arduboy.setTextSize(1);
  arduboy.setCursor( 48, 27 );
  arduboy.print(UNTERTITEL);
}

// Zeige den Startbildschirm (Titel und Menü und führe das Menü aus - bis Starttaste gedrückt)
void zeige_startschirm_und_fuehre_menue_aus() {
  menueZeile = 0;                                             // Aktuelle Menüzeile
  zeichne_und_zeige_titel_und_menue();                        // Anzeige von Titel und Menü (mit aktueller Menüzeile)
  // Schleife zur Menüauswahl läuft so lange bis Starttaste gedrückt wird
  boolean warteAufStarttaste = true;                          // true: Schleife zur Menüauswahl läuft bis Starttaste gedrückt (dann warteAufStarttaste=false)
  while ( warteAufStarttaste ){
    sound = MENUE_SOUND_WERTE[menueAuswahlen[3]];             // Sound wird hier bereits aus Menüauswahl übernommen, damit beim Menüklicken entsprechend ein Ton gespielt wird oder nicht
    // Starttasten (A und B) führen zum Abbruch der Menüschleife
    if ( arduboy.pressed(A_BUTTON) || arduboy.pressed(B_BUTTON) ) { 

    //cheungbx - if B button pressed, use paddle as controller for player1
    if (arduboy.pressed(B_BUTTON)) {
      usePaddle=true; 
 //     power_adc_enable();  
      }
    else usePaddle=false;  

      if (sound) beep.tone (beep.freq(TON_MENUE_FREQ), TON_MENUE_DAUER);
//      delay(COUNTDOWN_VERZOEGERUNG);
      warteAufStarttaste = false;
    }
    // "Runter-Knopf" blättert in die nächste Menüzeile
    if ( arduboy.pressed(DOWN_BUTTON) ) {
      if (sound) beep.tone (beep.freq(TON_MENUE_FREQ), TON_MENUE_DAUER);
      menueZeile++;
      if ( menueZeile > MENUE_ZEILEN ) {
        // menueZeile = 0; // scroll over to beginning
        menueZeile = MENUE_ZEILEN; // stop scrolling
      }
      zeichne_und_zeige_titel_und_menue();                                  
      delay(MENUE_VERZOEGERUNG); // Should be smarter debounce!!
    }
    // "Hoch-Knopf" blättert in die vorherige Menüzeile
    if ( arduboy.pressed(UP_BUTTON) ) {
      if (sound) beep.tone (beep.freq(TON_MENUE_FREQ), TON_MENUE_DAUER);
      menueZeile--;
      if ( menueZeile < 0 ) {
        // menueZeile = MENUE_ZEILEN; // scroll over to end
        menueZeile = 0; // stop scrolling
      }
      zeichne_und_zeige_titel_und_menue();
      delay(MENUE_VERZOEGERUNG);
    }
    // "Rechts-Knopf" wählt in der aktuellen Menüzeile die nächste Auswahlmöglichkeit aus
    if ( arduboy.pressed(RIGHT_BUTTON) ) {
      if (sound) beep.tone (beep.freq(TON_MENUE_FREQ), TON_MENUE_DAUER);
      menueAuswahlen[menueZeile]++;
      if ( menueAuswahlen[menueZeile] > MENUE_AUSWAHLMOEGLICHKEITEN[menueZeile] ) {
        menueAuswahlen[menueZeile] = 0; // scroll over to beginning
        // menueAuswahlen[menueZeile] = MENUE_AUSWAHLMOEGLICHKEITEN[menueZeile]; // stop scrolling
      }
      zeichne_und_zeige_titel_und_menue();
      delay(MENUE_VERZOEGERUNG);
    }
    // "Links-Knopf" wählt in der aktuellen Menüzeile die vorherige Auswahlmöglichkeit aus
    if ( arduboy.pressed(LEFT_BUTTON) ) {
      if (sound) beep.tone (beep.freq(TON_MENUE_FREQ), TON_MENUE_DAUER);
      menueAuswahlen[menueZeile]--;
      if ( menueAuswahlen[menueZeile] < 0 ) {
        menueAuswahlen[menueZeile] = MENUE_AUSWAHLMOEGLICHKEITEN[menueZeile]; // scroll over to end
        // menueAuswahlen[menueZeile] = 0; // stop scrolling
      }
      zeichne_und_zeige_titel_und_menue();
      delay(MENUE_VERZOEGERUNG);
    }
    delay(50);                                                // Kurze Pause, bevor Tastenabfrageschleife wieder ausgeführt wird
  }
  arduboy.clear();
}  

// Zeige, wer gewonnen hat zum Spielende
void zeige_spielende() {
  // Falls Gleichstand, zeige "Tied" - nur der Vollständidgkeit halber hier. Sollte bei aktueller Programmierung nicht vorkommen
  if ( punkte1 == punkte2 ) {                                 
    arduboy.setCursor(52, 28);
    arduboy.print("Tied");
  } else {
    // Zeige "Winner" im Spielfeld des Siegers
    if ( punkte1 > punkte2 ) arduboy.setCursor(13, 28);   
    if ( punkte2 > punkte1 ) arduboy.setCursor(77, 28);
    arduboy.print("Winner");
  } 
  arduboy.display();
  if (sound) beep.tone(beep.freq(TON_GAMEOVER_FREQ),TON_GAMEOVER_DAUER);
  delay(GAMEOVER_VERZOEGERUNG);
}

// Zeichne Spielfeld
void zeichne_spielfeld() {
  // Horizontale gepunktete Linien oben und unten
  if (SPIELFELDRAND) {
    for ( int i = 0; i < WIDTH; i+=2 ) {
      arduboy.drawPixel( i, 0, WHITE ); 
      arduboy.drawPixel( i, HEIGHT - 1, WHITE );
    }
  }
  // Vertikale gepunktete Linien links, Mitte, rechts
  for ( int j = 0; j < HEIGHT; j+=2 ) {
    arduboy.drawPixel( WIDTH/2 - 1, j, WHITE );
    if (SPIELFELDRAND) {
      arduboy.drawPixel( 0, j, WHITE );
      arduboy.drawPixel( WIDTH - 1, j, WHITE );
    }
  }
  // Zeige diverse Debug-Informationen auf dem Spielfeld an
  if (debug) {
    arduboy.setCursor(52,54);
    arduboy.print(v);
  }
}

// Ballbewegung in x- und in y-Ricthung
void bewege_ball(){
  male_ball( BLACK );                                         // Lösche den "alten" Ball durch Übermalen der alten Ballkoordinaten in schwarz
  xb = xb + dxb;                                              // Ändere die Ballkoordinaten um den aktuellen Änderungswert
  yb = yb + dyb;                                              //  - " -
  if ( xb >= XB_MAX ) {                                       // Pralle vom rechten Rand ab --- eigentlich überflüssige Abfrage, da sie in der Funktion prüfe_treffer() ja auch vorkommt und je nach Schlägerposition entsprechend gehandelt wird. Dennoch zunächst mal hier gelassen, weil Änderung vermutlich nicht nur Weglassen ist sondern ggf. eine größere Umprogrammierung bedeuten würde.
    dxb = -dxb;
    xb = XB_MAX;
  }
  if ( xb <= XB_MIN ) {                                       // Pralle vom linken Rand ab --- eigentlich überflüssige Abfrage, da sie in der Funktion prüfe_treffer() ja auch vorkommt und je nach Schlägerposition entsprechend gehandelt wird. Dennoch zunächst mal hier gelassen, weil Änderung vermutlich nicht nur Weglassen ist sondern ggf. eine größere Umprogrammierung bedeuten würde.
    dxb = -dxb;
    xb = XB_MIN;
  }
  if ( yb > YB_MAX ) {                                        // Pralle vom unteren Rand ab
    if (sound) beep.tone(beep.freq(TON_RAND_FREQ),TON_RAND_DAUER);
    dyb = -dyb;
    yb = YB_MAX;
  }
  if ( yb < YB_MIN ) {                                        // Pralle vom oberen Rand ab
    if (sound) beep.tone(beep.freq(TON_RAND_FREQ),TON_RAND_DAUER);
    dyb = -dyb;
    yb = YB_MIN;
  }
  male_ball( WHITE );                                         // Zeichne den Ball an den aktuellen Ballkoordinaten
}

// Zeichne den Ball (in weiß oder in schwarz)
void male_ball(boolean color) {
  for ( int i = 0; i < BALL_DURCHMESSER; i++ ) {              // Ball besteht aus BALL_DURCHMESSER Zeilen...
    for ( int j = 0; j < BALL_DURCHMESSER; j++ ) {            // ... und BALL_DURCHMESSER Spalten...
      arduboy.drawPixel( xb+i, yb+j, color );                 // ... von weißen Pixeln (oder schwarz beim Übermalen = Löschen).
    }
  }
}

// Bewege die Schläger gemäß Tastendruck
void bewege_schlaeger() {
  male_schlaeger( BLACK );                                    // Lösche die alten Schläger durch übermalen in schwarz

//cheungbx - change player1 and player 2 to paddel control
if (usePaddle) {
if (not ai1)  ys1 = map (constrain (analogRead(paddle1X), minPaddle, maxPaddle), minPaddle, maxPaddle,YS_MIN, YS_MAX);    

}
else {
  if ( arduboy.pressed(UP_BUTTON) ) {                         // Bewege linken Schläger hoch
    ys1=ys1-DYS1;
    if ( ys1 < YS_MIN ) ys1 = YS_MIN;
  }
  if ( arduboy.pressed(DOWN_BUTTON) ) {                       // Bewege linken Schläger runter
    ys1=ys1+DYS1;
    if ( ys1 > YS_MAX ) ys1 = YS_MAX;
  }
}
  
  if ( arduboy.pressed(B_BUTTON) ) {                          // Bewege rechten Schläger hoch
    ys2=ys2-DYS2;
    if ( ys2 < YS_MIN ) ys2 = YS_MIN;
  }
  if ( arduboy.pressed(A_BUTTON) ) {                          // Bewege rechten Schläger runter
    ys2=ys2+DYS2;
    if ( ys2 > YS_MAX ) ys2 = YS_MAX;
  }
  male_schlaeger( WHITE );
  /*
  if ( arduboy.pressed(LEFT_BUTTON) ) arduboy.invert(true);   // Invertiere Bildschirmfarben (zum Testen)
  if ( arduboy.pressed(RIGHT_BUTTON) ) arduboy.invert(false); // Bildschirmfarben wieder normal (zum Testen)
  */
}

// Bewege linken Schläger mittels "Artificial Intelligence"
void bewege_schlaeger_ai1() {
  // Wenn der Ball ganz links ist, setze den Schläger sprunghaft auf eine zufällige Position um den Ball herum...
  // ... mit einer Bandbreite, die der gewählten "AI-Stärke" entspricht.
  if ( xb <= XB_MIN ) {
    male_schlaeger(BLACK);
    ys1 = random( yb-SCHLAEGER_BREITE+1-AI_Intervallbreite_delta_einseitig_neu, yb+BALL_DURCHMESSER-1+1+AI_Intervallbreite_delta_einseitig_neu); 
    if ( ys1 < YS_MIN ) ys1 = YS_MIN;
    if ( ys1 > YS_MAX ) ys1 = YS_MAX;
    male_schlaeger(WHITE);
  }
  // Notiz: yb + BALL_DURCHMESSER < ys1, yb > ys1 + SCHLAEGER_BREITE
}

// Bewege rechten Schläger mittels "Artificial Intelligence"
void bewege_schlaeger_ai2() {
  // Wenn der Ball ganz rechts ist, setze den Schläger sprunghaft auf eine zufällige Position um den Ball herum...
  // ... mit einer Bandbreite, die der gewählten "AI-Stärke" entspricht.
  if ( xb >= XB_MAX ) {
    male_schlaeger(BLACK);
    ys2 = random( yb-SCHLAEGER_BREITE+1-AI_Intervallbreite_delta_einseitig_neu, yb+BALL_DURCHMESSER-1+1+AI_Intervallbreite_delta_einseitig_neu);
    if ( ys2 < YS_MIN ) ys2 = YS_MIN;
    if ( ys2 > YS_MAX ) ys2 = YS_MAX;
    male_schlaeger(WHITE);
  }
  // Notiz: yb + BALL_DURCHMESSER < ys2, yb > ys2 + SCHLAEGER_BREITE
}

// Zeichne die Schläger (in weiß bzw. in schwarz zum löschen/übermalen)
// Schläger sind vertikale Linien der Länge SCHLAEGER_BREITE, ganz links bzw. ganz rechts am Spielfeld.
void male_schlaeger(boolean color) {
  for ( int i = 0; i < SCHLAEGER_BREITE; i++ ) {
    arduboy.drawPixel( XS1, ys1+i, color );
  }
  for ( int i = 0; i < SCHLAEGER_BREITE; i++ ) {
    arduboy.drawPixel( XS2, ys2+i, color );
  }
}

// Prüfe, ob der Ball auf den Schläger trifft. 
// Lasse den Ball entsprechend abprallen bzw. verteile entsprechend Punkte.
void pruefe_treffer() {
  // Notiz: Trefferbedingungen: yb + BALL_DURCHMESSER < ys1, yb > ys1 + SCHLAEGER_BREITE
  // Wenn Ball ganz links
  if ( xb <= XB_MIN ) {
    if ( ( yb + BALL_DURCHMESSER < ys1 ) || ( yb > ys1 + SCHLAEGER_BREITE ) ) {   // Falls linker Schläger nicht den Ball trifft...
      if (sound) beep.tone(beep.freq(TON_PUNKT_FREQ),TON_PUNKT_DAUER);  
      punkte2++;                                                                  // ... erhöhe Punkte für Gegner
      if (punkte2 < punkteMax) delay(PUNKTGEWINN_VERZOEGERUNG);                   // ... warte kurz mit dem Countdown (falls Spiel noch nicht zu Ende)    
      dxbVorzeichen = +1;                                                         // ... setze Richtung für den nächsten Abschlag auf "nach rechts"
      xb = XB_ABSCHLAG1;                                                          // ... setze den Ball auf die Abschlagsposition (x nahe dem linken Spielfeldrand)
      yb = YB_ABSCHLAG;                                                           // ... setze den Ball auf die Abschlagsposition (y in der Mitte)
      setze_variablen_nach_punktgewinn();                                         // ... und bereite für nächsten Anstoß vor
    } else {                                                                      // Falls getroffen...
      if (sound) beep.tone(beep.freq(TON_SCHLAEGER_FREQ),TON_SCHLAEGER_DAUER);      
      ybMitte = yb + BALL_DURCHMESSER / 2;                                        // ... setze Hilfsvariablen
      ys1Mitte = ys1 + SCHLAEGER_BREITE / 2;
      diff1Mitte = ybMitte - ys1Mitte;                                            // ... berechne neue Ballrichtung als Funktion der "Trefferposition"...
      phi = map_float( diff1Mitte, -TREFFERLAENGE_HALBE, TREFFERLAENGE_HALBE, -PHI_MAX, PHI_MAX );  // ... und mappe den Richtungswinkel zwischen -PHI_MAX und PHI_MAX
      v = v + speedErhoehung;                                                     // ... erhöhe den Betrag des Geschwindigkeitsvektors (falls speedErhoehung > 0);
      dxb = v / sqrt( 1 + tan(phi) * tan(phi) );                                  // ... berechne neues dxb und dyb aus v und phi - und dem Wissen um die Bewegungsrichtung in x-Richtung (hier nach rechts)
      dyb = dxb * tan(phi);                                                       // ... berechne neues dxb und dyb aus v und phi - und dem Wissen um die Bewegungsrichtugn in x-Richtung (hier nach rechts)
    }
  }
  // Wenn Ball ganz rechts
  if ( xb >= XB_MAX ) {
    if ( ( yb + BALL_DURCHMESSER < ys2 ) || ( yb > ys2 + SCHLAEGER_BREITE ) ) {   // Falls linker Schläger nicht den Ball trifft...
      if (sound) beep.tone(beep.freq(TON_PUNKT_FREQ),TON_PUNKT_DAUER);
      punkte1++;                                                                  // ... erhöhe Punkte für Gegner
      if (punkte1 < punkteMax) delay(PUNKTGEWINN_VERZOEGERUNG);                     // ... warte kurz mit dem Countdown (falls Spiel noch nicht zu Ende)    
      dxbVorzeichen = -1;                                                         // ... setze Richtung für den nächsten Abschlag auf "nach links"
      xb = XB_ABSCHLAG2;                                                          // ... setze den Ball auf die Abschlagsposition (x nahe dem rechten Spielfeldrand)
      yb = YB_ABSCHLAG;                                                           // ... setze den Ball auf die Abschlagsposition (y in der Mitte)
      setze_variablen_nach_punktgewinn();                                         // ... und bereite für nächsten Anstoß vor
    } else {                                                                      // Falls getroffen...
      if (sound) beep.tone(beep.freq(TON_SCHLAEGER_FREQ),TON_SCHLAEGER_DAUER);
      ybMitte = yb + BALL_DURCHMESSER / 2;                                        // ... setze Hilfsvariablen
      ys2Mitte = ys2 + SCHLAEGER_BREITE / 2;
      diff2Mitte = ybMitte - ys2Mitte;                                            // ... berechne neue Ballrichtung als FUnktoin der "Trefferposition"...
      phi = map_float( diff2Mitte, -TREFFERLAENGE_HALBE, TREFFERLAENGE_HALBE, -PHI_MAX, PHI_MAX );  // ... und mappe den Richtungwinkeln zwischen -PHI_MAX und PHI_MAX
      v = v + speedErhoehung;                                                     // ... erhöhe den Betrag des Geschwindigkeitsvektors (falls speedErhoehung > 0);
      dxb = - v / sqrt( 1 + tan(phi) * tan(phi) );                                // ... berechne neues dxb und dyb aus v und phi - und dem Wissen um die Bewegungsrichtugn in x-Richtung (hier nach links)
      dyb = - dxb * tan(phi);                                                     // ... berechne neues dxb und dyb aus v und phi - und dem Wissen um die Bewegungsrichtugn in x-Richtung (hier nach links)  
    }
  }
  // Zeige die Punktzahlen an
  arduboy.setCursor( PUNKTE1_X, PUNKTE1_Y );
  arduboy.print(punkte1);
  arduboy.setCursor( PUNKTE2_X, PUNKTE2_Y );
  arduboy.print(punkte2);
  arduboy.display();
  // Prüfe, ob ein Spieler die Siegespunktzahl erreicht hat
  if ( ( punkte1 >= punkteMax ) || ( punkte2 >= punkteMax ) ) {                   // Falls ein Spieler gewonnen hat...
    gameover = true;                                                              // ... setze Steuervariable gameover=true, damit der Spielendebildschirm angezeigt wird.
  }
}

// Zeige Countdown und führe Anstoss aus
void fuehre_anstoss_aus() {
  zeichne_spielfeld();                                        // Spielfeld anzeigen
  bewege_schlaeger();                                         // Schläger anzeigen an aktueller (zurückgesetzer) Position
  male_ball(WHITE);
  pruefe_treffer();                                           // Hier nur aufgerufen, um Punktestand hinzuschreiben
  delay(COUNTDOWN_VERZOEGERUNG);
  for ( int k = 3; k > 0 ; k-- ) {                            // Countdown von 3 bis 1
    arduboy.setCursor( 61, 28 );
    arduboy.print(k);
    arduboy.display();
    if (sound) beep.tone(beep.freq(TON_COUNTDOWN_FREQ), TON_COUNTDOWN_DAUER); //ESP8266_Arduboy
    delay(COUNTDOWN_VERZOEGERUNG);
    }
  if (sound) beep.tone(beep.freq(TON_KICKOFF_FREQ), TON_KICKOFF_DAUER); // Und los geht das Spiel (mit Kickoffsound bei erster Ballbewegung (falls sound==true))
}

// Lineare Interpolation, um einen Wert im Intervall (x0,x1) auf das Intervall (y0,y1) linear abzubilden.
// Hilfsunktion, da die eingebaute Funtion map(x,x0,x1,y0,y1) nur für Integer funktioniert.
// float map_float(float value, float fromLow, float fromHigh, float toLow, toHigh)
float map_float(float x, float x0, float x1, float y0, float y1) {
  float y = y0 + (y1-y0)/(x1-x0) * (x-x0);
  return y;
}

// Einmalige Initialisierung bei Programmstart
void setup() {
    arduboy.begin();
//  arduboy.beginNoLogo();                                      // Initialisiere arduboy, aber zeige kein Logo
  arduboy.setFrameRate(FPS);                                  // Setze die Framerate
  arduboy.initRandomSeed();   // Initialisiere den Pseudozufallszahlengenerator, so dass bei jedem Start andere "Zufallszahlen" für Abschlag, AI, etc. geählt werden.   
  beep.begin();   //ESP8266_Arduboy
}

// Hauptprogramm
void loop() {
  // Warte bis der nächste Frame zu zeichnen ist
  if (!(arduboy.nextFrame()))
    return;
    
  // Lösche Bildschirminhalt
  arduboy.clear();

  // Falls (noch) kein Spiel läuft (intro==true), zeige Titel und Menü
  if ( intro ) {
    zeige_startschirm_und_fuehre_menue_aus();
    setze_variablen_vor_spielstart();  
    intro = false;                                            // Menüauswahl getroffen, Spiel soll jetzt laufen, also intro = false
  }

  // Falls erster Ball nach Spielanfang oder nach Punktgewinn (anstoss==true), führe Anstoss aus
  if ( anstoss ) {
    fuehre_anstoss_aus();
    anstoss = false;                                          // Anstoss ausgeführt, also anstoss = false
  }
  
  // Hier läuft das Spiel
  zeichne_spielfeld();                                        // Zeichne das Spielfeld
  bewege_schlaeger();                                         // Bewege die Schläger von Hand
  bewege_ball();                                              // Bewege den Ball
  if (ai1) { bewege_schlaeger_ai1(); }                        // Falls linker Schläger vom Computer gesteuert (ai1==true), bewege den Schläger automatisch 
  if (ai2) { bewege_schlaeger_ai2(); }                        // Falls rechter Schläger vom Computer gesteuert (ai2==true), bewege den Schläger automatisch
  pruefe_treffer();                                           // Prüfe ob/wie der Ball auf den Schläger trifft, berechne entsprechend den Abprall oder verteile entsprechend Punkte

  // Falls ein Spieler die Gewinnpunktzahl erreicht hat (gameover==true), zeige Spielende
  if ( gameover ) {
    zeige_spielende();                                        // Zeige den Gewinner an
    gameover = false;                                         // Gewinner angezeigt, ergo setzt gameover zurück auf false
    intro = true;                                             // Spiel beendet, also Neuanfang mit Menüanzeige
  }

  // Zeige Bildschirminhalt
  arduboy.display();
}
