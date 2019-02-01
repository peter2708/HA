/*
 * Next, change, store and update readings when switching between menus
 * and add max and min values -- happily constrain values, but writing old values to encoder eludes me
 */
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

void setup() {
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
  Serial.print("Hello Startup Message");
  }

// Some constraints
int maxmin1[2*maxPar] = {0,0,2,1,2,1,2,1};// max,min indexes for second sub menus
int maxs1[numOfMods]={3,1,2};                       // Maximum indexes for first sub menu  
// Some holders
int readings[6];
int par[numOfMods][maxPar]; //
int prevM;  // previous menu selection
int prevS1;
int prevS2;
int value1;
int value2;
int value3;
int modSel = 0;
int sub[numOfMods];
int sub2[numOfMods][maxPar];
String modLabel;
String subLabel;
// Some Labels
String modLabels[numOfMods]={"TONE","WAH","RING MOD"};
String subLabels[numOfMods][maxPar] = {"PAN","BASS GAIN","LOW MID G","HIGH MID G",
"DECAY","SENSITIVITY","ANIMAL","DIRECTION"};


void loop() {
  // set encoders
  // If module selection has changed, write the last correct value to the encoder
  if ((prevM!=modSel) || (prevS1!=sub[modSel])){
  midEnc.write(par[modSel][sub[modSel]]);
  prevM=modSel;
  prevS1=sub[modSel];
  }

  

  
// stash previous values
if(prevM==modSel){
  par[modSel][sub[modSel]]=readings[1];
}
  // read encoders
readings[0] = neckEnc.read();
if (readings[0]>numOfMods*modReg){
  neckEnc.write(numOfMods*modReg);
  }
if (readings[0]<0){
  neckEnc.write(0);
}
readings[1] = midEnc.read();
if (readings[1]>maxEnc){
  midEnc.write(maxEnc);
}
if (readings[1]<minEnc){
  midEnc.write(minEnc);
}
readings[2] = brijEnc.read();
if (readings[2]>maxEnc){
  brijEnc.write(maxEnc);
}
if (readings[2]<minEnc){
  neckEnc.write(minEnc);
}
// Read buttons
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
  if(readings[5]>maxPar-2){
    readings[5]=-1;
   }
   readings[5]++;
   }
value3=nvalue3;
}

// Assign parameter values
modSel = constrain(readings[0]/modReg,0,numOfMods-1);
modLabel = modLabels[modSel];
sub[modSel] = readings[4];
subLabel = subLabels[modSel][sub[modSel]];
printDebug();   // Will eventually be display



} 




void printDebug(){

display.clearDisplay();
display.setTextSize(2);
display.setTextColor(WHITE);
display.setCursor(0,0); 
display.print(modLabel); 
display.print("|"); 
display.println(readings[1]);
display.print(subLabel); 
//display.print("|"); 
//display.println(switch1);  
display.display();

Serial.println(readings[5]);
}
    





