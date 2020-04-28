#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include "secrets.h"

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

#define domeSlave 4                       // Addres of Pro Mini on I2C bus
#define motorSlave 2                     // Addres of Nano on I2C bus

char ssid[] = secretSSID;   // your network SSID (name)
char password[] = secretPass;   // your network password

WiFiServer server(80);                  // Set port to 80
String header;                          // This stores the HTTP request
String displayMode = "aan";
String movementState = "uit";           // movement state
// String sensorstate = "aan";             // sensors state
String soundState = "aan";              // sound state

int message = 0;                   // MESSAGE for main Arduino
int prevmessage = 0;
int volume = 30;

void setup() {
  Wire.begin(0,2);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Debug_print("Connecting to ");
  Debug_println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Debug_print(".");
  }
  Debug_println("");
  Debug_println("WiFi connected.");
  Debug_println("IP address: ");
  Debug_println(WiFi.localIP());
  server.begin();
}

void sendmessage(int Dest, int message)
{
//  if (prevmessage != message)
//  {
    Wire.beginTransmission(Dest);   // transmit to main device
    Wire.write(message);
    Wire.endTransmission();               // stop transmitting
    Debug_print("Send MESSAGE ");
    Debug_print(message);
    Debug_print(" to ");
    Debug_println(Dest);
//  }
//  prevmessage = message;
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients
  if (client) {                             // If a new client connects,
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /displayMode/on") >= 0) {
              displayMode = "aan";
              movementState = "uit";
  //            sensorstate = "aan";
              soundState = "aan";
              Debug_println("DisplayMode on");
              //mainMessage = 100;
              sendmessage(motorSlave,6);  // send RUNNING = false to motors
              sendmessage(domeSlave,15); // send SOUND = true to dome
            }
            if (header.indexOf("GET /displayMode/off") >= 0) {
              displayMode = "uit";
              movementState = "uit";
//              sensorstate = "uit";
              soundState = "uit";
              Debug_println("DisplayMode off");
              //mainMessage = 101;
              sendmessage(motorSlave,6);  // send RUNNING = false to motors
              sendmessage(domeSlave,14); // send SOUND = false to dome
            }
            if (header.indexOf("GET /movement/on") >= 0) {
//              if (displayMode == "uit" && sensorstate == "aan"){
              if (displayMode == "uit"){
                movementState = "aan";
                Debug_println("Movement on");
                sendmessage(motorSlave,7);  // movement on
                sendmessage(domeSlave,19);  // display mode off
              }
            }
            else if (header.indexOf("GET /movement/off") >= 0) {
              movementState = "uit";
              Debug_println("Movement off");
              sendmessage(motorSlave,6);  // movement off
              sendmessage(domeSlave,18);  // display mode on
            }
            if (header.indexOf("GET /sound/on") >= 0) {
              soundState = "aan";
              Debug_println("Sound on");
              sendmessage(domeSlave,15);
            }
            else if (header.indexOf("GET /sound/off") >= 0) {
              soundState = "uit";
              Debug_println("Sound off");
              sendmessage(domeSlave,14);
            }
            else if (header.indexOf("GET /volume/down") >= 0) {
              if (volume == 0){
                volume = 0;
                }
                else {
                  volume = volume -1;
                }
              sendmessage(domeSlave,17);
            }
            else if (header.indexOf("GET /volume/up") >= 0) {
              if (volume == 30){
                volume = 30;
                }
                else {
                  volume = volume +1;
                }
              sendmessage(domeSlave,16);
            }
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><title>Dalek control station</title> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
//            client.println("<meta http-equiv=refresh content=10;URL='//dalek.zwartegat.thuis/'>");
            client.println("<meta http-equiv=refresh content=10;URL='//192.168.30.30/'>");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the buttons
            client.println("<style>html { background-color: #B0C4DE; font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #ff2d00; border: none; border-radius: 12px; color: white; width: 150px; padding: 10px 10px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #08b203;}");
            client.println(".RoundButton {font-family: Helvetica; display: inline-block; height: 40px; width: 55px; background-color:#eae0c2;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer; border-radius: 12%; border: none;}");
            client.println(".RoundButton2 {font-family: Helvetica; display: inline-block; height: 40px; width: 55px; background-color: #eae0c2;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer; border-radius: 12%; border: none;}");
            client.println(".p1 {font-size: x-large; font-family: Helvectia; color: black} </style></head>");

            // Web Page Heading
            client.println("<body><h1>!! Exterminate !!</h1>");

            // If displayMode is off, it displays the ON button
              client.println("<p><b>DisplayMode = " + displayMode + "</p></b>");
            if (displayMode == "uit") {
              client.println("<p><a href=\"/displayMode/on\"><button class=\"button\">display</button></a></p>");
            } else {
              client.println("<p><a href=\"/displayMode/off\"><button class=\"button button2\">display</button></a></p>");
            }
            client.println("</body></html>");

            // Display current state, and ON/OFF buttons for movement
            client.println("<p><b>Motoren = " + movementState + "</p></b>");
            // If movement is off, it displays the ON button
            if (movementState == "uit") {
              client.println("<p><a href=\"/movement/on\"><button class=\"button\">motoren</button></a></p>");
            } else {
              client.println("<p><a href=\"/movement/off\"><button class=\"button button2\">motoren</button></a></p>");
            }
            client.println("<p><b>Geluid = " + soundState + "</p></b>");
            // If sensors is off, it displays the ON button
            if (soundState == "uit") {
              client.println("<p><a href=\"/sound/on\"><button class=\"button\">geluid</button></a></p>");
            } else {
              client.println("<p><a href=\"/sound/off\"><button class=\"button button2\">geluid</button></a></p>");
            }
            client.println("<p><b>Volume</p></b>");
            client.println("<p class = p1><a href=\"/volume/up\"><button class=\"RoundButton\">+</button></a>");
            client.println("<b>&nbsp;" + String(volume) + "&nbsp;</b>");
            client.println("<a href=\"/volume/down\"><button class=\"RoundButton2\">-</button></a></p>");
            client.println("</body></html>");
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    header = "";      // Clear the header variable
    client.stop();    // Close the connection
    Debug_println("Client disconnected.");
    Debug_println("");
  }
}
