/*  Created by Peter Williams 19/06/2018
 *   Added Comment
 *   
 *   NEW TONE CONTROL
 *   
 *   nECK AND bRIJ ARE SWAPPED    
Neck encoder:
29,28, A16
Middle encoder:
26, 27, A15(34)
Bridge encoder:
24,25 A14(33)
LED:
30 - 32
*/

/*Remember to adjust coefficients for storing values
click states on neck encoder
Also working on tone control */

/* ############### TO DO ############################
 *  1. Add global variables so that parameters can be used directly in effects
 *  1. b) add fade in   ########## FAILED  ##############
 *  2. Add audio board, ip,op
 *  3. Choose effects
 *  4. Test Effects
 *  5. Add neck control
 *  6. Add wiring colors
 *  
 *  Effects possibilities:
 *  1. Distortion
 *  2. Compression
 *  3. Chorus
 *  4. Octaver
 *  5. Bass Balls
 *  7. Sample Trigger 
 *      a. auto accomp drums
 *      b. single note trigger
 *      c. Drum loop
*   8. Convolution
*   9. Sytnh
*   10. Delay
*   11. Reverb
*   12. Loopstation
*   13. Tone controls OPFilter.setLowpass(0,400,.7); etc
 */

#include <Encoder.h>  // encoders library
#include "OneButton.h"  // Library for double, single and long press clicks, only works with one instance
#include <Bounce2.h>  // Simple debouncer for clicks on encoders 2 & 3


#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=193,166
AudioMixer4              PUBlend;        //xy=342,179
AudioFilterBiquad        PreProcFilter2; //xy=417,689
AudioFilterStateVariable filter1;        //xy=475.0001220703125,185.00003814697266
AudioFilterBiquad        PreProcFilter;  //xy=493,633
AudioEffectDelay         chBlock;        //xy=579,739
AudioFilterBiquad        sweep;        //xy=652.0005531311035,328.20008087158203
// AudioEffectGranular      granular1;      //xy=656,633
AudioFilterStateVariable filter3;        //xy=658.0002975463867,269.00010681152344
AudioFilterStateVariable filter2;        //xy=659.0002975463867,215.00010681152344
AudioMixer4              chMix;          //xy=718,712
AudioMixer4              filterMix;         //xy=838.0001602172852,159.00005531311035
AudioMixer4              MixOut;         //xy=1111.000316619873,153.0000457763672
AudioOutputI2S           i2s2;           //xy=1231.0003242492676,244.0000467300415
AudioConnection          patchCord1(i2s1, 0, PUBlend, 0);
AudioConnection          patchCord2(i2s1, 1, PUBlend, 1);
AudioConnection          patchCord3(PUBlend, 0, filter1, 0);
AudioConnection          patchCord4(PreProcFilter2, chBlock);
AudioConnection          patchCord5(filter1, 2, filter2, 0);
AudioConnection          patchCord6(filter1, 2, filter3, 0);
AudioConnection          patchCord7(filter1, 2, filterMix, 0);
AudioConnection          patchCord8(filter1, 2, sweep, 0);
// AudioConnection          patchCord9(PreProcFilter, granular1);
AudioConnection          patchCord10(sweep, 0, filterMix, 3);
AudioConnection          patchCord11(filter3, 2, filterMix, 2);
AudioConnection          patchCord12(filter2, 1, filterMix, 1);
AudioConnection          patchCord13(filterMix, 0, i2s2, 0);
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

  // ###### FSRS ################ //
  fsr1 = analogRead(fsr1Pin);
  fsr2 = analogRead(fsr2Pin);
  fsr = fsr1-fsr2;
  
//  Serial.print(fsr1);
//  Serial.print(" ### ");
//  Serial.print(fsr2);
//  Serial.print(" ### "); 

  // ###### PROCESSING  ############  //
float hpr = 0.5+Neck*0.01667;  // High pass increases resonance with neck 1
float lbg = Neck/200; // Bass boost increases with Neck 1
float lbf = 100-Neck/5; // Bass boost center deceases with increasing neck 1
float lbq = 0.707+Neck*0.02; // bass boost narrows with increasing neck 1
float lpf = 20000-Neck*40; // High roll off increases with neck 1
float lpq = 2-Neck*0.0075; // Slope of low pass decreases with increasing neck 1

// Neck 2
float sg = constrain(Neck2*0.08,0,4); // As Neck 2 increases high mid gain increases. A max of 4 db at 50
float nf = 800-constrain((Neck2-50)*12,0,600); // center freq decreases from 800 to 200 troughing at 100
float nq = 2- constrain((Neck2-50)*0.02,0,1.5); // 

// Neck 3
float pub1 = Mid/80;
float pub2 = Mid2/100;
PUBlend.gain(0,pub2);
PUBlend.gain(1,pub1);

filter1.frequency(35);
filter1.resonance(hpr);
filter2.frequency(lbf);
filter2.resonance(lbq);
filter3.frequency(lpf);
filter3.resonance(lpq);

sweep.setBandpass(0,1100,2);
sweep.setNotch(1,nf,nq);
//sweep.setNotch(2,800,2);
//sweep.setNotch(3,800,2);


filterMix.gain(0,0.5-lbg);
filterMix.gain(1,lbg);
filterMix.gain(2,lbg);
filterMix.gain(3,sg);


MixOut.gain(0,1);


  // Set parameters
 

// #### debug #####
Serial.print(String(Neck)+",");
Serial.print(String(Mid2)+",");
Serial.print(String(Mid)+",");
Serial.print(String(hpr)+",");
Serial.print(String(lbf)+",");
Serial.print(String(lbq)+",");
Serial.print(String(lpf)+",");
Serial.print(String(lpq)+",");
Serial.print(String(lbg)+",");
Serial.print(String(sg)+",");
//Serial.print(String(Neck3)+",");
Serial.print(String(Mid)+",");
Serial.print(String(nf)+",");
Serial.print(String(nq)+",");
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
