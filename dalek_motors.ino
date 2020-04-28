#include <Arduino.h>
#include <AccelStepper.h>
#include <Wire.h>

#define slaveAddress 2

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

AccelStepper leftStepper(AccelStepper::DRIVER, 10, 11); // dir,step
AccelStepper rightStepper(AccelStepper::DRIVER, 6, 5); // dir,step

int message = 1;
int prevMessage = 0;
const int maxSpeed = 2000;
bool running = false;  // Start without movement. Can be enabled throught the web-interface


void setup()
{
  Wire.begin(slaveAddress);                // join i2c bus
  Wire.onReceive(receiveEvent);             // register event
  Serial.begin(115200);
  leftStepper.setMaxSpeed(maxSpeed);
  rightStepper.setMaxSpeed(maxSpeed);
  leftStepper.setAcceleration(2000);
  rightStepper.setAcceleration(2000);
  leftStepper.moveTo(1000000);
  rightStepper.moveTo(1000000);
  delay(2000); // wait 20 sec.for the Mega to finish booting.
}

void receiveEvent(int bytes)
{
  message = Wire.read();    // read one character from the I2C
}

void go_forward()
{
  leftStepper.moveTo(1000000);
  rightStepper.moveTo(1000000);
}

void go_back()
{
  leftStepper.moveTo(-200);
  rightStepper.moveTo(-200);
}

void turn_left()
{
  leftStepper.setSpeed((maxSpeed - 500));
  rightStepper.setSpeed((maxSpeed / 5));
}

void turn_right()
{
  rightStepper.setSpeed((maxSpeed - 500));
  leftStepper.setSpeed(maxSpeed / 5);
}

void full_stop()
{
  leftStepper.setSpeed(0);
  rightStepper.setSpeed(0);
}

void test_message()
{
  if (prevMessage != message)
  {
    Debug_print("Ik heb een ");
    Debug_println(message);
    Debug_println(" ontvangen.");
  }
}

void check_message()
{
  switch (message)
  {
  case 1: // forward
    prevMessage = message;
    go_forward();
    break;
  case 2:  // full stop
    prevMessage = message;
    full_stop();
    break;
  case 3:  // turn left
    prevMessage = message;
    turn_left();
    break;
  case 4:  // turn right
    prevMessage = message;
    turn_right();
    break;
  case 5:  // go backwards
    prevMessage = message;
    go_back();
    break;
  case 6:  // disable movement
  if (prevMessage != message)
  {
    prevMessage = message;
    running = false;
    break;
  }
  case 7:  // enable movement
  if (prevMessage != message)
  {
    prevMessage = message;
    running = true;
    break;
  }
}
}

void run_steppers()
{
  leftStepper.run();
  rightStepper.run();
}

void loop()
{
  test_message();
  check_message();
  if (running == 1) run_steppers();
}
