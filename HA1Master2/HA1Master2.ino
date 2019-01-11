/*  Created by Peter Williams 19/06/2018
 *   All sensors tested
 *   Compression needs to be worked into tone
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

/*
 * From GUI
 */
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioMixer4              PUBlend;        //xy=139.00006294250488,314.000057220459
AudioInputI2S            i2s1;           //xy=193,166
AudioFilterStateVariable PREPROC;        //xy=285.00006103515625,322.0000114440918
AudioEffectWaveshaper    waveshape1;     //xy=373.99998474121094,817.3999938964844
AudioEffectChorus        chorus1;        //xy=374.99998474121094,740.3999938964844
AudioEffectFreeverb      freeverb1;      //xy=384.99998474121094,778.3999938964844
AudioAnalyzeNoteFrequency freq;      //xy=484.00012588500977,372.00031089782715
AudioAnalyzeRMS          rms1;           //xy=484.00001525878906,418.0001220703125
AudioFilterStateVariable midPass;        //xy=503.0003204345703,324.00014305114746
AudioFilterBiquad        Lowpass;        //xy=504.0000534057617,211.4000358581543
AudioFilterStateVariable bassPass;        //xy=505.00053787231445,267.0001525878906
AudioSynthWaveformDc     dc1;            //xy=670.0001564025879,323.0001230239868
AudioMixer4              EQMix;         //xy=694.0005531311035,217.000150680542
AudioSynthWaveformDc     dc2;            //xy=773.0000457763672,382.00024223327637
AudioSynthWaveform       waveform2;      //xy=775.0001106262207,421.0003719329834
AudioFilterStateVariable wah;        //xy=798.0001106262207,317.00010776519775
AudioSynthWaveform       waveform1;      //xy=861.0002365112305,246.00015449523926
AudioEffectMultiply      multiply2;      //xy=902.0001182556152,400.0002384185791
AudioMixer4              wahComb;         //xy=959.9999847412109,311.3999938964844
AudioEffectMultiply      multiply1;      //xy=1016.0002403259277,224.00011253356934
AudioMixer4              FXMix;         //xy=1233.0001106262207,201.00009727478027
AudioOutputI2S           i2s2;           //xy=1374.0005798339844,208.0000982284546
AudioConnection          patchCord1(PUBlend, 0, PREPROC, 0);
AudioConnection          patchCord2(i2s1, 0, PUBlend, 0);
AudioConnection          patchCord3(i2s1, 1, PUBlend, 1);
AudioConnection          patchCord4(PREPROC, 2, bassPass, 0);
AudioConnection          patchCord5(PREPROC, 2, EQMix, 0);
AudioConnection          patchCord6(PREPROC, 2, midPass, 0);
AudioConnection          patchCord7(PREPROC, 2, freq, 0);
AudioConnection          patchCord8(PREPROC, 2, rms1, 0);
AudioConnection          patchCord9(PREPROC, 2, Lowpass, 0);
AudioConnection          patchCord10(midPass, 1, EQMix, 2);
AudioConnection          patchCord11(Lowpass, 0, EQMix, 3);
AudioConnection          patchCord12(bassPass, 0, EQMix, 1);
AudioConnection          patchCord13(dc1, 0, wah, 1);
AudioConnection          patchCord14(EQMix, 0, multiply1, 0);
AudioConnection          patchCord15(EQMix, 0, wah, 0);
AudioConnection          patchCord16(EQMix, 0, FXMix, 0);
AudioConnection          patchCord17(dc2, 0, multiply2, 0);
AudioConnection          patchCord18(waveform2, 0, multiply2, 1);
AudioConnection          patchCord19(wah, 1, wahComb, 0);
AudioConnection          patchCord20(wah, 2, wahComb, 1);
AudioConnection          patchCord21(waveform1, 0, multiply1, 1);
AudioConnection          patchCord22(multiply2, 0, FXMix, 3);
AudioConnection          patchCord23(wahComb, 0, FXMix, 2);
AudioConnection          patchCord24(multiply1, 0, FXMix, 1);
AudioConnection          patchCord25(FXMix, 0, i2s2, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=205,115
// GUItool: end automatically generated code

/*
 * Eof From GUI
 */

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

/*
 * LED code definitions here
 */

/*
 * Eof LED definitions
 */
int fsr1Pin = A17;
int fsr2Pin = A18;
int fsr1; int fsr2; float fsr;

movingAvg avgFSR(10);                  // define the moving average object
movingAvg avg2FSR(25);                  // define the SECOND moving average object
// setup code here, to run once:
void setup() {
/*
 * For Teensy and Audio Shield
 */
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

  /*
   * Eof Teensy and Audio Shield
   */
  
  // Initiate FSR smoothing
   avgFSR.begin();
   avg2FSR.begin();
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
   // adjust synth pitch recognition
#define AUDIO_GUITARTUNER_BLOCKS 12;
  freq.begin(.01);
 
  
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

// Mixer Settings
// EQMix
int cleanch = 0;
int basspassch = 1;
int midpassch = 2;
int modch = 1;
int wahch = 2;
int synthch = 3;
// Initial Filter Settings
float CutsMidF = 1000;
float CutsMidQ = 3;
float CutsMidG = 0;
float bassPassF = 140;
float bassPassQ = 5;
float bassPassG = 0;
// initial volumes
float fsrVol = 1;
float vol = 1;
float vol2 = 1;
// main code here, to run repeatedly: 
void loop() {
 // ###### FSRS ################ //
  fsr1 = analogRead(fsr1Pin);
  fsr2 = analogRead(fsr2Pin);
  fsr = fsr1-fsr2;
  int avg = avgFSR.reading(fsr);
  int avg2 = avg2FSR.reading(avg);
  // Panning
// Bridge pick-up gain pu1g
float pu1g = .5+Mid/100;
// Neck Pick up gain pu2g
float pu2g = 1-pu1g;
// Tone Controls
bassPassG=pow(Bridge*0.01,0.8);
bassPassF = 230-Bridge*1.4;
CutsMidG=Bridge2*0.03;
vol = 1-constrain((bassPassG+CutsMidG*0.5),0,1);
// Testing
float queue = Neck/2+0.01;

float envelope = rms1.read();
dc1.amplitude(envelope,50);
dc2.amplitude(envelope);
float test = dc2.read();

wah.frequency(350);
wah.resonance(5);
wah.octaveControl(30);

wahComb.gain(0,1);

PREPROC.frequency(10);
PREPROC.resonance(5);

float pitch = freq.read();

waveform2.begin(1,pitch*0.5,WAVEFORM_SQUARE);

fsrVol =constrain((avg-(avg2))*0.07,0,2);

waveform1.begin(0.5,12,WAVEFORM_TRIANGLE);

// Process Sound
PUBlend.gain(1,pu1g);
PUBlend.gain(0,pu2g); 


bassPass.frequency(bassPassF);bassPass.resonance(bassPassQ);
midPass.frequency(CutsMidF);midPass.resonance(CutsMidQ);


//EQMix.gain(3,fsrVol);
EQMix.gain(midpassch,CutsMidG);
EQMix.gain(basspassch,bassPassG);
EQMix.gain(cleanch,vol);
vol2 = 1-(Neck2*0.01+Neck*0.01);
FXMix.gain(cleanch,vol2);
FXMix.gain(modch,fsrVol*Mid2*0.01);
FXMix.gain(wahch,Neck*0.01);
FXMix.gain(synthch,Neck2*0.01);





// #### debug #####  TO BE COMMENTED OUT IN GIGIING VERSION
Serial.print(Neck2*0.01);
Serial.print("   :   ");
Serial.print(vol2);
//Serial.print(test);
//Serial.print(dc1.read());
//Serial.print(String(Bridge)+",");
//Serial.print(String(Bridge2)+",");
//Serial.print(String(Bridge3)+",");
//Serial.print(String(Mid)+",");
//Serial.print(String(Mid2)+",");
//Serial.print(String(Neck)+",");
//Serial.print(String(Neck2)+",");
//Serial.print(String(fsr)+",");
//Serial.print(String(avg)+",");
//Serial.print("Sensor: "+String(Bridge)+"   BP1F: "+String(BP1F)+"   BP1Q: "+String(BP1Q)+"   Bass Gain: "+String(BassGain));
//Serial.print(String(bassPassF)+","+String(bassPassQ)+","+String(CutsMidG));
//Serial.print("Sensor: "+String(Bridge2)+"   BN1F: "+String(BN1F)+"   BN1aF: "+String(BN1aF)+"   BN1aQ: "+String(BN1aQ));
//Serial.print("Long time average "+ String(avg2)); 
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
  if (newBridge2Position >100){
    myEnc.write(100);
  }
   if (newBridge2Position != Bridge2Position) {
   Bridge2Position = newBridge2Position;
  Bridge2 = constrain(newBridge2Position,0,100);
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
  Bridge3 = constrain(newBridge3Position,0,100);
  //  delay (5);
    }}
// eof second level (long click) Bridge setting
/* ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 *  ¤¤¤¤¤¤¤¤¤¤¤ MIDDLE ENCODER ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 */
if (setting==1){
   if (newMidPosition <-50){
    myEnc2.write(50);
  }
  if (newMidPosition >50){
    myEnc2.write(50);
  }
   if (newMidPosition != midPosition) {
   midPosition = newMidPosition;
  Mid = constrain(newMidPosition,-50,50);
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
  if (newNeckPosition >100){
    myEnc3.write(100);
  }
   if (newNeckPosition != NeckPosition) {
   NeckPosition = newNeckPosition;
  Neck = constrain(newNeckPosition,0,100);
   }}

  if (setting2==2){
   if (newNeck2Position <0){
    myEnc3.write(0);
  }
  if (newNeck2Position >100){
    myEnc3.write(100);
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
