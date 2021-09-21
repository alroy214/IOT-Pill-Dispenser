/*
 "Pill Dispenser Monitor - Part 2 - Lights and LEDS manager"

  The application is used to for people with special needs like elderly people who live in a assited living facility.
  It helps them take take the correct pill of the day and notifies the family when:
    - When the temperature of pill dispenser is too hot and needs to be shaded.
    - When the elder takes the wrong pill.
    - When the elder takes the right pill.
    - When the day ends, it sends an update if the right pill was not taken.
  
  The circuit:
  Output - LED onboard Neopixels to indicate the current day
           LED off board to indicate the correct pill cell

  Video link: https://youtu.be/kLSAgJ6cDpY.
  Created By:
  Eitan Alroy 316486497
  Lior Baruch 327156998
*/
#define BLYNK_PRINT SerialUSB
#define BLYNK_DEBUG SerialUSB

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <Adafruit_CircuitPlayground.h>
#include <TimeLib.h>
#include <WidgetRTC.h>


// Please change values to your own!!
char auth[] = "insert Blynk authentication here";
char ssid[] = "insert Wifi SSID here";
char pass[] = "insert Wifi password here";

#define EspSerial Serial1
#define ESP8266_BAUD 115200

byte ledPins[] = {A0, A1, A2, A3, A4, A5, A0};
WidgetRTC rtc;

ESP8266 wifi(&EspSerial);

//time
int currWeekday = 0;

void setup() {
  CircuitPlayground.begin();
  SerialUSB.begin(9600);
  EspSerial.begin(ESP8266_BAUD);
  delay(10);
  pinOutput();
  Blynk.begin(auth, wifi, ssid, pass);
  delay(100);
}

// Changes pins to output voltge
void pinOutput() {
  for(int i = 0; i < sizeof(ledPins)/sizeof(ledPins[0]); i++) {
    pinMode(ledPins[i],OUTPUT);  
  }
}

BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
}

void loop() {
  Blynk.run();
  checkDayChanged(); 
}

// Checks if the day has changed
void checkDayChanged(){
  if(currWeekday != weekday()) {
      dayChanged();
      currWeekday = weekday();
  }
}

// If the day has changed moves turns the correct LED on and the other off
void dayChanged(){
  if(currWeekday != 0) {
    digitalWrite(ledPins[currWeekday-1], LOW);
  }
  digitalWrite(ledPins[weekday()-1], HIGH);
  setPixels();
}

// Sets the Neopixels to the number of the current day
void setPixels() {
  CircuitPlayground.clearPixels();
  for (int i = 0; i < weekday(); ++i) { 
    CircuitPlayground.setPixelColor(i, 128, 128, 128);
  }
}
