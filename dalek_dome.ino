#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <Wire.h>
#include <FastLED.h>

//*********************************************************************
// Debugging options
// Define the Debug_print() command to enable serial debugging.
// The Debug_print() command prints its output to the serial port.
// If you comment #define DEBUG, the compiler wil not include  the
// Debug_print() commands.
//*********************************************************************
// Debugging on or off
#define DEBUG

#ifdef DEBUG                                // If debugging on, then
#define Debug_print(x) Debug_print(x)      // define Debug_print()
#define Debug_println(x) Debug_println(x)  // define Debug_println()
#else                                       // if not
#define Debug_print(x)                      // remove Debug-print()
#define Debug_println(x)                    // remove Debug-println()
#endif

bool sound = true;  // Sound enabled. Can be disabled through Webinterface
bool display = true;  // Display mode enabled. Can be disabled through Webinterface

// I2C definitions and variables
#define SLAVE_ADDRESS 4
int message = 0;
int PREV_message = 0;


// FastLED definitions
#define NUM_LEDS 1
#define DATA_PIN 3
#define CHIPSET WS2811
#define COLOR_ORDER RGB
int BRIGHTNESS = 25;
CRGB leds[NUM_LEDS];

CRGB index_color[] = { CRGB::Khaki, CRGB::Aqua, CRGB::DarkMagenta, CRGB::DarkSeaGreen, CRGB::Amethyst, CRGB::RosyBrown, CRGB::OrangeRed, CRGB::Yellow, CRGB::LightCoral, CRGB::OldLace};

//bored bored
unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
unsigned long pulseMillis;
const unsigned long boredTime = 900000;   // time in milliseconds (15 min)
int boredCount = 0;

// MP3 player definitions and variables
SoftwareSerial mySoftwareSerial(5, 6); // RX, TX
DFRobotDFPlayerMini myMP3;
int volume = 30;


void printDetail(uint8_t type, int value);

void receiveEvent(int bytes)
{
  message = Wire.read();    // read one character from the I2C
}

void debug_message()  // for debugging purposes only
{
//  if (PREV_message != message){
    ("Ik heb een ");
    Debug_print(message);
    Debug_println(" ontvangen.");
    delay(500);
//    PREV_message = message;
//  }
}

void check_message()
{
  switch (message)
  {
  case 10:  //Default situation
  if (PREV_message != message){
    Debug_println("Back to normal, Blue Stalk");
    PREV_message = message;
    StalkBlue();
  }
  break;
  case 11:  // Stalk White and scream Stay Away!!
  if (PREV_message != message && display == true){
    Debug_println("Stay Away!!");
    PREV_message = message;
    Tooclose();
  }
  break;
  case 12:  // Stalk Red and scream EXTERMINTE!!
  if (PREV_message != message){
    Debug_println("Exterminate!!!");
    PREV_message = message;
    Exterminate();
  }
  break;
  case 14:   // Sound off
  if (PREV_message != message){
    PREV_message = message;
    sound = false;
    Debug_println("Sound OFF!");
  }
  break;
  case 15:   // Sound on
  if (PREV_message != message){
    PREV_message = message;
    sound = true;
    Debug_println("Sound ON!");
  }
  break;
  case 16:    // Volume up
  message = 0;
  Debug_println("Volume UP");
  myMP3.volumeUp();
  break;
  case 17:    // Volume down
  message = 0;
  Debug_println("Volume DOWN");
  myMP3.volumeDown();
  break;
  case 18:   // display mode on
  if (PREV_message != message){
    PREV_message = message;
    display = true;
    Debug_println("Display mode ON!");
  }
  break;
  case 19:   // display mode off
  if (PREV_message != message){
    PREV_message = message;
    display = false;
    Debug_println("Display mode OFF!");
  }
  break;
  }
}

void Playsound(int dir, int song){
  if (sound == true ){
    myMP3.playFolder(dir, song);
  }
}

void Moan()
{
  myMP3.volume(volume);
  Playsound(10,3);
}

void Tooclose()
{
  Debug_println("You are too close!!");
  myMP3.volume(volume);
  BRIGHTNESS = 255;
  leds[0] = CRGB::White;
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.show();
  Playsound(10,4);
  delay(2000);
  for (int i = BRIGHTNESS; i >= 0; i--){
    FastLED.setBrightness(i);
    delay(10);
    FastLED.show();
  }
  startMillis = millis();
}

void Exterminate()
{
  Debug_println("Exterminate!!");
  myMP3.volume(30);
  BRIGHTNESS = 255;
  leds[0] = CRGB::Red;
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.show();
  Playsound(10,1);
  delay(2000);
  for (int i = BRIGHTNESS; i >= 0; i--){
    FastLED.setBrightness(i);
    delay(10);
    FastLED.show();
  }
  myMP3.volume(volume);
  startMillis = millis();
}

void StalkBlue(){
  leds[0] = CRGB::Blue;
  BRIGHTNESS = 0;
  for (int i = BRIGHTNESS; i <= 255; i++){
    FastLED.setBrightness(i);
    delay(10);
    FastLED.show();
  }
}

void PulseStalk(){
  leds[0] = CRGB::Blue;
// turn light down
for (int i = 255; i >=20; i--){
  FastLED.setBrightness(i);
  //delay(5);
  FastLED.show();
}
// and up again
  for (int i = 20; i <= 255; i++){
    FastLED.setBrightness(i);
    //delay(5);
    FastLED.show();
  }
  pulseMillis = millis();
}

void Bored(){
  Debug_println("I am bored!!");
  myMP3.volume(volume);
  if (boredCount <=2){
    Playsound(10,3);  // Moan
    boredCount = boredCount + 1;
  }
  else {
    Debug_println("I am realy bored!!");
    Playsound(10,10);
    for (int t = 0; t <=6; t++){
      for (int i = 0; i<= 9; i++){
        leds[0] = index_color[i];
        FastLED.show();
        delay(500);
        boredCount =0;
      }
    }
  }
  startMillis = millis();
  pulseMillis = millis();
}

void bootdelay(){
  Debug_println("Waiting for all systems to boot");
  unsigned long bootMillis = millis();
  while (millis()- bootMillis < 15000) // 15 seconds bootdelay
  {
      leds[0] = CRGB::Red;
      FastLED.show();
      delay(333);
      leds[0] = CRGB::White;
      FastLED.show();
      delay(333);
      leds[0] = CRGB::Yellow;
      FastLED.show();
      delay(333);
//      leds[0] = CRGB::Black;
//      FastLED.show();
  }
}

test(){
  for (int t = 0; t <=6; t++){
    for (int i = 0; i<= 9; i++){
      leds[0] = index_color[i];
      FastLED.show();
      delay(500);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(Typical8mmPixel);
  FastLED.clear(1);
  while(!myMP3.begin(mySoftwareSerial))
  {
    Debug_println("Player not (yet) ready!");
    mySoftwareSerial.begin(9600);
    delay(500);
  }
  Wire.begin(SLAVE_ADDRESS);                // join i2c bus
  Wire.onReceive(receiveEvent);             // register event
  startMillis = millis();
  bootdelay();
  Debug_println("All systems go!!");
  myMP3.volume(volume);
  Playsound(10,3);
}

void loop()
{
//  debug_message();     // uncomment to see debugging messages
  check_message();
  Debug_println("Messages checked");
  currentMillis =  millis();
  if (currentMillis - pulseMillis >= 10000 && display == true)   // Pulsate stalklight every 10 sec.
  {
    PulseStalk();
  }
  if (currentMillis - startMillis >= boredTime && display == true)   // Am I bored yet?
  {
    Bored();  // Yes I am
  }

}
