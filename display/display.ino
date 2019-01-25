/*********************************************************************



This example is for a 128x32 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)


*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Encoder.h>


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


Encoder myEnc(29, 28);
void setup()   {                
  Serial.begin(9600);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
//  display.display();
//  delay(2000);

  // Clear the buffer.
  display.clearDisplay();

  // draw a single pixel
  // display.drawPixel(10, 10, WHITE);
  // Show the display buffer on the hardware.
  // NOTE: You _must_ call display after making any drawing commands
  // to make them visible on the display hardware!
  //display.display();
  // delay(2000);
  // display.clearDisplay();

  // draw many lines
 // testdrawline();
 // display.display();
 // delay(2000);
 // display.clearDisplay();

  // draw rectangles
//  testdrawrect();
//  display.display();
//  delay(2000);
//  display.clearDisplay();

  // draw multiple rectangles
//  testfillrect();
//  display.display();
//  delay(2000);
//  display.clearDisplay();

  // draw mulitple circles
//  testdrawcircle();
//  display.display();
//  delay(2000);
//  display.clearDisplay();

  // draw a white circle, 10 pixel radius
//  display.fillCircle(display.width()/2, display.height()/2, 10, WHITE);
//  display.display();
//  delay(2000);
//  display.clearDisplay();

//  testdrawroundrect();
//  delay(2000);
//  display.clearDisplay();

//  testfillroundrect();
//  delay(2000);
//  display.clearDisplay();

//  testdrawtriangle();
//  delay(2000);
//  display.clearDisplay();
   
//  testfilltriangle();
//  delay(2000);
//  display.clearDisplay();

  // draw the first ~12 characters in the font
  // 65 appears to be A
  // testdrawchar();
  //display.display();
  //delay(20000);
  //display.clearDisplay();

  // draw scrolling text
  //testscrolltext();
  //delay(2000);
 // display.clearDisplay();

  // text display tests
  
  // display.setTextColor(BLACK, WHITE); // 'inverted' text
  //float test = 1/3;
  //display.println(test);
  //display.setTextSize(2);
  //display.setTextColor(WHITE);
  //display.print("0x"); display.println(0xDEADBEEF, HEX);
  //display.display();
  //delay(2000);
  //display.clearDisplay();

  // miniature bitmap display
  //display.drawBitmap(30, 16,  logo16_glcd_bmp, 16, 16, 1);
  //display.display();
  //delay(1);

  // invert the display
  //display.invertDisplay(true);
  //delay(1000); 
  //display.invertDisplay(false);
  //delay(1000); 
  //display.clearDisplay();

  // draw a bitmap icon and 'animate' movement
 // testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);

 
}
long oldPosition  = -999;
long newPosition = 0;


void loop() {
/*
    Read encoder, send value to display and control horizontal position



 */
    
    makeDisplay();
  
  
 
}
void makeDisplay(){
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    if (newPosition > 50){
    display.println("TUNER");
    }
    if (newPosition <= 50){
    display.println("SYNTH");
    }
    display.display();
    display.setCursor(newPosition/3+30,17);
    //display.setCursor(0,0);
    oldPosition = newPosition;
    display.println('|');
    display.display();
}}


