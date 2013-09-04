// SerialInput_1 (Arduino)
//
// Illustrates reading from the serial port.
//
// Opens the serial port (Serial.begin), flushes the serial port buffer by continually
// reading (Serial.read) until Serial.available is 0.
//
// The program then continually loops.  If Serial.available is not zero, a character
// is read and if it is a numeric character, it is converted to an integer and this number
// of dots are written to the terminal.  Note that this is only a single digit.
//
// ***************************
// Important Note.
//
// In file arduino-0007\lib\targets\wiring.c, the implementation of serialAvailable is incorrect.
// It must be modified as shown below;
//
// int serialAvailable()
//{
//      return (RX_BUFFER_SIZE + rx_buffer_head - rx_buffer_tail) % RX_BUFFER_SIZE;
//      //return (rx_buffer_head - rx_buffer_tail) % RX_BUFFER_SIZE; // <<<<<flaw here
//}
//
// copyright, Peter H Anderson, June 3, '07

#include "LPD8806.h"
#include "SPI.h"

// Example to control LPD8806-based RGB LED Modules in a strip

/*****************************************************************************/

// Number of RGB LEDs in strand:
int nLEDs = 32;

// Chose 2 pins for output; can be any valid output pins:
int dataPin  = 11;
int clockPin = 13;

// First parameter is the number of LEDs in the strand.  The LED strips
// are 32 LEDs per meter but you can extend or cut the strip.  Next two
// parameters are SPI data and clock pins:
LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

// You can optionally use hardware SPI for faster writes, just leave out
// the data and clock pin parameters.  But this does limit use to very
// specific pins on the Arduino.  For "classic" Arduinos (Uno, Duemilanove,
// etc.), data = pin 11, clock = pin 13.  For Arduino Mega, data = pin 51,
// clock = pin 52.  For 32u4 Breakout Board+ and Teensy, data = pin B2,
// clock = pin B1.  For Leonardo, this can ONLY be done on the ICSP pins.
//LPD8806 strip = LPD8806(nLEDs);


#define TRUE !0
#define FALSE 0

void flush_buffer(void);
int is_num(char ch);
byte pixels[32][3] = {0};

void setup()
{
  Serial.begin(9600);
  delay(100);
  flush_buffer();
  // Serial.read();
  strip.begin();
  for (int i = 0; i < 32; i++){

  strip.setPixelColor(i, 0);
  }
  strip.show();
}

void loop()
{
  int  num_chars, num, n;
  int index = 0;
  byte red = 0;
  byte blue = 0;
  byte green = 0;



  if (Serial.available()>3)
  {
     if (Serial.peek() == 'p'){
        preset(Serial.parseInt());
     }
     else {
     index = Serial.parseInt();
     red = Serial.parseInt();
     green = Serial.parseInt();
     blue = Serial.parseInt();
     strobe = Serial.parseInt();
      //Serial.println(ch); // used for debugging
     pixels[index][0] = red;
     pixels[index][1] = green;
     pixels[index][2] = blue;


  }

  for (int i = 0; i < strip.numPixels(); i++) strip.setPixelColor(i, pixels[i][0], pixels[i][1], pixels[i][2]);
  strip.show();

}



void clearstrip(void){
   for (int i = 0; i <32; i++){
    strip.setPixelColor(i, 0);
   }
   strip.show();
}

void clearpixel(int i){
  strip.setPixelColor(i, 0);
  strip.show();
}

void preset(int choice){
  switch (choice) {
  case 0: altColorChase();
          break;
  case 1: differentColor();
          break;
  case 2: differentColorCycle();
          break;
  case 3: differentColorCycleRandom();
          break;
  case 4: rainbow(100);
          break;
  case 5: break;
  case 6: break;
  case 7: break;
  case 8: break;
  case 9: break;
  }
}

void altColorChase(){
   int i;
   uint32_t color = strip.Color(127, 0, 0);
   uint32_t colorChase = strip.Color(0, 0, 127);

   for(i = 0; i < nLEDs; i++) {
     strip.setPixelColor(i, color);
   }
   strip.show();

   for(i = 0; i < nLEDs; i++) {
     strip.setPixelColor(i, colorChase);
     strip.show();
     delay(50);
     strip.setPixelColor(i, color);
     strip.show();
   }

}

void differentColor(){
  int i;
   int color = 0;
   uint32_t t;
   for(i = 0; i < nLEDs; i++){
      t = Wheel(color);
      color+=12;
      strip.setPixelColor(i, t);
      strip.show();
   }
}

void differentColorCycle(){

   int i;
   static int color =0;

   uint32_t t;
   for(i = 0; i < nLEDs; i++){
      t = Wheel(color);
      color+=12;
      color %= 384;
      strip.setPixelColor(i, t);
      strip.show();

   }
   color+=12;
}



void differentColorCycleRandom(){

   int i;
   static int color =0;
   static int oldvals[ 32 ] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
   int newvals[ 32 ];
   int randomCount;
   uint32_t t;
   int randomIdx = 0;
    int randcolour;

   for(i = 0; i < nLEDs; i++){
     newvals[ i ] = -1;
   }
   for(i = 0; i < nLEDs; i++){
   //   do
   //   {

        color = rand() % 384;
        color += 12 - (color % 12);
        color%= 384;
  /*
        randomCount = (rand()% (nLEDs - i)) + 1;
        randomIdx = 0;
        while( randomCount > 0)
        {
          if( newvals[ randomCount ] == -1 )
          {
            randomCount--;
          }
            randomIdx++;
          }
       }
       while( oldvals [ randomIdx ] == color );
  */    newvals[ randomIdx ] = color;
      t = Wheel(color);
      strip.setPixelColor(i, t);
      strip.show();
   }
  // }
   for(i = 0; i < nLEDs; i++){
      oldvals[ i ] = newvals[ i ];
   }

}


// Fill the dots progressively along the strip.
void colorWipe(uint32_t c, uint8_t wait) {
  int i;

  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) {
  int i, j;

  for (j=0; j < 384; j++) {     // 3 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 384));
    }
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed
// along the chain
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j=0; j < 384 * 5; j++) {     // 5 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 384-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 384 / strip.numPixels()) + j) % 384) );
    }
    strip.show();   // write all the pixels out
    delay(wait);
  }
}



// Chase one dot down the full strip.
void colorChase(uint32_t c, uint8_t wait) {
  int i;

  // Start by turning all pixels off:
  for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

  // Then display one pixel at a time:
  for(i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c); // Set new pixel 'on'
    strip.show();              // Refresh LED states
    strip.setPixelColor(i, 0); // Erase pixel, but don't refresh!
    delay(wait);
  }

  strip.show(); // Refresh to turn off last pixel
}

/* Helper functions */

//Input a value 0 to 384 to get a color value.
//The colours are a transition r - g -b - back to r

uint32_t Wheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128;   //Red down
      g = WheelPos % 128;      // Green up
      b = 0;                  //blue off
      break;
    case 1:
      g = 127 - WheelPos % 128;  //green down
      b = WheelPos % 128;      //blue up
      r = 0;                  //red off
      break;
    case 2:
      b = 127 - WheelPos % 128;  //blue down
      r = WheelPos % 128;      //red up
      g = 0;                  //green off
      break;
  }
  return(strip.Color(r,g,b));
}

void flush_buffer(void)
{
  int num;
  while(1)
  {
    num = Serial.available();
    if (num == 0)
    {
      break;
    }
    else
    {
       Serial.read();
    }
  }
}
