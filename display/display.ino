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

#define NUMFLAKES 10
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

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
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

void loop() {
/*
    Read encoder, send value to display and control horizontal position



 */
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    display.clearDisplay();
    display.setCursor(newPosition/3+30,0);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    //display.setCursor(0,0);
    oldPosition = newPosition;
    Serial.println(newPosition);
    display.println(newPosition);
    display.display();
  }
  
 
}


void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];
 
  // initialize
  for (uint8_t f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS] = random(display.width());
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random(5) + 1;
    
    Serial.print("x: ");
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(" y: ");
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(" dy: ");
    Serial.println(icons[f][DELTAY], DEC);
  }

  while (1) {
    // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, WHITE);
    }
    display.display();
    delay(200);
    
    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > display.height()) {
        icons[f][XPOS] = random(display.width());
        icons[f][YPOS] = 0;
        icons[f][DELTAY] = random(5) + 1;
      }
    }
   }
}


void testdrawchar(void) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  for (uint8_t i=65; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 10 == 0))
      display.println();
  }    
  display.display();
  delay(1);
}

void testdrawcircle(void) {
  for (int16_t i=0; i<display.height(); i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, WHITE);
    display.display();
    delay(1);
  }
}

void testfillrect(void) {
  uint8_t color = 1;
  for (int16_t i=0; i<display.height()/2; i+=3) {
    // alternate colors
    display.fillRect(i, i, display.width()-i*2, display.height()-i*2, color%2);
    display.display();
    delay(1);
    color++;
  }
}

void testdrawtriangle(void) {
  for (int16_t i=0; i<min(display.width(),display.height())/2; i+=5) {
    display.drawTriangle(display.width()/2, display.height()/2-i,
                     display.width()/2-i, display.height()/2+i,
                     display.width()/2+i, display.height()/2+i, WHITE);
    display.display();
    delay(1);
  }
}

void testfilltriangle(void) {
  uint8_t color = WHITE;
  for (int16_t i=min(display.width(),display.height())/2; i>0; i-=5) {
    display.fillTriangle(display.width()/2, display.height()/2-i,
                     display.width()/2-i, display.height()/2+i,
                     display.width()/2+i, display.height()/2+i, WHITE);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display.display();
    delay(1);
  }
}

void testdrawroundrect(void) {
  for (int16_t i=0; i<display.height()/2-2; i+=2) {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, WHITE);
    display.display();
    delay(1);
  }
}

void testfillroundrect(void) {
  uint8_t color = WHITE;
  for (int16_t i=0; i<display.height()/2-2; i+=2) {
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, color);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display.display();
    delay(1);
  }
}
   
void testdrawrect(void) {
  for (int16_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, WHITE);
    display.display();
    delay(1);
  }
}

void testdrawline() {  
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, 0, i, display.height()-1, WHITE);
    display.display();
    delay(1);
  }
  for (int16_t i=0; i<display.height(); i+=4) {
    display.drawLine(0, 0, display.width()-1, i, WHITE);
    display.display();
    delay(1);
  }
  delay(250);
  
  display.clearDisplay();
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, display.height()-1, i, 0, WHITE);
    display.display();
    delay(1);
  }
  for (int16_t i=display.height()-1; i>=0; i-=4) {
    display.drawLine(0, display.height()-1, display.width()-1, i, WHITE);
    display.display();
    delay(1);
  }
  delay(250);
  
  display.clearDisplay();
  for (int16_t i=display.width()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, i, 0, WHITE);
    display.display();
    delay(1);
  }
  for (int16_t i=display.height()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, 0, i, WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();
  for (int16_t i=0; i<display.height(); i+=4) {
    display.drawLine(display.width()-1, 0, 0, i, WHITE);
    display.display();
    delay(1);
  }
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(display.width()-1, 0, i, display.height()-1, WHITE); 
    display.display();
    delay(1);
  }
  delay(250);
}

void testscrolltext(void) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10,0);
  display.clearDisplay();
  display.println("SYNTH");
  display.display();
  delay(1);
 
  //display.startscrollright(0x00, 0x0F);
  //delay(2000);
  //display.stopscroll();
  //delay(1000);
  //display.startscrollleft(0x00, 0x0F);
  delay(20000);
  //display.stopscroll();
  //delay(1000);    
  //display.startscrolldiagright(0x00, 0x07);
  //delay(2000);
  //display.startscrolldiagleft(0x00, 0x07);
  //delay(2000);
  //display.stopscroll();
}
