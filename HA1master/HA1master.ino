/*  Created by Peter Williams 19/06/2018
 *   Added Comment - testing commit 2
 *   
 *  Track pitch for synthesis   
 *  MixOut lost in IDE
 *  PUBlend set to even
 *  All processing delteted
 *  
 *  
 *  
 *  
Neck encoder:
29,28, A16
Middle encoder:
26, 27, A15(34)
Bridge encoder:
24,25 A14(33)
LED:
30 - 32
*/

#include <Encoder.h>  // encoders library
#include "OneButton.h"  // Library for double, single and long press clicks, only works with one instance
#include <Bounce2.h>  // Simple debouncer for clicks on encoders 2 & 3


#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=193,166
AudioAnalyzeNoteFrequency freq1;          //xy=278,422
AudioMixer4              PUBlend;        //xy=344.00000762939453,181.00000381469727
AudioOutputI2S           i2s2;           //xy=491.00003814697266,186.00000667572021
AudioConnection          patchCord1(i2s1, 0, PUBlend, 0);
AudioConnection          patchCord2(i2s1, 0, freq1, 0);
AudioConnection          patchCord3(i2s1, 1, PUBlend, 1);
AudioConnection          patchCord4(PUBlend, 0, i2s2, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=205,115
// GUItool: end automatically generated code



#define GRANULAR_MEMORY_SIZE 12800*2  // enough for 290 ms at 44.1 kHz
int16_t granularMemory[GRANULAR_MEMORY_SIZE];

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

// Use these with the Teensy 3.5 & 3.6 SD card
#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11  // not actually used
#define SDCARD_SCK_PIN   13  // not actually used
const int myInput = AUDIO_INPUT_LINEIN;

// Define bounce buttons
#define BUTTON_PIN_1 34
#define BUTTON_PIN_2 35
// Setup a new OneButton on pin A16. (Neck encoder)  
OneButton button(A14, true);
// Define encoders
Encoder myEnc(25, 24);  // My Neck Encoder
Encoder myEnc2(27, 26);  // My Mid Encoder
Encoder myEnc3(29, 28);  // My Bridge Encoder
// i2s1 Bridge pickup is 0, neck Pickup is 1
// i2s2 0 is jack output

// Instantiate a Bounce objects
Bounce debouncer1 = Bounce(); 
Bounce debouncer2 = Bounce(); 

// set up LED - Not currently in use, a perspex inlay could be prepared
int redPin = 30;
int greenPin = 31;
int bluePin = 32;

int fsr1Pin = A17;
int fsr2Pin = A18;
int fsr1; int fsr2; float fsr;



// setup code here, to run once:
void setup() {
   AudioMemory(120);
  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(1);
   // the Granular effect requires memory to operate
//  granular1.begin(granularMemory, GRANULAR_MEMORY_SIZE);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
  
    // Setup the buttons with internal pull-ups :
  pinMode(BUTTON_PIN_1,INPUT_PULLUP);
  pinMode(BUTTON_PIN_2,INPUT_PULLUP);
  // After setting up the button, setup the Bounce instances :
  debouncer1.attach(BUTTON_PIN_1);
  debouncer1.interval(5); // interval in ms
  debouncer2.attach(BUTTON_PIN_2);
  debouncer2.interval(5); // interval in ms
  // link the doubleclick function to be called on a doubleclick event. 
  button.attachPress(press);  
  button.attachClick(click);
  button.attachDoubleClick(doubleclick);
  // enable the leds
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  // De-bugging
  Serial.begin(9600);
  
  
  
} // setup
// starting encoder values
// ########## MAKE SURE THESE MATCH STARTING VALUES  
float neckPosition  = -999;
float Neck = 0;
float neck2Position  = -999;
float Neck2 = 0;
float neck3Position  = -999;
float Neck3 = 0;
float midPosition  = -999;
float Mid = 5;
float mid2Position  = -999;
float Mid2 = 0;
float brijPosition  = -999;
float Brij = 0;
float brij2Position  = -999;
float Brij2 = 0;
// mode switching
int setting = 1;
int setting2 = 1;
int level = 1;
// EQ Settings

// main code here, to run repeatedly: 
void loop() {
// Dynamic and tone
freq1.begin(0.1);

    if (freq1.available()) {
        float note = freq1.read();
        float prob = freq1.probability();
        Serial.print(note, prob);
    }

  // ###### FSRS ################ //
  fsr1 = analogRead(fsr1Pin);
  fsr2 = analogRead(fsr2Pin);
  fsr = fsr1-fsr2;
  
//  Serial.print(fsr1);
//  Serial.print(" ### ");
//  Serial.print(fsr2);
//  Serial.print(" ### "); 

float pub1 = 0.5;
float pub2 = 0.5;

PUBlend.gain(0,pub2);
PUBlend.gain(1,pub1);

//sweep.setNotch(2,800,2);
//sweep.setNotch(3,800,2);




// MixOut.gain(0,1);


  // Set parameters
 

// #### debug #####
//Serial.print(String(Neck)+",");
//Serial.print(String(Mid2)+",");
//Serial.print(String(Mid)+",");
//Serial.print(String(hpr)+",");
//Serial.print(String(lbf)+",");
//Serial.print(String(lbq)+",");
//Serial.print(String(lpf)+",");
//Serial.print(String(lpq)+",");
//Serial.print(String(lbg)+",");
//Serial.print(String(sg)+",");
////Serial.print(String(Neck3)+",");
//Serial.print(String(Mid)+",");
//Serial.print(String(nf)+",");
//Serial.print(String(nq)+",");
//Serial.print(String(Mid2)+",");
//Serial.print(String(Brij)+",");
//Serial.print(String(Brij2)+",");

Serial.println("");

// read encoder settings
  float newNeckPosition = myEnc.read();
  float newNeck2Position = myEnc.read();
  float newNeck3Position = myEnc.read();
  float newMidPosition = myEnc2.read();
  float newMid2Position = myEnc2.read();
  float newBrijPosition = myEnc3.read();
  float newBrij2Position = myEnc3.read();
  
/*  ############################ ENCODER READING #########################
 *  ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤ NECK ENCODER ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤   
 */
  if (level == 1){
    if (newNeckPosition <0){
    myEnc.write(0);
  }
  if (newNeckPosition >100){
    myEnc.write(100);
  }
   if (newNeckPosition != neckPosition) {
   neckPosition = newNeckPosition;
  Neck = constrain(newNeckPosition,0,100);
 }}
// eof default (single click) neck setting

// first level (double click) neck setting
  if (level == 2){
  if (newNeck2Position <0){
    myEnc.write(0);
  }
  if (newNeck2Position >200){
    myEnc.write(200);
  }
   if (newNeck2Position != neck2Position) {
   neck2Position = newNeck2Position;
  Neck2 = newNeck2Position;
 // Useful range: 100 - 
  // overall volume adjustments and a bass boost when low pass is set high
  
   //  delay (5);
    }}
// eof first level (double click) neck setting

// Second level (long click) neck setting
  if (level == 3){
  if (newNeck3Position <0){
    myEnc.write(0);
  }
  if (newNeck3Position >100){
    myEnc.write(200);
  }
   if (newNeck3Position != neck3Position) {
   neck3Position = newNeck3Position;
  Neck3 = newNeck3Position;
  //  delay (5);
    }}
// eof second level (long click) neck setting
/* ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 *  ¤¤¤¤¤¤¤¤¤¤¤ MIDDLE ENCODER ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 */
if (setting==1){
   if (newMidPosition <0){
    myEnc2.write(0);
  }
  if (newMidPosition >100){
    myEnc2.write(100);
  }
   if (newMidPosition != midPosition) {
   midPosition = newMidPosition;
  Mid = constrain(newMidPosition,0,100);
  }}
  
if (setting==2){
   if (newMid2Position <0){
    myEnc2.write(0);
  }
  if (newMid2Position >100){
    myEnc2.write(100);
  }
   if (newMid2Position != mid2Position) {
   mid2Position = newMid2Position;
  Mid2 = constrain(newMid2Position,0,100);
     }}
/* ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 *  ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤ BRIDGE ENCODER ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 */
     
if (setting2==1){
if (newBrijPosition <0){
    myEnc3.write(0);
    
  }
  if (newBrijPosition >200){
    myEnc3.write(200);
  }
   if (newBrijPosition != brijPosition) {
   brijPosition = newBrijPosition;
  Brij = constrain(newBrijPosition,-50,200);
   }}

  if (setting2==2){
   if (newBrij2Position <-50){
    myEnc3.write(-50);
  }
  if (newBrij2Position >50){
    myEnc3.write(50);
  }
   if (newBrij2Position != brij2Position) {
   brij2Position = newBrij2Position;
  Brij2 = constrain(newBrij2Position,-50,50);
 }}  
// ##################################### END OF ENCODER READINGS #################################
  otherclicks();
  // keep watching the push button on neck pickup:
  button.tick();

  
//  delay(10);
} // loop

// click functions for neck encoder
void doubleclick() {
  // Serial.println("double clicked");
  myEnc.write(Neck2);
 level = 2;
 delay(20);
} 
void click() {
 // Serial.println("single clicked");
 myEnc.write(Neck);
 level = 1;
 delay(20);
} 
void press() {
  myEnc.write(Neck3);
 level = 3;
 delay(20);
  
} 
void otherclicks(){
    // Update the Bounce instances :
  debouncer1.update();
  debouncer2.update();

  // Get the updated value :
  int value1 = debouncer1.read();
  int value2 = debouncer2.read();
  if (value1 == LOW){
   setting = 2;
   myEnc2.write(Mid2);
   delay(50);
  }
  else {
   setting = 1;
   myEnc2.write(Mid);
   //delay(50);
  }
  if (value2 == LOW){
   setting2 = 2;
   myEnc3.write(Brij2);
   delay(50);
    }
  else {
    setting2 = 1;
   myEnc3.write(Brij);
   delay(50);
    }}
 // End
