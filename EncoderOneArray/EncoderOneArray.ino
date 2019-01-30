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
int readings[6]={0,0,0,-1,-1,-1};  // necassary for click state to start at zero
int clicks[3];
int lastclicks[3];
//int switch1[1];

void loop() {
checkEncoders();        // GET ENCODER READINGS

otherclicks(4);
}


void checkEncoders(){
    readings[0] = neckEnc.read();
    readings[1] = midEnc.read();
    readings[2] = brijEnc.read();
    for (int i=0; i<3; i++){
  if (readings[i] != old[i]) {
    old[i] = readings[i];
    printDebug();
  }
  }
  }


void printDebug(){
    Serial.print("  Encoder readings:  ");
    for (int i=0; i < 6; i++){
    Serial.print(readings[i]);
    Serial.print("     ");}
   // Serial.print(readings[3]);
    Serial.println("");
}

void otherclicks(int np1){
    // Neck encoder click
    debouncer1.update();debouncer2.update();debouncer3.update();        // CHECK CLICKS
    clicks[0] = debouncer1.read(); 
    clicks[1] = debouncer2.read();
    clicks[2] = debouncer3.read();// GET CLICK1 VALUE
    for (int i = 0; i < 3; i++){
    
    if (clicks[i] != lastclicks[i]){
    if (clicks[i] == HIGH){
      // we went from off to on and we have a click
     readings[3+i]++;
     printDebug();
    }
   }
   if (readings[3+i]>np1-1){
    readings[3+i]=0;
    }  // make sure clicks loop through the correct number of parameters
    lastclicks[i]=clicks[i];
}
}

