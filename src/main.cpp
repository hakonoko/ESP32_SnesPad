// Clock Cycle: 1 2 3   4   5 6 7 8 9 10 11 12 13 14 15 16
// Button     : B Y Sel Sta U D L R A X  L  R  0  0  0  0 

// 1 : 5V    : White
// 2 : CLOCK : Yellow
// 3 : LATCH : Orange
// 4 : DATA  : Red
// 5 : GND   : Brown

#include <Arduino.h>

#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"

#define GPIO_0to31SET_REG   *((volatile unsigned long *)GPIO_OUT_W1TS_REG)
#define GPIO_0to31CLR_REG   *((volatile unsigned long *)GPIO_OUT_W1TC_REG)

#define SFC_CLOCK   7
#define SFC_LATCH   6
#define SFC_DATA    4

// Initialize the OLED display using Arduino Wire:
SSD1306Wire display(0x3c, 20, 21);   // ADDRESS, SDA, SCL

volatile bool dataarr[16];
volatile int bitcounter = 0;

TaskHandle_t drawButtonsTaskHandle = NULL;
String buttonNames[] = {"B", "Y", "Sel", "Sta", "U", "D", "L", "R", "A", "X", "L", "R", "0", "0", "0", "0" };

void setupPins(){
  pinMode(SFC_LATCH, INPUT);
  pinMode(SFC_CLOCK, INPUT);
  pinMode(SFC_DATA, OUTPUT);

  Serial.println("pin setup Complete.");
}

// void drawButtons(void *pvParameters){
//   display.clear();
//   display.drawString(0, 0, "Hello world");

//   for(int i=0; i < 16; i++){
//     display.drawString(30, 15*i, buttonNames[i]);
//   }
//   display.display();
// }

void IRAM_ATTR fastDigitalWrite(bool val, uint8_t pin){
  if(val){
    GPIO_0to31SET_REG = 1 << pin;
    //GPIO.out_w1ts.out_w1ts = 1 << pin;
  }else{
    GPIO_0to31CLR_REG = 1 << pin;
    //GPIO.out_w1tc.out_w1tc = 1 << pin;
  }
}

void IRAM_ATTR latching() {
  bitcounter = 0;
  fastDigitalWrite(dataarr[bitcounter], SFC_DATA);
}

void IRAM_ATTR clocking() {
  if (bitcounter > 15) {
    fastDigitalWrite(HIGH, SFC_DATA);
  }else{
    bitcounter++;
    fastDigitalWrite(dataarr[bitcounter], SFC_DATA);
  }
  
  // xTaskCreateUniversal(
  //   drawButtons,
  //   "DrawButtons",
  //   8192,
  //   NULL,
  //   0,
  //   &drawButtonsTaskHandle,
  //   CONFIG_ARDUINO_RUNNING_CORE
  //   );
  //drawButtons();
}

void setup() {
  Serial.begin(115200);
  
  Serial.println();
  Serial.println("wake.");

  // Initialising the UI will init the display too.
  display.init();

  //display.flipScreenVertically();   // ディスプレイを縦に回転
  display.setFont(ArialMT_Plain_10);

  display.drawString(0, 0, "Hello world");
  display.display();

  setupPins();

  // 
  attachInterrupt(SFC_LATCH, latching, RISING);
  attachInterrupt(SFC_CLOCK, clocking, RISING);

  for ( int i = 0; i < 16; i++) {
    if (i != 4) {
      dataarr[i] = false;
    } else {
      dataarr[i] = true;
    }
  }
}

void loop() {
  if (!Serial.available()) return;
  
  uint8_t buf[2];
  Serial.readBytes(buf, 2);
  uint16_t data = (buf[0] << 8) | buf[1];

  Serial.println("ValueBIN: " + String(data, BIN));
  Serial.println("ValueHEX: " + String(data, HEX));

  for (size_t i = 0; i < 16; i++)
  {
    /* code */
    if ((data >> 15 - i) == 1) {
      dataarr[i] = false;
    } else {
      dataarr[i] = true;
    }
  }

  Serial.flush();
}