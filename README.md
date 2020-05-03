# My "New Series" Daleks

One of the most intriguing (for me at least) characters of the BBC series 'Doctor Who' is the Dalek. 
Their first appearence in the series was in the 1963 Doctor Who episode 'The Daleks', (also known as 'The Mutants and The Dead Planet'.<br />
Since their first appearance there have been a number of variations of the Daleks. In the sixth episode of the 'new series' (started in march 2005) a completely redesigned lone Dalek appeared. <br />
This design is known as the New Series Dalek and I just had to have one. :wink:

<p align="center">
  <img width="300" height="400" src="/images/dalek.png">
</p>

## The build

Before you start reading and maybe even using my code a small advance warning:<br />
**This build is overly and unnecessary complex, and I did this on purpose**<br />

There are three main reasons for this complexity:
1. During pre-build tests I ran into problems with the not-so multithreading behaviour of the good old arduino.
2. I happened to have a box with a number of different Arduinos laying arround.
3. I wanted to get some experience with I2C communication between microcontrollers.

## The Dalek body

The Actual Dalek is build from plan I purchased on the project dalek forum. These are realy detailed and made building anlot easier for me.
I started with the skirt. I made the pannels from plywood and glued them together. And after a trip to the "Action" to get some pingpong balls and some paint at the hardware store the skirt looks like this:

<img src="/images/skirt_1.jpg" width="30%"></img> <img src="/images/skirt_2.jpg" width="30%"></img> <img src="/images/skirt_3.jpg" width="30%"></img>  

In the sametime I was building the skirt I was finishing the build of a new 3D printer with a build volume of40x40x40 cm. 
So I decided to print the rest of the Dalek.<br />
One of the Project Dalek forum moderators, Audry2, created a scale Renegade Dalek kit wich can be downloaded from the forum.
I found an other version from the same author on thingiverse [thing 369866](https://www.thingiverse.com/thing:369866).<br />
I used this version to print the rest of my Dalek.<br />
After some test prints I found that if I resized the parts 385% they where at the same scale as the skirt I build before. <br />
Knowing that, I started printing, sanding, painting and glueing it all together. 
<p align="center">
  <img width="300" height="250" src="/images/shoulders.jpg">
</p>
The complete result is the Dalek shown at the top of this readme.

## The electronics
The Dalek is automated using 4 different types of Arduinos, 3 Ultrasonic sensors for obstacle detection and avoidance and two NEMA17 stepper motors for movement.<br /> 
2 bright white LEDs that I salvaged from two **IKEA OLEBY** flashlights act as dome lights and I have put a 5mm ws2811 RGB LED in the eyestalk.<br />
Sound is produced by a MP3 module and a car speaker that I had lying around .<br/>
The complete schematics:
<p align="center">
  <img width="800" height="400" src="/images/dalek_fritzing.png">
</p>

### Ultrasonic sensors
I use three Maxbotix EZ1 ultra-sonic sensors for obstacle avoidance. The sensors are daisychained and triggered by a common triggerPin. <br />
I am using two pins for each of these. The triggerPin is wired to RX. The second pin is wired to PWM for input.
The variables LEFT_SONIC, FRONT_SONIC and RIGHT_SONIC in the main code contain the Arduino pwm numbers.<br />
The sensors are connected to an Arduino Mega and are polled coninuesly. The result of the polling is put on the I2C bus so the other Arduinis on the bus can react to the results.
### Movement
For movement I am using two NEMA17 steppermotors. These are controlled by two Big Easy driversconnected to an Arduino nano.<br />
The nano listens to the I2C bus and reacts to messages the mega sends to it and steers the Dalek accordingly. This results in the Dalek steering its way around objects end backing out of blockages.<br/>
The Dalek starts in what I call **Display Mode**. This means it is fully operational except for movement. In this mode you can put it in your living room wher it stays where you put it but it still detects if someone or something is getting close and reacts to that.<br />
This Display Mode mode can be disabled through the web interface.
### Web interface
An ESP-01 is functioning as webinterface.
<p align="center">
  <img width="200" height="300" src="/images/dalek_webgui.png">
</p>
Through this webinterface you can disable or enable:

1. Display mode<br />
This mode is enabled by default. It disables mivement but leaves all other functions operational. The Dalek wil still sense objects in it's surroundings and will react to it with sound, domelights and the eyestalkwill change colour. 
2. Motors<br />
This wil enable movement. **BUT** it can only be enabled after first disabling Display mode.This is a safety measure to prevent the accidental enabling of movement while the Dalek is put on a table for display purposes and runs of the table. You have to do two thing to enable motion.
3. Sound
Sound is enabled by default. If you get tired of the Dalek moaning out of boardom every once in a while you can silence it.The eye stalk will stil work and change color if something or someone gets too close.
### Lights and sound
Sound is handled by a DFPlayer module wich is an A Mini MP3 Player you can connect to an arduino. In my case it is connected to an arduino pro mini, together with the domelights and the LED in the eyestalk.<br />
The DFPlayer holds a SD card where the soundfiles are stored. Depending on the I2C message it receives from the mega or the ESP to set the mode the Dalek is in it wil or wil not play soundfiles.<br />
During startup the eyestalk flashes some colours to indicate it is not ready to go yet. If it's ready it makes a moaning sound an light up blue and pulsating. If there is something whitin the 'personal space' of the Dalek the Eyeystalk wil change color. White when something enters the outer limits and red when the inner limit is triggered. At the same time it will scream Stay away or Exterminate.<br />
The domelights pulsate while sound is played by the sound module.This is dome by putting a TIP120 MOSFET on one of the audio outputs. This makes the domelights flash in sync with any soundfile played. 
## The arduino code


## Acknowledgments

This Dalek would never have been build without the help of all the great guys at the [Dalek Forum](https://www.projectdalek.com/) and the plans and schematics they have produced there.
