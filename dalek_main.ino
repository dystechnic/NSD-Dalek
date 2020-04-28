#include <Arduino.h>
#include <Wire.h>

#define motorSlave 2                     // Addres of Nano on I2C bus
#define domeSlave 4                       // Addres of Pro Mini on I2C bus

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
#define Debug_print(x) Serial.print(x)      // define Debug_print()
#define Debug_println(x) Serial.println(x)  // define Debug_println()
#else                                       // if not
#define Debug_print(x)                      // remove Debug-print()
#define Debug_println(x)                    // remove Debug-println()
#endif

// I have 3 daisychained Maxtronic
const int rightSonic= 2;
const int centerSonic = 3;
const int leftSonic = 4;
const int triggerPin = 5;

int message = 0;
int prevMessage = 0;

bool minDetectedOnce = false;
bool midDetectedOnce = false;

long rightPulse, centerPulse, leftPulse, rightCM, centerCM, leftCM, rightCurrent, centerCurrent, leftCurrent, rightPrevious, centerPrevious, leftPrevious;
const int minSonicRange = 30; // 30 cm
const int midSonicRange = 50; // 50 cm
const int maxSonicRange = 300; // 300 cm

void setup ()
{
  Wire.begin();
  Serial.begin(115200);
  pinMode(rightSonic, INPUT);
  pinMode(centerSonic, INPUT);
  pinMode(leftSonic, INPUT);
  pinMode(triggerPin,OUTPUT);
}

void read_sensors()
{
  rightPulse = pulseIn(rightSonic, HIGH);
  centerPulse = pulseIn(centerSonic, HIGH);
  leftPulse = pulseIn(leftSonic, HIGH);

  /*
    Sound travels at 343 meters per second, which means it needs 29.155 microseconds per centimeter.
    So, we have to divide the duration by 29 and then by 2 because the sound has to travel the distance twice.
    It travels to the object and then back to the sensor.
  */

  rightCM = rightPulse / 29 / 2;
  rightPrevious = rightCurrent;
  rightCurrent = rightCM;

  centerCM = centerPulse /29 /2;
  centerPrevious = centerCurrent;
  centerCurrent = centerCM;

  leftCM = leftPulse /29 /2;
  leftPrevious = leftCurrent;
  leftCurrent = leftCM;
}

void start_sonic(){
  digitalWrite(triggerPin,HIGH);
  delayMicroseconds(25);
  digitalWrite(triggerPin,LOW);
}

void debug_sensors()
{

  if (rightCurrent > minSonicRange && centerCurrent > minSonicRange && leftCurrent > minSonicRange) // Enough clearence everywhere
  {
    Debug_println("Nothing within range. Everything > minSonicRange");
  }
  if (rightCurrent <= minSonicRange && centerCurrent <= minSonicRange && leftCurrent <= minSonicRange) // We are stuck in all directions
  {
    Debug_println("Stuck in every direction. Everything <= minSonicRange");
  }
  if (rightCurrent <= midSonicRange && centerCurrent > midSonicRange && leftCurrent > midSonicRange) // Middle distance right triggered
  {
    Debug_println("Not enough clearence on the right side. Right <= midSonicRange");
  }
  if (rightCurrent > midSonicRange && centerCurrent > midSonicRange && leftCurrent <= midSonicRange)  // Middle distance left triggered
  {
    Debug_println("Not enough clearence on the left side. Left <= midSonicRange");
  }
  if (centerCurrent <= midSonicRange && rightCurrent > midSonicRange && leftCurrent > midSonicRange)  // Middle distance center triggered
  {
    Debug_println("Not enough clearence in front. Center <= midSonicRange");
  }
  if (rightCurrent <= minSonicRange && centerCurrent > minSonicRange && leftCurrent > minSonicRange) // Not enough clearence on the right side
  {
    Debug_println("Not enough clearence on the right side. Right <= minSonicRange");
  }
  if (rightCurrent > minSonicRange && centerCurrent > minSonicRange && leftCurrent <= minSonicRange)  // Not enough clearence on the left side
  {
    Debug_println("Not enough clearence on the left side. Left <= minSonicRange");
  }
  if (centerCurrent <= minSonicRange && rightCurrent > minSonicRange && leftCurrent > minSonicRange)  // Not enough clearence in front
  {
    Debug_println("Not enough clearence in front. Center <= minSonicRange");
  }
}

void print_distances()
{
  Debug_print("rightCurrent = ");
  Debug_println(rightCM);
  Debug_print("centerCurrent = ");
  Debug_println(centerCM);
  Debug_print("leftCurrent = ");
  Debug_println(leftCM);
  delay(500);
}

void action()
{
  if (rightCurrent > minSonicRange && centerCurrent > minSonicRange && leftCurrent > minSonicRange) // Enough clearence everywhere
  {
    Debug_println("Nothing within range. Everything > minSonicRange");
    sendmessage(motorSlave,1); // move forward
    sendmessage(domeSlave,10); // normal operation
    minDetectedOnce = false;
    midDetectedOnce = false;
  }
  if (rightCurrent <= minSonicRange && centerCurrent <= minSonicRange && leftCurrent <= minSonicRange) // We are stuck in all directions or something came too close
  {
    Debug_println("We are stuck");
    sendmessage(motorSlave,2);   // full stop
  }
  if (rightCurrent <= minSonicRange && centerCurrent > minSonicRange && leftCurrent > minSonicRange) // Not enough clearence on the right side
  {
    Debug_println("Not enough clearence on the right side");
    sendmessage(motorSlave,3);   // move left
  }
  if (rightCurrent > minSonicRange && centerCurrent > minSonicRange && leftCurrent <= minSonicRange)  // Not enough clearence on the left side
  {
    Debug_println("ot enough clearence on the left side");
    sendmessage(motorSlave,4);   // move right
  }
  if (centerCurrent <= minSonicRange && rightCurrent > minSonicRange && leftCurrent > minSonicRange)  // Not enough clearence in front
  {
    Debug_println("Not enough clearence in front");
    sendmessage(motorSlave,5);   // reverse
  }
  if (rightCurrent <= midSonicRange || centerCurrent <= midSonicRange || leftCurrent <= midSonicRange)  // Something entered midSonicRange
  {
    Debug_println("mid detection range");
    if (midDetectedOnce == false)
    {
      Debug_println("mid not detected earlier");
      sendmessage(domeSlave,11);  // Stalk White and scream Stay Away!!
      midDetectedOnce = true;
    }
  }
  if (rightCurrent <= minSonicRange || centerCurrent <= minSonicRange || leftCurrent <= minSonicRange) // something came too close
  {
    Debug_println("min detection range");
    if (!minDetectedOnce)
    {
      Debug_println("min not detected earlier");
      sendmessage(domeSlave,12);  // Stalk Red and scream EXTERMINTE!!
      minDetectedOnce = true;
    }
  }
}

void sendmessage(int Dest, int message)
{
  if (prevMessage != message)
  {
    Wire.beginTransmission(Dest);   // transmit to destination device
    Wire.write(message);
    Wire.endTransmission();               // stop transmitting
  }
  prevMessage = message;
}

void loop ()
{
  start_sonic();
  read_sensors();
  delay(500);
//  debug_sensors();   // uncomment for debugging messages
  action();
}
