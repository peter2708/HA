/*
 *  Current issue, Check that all tone references are correct. Mid low and mid high are very quiet in gain mode
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
AudioInputI2S            ip; //xy=83.00000381469727,214.40002632141113
AudioMixer4              PUBlend; //xy=224.00007247924805,227.40009689331055
AudioFilterStateVariable LowMidTone; //xy=376.00011444091797,314.40013885498047
AudioFilterBiquad        LoPass; //xy=394.0000343322754,366.40002822875977
AudioFilterStateVariable BassTone; //xy=401.0001335144043,416.4002285003662
AudioFilterStateVariable HighMidTone; //xy=418.00014877319336,260.40014839172363
AudioAnalyzeRMS          cleanLevel;           //xy=437.99998474121094,518.9999923706055
AudioMixer4              tMix2; //xy=594.0002899169922,340.40028381347656
AudioMixer4              tMix1; //xy=595.0002899169922,272.4002799987793
AudioMixer4              tMix3;         //xy=595.0002975463867,409.40010261535645
AudioMixer4              tMix4; //xy=770.0000381469727,330.4000053405762
AudioAnalyzeRMS          HighMidToneLevel; //xy=785,240
AudioAnalyzeRMS          lowMidToneLevel; //xy=839.0000381469727,402.0000219345093
AudioAmplifier           amp1;           //xy=896.0001754760742,329.3999900817871
AudioOutputI2S           op; //xy=1017.0001792907715,336.40003871917725
AudioConnection          patchCord1(ip, 0, PUBlend, 0);
AudioConnection          patchCord2(ip, 1, PUBlend, 1);
AudioConnection          patchCord3(PUBlend, 0, BassTone, 0);
AudioConnection          patchCord4(PUBlend, LoPass);
AudioConnection          patchCord5(PUBlend, 0, HighMidTone, 0);
AudioConnection          patchCord6(PUBlend, 0, LowMidTone, 0);
AudioConnection          patchCord7(PUBlend, 0, tMix4, 0);
AudioConnection          patchCord8(PUBlend, cleanLevel);
AudioConnection          patchCord9(LowMidTone, 0, tMix2, 1);
AudioConnection          patchCord10(LowMidTone, 1, tMix2, 0);
AudioConnection          patchCord11(LowMidTone, 2, tMix2, 2);
AudioConnection          patchCord12(LoPass, 0, tMix3, 0);
AudioConnection          patchCord13(BassTone, 0, tMix3, 2);
AudioConnection          patchCord14(BassTone, 1, tMix3, 1);
AudioConnection          patchCord15(BassTone, 2, tMix3, 3);
AudioConnection          patchCord16(HighMidTone, 0, tMix1, 1);
AudioConnection          patchCord17(HighMidTone, 1, tMix1, 0);
AudioConnection          patchCord18(HighMidTone, 2, tMix1, 2);
AudioConnection          patchCord19(tMix2, 0, tMix4, 2);
AudioConnection          patchCord20(tMix2, lowMidToneLevel);
AudioConnection          patchCord21(tMix1, 0, tMix4, 1);
AudioConnection          patchCord22(tMix1, HighMidToneLevel);
AudioConnection          patchCord23(tMix3, 0, tMix4, 3);
AudioConnection          patchCord24(tMix4, amp1);
AudioConnection          patchCord25(amp1, 0, op, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=95.99998474121094,277.39998626708984
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
int sub2LabelsIDX[numOfMods][10][10];
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

for (int i = 0;i<numOfMods;i++){
  for (int j = 0;j<maxPar;j++){
  maxsub2[i][j]=2;
}}
  
maxsub2[0][0]=1;

for (int i = 0;i<numOfMods;i++){
  for (int j = 0;j<maxPar;j++){
    for (int k = 0;k<maxPar;k++){
sub2LabelsIDX[i][j][k]=9;
  }}}
sub2LabelsIDX[0][0][0]=0;
sub2LabelsIDX[0][1][0]=1;
sub2LabelsIDX[0][2][0]=1;
sub2LabelsIDX[0][3][0]=1;
sub2LabelsIDX[0][1][1]=2;
sub2LabelsIDX[0][2][1]=2;
sub2LabelsIDX[0][3][1]=2;
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
String sub2Labels[10] = {"NECK","FREQUENCY","RESONANCE","","","","","","","nine"};
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
int sub2LabelIDX = sub2LabelsIDX[modSel][sub1][sub2];
sub2Label = sub2Labels[sub2LabelIDX];

// Audio Algorithms
 // For neck control of PUG
if ((par2[0][0][0]>10)||(par2[0][0][0]<-10)){  
  pugFsrC=par2[0][0][0]*0.04;     // if neck control is on, use this, otherwise, 1  
} 
else {pugFsrC=0;} 
float bAvgPug=constrain((fsr-120)*pugFsrC,-50,50);
// Pan Control
float bpug=constrain((50+(par1[0][0])+bAvgPug)*0.01,0,1);//*bAvgPug;    // Bridge pick up gain
float npug=1-bpug;   // Neck pick up gain


// For Bass Tone
float bg = constrain(map(float(par1[0][1]),0,50,0,0.5),0,0.5);
float bc = -constrain(map(float(par1[0][1]),0,50,0,0.5),-0.5,0);

float bF = map(float(par2[0][1][0]),-50,50,80,250);
float lpF = 10000-map(float(par1[0][1]),0,50,0,5000);
float lpQ = map(float(par1[0][1]),0,50,2,5);
float bQ = map(float(par2[0][1][1]),-50,50,.707,5);
// For Low Mid Tone
float lmg = constrain(map(float(par1[0][2]),0,50,0,1),0,1);
float lmc = -constrain(map(float(par1[0][2]),0,50,0,0.5),-0.5,0);

float lmF = map(float(par2[0][2][0]),-50,50,150,2000);
float lmQ = map(float(par2[0][2][1]),-50,50,.707,5);
// For High mid Tone
float hmg = constrain(map(float(par1[0][3]),0,50,0,1),0,1);
float hmc = -constrain(map(float(par1[0][3]),0,50,0,0.5),-0.5,0);

float hmF = map(float(par2[0][3][0]),-50,50,1500,5000);
float hmQ = map(float(par2[0][3][1]),-50,50,.707,5);

// Filters --- Tone
LoPass.setLowpass(0,lpF,lpQ);

BassTone.frequency(bF);
BassTone.resonance(bQ);

LowMidTone.frequency(lmF);
LowMidTone.resonance(lmQ);

HighMidTone.frequency(hmF);
HighMidTone.resonance(hmQ);
// Clean/Tone Balance

float cg = pow(0.1,(bg+bc+hmc+lmc)*2+hmg+lmg);    //Inspect
// Tone Mixers
tMix4.gain(0,cg);
tMix4.gain(1,1);
tMix4.gain(2,1);
tMix4.gain(3,1);
//
tMix3.gain(0,bg);
tMix3.gain(1,bg);
tMix3.gain(2,bc);
tMix3.gain(3,bc);
//
tMix2.gain(0,lmg);
tMix2.gain(1,lmc);
tMix2.gain(2,lmc);
tMix2.gain(3,0);
//
tMix1.gain(0,hmg);
tMix1.gain(1,hmc);
tMix1.gain(2,hmc);
tMix1.gain(3,0);

// Audio processing
/*
 * Tone Section
 */
// Pick Up Balance

PUBlend.gain(0,npug);                                   // Neck pick up gain
PUBlend.gain(1,bpug);                                   // Bridge pick up gain
PUBlend.gain(2,0);
PUBlend.gain(3,0);
// Filters






debug(lmg);
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

void debug(float v1){
//Serial.print("Bridge Pick Up: ");
//Serial.print(v1);
//Serial.print("\t");
//Serial.print("Module: ");
//Serial.print(modLabel);
//Serial.print("\t");
//Serial.print("Sub1: ");
//Serial.print(subLabel);
//Serial.print("\t");
//Serial.print(par1[modSel][sub1]);
//Serial.print("\t");
//Serial.print("Sub2: ");
//Serial.print(sub2Label);
//Serial.print("\t");
//Serial.print(par2[modSel][sub1][sub2]);
//Serial.print("\t");
Serial.print("Clean RMS:  ");
Serial.println(cleanLevel.read());
Serial.print("\t");
Serial.print("Low Mid Tone RMS:  ");
Serial.println(lowMidToneLevel.read());
Serial.print("\t");
Serial.print("High Mid Tone Level:  ");
Serial.println(HighMidToneLevel.read());

}

void welcome(){

display.clearDisplay();
display.setTextSize(3);
display.setTextColor(WHITE);
display.setCursor(0,0); 
display.print("HA-BASS"); 
display.display();
}    





