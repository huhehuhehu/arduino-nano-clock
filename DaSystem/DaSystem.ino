/*STILL MISSING:
      BUZZER
      ALARM ACTION

*/

#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Math.h>
#include "RTClib.h"
#include <RTC_DS3231.h>
#include <SPI.h>


#define LED_PIN 3
#define NUMPIXELS 33
#define OFFSET 5
#define OFFSET_D 7
#define MOON 0
#define SUN 1
#define EYE_R 2
#define EYE_T 3
#define EYE_L 4

#define SET_COLOR 'r'

#define MAX_HOUR 24
#define MAX_MINUTE 60.0

#define OFF 0
#define MAX_BRIGHTNESS 220
#define MIN_BRIGHTNESS 20
#define INITIAL_LUM 50
#define MINS_DENOM 360.0

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

RTC_DS3231 RTC;
DateTime now;

int maxLum, preMin;
uint32_t lumR, lumG, lumB;



byte numbers[] = {
//B01234567
  B11101110,    // 0
  B00100010,    // 1
  B11010110,    // 2     5555
  B01110110,    // 3   4     6
  B00111010,    // 4     3333
  B01111100,    // 5   0     2
  B11111100,    // 6     1111
  B00100110,    // 7
  B11111110,    // 8
  B01111110    // 9
};


void setup() {
  preMin = 61;
  Wire.begin();
  RTC.begin();
  
  //RTC.adjust(DateTime(__DATE__,__TIME__));
  
  initialiseLED();

  //  Serial.begin(9600);
}


void initialiseLED() {
  maxLum = INITIAL_LUM;

  strip.begin();
  initialiseEYES();
  strip.show();
}

void loop() {
  now = RTC.now();
  if(preMin != now.minute()){
    displayClock(now.hour(), now.minute());
    strip.show();
  }
}

void resetClockLED() {
  for (int i = 5; i < OFFSET + 4 * OFFSET_D; i++) {
    strip.setPixelColor(i, OFF);
  }
}

void initialiseEYES(){
  strip.setPixelColor(EYE_L, getC('r'));
  strip.setPixelColor(EYE_R, getC('r'));
  strip.setPixelColor(EYE_T, getC('y'));
}

uint32_t getC(char a) {
  switch (a) {
    case 'w':
      return strip.Color(maxLum, maxLum, maxLum);
    case 'y':
      return strip.Color(maxLum, maxLum, OFF);
    case 'b':
      return strip.Color(OFF, maxLum / 2, maxLum);
    case 't':
      return strip.Color(OFF, maxLum, maxLum);
    case 'r':
      return strip.Color(maxLum, OFF, OFF);
    case 'p':
      return strip.Color(maxLum, OFF, maxLum / 2);
    case 'o':
      return strip.Color(maxLum, maxLum * 0.666, maxLum / 5);
    case 's':
      return strip.Color(lumR, lumG, lumB);
    case '0':
    default:
      return strip.Color(OFF, OFF, OFF);

  }
}


//Function for displaying settings value
void displayClock(int hour, int minute) {

  if(hour >= 18)maxLum = (maxLum > MIN_BRIGHTNESS) ? maxLum-10 : MIN_BRIGHTNESS;
  else if(hour >= 5) maxLum = (maxLum < MAX_BRIGHTNESS) ? maxLum+10 : MAX_BRIGHTNESS;

  if (hour < 6 || hour > 18) {
    strip.setPixelColor(0, getC('p'));
    strip.setPixelColor(1, OFF);
  } else{
    strip.setPixelColor(0, getC('o'));
    strip.setPixelColor(1, getC('o'));
  }

//  if (hour < 6) {
//    lumR = 0;
//    lumG = (hour * MAX_MINUTE + (float)minute) / MINS_DENOM * maxLum;
//    lumB = maxLum;
//    strip.setPixelColor(0, getC('p'));
//    strip.setPixelColor(1, OFF);
//  } else if (hour < 12) {
//    lumR = 0;
//    lumG = maxLum;
//    lumB = maxLum - ((hour - 6) * MAX_MINUTE + (float)minute) / MINS_DENOM * maxLum;
//    strip.setPixelColor(0, getC('o'));
//    strip.setPixelColor(1, getC('o'));
//  } else if (hour < 18) {
//    lumR = ((hour - 12) * MAX_MINUTE + (float)minute) / MINS_DENOM * maxLum;
//    lumG = maxLum - lumR;
//    lumB = 0;
//    strip.setPixelColor(0, getC('o'));
//    strip.setPixelColor(1, getC('o'));
//  } else {
//    lumG = 0;
//    lumB = ((hour - 18) * MAX_MINUTE + (float)minute) / MINS_DENOM * maxLum;
//    lumR = maxLum - lumB;
//    strip.setPixelColor(0, getC('p'));
//    strip.setPixelColor(1, OFF);
//  }
  
  uint8_t hourMSB = (((hour % 12) < 10) && hour != 12) ? 0 : 1;;
  uint8_t hourLSB = (hourMSB == 0)? hour % 12 : hour % 12 - 10;
  if(hour == 12) hourLSB = 2;
  uint8_t minuteMSB = minute / 10;
  uint8_t minuteLSB = minute % 10;

  for (int i = 0; i < 7; i++) {
    if (numbers[hourMSB] & (1 << 7 - i)) strip.setPixelColor(i + OFFSET, getC(SET_COLOR));
    else {
      strip.setPixelColor(i + OFFSET, OFF);
    }
  }
  // second digit (least significant)
   for (int i = 0; i < 7; i++) {
    if (numbers[hourLSB] & (1 << 7 - i)) {
      strip.setPixelColor(i + OFFSET + OFFSET_D, getC(SET_COLOR));
    }
    else {
      strip.setPixelColor(i + OFFSET + OFFSET_D, OFF);
    }
  }
  
  for (int i = 0; i < 7; i++) {
    if (numbers[minuteMSB] & (1 << 7 - i)) {
      strip.setPixelColor(i + OFFSET + 2 * OFFSET_D, getC(SET_COLOR));
    }
    else {
      strip.setPixelColor(i + OFFSET + 2 *  OFFSET_D, OFF);
    }
  }
  
  for (int i = 0; i < 7; i++) {
    if (numbers[minuteLSB] & (1 << 7 - i)) {
      strip.setPixelColor(i + OFFSET + 3 * OFFSET_D, getC(SET_COLOR));
    }
    else {
      strip.setPixelColor(i + OFFSET + 3 * OFFSET_D, OFF);
    }
  }
  preMin = minute;
}


int modu(int x, int y) {
  return (x % y < 0) ? y + x % y : x % y;
}

