/*  Created by Peter Williams 19/06/2018
 *   Only sensors
 *  
Bridge encoder:
29,28, A16
Middle encoder:
26, 27, A15(34)
Bridge encoder:
24,25 A14(33)
LED:
30 - 32
*/

#include <movingAvg.h>                  // https://github.com/JChristensen/movingAvg
#include <Encoder.h>  // encoders library
#include "OneButton.h"  // Library for double, single and long press clicks, only works with one instance
#include <Bounce2.h>  // Simple debouncer for clicks on encoders 2 & 3

// Define bounce buttons
#define BUTTON_PIN_1 34
#define BUTTON_PIN_2 35
// Setup a new OneButton on pin A16. (Bridge encoder)  
OneButton button(A14, true);
// Define encoders
/* Pot 3 ==> the Bridge encoder
 *  Position one click - adjusts bass
 *  Position double click - adjusts treble
 *  position long press - Some effect parameter
 */
Encoder myEnc(25, 24);
/* The middle encoder has open and pressed settings
 *  The open setting is pick-up sweep
 *  two Leds should be added to define balance
 */
Encoder myEnc2(27, 26); 
/* The Neck encoder has open and pressed settings
 *  These can be used for more effects
 */
Encoder myEnc3(29, 28);  

// Instantiate a Bounce objects
Bounce debouncer1 = Bounce(); 
Bounce debouncer2 = Bounce(); 

int fsr1Pin = A17;
int fsr2Pin = A18;
int fsr1; int fsr2; float fsr;

movingAvg avgFSR(10);                  // define the moving average object

// setup code here, to run once:
void setup() {
   avgFSR.begin();
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

  // De-bugging
  Serial.begin(9600);
  
  
  
} // setup
// starting encoder values
// ########## MAKE SURE THESE MATCH STARTING VALUES  
float BridgePosition  = -999;
float Bridge = 0;
float Bridge2Position  = -999;
float Bridge2 = 0;
float Bridge3Position  = -999;
float Bridge3 = 0;
float midPosition  = -999;
float Mid = 5;
float mid2Position  = -999;
float Mid2 = 0;
float NeckPosition  = -999;
float Neck = 0;
float Neck2Position  = -999;
float Neck2 = 0;
// mode switching
int setting = 1;
int setting2 = 1;
int level = 1;
// EQ Settings

// main code here, to run repeatedly: 
void loop() {
// Dynamic and tone


  // ###### FSRS ################ //
  fsr1 = analogRead(fsr1Pin);
  fsr2 = analogRead(fsr2Pin);
  fsr = fsr1-fsr2;
  
  // Set parameters
 

// #### debug #####
Serial.print(String(Bridge)+",");
Serial.print(String(Bridge2)+",");
Serial.print(String(Bridge3)+",");

Serial.print(String(Mid)+",");
Serial.print(String(Mid2)+",");

Serial.print(String(Neck)+",");
Serial.print(String(Neck2)+",");

Serial.print(String(fsr)+",");
int avg = avgFSR.reading(fsr);  
Serial.print(String(avg)+",");

Serial.println("");

// read encoder settings
  float newBridgePosition = myEnc.read();
  float newBridge2Position = myEnc.read();
  float newBridge3Position = myEnc.read();
  float newMidPosition = myEnc2.read();
  float newMid2Position = myEnc2.read();
  float newNeckPosition = myEnc3.read();
  float newNeck2Position = myEnc3.read();
  
/*  ############################ ENCODER READING #########################
 *  ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤ Bridge ENCODER ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤   
 */
  if (level == 1){
    if (newBridgePosition <0){
    myEnc.write(0);
  }
  if (newBridgePosition >100){
    myEnc.write(100);
  }
   if (newBridgePosition != BridgePosition) {
   BridgePosition = newBridgePosition;
  Bridge = constrain(newBridgePosition,0,100);
 }}
// eof default (single click) Bridge setting

// first level (double click) Bridge setting
  if (level == 2){
  if (newBridge2Position <0){
    myEnc.write(0);
  }
  if (newBridge2Position >200){
    myEnc.write(200);
  }
   if (newBridge2Position != Bridge2Position) {
   Bridge2Position = newBridge2Position;
  Bridge2 = newBridge2Position;
 // Useful range: 100 - 
  // overall volume adjustments and a bass boost when low pass is set high
  
   //  delay (5);
    }}
// eof first level (double click) Bridge setting

// Second level (long click) Bridge setting
  if (level == 3){
  if (newBridge3Position <0){
    myEnc.write(0);
  }
  if (newBridge3Position >100){
    myEnc.write(200);
  }
   if (newBridge3Position != Bridge3Position) {
   Bridge3Position = newBridge3Position;
  Bridge3 = newBridge3Position;
  //  delay (5);
    }}
// eof second level (long click) Bridge setting
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
 *  ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤ Neck ENCODER ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 */
     
if (setting2==1){
if (newNeckPosition <0){
    myEnc3.write(0);
    
  }
  if (newNeckPosition >200){
    myEnc3.write(200);
  }
   if (newNeckPosition != NeckPosition) {
   NeckPosition = newNeckPosition;
  Neck = constrain(newNeckPosition,-50,200);
   }}

  if (setting2==2){
   if (newNeck2Position <-50){
    myEnc3.write(-50);
  }
  if (newNeck2Position >50){
    myEnc3.write(50);
  }
   if (newNeck2Position != Neck2Position) {
   Neck2Position = newNeck2Position;
  Neck2 = constrain(newNeck2Position,-50,50);
 }}  
// ##################################### END OF ENCODER READINGS #################################
  otherclicks();
  // keep watching the push button on Bridge pickup:
  button.tick();

  
//  delay(10);
} // loop

// click functions for Bridge encoder
void doubleclick() {
  // Serial.println("double clicked");
  myEnc.write(Bridge2);
 level = 2;
 delay(20);
} 
void click() {
 // Serial.println("single clicked");
 myEnc.write(Bridge);
 level = 1;
 delay(20);
} 
void press() {
  myEnc.write(Bridge3);
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
   myEnc3.write(Neck2);
   delay(50);
    }
  else {
    setting2 = 1;
   myEnc3.write(Neck);
   delay(50);
    }}
 // End
