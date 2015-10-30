/*
 * This is a minimal example, see extra-examples.cpp for a version
 * with more explantory documentation, example routines, how to 
 * hook up your pixels and all of the pixel types that are supported.
 *
 */

#include "application.h"
#include "neopixel/neopixel.h"

//SYSTEM_MODE(AUTOMATIC);
SYSTEM_MODE(SEMI_AUTOMATIC);

#define LED_PIN D3
#define TOGGLE_PIN D2
#define WIFI_PIN D1

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN A5
#define PIXEL_COUNT 120
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

// LED Matrix for Mask
/*
000---001---002---003---004---005---006---007---008---009---010---011---012---013---014 // 15
---028---027---026---025---024---023---022---021---020---019---018---017---016---015--- // 14
------029---030---031---032---033---034---035---036---037---038---039---040---041------ // 13
---------053---052---051---050---049---048---047---046---045---044---043---042--------- // 12
------054---055---056---057---058---059---060---061---062---063---064---065---066------ // 13
---------078---077---076---075---074---073---072---071---070---069---068---067--------- // 12
------------079---080---081---082---083---084---085---086---087---088---089------------ // 11
---------------099---098---097---096---095---094---093---092---091---090--------------- // 10
------------------100---101---102---103---104---105---106---107---108------------------ // 9
---------------------------114---113---112---111---110---109--------------------------- // 6
------------------------------115---116---117---118---119------------------------------ // 5
*/

int animationIndex = 0;
bool animationToggled = false; // Every function must check this between frames in order to allow for a quick response to the button

void setup() 
{   
    attachInterrupt(TOGGLE_PIN, toggleAnimation, FALLING);
    attachInterrupt(WIFI_PIN, connect, CHANGE); // This will allow you to create a button or switch that triggers the WiFi connection call
    
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(10);
}

void loop() 
{
    if(animationIndex == 0)
    {
        rainbow(20);
    }else if(animationIndex == 1)
    {
        waveform(20);
    }else if(animationIndex == 2)
    {    
        heartAnimation(200);
    }else{
        rainbow(20);
    }
    
    if(animationToggled){
        animationToggled = false;
    }
}

void heartAnimation(uint8_t wait){
    uint16_t i;
    for(i=0; i<20 && !animationToggled; i++) {
        xsmallHeart();
        strip.show();
        delay(wait*2); 
        
        smallHeart();
        strip.show();
        delay(wait); 
        
        bigHeart();
        strip.show();
        delay(wait*2);
    }
}

void xsmallHeart()
{
    uint16_t i, j;
    
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0,0,0));
    }
    
    fillFrom(48, 49, strip.Color(255, 0, 0));
    fillFrom(46, 47, strip.Color(255, 0, 0));
    
    strip.setPixelColor(104, strip.Color(255, 0, 0));
    
    fillFrom(94, 95, strip.Color(255, 0, 0));
    
    fillFrom(83, 85, strip.Color(255, 0, 0));
    
    fillFrom(71, 74, strip.Color(255, 0, 0));
    
    fillFrom(58, 62, strip.Color(255, 0, 0));
}

void smallHeart()
{
    uint16_t i, j;
    
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0,0,0));
    }
    
    strip.setPixelColor(117, strip.Color(255, 0, 0));
    
    strip.setPixelColor(33, strip.Color(255, 0, 0));
    strip.setPixelColor(34, strip.Color(255, 0, 0));
    
    fillFrom(48, 50, strip.Color(255, 0, 0));
    
    strip.setPixelColor(60, strip.Color(255, 0, 0));
    
    strip.setPixelColor(111, strip.Color(255, 0, 0));
    strip.setPixelColor(112, strip.Color(255, 0, 0));
    
    fillFrom(103, 105, strip.Color(255, 0, 0));
    
    fillFrom(93, 96, strip.Color(255, 0, 0));
    
    fillFrom(82, 86, strip.Color(255, 0, 0));
    
    fillFrom(70, 75, strip.Color(255, 0, 0));
    
    strip.setPixelColor(37, strip.Color(255, 0, 0));
    strip.setPixelColor(36, strip.Color(255, 0, 0));
    
    fillFrom(45, 47, strip.Color(255, 0, 0));
    
    fillFrom(58, 62, strip.Color(255, 0, 0));
}

void bigHeart()
{
    uint16_t i, j;
    
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0,0,0));
    }
    
    fillFrom(4, 5, strip.Color(255, 0, 0));
    fillFrom(9, 10, strip.Color(255, 0, 0));
    fillFrom(18, 20, strip.Color(255, 0, 0));
    fillFrom(23, 25, strip.Color(255, 0, 0));
    fillFrom(31, 39, strip.Color(255, 0, 0));
    fillFrom(44, 51, strip.Color(255, 0, 0));
    fillFrom(56, 64, strip.Color(255, 0, 0));
    fillFrom(69, 76, strip.Color(255, 0, 0));
    fillFrom(81, 87, strip.Color(255, 0, 0));
    fillFrom(92, 97, strip.Color(255, 0, 0));
    fillFrom(102, 106, strip.Color(255, 0, 0));
    fillFrom(110, 113, strip.Color(255, 0, 0));
    fillFrom(116, 118, strip.Color(255, 0, 0));
}

void waveform(uint8_t wait) {
  uint16_t j;
  for(j=0; j<256 && !animationToggled; j++) {
    strip.clear();
    fillFrom(54, 66, strip.Color(0,0,255));
    
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256 && !animationToggled; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void fillFrom(uint16_t start, uint16_t finish, uint32_t color)
{
    uint16_t i;
    for(i = start; i<=finish; i++)
    {
        strip.setPixelColor(i, color);
    }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

// Pretends that it is a 30 by 15 Matrix
uint16_t customMappingFunction (uint16_t x, uint16_t y){
    int doubleMaxCountInRow = 30;
    int maxCountInRow = 15;
    int countInRow[11];
    countInRow[0] = 15;
    countInRow[1] = 14;
    countInRow[2] = 13;
    countInRow[3] = 12;
    countInRow[4] = 13;
    countInRow[5] = 12;
    countInRow[6] = 11;
    countInRow[7] = 10;
    countInRow[8] = 9;
    countInRow[9] = 6;
    countInRow[10] = 5;
    
    int numSpacersPerSide = maxCountInRow - countInRow[y];
    if(((y % 2) == 0) && ((x % 2) == 0)) // this means that it's an even row, so all odd pixels are dropped in the row
    {
        // count up the number of pixels in the rows prior
        int pixelCount = 0;
        for(int i = 0; i < y; i++){
            pixelCount += countInRow[i];
        }
        // now add the number of pixels to get to that location in the row
        int leftover = x;
        leftover -= numSpacersPerSide; // removes the excess amount
        leftover = leftover / 2; // divides by two due to the dropping
        leftover++; // add one since we are selecting the "next" pixel
        
        return leftover + pixelCount;
    }else if(((y % 2) > 0) && ((x % 2) > 0)){  // this means that it's an odd row, so all even pixels are dropped in the row
        // count up the number of pixels in the rows prior
        int pixelCount = 0;
        for(int i = 0; i < y; i++){
            pixelCount += countInRow[i];
        }
        // now add the number of pixels to get to that location in the row
        int leftover = x;
        leftover -= numSpacersPerSide; // removes the excess amount
        leftover = leftover / 2; // divides by two due to the dropping
        leftover++; // add one since we are selecting the "next" pixel
        return countInRow[y] - leftover + pixelCount;
    }else{ // it's a dropped pixel so forget it
        return 11*15-1;
    }
}

void toggleAnimation(){
    
    animationIndex++;
    if(animationIndex > 2)
    {
        animationIndex = 0;
    }
    strip.clear();
    animationToggled = true;
}

void connect() {
  strip.setBrightness(30);
  if (Spark.connected() == false) {
    Spark.connect();
  }
}
