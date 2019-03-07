/*
 * HA3 IS A FUNDAMENTAL RE-WORKING
 * 
 * WE WILL NOW HAVE A SIMPLER MENU:
 * NECK ENCODER (N) WILL SWEEP BETWEEN MODULES, THE OTHER TWO PICK-UPS (B & M)  HAVE ONE PARAMETER ONLY EACH FOR EACH MODULE
 *    1: TONE - A TONE (B) AND PICK-UP SWEEP (M) CONTROL
 *    2: EFFECTS, ESSENTIALLY PRESETS OF MULTIPLE EFFECTS: GRADUAL SWEEP (M), HOW MUCH CONTROL IS ON FSRS (B)
 *    3 AND OVER: SANDBOXING OF OTHER EFFECTS
 *    
 *  EACH TIME A POT IS SWEPT IT WILL AFFECT IT'S OWN DISPLAY
 *  
 *  EVENTUALLY MODULE 3 AND UP WILL BE DISABLEED AND CLICKS WILL A) SWITCH BETWEEN SELECTED SETTINGS &D STANDARD, B) SELECT TUNER
 * 
 * STATUS - MENU GLITCHY BUT WORKS
 * STATUS - AUDIO FROM SCRATCH
 * STATUS - PAN BALANCE WORKS
 * STATUS - TONE WORKS
 * STATUS - ENVELOPE FILTER WORKS
 * STATUS - CRUDELY IMPLETMENTED
 * 
 * 
 * ##############    TODO   ##########
 * TONE, REFINE
 * ENVELOPE FILTER - USE NECK TO SWEEP BETWEEN HIGH, BAND AND LOW PASS
 * DISTORTION - HOW TO CONTROL AND ROUTE?, WAVESSHAPE CANNOT BE CALLED LIVE
 * RING MODULATION
 * */

// LIBRARIES
#include <movingAvg.h>                  // https://github.com/JChristensen/movingAvg
#include <Encoder.h>
#include <Bounce2.h>  // Simple debouncer for clicks on encoders 
#include <Adafruit_GFX.h> // For Display
#include <Adafruit_SSD1306.h> // For Display
#include "effect_waveshaper.h"
// Audio
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>


Encoder neckEnc(29, 28);  // Neck rotation
Encoder midEnc(27, 26);  // Mid rotation
Encoder brijEnc(25,24);      // Bridge Encoder

#define CLICK_PIN3 33 // mid encoder 34 is broken, use neck encoder for now
#define CLICK_PIN2 35 // mid encoder 34 is broken, use neck encoder for now
#define CLICK_PIN1 34 // Neck clicker Is actually 35, mid encoder is boroken

#define numOfMods 10  // THE NUMBER OF MODULES
#define maxPar 1 // MAX NUMBER OF PARAMETERS PER SUB MENU
#define maxEnc 100 // Maximum encoder reading
#define minEnc -100 // Minimum encoder reading
#define modReg 20 // the rotation region per MODULE

// Instantiate a Bounce object
Bounce debouncer1 = Bounce(); 
Bounce debouncer2 = Bounce(); 
Bounce debouncer3 = Bounce(); 

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define l 17


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




// GUItool: begin automatically generated code
AudioFilterBiquad        preF;        //xy=220.99998474121094,199.99999237060547
AudioAnalyzeFFT256       attackRead;     //xy=235.0000114440918,261.00000953674316
AudioInputI2S            ip;             //xy=247,108
AudioSynthWaveformDc     dc1;            //xy=278.00003814697266,302.000057220459
AudioMixer4              PUBlend;        //xy=388,121
AudioEffectEnvelope      sweep;          //xy=402.00003814697266,302.000057220459
AudioFilterStateVariable bass;           //xy=519.0000305175781,152.00000762939453
AudioFilterStateVariable mid;            //xy=519.0000305175781,202.00000953674316
AudioFilterStateVariable wah;            //xy=588.0003623962402,258.00012397766113
AudioEffectWaveshaper    dist2; //xy=603.0000343322754,573.0000267028809
AudioEffectWaveshaper    dist1;     //xy=604.0001602172852,515.0000867843628
AudioMixer4              tonemix;        //xy=653.0000381469727,143.00000762939453
AudioMixer4              fx1;         //xy=869.000171661377,231.00003051757812
AudioMixer4              fx2;         //xy=938.000171661377,519.0000839233398
AudioOutputI2S           op;             //xy=1103.0001754760742,232.00000715255737
AudioConnection          patchCord1(preF, attackRead);
AudioConnection          patchCord2(ip, 0, PUBlend, 0);
AudioConnection          patchCord3(ip, 1, PUBlend, 1);
AudioConnection          patchCord4(dc1, sweep);
AudioConnection          patchCord5(PUBlend, 0, bass, 0);
AudioConnection          patchCord6(PUBlend, 0, tonemix, 0);
AudioConnection          patchCord7(PUBlend, 0, mid, 0);
AudioConnection          patchCord8(PUBlend, preF);
AudioConnection          patchCord9(sweep, 0, wah, 1);
AudioConnection          patchCord10(bass, 1, tonemix, 1);
AudioConnection          patchCord11(mid, 1, tonemix, 2);
AudioConnection          patchCord12(wah, 1, fx1, 1);
AudioConnection          patchCord13(wah, 2, fx1, 2);
AudioConnection          patchCord14(dist2, 0, fx2, 2);
AudioConnection          patchCord15(dist1, 0, fx2, 1);
AudioConnection          patchCord16(dist1, dist2);
AudioConnection          patchCord17(tonemix, 0, fx1, 0);
AudioConnection          patchCord18(tonemix, 0, wah, 0);
AudioConnection          patchCord19(fx1, dist1);
AudioConnection          patchCord20(fx1, 0, fx2, 0);
AudioConnection          patchCord21(fx2, 0, op, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=154,49
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

// Eof Audio
int fsr1Pin = A17;
int fsr2Pin = A18;
int fsr1; int fsr2; float fsr;

movingAvg avgFSR(10);                  // define the moving average object
movingAvg avg2FSR(25);                  // define the SECOND moving average object

movingAvg avgPeak(2);                  // Smoothing filter for attack analysis

unsigned long startUpTime; 
int maxsub2[numOfMods][maxPar];
int sub2LabelsIDX[numOfMods][10][10];
unsigned long nowT;

void setup() {


//  pitch.begin(.99999); DON'T NEED PITCH YET
  // Initiate peak smoothing
   avgPeak.begin();
   // Initiate FSR smoothing
   avgFSR.begin();
   avg2FSR.begin();
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
   // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.clearDisplay(); // Cleraing the main buffer
  Serial.begin(9600);
    // Setup the button with an internal pull-up :
  pinMode(CLICK_PIN1,INPUT_PULLUP);
  pinMode(CLICK_PIN2,INPUT_PULLUP);
  pinMode(CLICK_PIN3,INPUT_PULLUP);
  // After setting up the button, setup the Bounce instance :
  debouncer1.attach(CLICK_PIN1);
  debouncer1.interval(5); // interval in ms
  debouncer2.attach(CLICK_PIN2);
  debouncer2.interval(5); // interval in ms
  debouncer3.attach(CLICK_PIN3);
  debouncer3.interval(5); // interval in ms

  // Add Welcome
  welcome();

}

// Some constraints

// Some holders
int readings[6];
int par1[numOfMods][maxPar];//[maxPar]; //
int par2[numOfMods][maxPar]; //
int prevM;  // previous menu selection
int prevS1;
int prevS2;
int prevMenu[3];
int value1;
int value2;
int value3;
int modSel = 0;
int sub1;
int sub2;
float pugFsrC;    // Pick up gain FSR coefiiecient
String modLabel;

// Some Labels
String modLabels[numOfMods]={"TONE","PRESETS","SANDBOX1","SANDBOX2"};

// Some intialisers
int oldModSel = 0;
int oldSub1 = 0;
int oldR1 = 0;
unsigned long timer = 0;
unsigned long oldTimer = 0;
int MaxPar = maxPar;
// Assign Some constants

// Some test variables
float oldPeak = 0;

// cONTROL VARIABLES

float pug;
float oldpub;
float bassPg;
float bassPf;
float bassPr;
float midPg;
float midPf;
float midPr;
float trebleBump;
boolean waveset = 0;
int wsl = 3;
float firstharmonic[3];

void loop() {
// ############ read inputs ##########
float avg2 = readFSRS();
readEncs();
// ########### peak detection #############
preF.setHighpass(0,80,5);
preF.setLowpass(1,165,4);
float peak = attackRead.read(2,3)*50;
float slope = constrain(peak - oldPeak,0,1);
oldPeak = peak;
//  ############ TONE  ##############

pickupblend();
tonefilters();

// ######### WAH  #########
envFilter(peak,slope,avg2);
// ######## DISTORTION ####### 
if (waveset == 0){

for (int i=0;i<wsl;i++){
  firstharmonic[i]=abs(2*float(i)/(wsl-1)-1);
  Serial.print(firstharmonic[i]);
  Serial.print("\t");
  dist1.shape(firstharmonic,wsl);
  dist2.shape(firstharmonic,wsl);
}
Serial.println("");
waveset = 1;
}

fx2.gain(0,0.5);
fx2.gain(1,.5);
fx2.gain(2,0.5);
// ############## effects mixer ##############
float wg = constrain(map(float(par2[2][0]),0,maxEnc,0,1),0,1);
float wg2 = constrain(map(float(par2[2][0]),maxEnc*.5,maxEnc,0,1),0,1);
//Serial.println(wg);
fx1.gain(0,1-(wg+wg2)/2);

fx1.gain(1,wg);

fx1.gain(2,wg2);





  
 
// Assign parameter values
modSel = constrain(readings[0]/modReg,0,numOfMods-1);
modLabel = modLabels[modSel];
sub1 = readings[4];
sub2 = readings[5];







// call display
//makeDisplay(modLabel);
//makeDisplay(modLabel);
String subLabel = ""; 
String sub2Label = "";
//makeD2(modLabel,subLabel,sub2Label,modSel,sub1,sub2);



} 

void pickupblend(){
  pug = constrain(map(float(par1[0][0]),minEnc,maxEnc,-.5,.5),-.5,5);
PUBlend.gain(0,.5-pug);
PUBlend.gain(1,.5+pug);
if (oldpub!=pug){
  dispPUB();
  oldpub = pug;
}

}
void tonefilters(){
 float tb1 = constrain(map(float(par2[0][0]),-1,-30,0,0.85),0,0.85);
 float tb2 = constrain(map(float(par2[0][0]),-30,-60,0.85,0),0,0.85);
  trebleBump = tb1 + tb2 -0.85;
  bassPg = constrain(map(float(par2[0][0]),minEnc,maxEnc,-1,1),-1,0);
  bassPf = map(float(par2[0][0]),minEnc,0,120,80);
  bassPr = map(float(par2[0][0]),minEnc,0,4,2);

  midPg = trebleBump+constrain(map(float(par2[0][0]),minEnc,maxEnc,-1,1),0,1);
  if (par2[0][0]>0){
  midPf = map(float(par2[0][0]),0,maxEnc,800,2000);
  midPr = map(float(par2[0][0]),0,maxEnc,2,5);
  }
  else{
  midPf = 1800;
  midPr = 4;  
  }

  
/* Debug Tone
 *  
 */
//String LINE1  = "bass pass ";
//LINE1 = LINE1 + "G: " + bassPg + "\t F: "
// + bassPf + "\t Q: " + bassPr + "\t";
//String LINE2  = "mid pass ";
//LINE2 = LINE2 + "G: " + midPg + "\t F: "
// + midPf + "\t Q: " + midPr + "\t" + "bump: " + trebleBump; 
//Serial.print(LINE1);
//Serial.println(LINE2);
//  tonemix.gain(0,1+bassPg+midPg);
//  tonemix.gain(1,-bassPg);
//  tonemix.gain(2,midPg);
//  bass.frequency(bassPf);
//  bass.resonance(bassPr);
//  mid.frequency(midPf);
//  mid.resonance(midPr);

  
}
void envFilter(float peak, float slope,float avg2){
  float wspeed = abs(map(peak,0,4,600,200));
dc1.amplitude(1);
sweep.delay(0);
   sweep.delay(5); 
   sweep.attack(wspeed);
   sweep.hold(100);
   sweep.decay(0.5*wspeed);
   sweep.sustain(0);
if (slope>0.4){
  sweep.noteOn();
   }
float wf = constrain(map(avg2,80,150,700,400),300,1000);
float woc = constrain(map(avg2,80,150,2.5,.5),0,3);
float wr = abs(constrain(map(avg2,120,150,0,2),-2,2))+2;
wah.frequency(wf);
wah.resonance(wr);
wah.octaveControl(woc);
//Serial.print(avg2);
//Serial.print("\t");
//Serial.print(wr);
//Serial.print("\t");
//Serial.print(woc);
//Serial.print("\t");
//Serial.println(wf);
}
float readFSRS(){
  // FSRS 
  fsr1 = analogRead(fsr1Pin);
  fsr2 = analogRead(fsr2Pin);
  fsr = fsr1-fsr2;
  float avg = avgFSR.reading(fsr);
  float avg2 = avg2FSR.reading(avg);
  return avg2;
}
void readEncs() {
    // set encoders

if ((prevMenu[0]!=modSel)  // IF THE MODULE SELECTION HAS CHANGED
|| (prevMenu[1]!=sub1)){  // OR THE SUB MENU HAS CHANGED
  readings[5]=0;
 }    
  // If module selection has changed, write the last correct value to the encoder
  if ((prevMenu[0]!=modSel) || (prevMenu[1]!=sub1)||(prevMenu[2]!=sub2)){
   
 midEnc.write(par1[modSel][sub1]);
 brijEnc.write(par2[modSel][sub2]);
  prevMenu[0]=modSel;
  prevMenu[1]=sub1;
  prevMenu[2]=sub2;
  }



// stash previous values
if((prevMenu[0]==modSel)&&(prevMenu[1]==sub1)){
  // menu[0]
 par1[modSel][sub1]=readings[1];}
if ((prevMenu[0]==modSel)&&(prevMenu[1]==sub1)&&(prevMenu[2]==sub2)){
 par2[modSel][sub2]=readings[2];
}
  // read encoders

  if (readings[0]!=neckEnc.read()){
    Serial.println("call (N)");
    makeNeck();
  }
//    if (readings[1]!=midEnc.read()){
//    Serial.println("call (M)");
//    makeMid();
//  }
      if (readings[2]!=brijEnc.read()){
    Serial.println("call (B)");
    makeBrij();
  }
  // Neck
readings[0] = neckEnc.read();
if (readings[0]>numOfMods*modReg){
  neckEnc.write(numOfMods*modReg);
  }
if (readings[0]<0){
  neckEnc.write(0);
}
// Mid
readings[1] = midEnc.read();
if (readings[1]>maxEnc){
  midEnc.write(maxEnc);
}
if (readings[1]<minEnc){
  midEnc.write(minEnc);
}
// Brij
readings[2] = brijEnc.read();
if (readings[2]>maxEnc){
  brijEnc.write(maxEnc);
}
if (readings[2]<minEnc){
  brijEnc.write(minEnc);
}
}


void makeDisplay1(String label1){
 
//    display.clearDisplay();
//    display.setTextSize(1);
//    display.setTextColor(WHITE);
//    display.setCursor(0,0); 
//    display.println(label1);   
//    display.display();

    
  
  
  
}


void makeD2(String lab1,String lab2,String lab3, int mod, int sub1,int sub2){
  
  // make some displays
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0); 
    display.println(lab1);
    display.setTextSize(1);
    display.println(par1[mod][sub1]);
    display.println(par2[mod][sub2]);
    display.display();
  }
  void makeBrij(){
     display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0); 
    display.println(par2[modSel][sub2]);
    display.display();
  }
    void dispPUB(){
     display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0); 
    display.println(pug);
    display.display();
  }

void makeNeck(){
  
  // make some displays
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0); 
    display.println(modLabel);
    display.display();
  }
  void welcome(){
  
  // make some displays
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0); 
    display.println("WILLIAMS");
    display.println("HA BASS");
    display.display();
  }
