#include <Arduino.h>

#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"

#define SFC_LATCH   6
#define SFC_CLOCK   5
#define SFC_DATA    4

// Initialize the OLED display using Arduino Wire:
SSD1306Wire display(0x3c, 20, 21);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h e.g. https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h

int latchCnt = 0;
bool latch = false;

void setupPins(){
  pinMode(SFC_LATCH, INPUT);
  pinMode(SFC_CLOCK, INPUT);
  pinMode(SFC_DATA, OUTPUT);

  Serial.println("pin setup Complete.");
}

void drawSignal(){
  if(digitalRead(SFC_LATCH) == HIGH){
    delayMicroseconds(16670);   //16.67ms待機
    latchCnt++;
    if(latchCnt > 60){
      latchCnt = 0;
      latch = !latch;
      Serial.println("Value: " + String(latch));

      display.clear();
      display.drawString(0, 0, "Hello world");
      display.drawString(0, 32, "Value: " + String(latch));
      display.display();
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  Serial.println();
  Serial.println("wake.");

  // Initialising the UI will init the display too.
  display.init();

  //display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);

  display.drawString(0, 0, "Hello world");
  display.display();

  // put your setup code here, to run once:
  setupPins();
}

void loop() {
  // put your main code here, to run repeatedly:
  drawSignal();
}