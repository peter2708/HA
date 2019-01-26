/*  Created by Peter Williams 26/01/2019
 *   Added Comment - testing commit 2
 *   
 *  Neck step.... place encoder readings into arrays
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
//#include "OneButton.h"  // Library for double, single and long press clicks, only works with one instance
#include <Bounce2.h>  // Simple debouncer for clicks on encoders 2 & 3
#include <Adafruit_GFX.h> // For Display
#include <Adafruit_SSD1306.h> // For Display


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

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

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
#define BUTTON_PIN_2 33
#define BUTTON_PIN_3 35
// Setup a new OneButton on pin A16. (Neck encoder)  
//OneButton button(A14, true);
// Define encoders
Encoder myEnc(29, 28);  // My Neck Encoder
Encoder myEnc2(27, 26);  // My Mid Encoder
Encoder myEnc3(25, 24);  // My Bridge Encoder
// i2s1 Bridge pickup is 0, neck Pickup is 1
// i2s2 0 is jack output

// Instantiate a Bounce objects
Bounce debouncer1 = Bounce(); 
Bounce debouncer2 = Bounce(); 
Bounce debouncer3 = Bounce();

// set up LED - Not currently in use, a perspex inlay could be prepared
int redPin = 30;
int greenPin = 31;
int bluePin = 32;

int fsr1Pin = A17;
int fsr2Pin = A18;
int fsr1; int fsr2; float fsr;



// setup code here, to run once:
void setup() {
   // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.clearDisplay(); // Cleraing the main buffer
  
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
  pinMode(BUTTON_PIN_3,INPUT_PULLUP);
  // After setting up the button, setup the Bounce instances :
  debouncer1.attach(BUTTON_PIN_1);
  debouncer1.interval(5); // interval in ms
  debouncer2.attach(BUTTON_PIN_2);
  debouncer2.interval(5); // interval in ms
  debouncer3.attach(BUTTON_PIN_3);
  debouncer3.interval(5); // interval in ms
  // link the doubleclick function to be called on a doubleclick event. 
//  button.attachPress(press);  
//  button.attachClick(click);
//  button.attachDoubleClick(doubleclick);
  // enable the leds
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  // De-bugging
  Serial.begin(9600);
  
  
  
} // setup
// Set up encoder arrays
char* names[11] = {"DEFAULT", "PARAMETRIC", "CHORUS", "DISTORTION", "WAH", "RING MOD", "TREMELO", "GRANULAR", "SYNTH", "REVERB", "DELAY"};
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


float pub1 = 0.5;
float pub2 = 0.5;

PUBlend.gain(0,pub2);
PUBlend.gain(1,pub1);



// read encoder settings
  float newNeckPosition = myEnc.read();
  float newNeck2Position = myEnc.read();
  float newMidPosition = myEnc2.read();
  float newMid2Position = myEnc2.read();
  float newBrijPosition = myEnc3.read();
  float newBrij2Position = myEnc3.read();
  
/*  ############################ ENCODER READING #########################
 *  ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤ NECK ENCODER ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤   
 */
 if (setting==1){
   if (newNeckPosition <0){
    myEnc.write(0);
  }
  if (newNeckPosition >100){
    myEnc.write(100);
  }
   if (newNeckPosition != neckPosition) {
    makeDisplay();
   neckPosition = newNeckPosition;
  Neck = constrain(newNeckPosition,0,100);
  }}
  
if (setting==2){
   if (newNeck2Position <0){
    myEnc.write(0);
  }
  if (newNeck2Position >100){
    myEnc.write(100);
  }
   if (newNeck2Position != neck2Position) {
    makeDisplay();
   neck2Position = newNeck2Position;
  Neck2 = constrain(newNeck2Position,0,100);
     }}
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
    makeDisplay();
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
    makeDisplay();
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
    makeDisplay();
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
    makeDisplay();
   brij2Position = newBrij2Position;
  Brij2 = constrain(newBrij2Position,-50,50);
 }}  
// ##################################### END OF ENCODER READINGS #################################
  otherclicks();
  // keep watching the push button on neck pickup:
//  button.tick();

  
//  delay(10);
//Serial.print(Neck);
//Serial.print("  |  ");
//Serial.print(Neck2);
//Serial.print("  |  ");
//Serial.print(Mid);
//Serial.print("  |  ");
//Serial.print(Mid2);
//Serial.print("  |  ");
//Serial.print(Brij);
//Serial.print("  |  ");
//Serial.println(Brij2);
//Serial.print("  |  ");
} //eof loop

// click functions for neck encoder
//void doubleclick() {
//  // Serial.println("double clicked");
//  myEnc.write(Neck2);
// level = 2;
// delay(20);
//} 
//void click() {
// // Serial.println("single clicked");
// myEnc.write(Neck);
// level = 1;
// delay(20);
//} 
//void press() {
//  myEnc.write(Neck3);
// level = 3;
// delay(20);
//  
//} 
void otherclicks(){
    // Update the Bounce instances :
  debouncer1.update();
  debouncer2.update();
  debouncer3.update();
  // Get the updated value :
  int value1 = debouncer1.read();
  int value2 = debouncer2.read();
  int value3 = debouncer3.read();
  if (value3 == LOW){
   setting = 2;
   myEnc.write(Neck2);
   delay(50);
  }
  else {
   setting = 1;
   myEnc.write(Neck);
   //delay(50);
  }
  
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

void makeDisplay(){
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0); 
    int modSel = Neck*.1;
    display.println(names[modSel]);   
    display.display();
      display.setCursor(Mid,17);
    //display.setCursor(0,0);
    display.println(Mid);
    display.display();
}

 // End
