/*
 *  Current issue, fill the max values table with fours
 */
#include <movingAvg.h>                  // https://github.com/JChristensen/movingAvg
#include <Encoder.h>
#include <Bounce2.h>  // Simple debouncer for clicks on encoders 
#include <Adafruit_GFX.h> // For Display
#include <Adafruit_SSD1306.h> // For Display
Encoder neckEnc(29, 28);  // Neck rotation
Encoder midEnc(27, 26);  // Mid rotation
Encoder brijEnc(25,24);      // Bridge Encoder

#define CLICK_PIN3 33
#define CLICK_PIN2 34
#define CLICK_PIN1 35 // Neck clicker

#define numOfMods 3  // THE NUMBER OF MODULES
#define maxPar 4 // MAX NUMBER OF PARAMETERS PER SUB MENU
#define maxEnc 50 // Maximum encoder reading
#define minEnc -50 // Minimum encoder reading
#define modReg 20 // the rotation region per monitor

// Instantiate a Bounce object
Bounce debouncer1 = Bounce(); 
Bounce debouncer2 = Bounce(); 
Bounce debouncer3 = Bounce(); 

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

// Audio
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
AudioMixer4              midToneMix;         //xy=463.00018310546875,403.00018310546875
AudioMixer4              BassToneMix;         //xy=466.99999237060547,332.0000801086426
AudioFilterStateVariable LowMidTone;        //xy=617.0001602172852,445.0001525878906
AudioMixer4              ToneMix;         //xy=815.0001029968262,387.0001792907715
AudioOutputI2S           i2s2;           //xy=960.0008316040039,392.00024032592773
AudioConnection          patchCord1(i2s1, 0, PUBlend, 0);
AudioConnection          patchCord2(i2s1, 1, PUBlend, 1);
AudioConnection          patchCord3(PUBlend, 0, BassTone, 0);
AudioConnection          patchCord4(PUBlend, 0, BassToneMix, 0);
AudioConnection          patchCord5(PUBlend, LoPass);
AudioConnection          patchCord6(PUBlend, 0, midToneMix, 0);
AudioConnection          patchCord7(PUBlend, 0, MidTone, 0);
AudioConnection          patchCord8(MidTone, 0, midToneMix, 2);
AudioConnection          patchCord9(MidTone, 1, midToneMix, 1);
AudioConnection          patchCord10(MidTone, 2, midToneMix, 3);
AudioConnection          patchCord11(BassTone, 1, BassToneMix, 1);
AudioConnection          patchCord12(BassTone, 2, BassToneMix, 2);
AudioConnection          patchCord13(LoPass, 0, BassToneMix, 3);
AudioConnection          patchCord14(midToneMix, 0, ToneMix, 1);
AudioConnection          patchCord15(midToneMix, 0, LowMidTone, 0);
AudioConnection          patchCord16(BassToneMix, 0, ToneMix, 0);
AudioConnection          patchCord17(LowMidTone, 0, ToneMix, 2);
AudioConnection          patchCord18(LowMidTone, 2, ToneMix, 3);
AudioConnection          patchCord19(ToneMix, 0, i2s2, 0);
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

// Eof Audio
int fsr1Pin = A17;
int fsr2Pin = A18;
int fsr1; int fsr2; float fsr;

movingAvg avgFSR(10);                  // define the moving average object
movingAvg avg2FSR(25);                  // define the SECOND moving average object

 
int maxsub2[numOfMods][maxPar];
void setup() {
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
  welcome();
  
maxsub2[0][0]=1;
maxsub2[1][0]=4;
  }

// Some constraints
int maxmin1[2*maxPar] = {0,0,2,1,2,1,2,1};// max,min indexes for second sub menus
int maxs1[numOfMods]={3,1,2};                       // Maximum indexes for first sub menu  
// Some holders
int readings[6];
int par1[numOfMods][maxPar];//[maxPar]; //
int par2[numOfMods][maxPar][maxPar]; //
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
String subLabel;
String sub2Label;
int sub2LabIx[numOfMods][maxPar][maxPar];
// Some Labels
String modLabels[numOfMods]={"TONE","WAH","RING MOD"};
String subLabels[numOfMods][maxPar] = {"PAN","BASS GAIN","LOW MID G","HIGH MID G",
"DECAY","SENSITIVITY","ANIMAL","DIRECTION"};
String sub2Labels[numOfMods][maxPar] = {"NECK","RESONANCE","FREQUENCY"};
// Some intialisers
int oldModSel = 0;
int oldSub1 = 0;
int oldR1 = 0;
// Assign Some constants


void loop() {
readEncs();


// Read buttons

readClicks();

// FSRS 
  fsr1 = analogRead(fsr1Pin);
  fsr2 = analogRead(fsr2Pin);
  fsr = fsr1-fsr2;
  int avg = avgFSR.reading(fsr);
  int avg2 = avg2FSR.reading(avg);
  
// Assign parameter values
modSel = constrain(readings[0]/modReg,0,numOfMods-1);
modLabel = modLabels[modSel];
sub1 = readings[4];
sub2 = readings[5];
subLabel = subLabels[modSel][sub1];
sub2Label = sub2Labels[modSel][sub2];

// Audio Algorithms
 // For neck control of PUG
if ((par2[0][0][0]>10)||(par2[0][0][0]<-10)){  
  pugFsrC=par2[0][0][0]*0.02;     // if neck control is on, use this, otherwise, 1  
} 
else {pugFsrC=0;} 
float bAvgPug=constrain((fsr-120)*pugFsrC,-50,50);
// Pan Control
float bpug=constrain((50+(par1[0][0])+bAvgPug)*0.01,0,1);//*bAvgPug;    // Bridge pick up gain
float npug=1-bpug;                                              // Neck pick up gain
// For Bass Tone
float bbpg = constrain(par1[0][1]*0.01,0,0.5);

// Audio processing
/*
 * Tone Section
 */
// Pick Up Balance
PUBlend.gain(0,npug);                                   // Neck pick up gain
PUBlend.gain(1,bpug);                                   // Bridge pick up gain
PUBlend.gain(2,0);
PUBlend.gain(3,0);
// Bass Tone Filter Control
BassTone.frequency(120);
BassTone.resonance(3);
// Bass Tone Mix
BassToneMix.gain(0,0.5-bbpg);
BassToneMix.gain(1,bbpg);
BassToneMix.gain(2,0);
BassToneMix.gain(3,0);
// Mid Tone Mix
midToneMix.gain(0,1);
midToneMix.gain(1,0);
midToneMix.gain(2,0);
midToneMix.gain(3,0);
// Tone Mix
ToneMix.gain(0,0.25);
ToneMix.gain(1,0.25);
ToneMix.gain(2,0.25);
ToneMix.gain(3,0.25);

debug();
} 
void readEncs() {
    // set encoders

if ((prevMenu[0]!=modSel) || (prevMenu[1]!=sub1)){
  readings[5]=0;
 }    
  // If module selection has changed, write the last correct value to the encoder
  if ((prevMenu[0]!=modSel) || (prevMenu[1]!=sub1)||(prevMenu[2]!=sub2)){
 midEnc.write(par1[modSel][sub1]);
 brijEnc.write(par2[modSel][sub1][sub2]);
  prevMenu[0]=modSel;
  prevMenu[1]=sub1;
  prevMenu[2]=sub2;
  }



// stash previous values
if((prevMenu[0]==modSel)&&(prevMenu[1]==sub1)){
  // menu[0]
 par1[modSel][sub1]=readings[1];}
if ((prevMenu[0]==modSel)&&(prevMenu[1]==sub1)&&(prevMenu[2]==sub2)){
 par2[modSel][sub1][sub2]=readings[2];
}
  // read encoders
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
void readClicks(){
  // Neck
  debouncer1.update();
int nvalue1 = debouncer1.read();
if (nvalue1!=value1){
if (value1 == HIGH){
  if(readings[3]>0){
    readings[3]=-1;
  }
   readings[3]++;
   }
value1=nvalue1;
}
// Mid
debouncer2.update();
int nvalue2 = debouncer2.read();
if (nvalue2!=value2){
if (value2 == HIGH){
  if(readings[4]>maxPar-2){
    readings[4]=-1;
  }
   readings[4]++;
   }
value2=nvalue2;
}
// Bridge
debouncer3.update();
int nvalue3 = debouncer3.read();
 if (nvalue3!=value3){
if (value3 == HIGH){
  
  if(readings[5]>maxsub2[modSel][sub1]-2){
    readings[5]=-1;
   }
   readings[5]++;
   }
value3=nvalue3;
}

}

void debug(){

Serial.print("Module: ");
Serial.print(modLabel);
Serial.print("\t");
Serial.print("Sub1: ");
Serial.print(subLabel);
Serial.print("\t");
Serial.print(par1[modSel][sub1]);
Serial.print("\t");
Serial.print("Sub2: ");
Serial.print(sub2);
Serial.print("\t");
Serial.print(par2[modSel][sub1][sub2]);


Serial.print("\t");
Serial.print("Maximum number of sub menus");
Serial.println(maxsub2[modSel][sub1]);


}

void welcome(){

display.clearDisplay();
display.setTextSize(3);
display.setTextColor(WHITE);
display.setCursor(0,0); 
display.print("HA-BASS"); 
display.display();
}    





