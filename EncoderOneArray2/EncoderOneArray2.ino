/*
 * The label array and adressing needs to be fixed
 */

#include <Encoder.h>
#include <Bounce2.h>  // Simple debouncer for clicks on encoders 
Encoder neckEnc(29, 28);  // Neck rotation
Encoder midEnc(27, 26);  // Mid rotation
Encoder brijEnc(25,24);      // Bridge Encoder

#define CLICK_PIN3 33
#define CLICK_PIN2 34
#define CLICK_PIN1 35 // Neck clicker

// Instantiate a Bounce object
Bounce debouncer1 = Bounce(); 
Bounce debouncer2 = Bounce(); 
Bounce debouncer3 = Bounce(); 

void setup() {
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
  }
int old[3];
int readings[6]; // 3 ROTATE ENCODER READINGS FOLLOWED BY 3 CLICK READINGS
int numOfMods = 3;  // THE NUMBER OF MODULES
int clicks[3]={0,0,0};      // required for click updates
int lastclicks[3]; // required for click updates
int modSel=0;       // Module Selection number 
float divisor = pow(110,-1)*numOfMods;
int sub1[4];
int sub2[4];
int previousModSel = 99;
int sub1max = 4;
int sub1min = 0;
int sub2max = 4;
int sub2min = 0;

// Some Labels
String modules[3] = {"Tone","Synth","Chorus"};
String labels[4][6] = {"PAN","BASS GAIN","LOW MID GAIN","HIGH MID GAIN",  // sub1 module 1 [0][0:3]
"WAVE","HARMONY","","",                                // sub1 module 2
"RATE","DEPTH","LOW PASS","",                                // sub1 module 3
"NECK CONTROL","FREQ","RESONANCE","",         // sub2 module 1
"MIX","NECK","","",                                // sub2 module 2
"MIX","NECK","",""};                              // sub2 module 3

//String sub2aLabels[4] = {"NECK CONTROL","FREQ","RESONANCE"};
String sub1Label;
String sub2Label;
// String 


void loop() {
labelsAndSelections();
checkEncoders();        // GET ENCODER READINGS
otherclicks(3,4,0);         // Get click states
}  /*
EOF LOOP
*/


void checkEncoders(){
    readings[0] = neckEnc.read();
    readings[1] = midEnc.read();
    readings[2] = brijEnc.read();
    for (int i=0; i<3; i++){
  if (readings[i] != old[i]) {
    printDebug();
    old[i] = readings[i];
  }
  }
  if (readings[0]>110){
    neckEnc.write(0);
  }
  if (readings[0]<0){
    neckEnc.write(110);
  }
  }

void otherclicks(int sub1max, int sub2max, int sub2min){
    // Neck encoder click
    debouncer1.update();debouncer2.update();debouncer3.update();        // CHECK CLICKS
    clicks[0] = debouncer1.read(); 
    clicks[1] = debouncer2.read();
    clicks[2] = debouncer3.read();// GET CLICK1 VALUE
    for (int i = 0; i < 3; i++){
    if (clicks[i] != lastclicks[i]){
    if (clicks[i] == LOW){
      // we went from off to on and we have a click
      readings[3+i]++;
      printDebug();
      delay(50);
   }
   }

if (readings[4]>sub1max){
  readings[4]=0;
    } 
    if (readings[5]>sub2max){
  readings[5]=sub2min;
    }// make sure clicks loop through the correct number of parameters
    
    lastclicks[i]=clicks[i];
}
}
void labelsAndSelections(){
  modSel = constrain(readings[0]*divisor,0,numOfMods-1);  // Determine Module Selection Number
if (modSel != previousModSel){
  // reset sub1 and sub2
  readings[4]=0;readings[5]=0;
  previousModSel = modSel;
  }
sub1Label = labels[modSel][readings[4]];
}
void printDebug(){
    Serial.print("  Encoder readings:  ");
    for (int i=1; i < 6; i++){
    Serial.print(readings[i]);
    Serial.print("     ");}
   // Serial.print(readings[3]);
    Serial.print("");
//Serial.print(readings[4]);
//Serial.print(readings[0]);
Serial.print("  ");
Serial.print(modules[modSel]);
Serial.print("  ");
Serial.print(sub1Label);
Serial.print("  ");
Serial.println(sub2Label);




}



