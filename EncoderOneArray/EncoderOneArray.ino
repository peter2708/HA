#include <Encoder.h>
Encoder neckEnc(29, 28);  // Neck rotation
Encoder midEnc(27, 26);  // Neck rotation

void setup() {
  Serial.begin(9600);
  }
int old[2];
int readings[2];

void loop() {
checkNeckEncoder();
checkMidEncoder();  
}


void checkNeckEncoder(){
  readings[0] = neckEnc.read();
  if (readings[0] != old[0]) {
    old[0] = readings[0];
    printDebug();
  }}
void checkMidEncoder(){
readings[1] = midEnc.read();
  if (readings[1] != old[1]) {
    old[1] = readings[1];
    printDebug();
  }}

void printDebug(){
  Serial.print(readings[0]);
    Serial.print("     ");
    Serial.println(readings[1]);
}

