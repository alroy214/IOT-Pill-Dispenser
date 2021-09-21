/*
 "Pill Dispenser Monitor - Part 1 - Touch Monitor"

  The application is used to for people with special needs like elderly people who live in a assited living facility.
  It helps them take take the correct pill of the day and notifies the family when:
    - When the temperature of pill dispenser is too hot and needs to be shaded.
    - When the elder takes the wrong pill.
    - When the elder takes the right pill.
    - When the day ends, it sends an update if the right pill was not taken.
  
  The circuit:
  Input  - temperature (thermometer), touch (capacity from pins).
  Output - speaker (speech), LED onboard Neopixels to tell the status
           and via Blynk and Integromat Android and Telegram messages.

  Video link: https://youtu.be/kLSAgJ6cDpY.
  Created By:
  Eitan Alroy 316486497
  Lior Baruch 327156998
*/

#define BLYNK_PRINT SerialUSB
#define BLYNK_DEBUG SerialUSB
#define EspSerial Serial1
#define ESP8266_BAUD 115200

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <Adafruit_CircuitPlayground.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

char auth[] = "insert Blynk authentication here";
char ssid[] = "insert Wifi SSID here";
char pass[] = "insert Wifi password here";


// Constants used for speach during high temperature
const uint8_t spDANGER[]            PROGMEM = {0x2D,0xBF,0x21,0x92,0x59,0xB4,0x9F,0xA2,0x87,0x10,0x8E,0xDC,0x72,0xAB,0x5B,0x9D,0x62,0xA6,0x42,0x9E,0x9C,0xB8,0xB3,0x95,0x0D,0xAF,0x14,0x15,0xA5,0x47,0xDE,0x1D,0x7A,0x78,0x3A,0x49,0x65,0x55,0xD0,0x5E,0xAE,0x3A,0xB5,0x53,0x93,0x88,0x65,0xE2,0x00,0xEC,0x9A,0xEA,0x80,0x65,0x82,0xC7,0xD8,0x63,0x0A,0x9A,0x65,0x5D,0x53,0xC9,0x49,0x5C,0xE1,0x7D,0x2F,0x73,0x2F,0x47,0x59,0xC2,0xDE,0x9A,0x27,0x5F,0xF1,0x8B,0xDF,0xFF,0x03};
const uint8_t spTEMPERATURE[]       PROGMEM = {0x0A,0x38,0xDE,0x32,0x00,0x2F,0xBB,0x37,0xBF,0x59,0x57,0x76,0x6F,0xB8,0xB2,0x16,0xCA,0xC4,0x75,0xCB,0x4A,0xAB,0x4A,0xF3,0xF6,0xCD,0x2B,0x2D,0x66,0x94,0xD7,0xBA,0xB4,0x34,0x79,0x93,0x52,0x97,0x16,0xB2,0x28,0x5B,0x4D,0x43,0x36,0x10,0x20,0xAB,0xB2,0x52,0xC4,0x26,0x9A,0x26,0x49,0x47,0x9B,0x1B,0xA5,0xA6,0x74,0x5D,0x43,0xF1,0x65,0x14,0x91,0xAD,0xED,0xB5,0x99,0xB1,0x69,0x1A,0x20,0xC0,0x0A,0x84,0x0E,0xD8,0xD3,0x23,0x01,0xA3,0x4C,0x1A,0xA0,0xF5,0xC9,0xD6,0x95,0xE0,0x24,0x1D,0xD9,0x5A,0x9B,0x9C,0x8B,0xAE,0x79,0x2B,0x43,0xAC,0xA6,0xDE,0x9C,0x35,0x9D,0xB1,0xB3,0x47,0x52,0xD7,0x74,0xC6,0x2E,0x52,0xA1,0x5E,0xC2,0x1D,0x3B,0xEB,0xB8,0x65,0x0D,0x5F,0xAA,0x26,0xB6,0xE2,0x35,0x7C,0xA9,0x2A,0xFB,0x6A,0x16,0xF7,0xE7,0x9E,0x4C,0xEB,0xD9,0xFE,0x1F};
const uint8_t spTOO[]               PROGMEM = {0x02,0xD8,0x51,0x3C,0x00,0xC7,0x7A,0x18,0x20,0x85,0xE2,0xE5,0x16,0x61,0x45,0x65,0xD9,0x6F,0xBC,0xE3,0x99,0xB4,0x34,0x51,0x6B,0x49,0xC9,0xDE,0xAB,0x56,0x3B,0x11,0xA9,0x2E,0xD9,0x73,0xEB,0x7A,0x69,0x2A,0xCD,0xB5,0x9B,0x1A,0x58,0x2A,0x73,0xF3,0xCD,0x6A,0x90,0x62,0x8A,0xD3,0xD3,0xAA,0x41,0xF1,0x4E,0x77,0x75,0xF2};
const uint8_t spHIGH[]              PROGMEM = {0x28,0x5A,0x1C,0x01,0x45,0x7B,0x94,0x32,0x87,0xB0,0x32,0x8E,0xB2,0xF2,0xE4,0xC6,0x33,0x65,0xF6,0xCA,0x53,0x68,0x8F,0xD4,0xC5,0xAB,0x48,0xA9,0x22,0xDD,0x17,0xAD,0x32,0xD5,0xF4,0x74,0x5F,0xBC,0xBA,0x1C,0xCB,0x32,0x74,0xF1,0x1A,0x8A,0x6F,0x8B,0xD4,0xC5,0x6B,0xAD,0xAE,0xCD,0x4B,0x17,0xAF,0xA3,0xBA,0x56,0x2F,0x7D,0x34,0xEE,0xEA,0x5A,0xAC,0x6D,0x51,0x7B,0x9A,0x6D,0xF1,0xD2,0x47,0xFD,0x2D,0x6F,0xD1,0x21,0x33,0xF9,0x30,0x7D,0x2D,0x88,0x74,0xD4,0xE4,0xF4,0x57,0xAF,0x34,0xD5,0x93,0xD2,0xDF,0x82,0x61,0x76,0xCF,0xFE,0x7F};
const uint8_t spTHAT_IS_INCORRECT[] PROGMEM = {0xA5,0x2B,0x51,0x25,0xC8,0x7C,0x9F,0x2A,0x78,0xCD,0xD0,0x7A,0x7A,0x0A,0x1F,0x2D,0xDD,0x7B,0xEE,0x29,0x6C,0x48,0xCF,0xE8,0x29,0xA7,0x30,0x21,0x3D,0x63,0xCA,0x0A,0x40,0xB8,0xA8,0x15,0x67,0x6F,0xEA,0xEA,0x6B,0x4E,0x93,0x42,0x88,0xAA,0xBF,0x3D,0x6D,0x49,0x2E,0x26,0xFE,0x76,0x75,0x39,0x9A,0xA8,0xC6,0xD7,0xD1,0x97,0x6C,0x2A,0x6A,0x7B,0xDA,0x90,0xA3,0x29,0x8B,0x6D,0x6E,0x43,0x49,0xA6,0x4A,0xDC,0xAF,0xF5,0xC5,0x99,0x38,0xF3,0xE7,0xD2,0x17,0x19,0xE2,0x2C,0xBD,0x15,0x70,0x04,0xB1,0x06,0x0C,0xF0,0xB3,0x72,0x00,0x06,0x63,0x63,0x00,0x37,0xD6,0x02,0xD0,0x81,0x67,0x15,0xB9,0x98,0xAA,0xD6,0xE2,0x9B,0x9C,0x38,0x47,0xD3,0xE0,0x6E,0xB2,0xA2,0x64,0x9C,0x8A,0x62,0x76,0x89,0xA2,0x87,0xF1,0x74,0x35,0x25,0xB2,0x52,0xDC,0x27,0x6C,0xAB,0x88,0x19,0xCF,0x37,0x77,0x02,0x01,0x88,0xD9,0x3A,0x44,0xBA,0x72,0x5B,0x52,0xAA,0x1A,0xA5,0x48,0x2E,0x4E,0x4F,0xEA,0x62,0x62,0xB6,0x24,0x32,0x55,0xAB,0x89,0xF9,0xA6,0x2A,0x17,0xB7,0x25,0xD2,0x93,0x26,0xD4,0x73,0x8F,0x50,0xE7,0xF4,0x92,0x6C,0xB6,0x22,0xEF,0xCA,0x4B,0x6A,0xF2,0x8A,0x7D,0xF4,0x0A,0xFD,0x58,0xA7,0xF1,0xD1,0xB2,0x6C,0x6B,0xEF,0xBE,0x4F,0x75,0x06,0x00,0x00,0x03,0x4C,0x27,0x2A,0x80,0x61,0xD4,0x18,0x50,0xB4,0xF2,0x03,0x00,0x00};
const uint8_t spTHAT_IS_RIGHT[]     PROGMEM = {0xA6,0xD1,0xC6,0x25,0x68,0x52,0x97,0xCE,0x5B,0x8A,0xE0,0xE8,0x74,0xBA,0x12,0x55,0x82,0xCC,0xF7,0xA9,0x7C,0xB4,0x74,0xEF,0xB9,0xB7,0xB8,0xC5,0x2D,0x14,0x20,0x83,0x07,0x07,0x5A,0x1D,0xB3,0x06,0x5A,0xCE,0x5B,0x4D,0x88,0xA6,0x6C,0x7E,0xA7,0xB5,0x51,0x9B,0xB1,0xE9,0xE6,0x00,0x04,0xAA,0xEC,0x01,0x0B,0x00,0xAA,0x64,0x35,0xF7,0x42,0xAF,0xB9,0x68,0xB9,0x1A,0x96,0x19,0xD0,0x65,0xE4,0x6A,0x56,0x79,0xC0,0xEA,0x93,0x39,0x5F,0x53,0xE4,0x8D,0x6F,0x76,0x8B,0x53,0x97,0x6A,0x66,0x9C,0xB5,0xFB,0xD8,0xA7,0x3A,0x87,0xB9,0x54,0x31,0xD1,0x6A,0x04,0x00,0x06,0xC8,0x36,0xC2,0x00,0x3B,0xB0,0xC0,0x03,0x00,0x00};

// Timers for correct flow of information to the server
int blynkLoopInterval = 1000;
int integromantLoopInterval = 30000;

int previousMillisBlynkLoopDelay = 0;
int previousMillisIntegromantLoopDelay = 0;

//Virtual Pins used in Blynk app
//V0 - Post (Current Temperature, Temperature Status), V1 - Temperature Value Blynk
//V2 - Post (Right Pill, Wrong Pill, End Day), V3 - Receive Wrong Pill, V4 - Reset Pills, V5 - Pill Status
byte capsensePins[] = {A0, A1, A2, A3, A4, A5};
int touchIndication = 650;

//avg
double avgTouch [] = {0, 0, 0, 0, 0, 0};
double avgTemperature = 0;

//max
double maxTouch [] = {0, 0, 0, 0, 0, 0};
double maxTemperature = 0;

//pills
bool tookRightPill = false;
bool tookWrongPill = false;
bool notifiedRightPill = false;
bool notifiedWrongPill = false;

//temperature
bool tempOverheat = false;
int overheatTemperature = 40;

//time
int currWeekday = 0;

// Wifi and widget inits
ESP8266 wifi(&EspSerial);
WidgetRTC rtc;
WidgetLED led(V5);

void setup() {
  CircuitPlayground.begin();
  SerialUSB.begin(9600);
  EspSerial.begin(ESP8266_BAUD);
  delay(10);
  Blynk.begin(auth, wifi, ssid, pass);
  led.on();
  led.setValue(255); // Indecates in Blynk app default color
  setPixelsColors(); // Starts in the default colors
}

void loop() {
  Blynk.run();
  
  updateMax(); // updated the max values of the loop
  
  unsigned long currentMillis = millis();

  // Waits for a dedicated time for a Blynk loop
  if((currentMillis - previousMillisBlynkLoopDelay) > blynkLoopInterval){
    avgTemperature = 0.8 * avgTemperature + 0.2 * CircuitPlayground.temperature(); // updates avg temp
    Blynk.virtualWrite(V1, avgTemperature); // sends to Blynk temp gauge
    updateAvgTouched(); // updates the touch cap avgs
    previousMillisBlynkLoopDelay = currentMillis;
  }

  // Waits for a dedicated time for a Integromant loop
  if(currentMillis - previousMillisIntegromantLoopDelay > integromantLoopInterval){
    checkStatus();
    resetMaxToucedArray();
    maxTemperature = 0;
    previousMillisIntegromantLoopDelay = currentMillis;
  }
  delay(100);
}

// Checks the current touch or heat status and updates the weekday
void checkStatus(){
  if(currWeekday != weekday()) { // check if day changed
      if(currWeekday != 0) {
        dayChanged();
      }
      currWeekday = weekday();
  }
  checkOverheating();
  checkTouched();
}

// Checks the temp and if too high notifies the integromant
void checkOverheating() {
  if(!tempOverheat && maxTemperature > overheatTemperature){ // Temp Overheat
    Blynk.virtualWrite(V0, maxTemperature, true);
    tempOverheat = true;
    setPixelsColors();
  }
  if(tempOverheat && maxTemperature <= overheatTemperature) { // Temp Cooled
    tempOverheat = false;
    setPixelsColors();
  }
}

// If the day has changed and right pill wasn't taken, sends message to integromant
void dayChanged(){
   if(!tookRightPill) {
      Blynk.virtualWrite(V2, false, tookWrongPill, true);
   }
   resetValues();
}

// Resets all pill flags
void resetValues() {
  tookRightPill = false;
  tookWrongPill = false;
  notifiedRightPill = false;
  notifiedWrongPill = false;
}

// Checks touch cap levels
void checkTouched(){
  Serial.print(currWeekday);
  for(int i = 0; i < sizeof(maxTouch)/sizeof(maxTouch[0]); i++) { // loops through all available cap pins
    if(maxTouch[i] > touchIndication){
        if((i + 1) == currWeekday && !notifiedRightPill){
            tookRightPill = true; // check duplication
            Blynk.virtualWrite(V2, tookRightPill, tookWrongPill, false);
            notifiedRightPill = true;
            setPixelsColors();
            break;
        } else if((i + 1) != currWeekday && !notifiedWrongPill) {
            tookWrongPill = true;
            Blynk.virtualWrite(V2, tookRightPill, tookWrongPill, false);
            notifiedWrongPill = true;
            setPixelsColors();
            break;
        }
     }
  }
}

int updateAvgTouched(){
  for(int i = 0; i < min(sizeof(avgTouch)/sizeof(avgTouch[0]),sizeof(capsensePins)/sizeof(capsensePins[0])); i++) { // loops through all available cap pins
    avgTouch[i] = avgTouch[i] * 0.8 + 0.2 * CircuitPlayground.readCap(capsensePins[i]); // updates the average capacity for pins
  }
}

void updateMax(){
  updateMaxTouched();
  maxTemperature = max(maxTemperature, avgTemperature);
}

void resetMaxToucedArray() {
  for(int i = 0; i < sizeof(maxTouch)/sizeof(maxTouch[0]); i++) { // loops through all available cap pins
    maxTouch[i] = 0;
  }
}

void updateMaxTouched(){
  for(int i = 0; i < sizeof(maxTouch)/sizeof(maxTouch[0]); i++) { // loops through all available cap pins
     maxTouch[i] = max(maxTouch[i], avgTouch[i]);
  }
}

// Sets the coorect colors on the Neopixels based on flags
void setPixelsColors() {
  if(tempOverheat) { // device is in hot temp
    setAllPixelsColors(255, 0, 0); // Sets colors to red
    Blynk.setProperty(V5, "color", "#FF0000");
    CircuitPlayground.speaker.say(spDANGER);
    CircuitPlayground.speaker.say(spTEMPERATURE);
    CircuitPlayground.speaker.say(spTOO);
    CircuitPlayground.speaker.say(spHIGH);
  } else if(tookWrongPill) {
      setAllPixelsColors(255, 255, 0); // Sets colors to yellow
      Blynk.setProperty(V5, "color", "#FFFF00");
      CircuitPlayground.speaker.say(spTHAT_IS_INCORRECT);
  } else if (tookRightPill) {
      setAllPixelsColors(0, 255, 0); // Sets colors to green
      Blynk.setProperty(V5, "color", "#00FF00");
      CircuitPlayground.speaker.say(spTHAT_IS_RIGHT);
  } else {
      setAllPixelsColors(0, 0, 255); // Sets colors to blue
      Blynk.setProperty(V5, "color", "#0000FF");
  }
}

void setAllPixelsColors(int red, int green , int blue) {
  for (int i = 0; i < 10; ++i) { 
    CircuitPlayground.setPixelColor(i, red, green, blue);
  }
}

BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
}

BLYNK_WRITE(V3) {
 tookWrongPill = param.asInt();
 notifiedWrongPill = param.asInt();
 setPixelsColors();
}

BLYNK_WRITE(V4) {
 resetValues();
 setPixelsColors();
}