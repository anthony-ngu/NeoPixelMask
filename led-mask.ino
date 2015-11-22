/*
 * This is the code for the LED Mask showcased through: https://www.youtube.com/watch?v=KEPldnI0fgk
 */

#include "application.h"
#include "neopixel/neopixel.h"

// SYSTEM_MODE(AUTOMATIC);
SYSTEM_MODE(SEMI_AUTOMATIC);

#define PATTERN_INDEX_MAX 9
#define DEBOUNCE_DELAY 300

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

int patternIndex = 0; // This is the index to which pattern to play
bool patternToggled = true; // Every function must check this between frames in order to allow for a quick response to the button
unsigned long lastDebounceTime;

void setup() 
{
    attachInterrupt(TOGGLE_PIN, togglePattern, FALLING); // This will allow you to create a button that triggers the patternChange
    attachInterrupt(WIFI_PIN, connect, CHANGE); // This will allow you to create switch that triggers the WiFi connection call
  
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    strip.setBrightness(10);
}

void loop() 
{
    // Unfortunately, particle's build doesn't currently allow for function pointers,
    // so we'll have to do a simple switch case
    switch(patternIndex)
    {
        case 0:
            rainbow(20);
            break;
        case 1:
            bucktoothJack();
            break;
        case 2:
            subzero();
            break;
        case 3:
            winter(200);
            break;
        case 4:
            rainyWeather(200);
            break;
        case 5:
            heartAnimation(200);
            break;
        case 6:
            smile();
            break;
        case 7:
            hi();
            break;
        case 8:
            bye();
            break;
        case 9:
            rainbowMask();
            break;
        default:
            rainbow(20);
            break;
    }
    
    if(patternToggled){
        patternToggled = false;
    }
}

// Pretends that it is a 30 by 15 Matrix
uint16_t customMappingFunction (uint16_t x, uint16_t y){
    if(x > 28 || y > 10)
    {
        return 11*15-1;
    }
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
    if(((y % 2) == 0) && ((x % 2) == 0)) // this means that it's an even row, so all odd pixels are dropped in the row.
    {
        // count up the number of pixels in the rows prior
        int pixelCount = 0;
        for(int i = 0; i < y; i++){
            pixelCount += countInRow[i];
        }
        // now add the number of pixels to get to that location in the row
        int leftover = x;
        leftover -= numSpacersPerSide; // removes the excess amount
        if(leftover < 0) // Also, drop the spacers on the left side
        {
            return 11*15-1;
        }
        
        leftover = leftover / 2; // divides by two due to the dropping

        if(leftover >= countInRow[y]){ // Also, drop the spacers on the right side
             return 11*15-1;
        }
        
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
        if(leftover < 0) // Also, drop the spacers on the left side
        {
            return 11*15-1;
        }
        
        leftover = leftover / 2; // divides by two due to the dropping
        
        if(leftover >= countInRow[y]){ // Also, drop the spacers on the right side
             return 11*15-1;
        }
        
        return countInRow[y] - leftover + pixelCount-1;
    }else{ // it's a dropped pixel so forget it
        return 11*15-1;
    }
}

void togglePattern(){
    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) //if current time minus the last trigger time is greater than
    {                                                  //the delay (debounce) time, button is completley closed.
        lastDebounceTime = millis();

        //switch was pressed, do whatever you need to here
        patternIndex++;
        if(patternIndex > PATTERN_INDEX_MAX)
        {
            patternIndex = 0;
        }
        patternToggled = true;
    }
}

void connect() {
    strip.setBrightness(30);
    if (Spark.connected() == false) {
        Spark.connect();
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

void rainbow(uint8_t wait) {
    uint16_t i, j;
    
    for(j=0; j<256 && !patternToggled; j++) {
        for(i=0; i<strip.numPixels(); i++) {
            strip.setPixelColor(i, Wheel((i+j) & 255));
        }
        strip.show();
        delay(wait);
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

void heartAnimation(uint8_t wait){
    uint16_t i;
    for(i=0; i<20 && !patternToggled; i++) {
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
    strip.clear();
    
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
    strip.clear();
    
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
    strip.clear();
    
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

void smile(){
    strip.clear();
    fillFrom(0, 66, strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(0, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(2, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(4, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(6, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(8, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(10, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(12, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(14, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(16, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(18, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(20, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(22, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(24, 0), strip.Color(254, 254, 254));
    strip.setPixelColor(customMappingFunction(26, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(28, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(1, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(3, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(5, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(7, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(9, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(11, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(13, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(15, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(17, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(19, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(21, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(23, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(25, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(27, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(0, 2), strip.Color(254, 254, 254));
    strip.setPixelColor(customMappingFunction(2, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(4, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(6, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(8, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(10, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(12, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(14, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(16, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(18, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(20, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(22, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(24, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(26, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(28, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(1, 3), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(3, 3), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(5, 3), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(7, 3), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(9, 3), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(11, 3), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(13, 3), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(15, 3), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(17, 3), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(19, 3), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(21, 3), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(23, 3), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(25, 3), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(27, 3), strip.Color(253, 253, 253));
    strip.setPixelColor(customMappingFunction(0, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(2, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(4, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(6, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(8, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(10, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(12, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(14, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(16, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(18, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(20, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(22, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(24, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(26, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(28, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(1, 5), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(3, 5), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(5, 5), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(7, 5), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(9, 5), strip.Color(0, 0, 0));
    strip.setPixelColor(customMappingFunction(11, 5), strip.Color(0, 0, 0));
    strip.setPixelColor(customMappingFunction(13, 5), strip.Color(0, 0, 0));
    strip.setPixelColor(customMappingFunction(15, 5), strip.Color(0, 0, 0));
    strip.setPixelColor(customMappingFunction(17, 5), strip.Color(0, 0, 0));
    strip.setPixelColor(customMappingFunction(19, 5), strip.Color(0, 0, 0));
    strip.setPixelColor(customMappingFunction(21, 5), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(23, 5), strip.Color(254, 254, 254));
    strip.setPixelColor(customMappingFunction(25, 5), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(27, 5), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(0, 6), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(2, 6), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(4, 6), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(6, 6), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(8, 6), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(10, 6), strip.Color(0, 0, 0));
    strip.setPixelColor(customMappingFunction(12, 6), strip.Color(0, 0, 0));
    strip.setPixelColor(customMappingFunction(14, 6), strip.Color(0, 0, 0));
    strip.setPixelColor(customMappingFunction(16, 6), strip.Color(0, 0, 0));
    strip.setPixelColor(customMappingFunction(18, 6), strip.Color(0, 0, 0));
    strip.setPixelColor(customMappingFunction(20, 6), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(22, 6), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(24, 6), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(26, 6), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(28, 6), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(1, 7), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(3, 7), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(5, 7), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(7, 7), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(9, 7), strip.Color(0, 0, 0));
    strip.setPixelColor(customMappingFunction(11, 7), strip.Color(26, 5, 6));
    strip.setPixelColor(customMappingFunction(13, 7), strip.Color(0, 0, 0));
    strip.setPixelColor(customMappingFunction(15, 7), strip.Color(14, 5, 5));
    strip.setPixelColor(customMappingFunction(17, 7), strip.Color(14, 3, 3));
    strip.setPixelColor(customMappingFunction(19, 7), strip.Color(0, 0, 0));
    strip.setPixelColor(customMappingFunction(21, 7), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(23, 7), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(25, 7), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(27, 7), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(0, 8), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(2, 8), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(4, 8), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(6, 8), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(8, 8), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(10, 8), strip.Color(255, 2, 2));
    strip.setPixelColor(customMappingFunction(12, 8), strip.Color(231, 2, 2));
    strip.setPixelColor(customMappingFunction(14, 8), strip.Color(231, 2, 2));
    strip.setPixelColor(customMappingFunction(16, 8), strip.Color(253, 2, 1));
    strip.setPixelColor(customMappingFunction(18, 8), strip.Color(231, 2, 2));
    strip.setPixelColor(customMappingFunction(20, 8), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(22, 8), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(24, 8), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(26, 8), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(28, 8), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(1, 9), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(3, 9), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(5, 9), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(7, 9), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(9, 9), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(11, 9), strip.Color(231, 2, 2));
    strip.setPixelColor(customMappingFunction(13, 9), strip.Color(231, 2, 2));
    strip.setPixelColor(customMappingFunction(15, 9), strip.Color(231, 2, 2));
    strip.setPixelColor(customMappingFunction(17, 9), strip.Color(231, 2, 2));
    strip.setPixelColor(customMappingFunction(19, 9), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(21, 9), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(23, 9), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(25, 9), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(27, 9), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(0, 10), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(2, 10), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(4, 10), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(6, 10), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(8, 10), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(10, 10), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(12, 10), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(14, 10), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(16, 10), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(18, 10), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(20, 10), strip.Color(254, 255, 255));
    strip.setPixelColor(customMappingFunction(22, 10), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(24, 10), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(26, 10), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(28, 10), strip.Color(255, 255, 255));
    strip.show();
}

void hi(){
    strip.clear();
    strip.setPixelColor(customMappingFunction(9, 1), strip.Color(255, 108, 3));
    strip.setPixelColor(customMappingFunction(13, 1), strip.Color(255, 108, 3));
    strip.setPixelColor(customMappingFunction(17, 1), strip.Color(255, 108, 3));
    strip.setPixelColor(customMappingFunction(16, 2), strip.Color(255, 108, 3));
    strip.setPixelColor(customMappingFunction(18, 2), strip.Color(255, 108, 3));
    strip.setPixelColor(customMappingFunction(9, 3), strip.Color(255, 108, 3));
    strip.setPixelColor(customMappingFunction(13, 3), strip.Color(255, 108, 3));
    strip.setPixelColor(customMappingFunction(17, 3), strip.Color(255, 108, 3));
    strip.setPixelColor(customMappingFunction(10, 4), strip.Color(255, 108, 3));
    strip.setPixelColor(customMappingFunction(12, 4), strip.Color(255, 108, 3));
    strip.setPixelColor(customMappingFunction(9, 5), strip.Color(255, 108, 3));
    strip.setPixelColor(customMappingFunction(13, 5), strip.Color(255, 108, 3));
    strip.setPixelColor(customMappingFunction(17, 5), strip.Color(255, 108, 3));
    strip.setPixelColor(customMappingFunction(9, 7), strip.Color(255, 108, 3));
    strip.setPixelColor(customMappingFunction(13, 7), strip.Color(255, 108, 3));
    strip.setPixelColor(customMappingFunction(17, 7), strip.Color(255, 108, 3));
    strip.show();
}

void bye(){
    strip.clear();
    strip.setPixelColor(customMappingFunction(6, 2), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(8, 2), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(12, 2), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(16, 2), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(18, 2), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(20, 2), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(22, 2), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(9, 3), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(13, 3), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(15, 3), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(6, 4), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(10, 4), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(14, 4), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(18, 4), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(7, 5), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(9, 5), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(19, 5), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(21, 5), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(6, 6), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(10, 6), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(14, 6), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(18, 6), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(9, 7), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(6, 8), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(8, 8), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(14, 8), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(18, 8), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(20, 8), strip.Color(112, 2, 255));
    strip.setPixelColor(customMappingFunction(22, 8), strip.Color(112, 2, 255));
    strip.show();
}

void winter(int delayAmount){
    strip.clear();
    
    for(int i = 0; i<40; i++)
    {
        strip.setPixelColor(random(120), strip.Color(255, 255, 255));
    }
    strip.setPixelColor(customMappingFunction(14, 0), strip.Color(1, 107, 47));
    strip.setPixelColor(customMappingFunction(13, 1), strip.Color(1, 107, 47));
    strip.setPixelColor(customMappingFunction(15, 1), strip.Color(1, 107, 47));
    strip.setPixelColor(customMappingFunction(12, 2), strip.Color(1, 107, 47));
    strip.setPixelColor(customMappingFunction(14, 2), strip.Color(1, 108, 24));
    strip.setPixelColor(customMappingFunction(16, 2), strip.Color(1, 107, 47));
    strip.setPixelColor(customMappingFunction(13, 3), strip.Color(1, 108, 24));
    strip.setPixelColor(customMappingFunction(15, 3), strip.Color(1, 108, 24));
    strip.setPixelColor(customMappingFunction(12, 4), strip.Color(1, 107, 47));
    strip.setPixelColor(customMappingFunction(14, 4), strip.Color(1, 108, 24));
    strip.setPixelColor(customMappingFunction(16, 4), strip.Color(1, 107, 47));
    strip.setPixelColor(customMappingFunction(11, 5), strip.Color(1, 107, 47));
    strip.setPixelColor(customMappingFunction(13, 5), strip.Color(1, 108, 24));
    strip.setPixelColor(customMappingFunction(15, 5), strip.Color(1, 108, 24));
    strip.setPixelColor(customMappingFunction(17, 5), strip.Color(1, 107, 47));
    strip.setPixelColor(customMappingFunction(12, 6), strip.Color(1, 108, 24));
    strip.setPixelColor(customMappingFunction(14, 6), strip.Color(1, 107, 47));
    strip.setPixelColor(customMappingFunction(16, 6), strip.Color(1, 108, 24));
    strip.setPixelColor(customMappingFunction(11, 7), strip.Color(1, 107, 47));
    strip.setPixelColor(customMappingFunction(13, 7), strip.Color(1, 107, 47));
    strip.setPixelColor(customMappingFunction(15, 7), strip.Color(1, 107, 47));
    strip.setPixelColor(customMappingFunction(17, 7), strip.Color(1, 107, 47));
    strip.setPixelColor(customMappingFunction(12, 8), strip.Color(1, 107, 47));
    strip.setPixelColor(customMappingFunction(14, 8), strip.Color(107, 84, 1));
    strip.setPixelColor(customMappingFunction(16, 8), strip.Color(1, 107, 47));
    strip.setPixelColor(customMappingFunction(14, 10), strip.Color(107, 84, 1));
    strip.show();
    delay(delayAmount);
}

void rainyWeather(int delayAmount){
    strip.clear();
    
    for(int i = 0; i<40; i++)
    {
        strip.setPixelColor(random(120), strip.Color(57, 78, 215));
    }
    strip.show();
    delay(delayAmount);
}

void bucktoothJack(){
    strip.clear();
    strip.setPixelColor(customMappingFunction(0, 0), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(2, 0), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(4, 0), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(6, 0), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(8, 0), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(10, 0), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(12, 0), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(14, 0), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(16, 0), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(18, 0), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(20, 0), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(22, 0), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(24, 0), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(26, 0), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(28, 0), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(1, 1), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(3, 1), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(5, 1), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(7, 1), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(9, 1), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(11, 1), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(13, 1), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(15, 1), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(17, 1), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(19, 1), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(21, 1), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(23, 1), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(25, 1), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(27, 1), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(0, 2), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(2, 2), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(4, 2), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(10, 2), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(12, 2), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(16, 2), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(18, 2), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(24, 2), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(26, 2), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(28, 2), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(1, 3), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(3, 3), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(5, 3), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(11, 3), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(17, 3), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(25, 3), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(27, 3), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(0, 4), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(2, 4), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(4, 4), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(10, 4), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(12, 4), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(16, 4), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(18, 4), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(24, 4), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(26, 4), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(28, 4), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(1, 5), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(3, 5), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(5, 5), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(23, 5), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(25, 5), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(27, 5), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(0, 6), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(2, 6), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(4, 6), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(6, 6), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(22, 6), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(24, 6), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(26, 6), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(28, 6), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(1, 7), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(3, 7), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(5, 7), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(7, 7), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(21, 7), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(23, 7), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(25, 7), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(27, 7), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(0, 8), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(2, 8), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(4, 8), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(6, 8), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(8, 8), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(20, 8), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(22, 8), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(24, 8), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(26, 8), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(28, 8), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(1, 9), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(3, 9), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(5, 9), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(7, 9), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(9, 9), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(11, 9), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(13, 9), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(15, 9), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(17, 9), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(19, 9), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(21, 9), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(23, 9), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(25, 9), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(27, 9), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(0, 10), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(2, 10), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(4, 10), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(6, 10), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(8, 10), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(10, 10), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(12, 10), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(14, 10), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(16, 10), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(18, 10), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(20, 10), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(22, 10), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(24, 10), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(26, 10), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(28, 10), strip.Color(255, 116, 3));
    strip.show();
}

void subzero()
{
    strip.clear();
    strip.setPixelColor(customMappingFunction(0, 0), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(2, 0), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(4, 0), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(6, 0), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(8, 0), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(10, 0), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(12, 0), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(16, 0), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(18, 0), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(20, 0), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(22, 0), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(24, 0), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(26, 0), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(28, 0), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(1, 1), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(3, 1), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(5, 1), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(7, 1), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(9, 1), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(11, 1), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(17, 1), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(19, 1), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(21, 1), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(23, 1), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(25, 1), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(27, 1), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(2, 2), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(4, 2), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(6, 2), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(8, 2), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(10, 2), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(12, 2), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(16, 2), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(18, 2), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(20, 2), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(22, 2), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(24, 2), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(26, 2), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(3, 3), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(5, 3), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(7, 3), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(9, 3), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(11, 3), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(17, 3), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(19, 3), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(21, 3), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(23, 3), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(25, 3), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(4, 4), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(8, 4), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(10, 4), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(14, 4), strip.Color(137, 137, 137));
    strip.setPixelColor(customMappingFunction(18, 4), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(20, 4), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(24, 4), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(3, 5), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(5, 5), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(9, 5), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(13, 5), strip.Color(137, 137, 137));
    strip.setPixelColor(customMappingFunction(15, 5), strip.Color(137, 137, 137));
    strip.setPixelColor(customMappingFunction(19, 5), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(23, 5), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(25, 5), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(4, 6), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(6, 6), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(8, 6), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(12, 6), strip.Color(137, 137, 137));
    strip.setPixelColor(customMappingFunction(16, 6), strip.Color(137, 137, 137));
    strip.setPixelColor(customMappingFunction(20, 6), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(22, 6), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(24, 6), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(5, 7), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(7, 7), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(9, 7), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(11, 7), strip.Color(137, 137, 137));
    strip.setPixelColor(customMappingFunction(17, 7), strip.Color(137, 137, 137));
    strip.setPixelColor(customMappingFunction(19, 7), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(21, 7), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(23, 7), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(6, 8), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(8, 8), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(14, 8), strip.Color(137, 137, 137));
    strip.setPixelColor(customMappingFunction(20, 8), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(22, 8), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(7, 9), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(9, 9), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(13, 9), strip.Color(137, 137, 137));
    strip.setPixelColor(customMappingFunction(15, 9), strip.Color(137, 137, 137));
    strip.setPixelColor(customMappingFunction(19, 9), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(21, 9), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(8, 10), strip.Color(4, 102, 214));
    strip.setPixelColor(customMappingFunction(12, 10), strip.Color(137, 137, 137));
    strip.setPixelColor(customMappingFunction(16, 10), strip.Color(137, 137, 137));
    strip.setPixelColor(customMappingFunction(20, 10), strip.Color(4, 102, 214));

    strip.show();
}

void rainbowMask()
{
    strip.clear();
    strip.setPixelColor(customMappingFunction(0, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(2, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(4, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(6, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(8, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(10, 0), strip.Color(3, 3, 255));
    strip.setPixelColor(customMappingFunction(18, 0), strip.Color(3, 3, 255));
    strip.setPixelColor(customMappingFunction(20, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(22, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(24, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(26, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(28, 0), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(1, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(3, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(5, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(7, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(9, 1), strip.Color(3, 255, 28));
    strip.setPixelColor(customMappingFunction(11, 1), strip.Color(104, 3, 255));
    strip.setPixelColor(customMappingFunction(17, 1), strip.Color(104, 3, 255));
    strip.setPixelColor(customMappingFunction(19, 1), strip.Color(3, 255, 28));
    strip.setPixelColor(customMappingFunction(21, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(23, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(25, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(27, 1), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(0, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(2, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(4, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(6, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(8, 2), strip.Color(255, 242, 3));
    strip.setPixelColor(customMappingFunction(10, 2), strip.Color(3, 3, 255));
    strip.setPixelColor(customMappingFunction(12, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(16, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(18, 2), strip.Color(3, 3, 255));
    strip.setPixelColor(customMappingFunction(20, 2), strip.Color(255, 242, 3));
    strip.setPixelColor(customMappingFunction(22, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(24, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(26, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(28, 2), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(1, 3), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(3, 3), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(5, 3), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(7, 3), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(9, 3), strip.Color(3, 255, 28));
    strip.setPixelColor(customMappingFunction(11, 3), strip.Color(104, 3, 255));
    strip.setPixelColor(customMappingFunction(13, 3), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(15, 3), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(17, 3), strip.Color(104, 3, 255));
    strip.setPixelColor(customMappingFunction(19, 3), strip.Color(3, 255, 28));
    strip.setPixelColor(customMappingFunction(21, 3), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(23, 3), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(25, 3), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(27, 3), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(0, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(2, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(4, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(6, 4), strip.Color(255, 3, 3));
    strip.setPixelColor(customMappingFunction(8, 4), strip.Color(255, 242, 3));
    strip.setPixelColor(customMappingFunction(10, 4), strip.Color(3, 3, 255));
    strip.setPixelColor(customMappingFunction(12, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(14, 4), strip.Color(137, 137, 137));
    strip.setPixelColor(customMappingFunction(16, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(18, 4), strip.Color(3, 3, 255));
    strip.setPixelColor(customMappingFunction(20, 4), strip.Color(255, 242, 3));
    strip.setPixelColor(customMappingFunction(22, 4), strip.Color(255, 3, 3));
    strip.setPixelColor(customMappingFunction(24, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(26, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(28, 4), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(1, 5), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(3, 5), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(5, 5), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(7, 5), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(9, 5), strip.Color(3, 255, 28));
    strip.setPixelColor(customMappingFunction(11, 5), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(13, 5), strip.Color(137, 137, 137));
    strip.setPixelColor(customMappingFunction(15, 5), strip.Color(137, 137, 137));
    strip.setPixelColor(customMappingFunction(17, 5), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(19, 5), strip.Color(3, 255, 28));
    strip.setPixelColor(customMappingFunction(21, 5), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(23, 5), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(25, 5), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(27, 5), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(0, 6), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(2, 6), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(4, 6), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(6, 6), strip.Color(255, 3, 3));
    strip.setPixelColor(customMappingFunction(8, 6), strip.Color(255, 242, 3));
    strip.setPixelColor(customMappingFunction(10, 6), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(14, 6), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(18, 6), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(20, 6), strip.Color(255, 242, 3));
    strip.setPixelColor(customMappingFunction(22, 6), strip.Color(255, 3, 3));
    strip.setPixelColor(customMappingFunction(24, 6), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(26, 6), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(28, 6), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(1, 7), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(3, 7), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(5, 7), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(7, 7), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(9, 7), strip.Color(3, 255, 28));
    strip.setPixelColor(customMappingFunction(13, 7), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(15, 7), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(19, 7), strip.Color(3, 255, 28));
    strip.setPixelColor(customMappingFunction(21, 7), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(23, 7), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(25, 7), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(27, 7), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(0, 8), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(2, 8), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(4, 8), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(6, 8), strip.Color(255, 3, 3));
    strip.setPixelColor(customMappingFunction(8, 8), strip.Color(255, 242, 3));
    strip.setPixelColor(customMappingFunction(10, 8), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(12, 8), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(14, 8), strip.Color(137, 137, 137));
    strip.setPixelColor(customMappingFunction(16, 8), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(18, 8), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(20, 8), strip.Color(255, 242, 3));
    strip.setPixelColor(customMappingFunction(22, 8), strip.Color(255, 3, 3));
    strip.setPixelColor(customMappingFunction(24, 8), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(26, 8), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(28, 8), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(1, 9), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(3, 9), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(5, 9), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(7, 9), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(9, 9), strip.Color(3, 255, 28));
    strip.setPixelColor(customMappingFunction(11, 9), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(13, 9), strip.Color(137, 137, 137));
    strip.setPixelColor(customMappingFunction(15, 9), strip.Color(137, 137, 137));
    strip.setPixelColor(customMappingFunction(17, 9), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(19, 9), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(21, 9), strip.Color(255, 116, 3));
    strip.setPixelColor(customMappingFunction(23, 9), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(25, 9), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(27, 9), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(0, 10), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(2, 10), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(4, 10), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(6, 10), strip.Color(255, 3, 3));
    strip.setPixelColor(customMappingFunction(8, 10), strip.Color(255, 242, 3));
    strip.setPixelColor(customMappingFunction(10, 10), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(14, 10), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(18, 10), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(20, 10), strip.Color(255, 242, 3));
    strip.setPixelColor(customMappingFunction(22, 10), strip.Color(255, 3, 3));
    strip.setPixelColor(customMappingFunction(24, 10), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(26, 10), strip.Color(255, 255, 255));
    strip.setPixelColor(customMappingFunction(28, 10), strip.Color(255, 255, 255));
    strip.show();
}