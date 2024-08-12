#include <Arduino.h>

#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"

#define SFC_CLOCK   6
#define SFC_LATCH   5
#define SFC_DATA    4

// Initialize the OLED display using Arduino Wire:
SSD1306Wire display(0x3c, 20, 21);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h e.g. https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h

int latchCnt = 0;
bool latch = false;

uint16_t buttons  = 0b0000010000000000;
uint16_t buttons2 = 0b0000100000000000;
//uint16_t buttons = 0b0010000000000000;
uint16_t buttonsTemp = 0;
bool buttonStat = false;
unsigned long elapsedTime = 0;

void setupPins(){
  pinMode(SFC_LATCH, INPUT);
  pinMode(SFC_CLOCK, INPUT);
  pinMode(SFC_DATA, OUTPUT);

  Serial.println("pin setup Complete.");
}

void drawButtons(uint32_t data){
  Serial.println("Value: " + String(data, HEX));

  display.clear();
  display.drawString(0, 0, "Hello world");
  display.drawString(0, 32, "Value: " + String(data, HEX));
  display.display();
}

// Clock Cycle: 1 2 3   4   5 6 7 8 9 10 11 12 13 14 15 16
// Button     : B Y Sel Sta U D L R A X  L  R  0  0  0  0 
void sendData(){
  if(!latch){ 
    // LATCHが立つまで待機
    if(digitalRead(SFC_LATCH) == HIGH){
      latch = true;
    }
  }else{
    unsigned long _time = millis();
    if(_time - elapsedTime > 1000){
      elapsedTime = millis();

      buttonStat = !buttonStat;
      if(buttonStat){
        buttonsTemp = buttons;
      }else{
        buttonsTemp = buttons2;
      }
    }
  }

  if(latch){
    for (int i = 0; i < 16; i++)
    {
      // CLOCKがHIGHになるまで待機
      while(digitalRead(SFC_CLOCK) == LOW){
        delayMicroseconds(1);
      }
      bool push = (buttonsTemp >> (16 - i)) & 0x1;
      digitalWrite(SFC_DATA, push); // ボタン
      delayMicroseconds(12);  // 12us待機
      digitalWrite(SFC_DATA, LOW);  // LOWに変
    }

    drawButtons(buttonsTemp);

    latch = false;
  }
}

// void drawSignal(){
//     delayMicroseconds(16670);   //16.67ms待機
//     latchCnt++;
//     if(latchCnt > 60){
//       latchCnt = 0;
//       latch = !latch;
//       Serial.println("Value: " + String(latch));

//       display.clear();
//       display.drawString(0, 0, "Hello world");
//       display.drawString(0, 32, "Value: " + String(latch));
//       display.display();
//     }
// }

void setup() {
  Serial.begin(115200);
  
  Serial.println();
  Serial.println("wake.");

  // Initialising the UI will init the display too.
  display.init();

  //display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  display.drawString(0, 0, "Hello world");
  display.display();

  // put your setup code here, to run once:
  setupPins();
}

void loop() {
  // put your main code here, to run repeatedly:
  sendData();
}