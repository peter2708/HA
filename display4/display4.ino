/*  Created by Peter Williams 26/01/2019
 *   Added Comment - testing commit 2
 *   ############ TODO ############
 *   CONVERT Brij TO AN ARRAY an build an array containing display settings
 *  Neck step.... continue to place encoder readings into arrays
 *  Check that all my encoder floats have to be floats
 *  MOST IMPORTANTLY
 *  Find a way to set which pots have center or zero defaults dependant on module selection
 *  
 *  
 *  git here:  https://github.com/peter2708/HA.git
 *  
 *  git push --mirror will do for now
 *  
 *  
Neck encoder:
29,28, A16(35)
Middle encoder:
26, 27, A15(34)
Bridge encoder:
24,25 A14(33)
LED:
30 - 32
*/

#include <Encoder.h>  // encoders library
#include <Bounce2.h>  // Simple debouncer for clicks on encoders 
#include <Adafruit_GFX.h> // For Display
#include <Adafruit_SSD1306.h> // For Display


#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=99.00000762939453,230.00000762939453
AudioMixer4              PUBlend;        //xy=139.00006294250488,314.000057220459
AudioFilterStateVariable MidTone;        //xy=267.99999618530273,451.0000228881836
AudioFilterStateVariable BassTone;        //xy=278.0000762939453,353.0001468658447
AudioFilterBiquad        LoPass;        //xy=284.99999618530273,398.00002002716064
AudioMixer4              midToneMix;         //xy=464.0001220703125,406.00011825561523
AudioMixer4              BassToneMix;         //xy=466.99999237060547,332.0000801086426
AudioFilterStateVariable LowMidTone;        //xy=634.0000915527344,434.00012016296387
AudioMixer4              ToneMix;         //xy=815.0001029968262,387.0001792907715
AudioOutputI2S           i2s2;           //xy=960.0008316040039,392.00024032592773
AudioConnection          patchCord1(i2s1, 0, PUBlend, 0);
AudioConnection          patchCord2(i2s1, 1, PUBlend, 1);
AudioConnection          patchCord3(PUBlend, 0, BassTone, 0);
AudioConnection          patchCord4(PUBlend, 0, BassToneMix, 0);
AudioConnection          patchCord5(PUBlend, LoPass);
AudioConnection          patchCord6(PUBlend, 0, midToneMix, 0);
AudioConnection          patchCord7(PUBlend, 0, MidTone, 0);
AudioConnection          patchCord8(MidTone, 0, midToneMix, 2);   // Provision of High Mid cut/boost at around 1000 Hz
AudioConnection          patchCord9(MidTone, 1, midToneMix, 1);   // Provision of High Mid cut/boost at around 1000 Hz
AudioConnection          patchCord10(MidTone, 2, midToneMix, 3);  // Provision of High Mid cut/boost at around 1000 Hz
AudioConnection          patchCord11(BassTone, 1, BassToneMix, 1);
AudioConnection          patchCord12(BassTone, 2, BassToneMix, 2);
AudioConnection          patchCord13(LoPass, 0, BassToneMix, 3);
AudioConnection          patchCord14(midToneMix, 0, ToneMix, 1);
AudioConnection          patchCord15(midToneMix, 0, LowMidTone, 0);
AudioConnection          patchCord16(BassToneMix, 0, ToneMix, 0);
AudioConnection          patchCord17(LowMidTone, 0, ToneMix, 2);  // Provision of Low Mid cut at between 200 and 800 Hz
AudioConnection          patchCord18(LowMidTone, 2, ToneMix, 3);
AudioConnection          patchCord19(ToneMix, 0, i2s2, 0);
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
int nmods = 11;

const String names[11] = {"DEFAULT", "PARAMETRIC", "CHORUS", "DISTORTION", "WAH", "RING MOD", "TREMELO", "GRANULAR", "SYNTH", "REVERB", "DELAY"};

const int numParMid[11] = // This is the number of parameters on midlle encoder per module
{4,2,3,4,5,6,7,8,9,10,11};

const String parNames[8] = {"PAN","BASS","LOW MID","HIGH MID" , // First set of parameters
"GAIN","NECK CONTROL","Q","NECK"};                      // Second Module parameters




// Parameter Limits



// starting encoder values
// ########## MAKE SURE THESE MATCH STARTING VALUES  
float newMidPosition[11][10];


float neckPosition  = -999;
float Neck = 0;
float neck2Position  = 0;
float Neck2 = 0;
float neck3Position  = 0;
float Neck3 = 0;
int midPosition  = 0;
int Mid[11][10];
float mid2Position  = 0;
float Mid2[11];
float brijPosition  = 0;
float Brij[11];
float brij2Position  = 0;
float Brij2[11];
// mode switching
int setting1 = 1;
int setting2 = 1;
int setting = 1;
int level = 1;
// EQ Settings
float pub1 = 0.5;
float pub2 = 0.5;
int modSel = 0;

int value1 = 0;
int lastvalue1 = 1;
int switch1 = 0;

// main code here, to run repeatedly: 
void loop() {
int modSel = Neck*0.01*nmods;   // Set module selection
/*
 * Checking multidimensional array
 */
  
  

/*
 * Working on a button switch
 */
  Serial.print("   Number of parameter:   ");
  Serial.print(numParMid[modSel]-1);
  Serial.print("  lastclickstate1:   ");
  Serial.print(lastvalue1);
  Serial.print("   clickstate1:    ");
  Serial.print(value1);
  Serial.print("   switch1:   ");
  Serial.print(switch1);
  Serial.print("  Read parameter label:  ");
  Serial.print(parNames[switch1]);
  Serial.print("   Current encoder mid reading:   ");
  Serial.println(Mid[modSel][switch1]);
  
  /*
   * e of working on a button switch
   */



// read encoder settings
  float newNeckPosition = myEnc.read();
//  float newNeck2Position = myEnc.read();
  newMidPosition[modSel][switch1] = myEnc2.read();
 // Serial.println(Mid[modSel][switch1]); 
//  float newMid2Position = myEnc2.read();
//  float newBrijPosition = myEnc3.read();
//  float newBrij2Position = myEnc3.read();
  
/*  ############################ ENCODER READING #########################
 *   
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
    makeDisplay1(names[modSel]);
   neckPosition = newNeckPosition;
  Neck = constrain(newNeckPosition,0,100);
  }}
  
//if (setting==2){
//   if (newNeck2Position <0){
//    myEnc.write(0);
//  }
//  if (newNeck2Position >100){
//    myEnc.write(100);
//  }
//   if (newNeck2Position != neck2Position) {
//   
//   neck2Position = newNeck2Position;
//  Neck2 = constrain(newNeck2Position,0,100);
//     }}
/* ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 *  ¤¤¤¤¤¤¤¤¤¤¤ MIDDLE ENCODER ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 */
 

   if (newMidPosition[modSel][switch1] <-50){
    myEnc2.write(-50);
  }
  if (newMidPosition[modSel][switch1] >50){
    myEnc2.write(50);
  }
   if (newMidPosition[modSel][switch1] != midPosition) {
Serial.println("Display update with parameter value has been called");
  makeDisplay(parNames[switch1],50,Mid[modSel][switch1]);

midPosition = newMidPosition[modSel][switch1];
  Mid[modSel][switch1] = constrain(newMidPosition[modSel][switch1],-50,50);
  }
  

/* ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 *  ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤ BRIDGE ENCODER ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 */
     
//if (setting2==1){
//if (newBrijPosition <-50){
//    myEnc3.write(-50);
//    
//  }
//  if (newBrijPosition >50){
//    myEnc3.write(50);
//  }
//   if (newBrijPosition != brijPosition) {
//  
//  makeDisplay(parNames[modSel*4+2],50,Brij[modSel]);
//
//   brijPosition = newBrijPosition;
//  Brij[modSel] = constrain(newBrijPosition,-50,50);
//   }}
//
//  if (setting2==2){
//   if (newBrij2Position <-50){
//    myEnc3.write(-50);
//  }
//  if (newBrij2Position >50){
//    myEnc3.write(50);
//  }
//   if (newBrij2Position != brij2Position) {
//   
//  makeDisplay(parNames[modSel*4+3],50,Brij2[modSel]);
//
//   brij2Position = newBrij2Position;
//  Brij2[modSel] = constrain(newBrij2Position,-50,50);
// }}  
// ##################################### END OF ENCODER READINGS #################################
  otherclicks(numParMid[modSel]);
  // keep watching the push button on neck pickup:


} //eof loop

void otherclicks(int np1){
    // Middle encoder
  debouncer1.update();
  value1 = debouncer1.read();
  if (value1 != lastvalue1){
    if (value1 == HIGH){
      // we went from off to on and we have a click
      
      
     switch1++;
    }
   }
   if (switch1>np1-1){
    switch1=0;
    }  // make sure clicks loop through the correct number of parameters
    lastvalue1=value1;

    
  debouncer2.update();
  debouncer3.update();
  // Get the updated value :
  
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
  if (value2 == LOW){
   setting2 = 2;
   myEnc3.write(Brij2[modSel]);
   delay(50);
    }
  else {
    setting2 = 1;
   myEnc3.write(Brij[modSel]);
   delay(50);
    }
    
    }



void makeDisplay1(String label1){
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0); 
    display.println(label1);   
    display.display();
}
void makeDisplay(String label2, int dispZ, float val){
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0); 
    display.print(label2);display.print(":   ");
    display.drawRect(1, display.height()/2+2, display.width()-6, display.height()/2-2, WHITE);
    display.setTextSize(2);
    display.setCursor((dispZ-val)*1.12,display.height()/2+2);
    display.print('|');
    display.display();
}

 // End
