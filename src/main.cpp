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
#define SFC_DATA    5

// Initialize the OLED display using Arduino Wire:
SSD1306Wire display(0x3c, 20, 21);   // ADDRESS, SDA, SCL

volatile uint16_t buttons;
//volatile bool dataarr[16];
volatile int bitcounter = 0;

volatile uint16_t buttons = 0;

// TaskHandle_t drawButtonsTaskHandle = NULL;
// String buttonNames[] = {"B", "Y", "Sel", "Sta", "U", "D", "L", "R", "A", "X", "L", "R", "0", "0", "0", "0" };

// QueueHandle_t xDisplayQueue; // キューのハンドラ
// uint16_t _temp;

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

bool IRAM_ATTR getButton(uint8_t bit){
  return !(buttons & (1 << bit));
}

void IRAM_ATTR latching() {
  //ulong timer = micros();
  bitcounter = 0;
  fastDigitalWrite((~buttons & (1 << (16 - bitcounter))), SFC_DATA);  // LOWがボタン押した判定なので反転する
  //Serial.println(micros() - timer);

  // uint16_t _temp = buttons;
  // xQueueSendFromISR(xDisplayQueue, &_temp, 0);
}

void IRAM_ATTR clocking() {
  if (bitcounter > 12) {
    fastDigitalWrite(HIGH, SFC_DATA);
  }else{
    bitcounter++;
    fastDigitalWrite((~buttons & (1 << (16 - bitcounter))), SFC_DATA);  // LOWがボタン押した判定なので反転する
  }
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

  attachInterrupt(SFC_LATCH, latching, RISING);
  attachInterrupt(SFC_CLOCK, clocking, RISING);

  //xDisplayQueue = xQueueCreate(1, sizeof(uint16_t));
  //xTaskCreate(drawButtons, "DrawButtons", 1024, NULL, 0, NULL);
}

void loop() {
  if (Serial.available()){
    uint8_t buf[2];
    Serial.readBytes(buf, 2);

    buttons = (buf[0] << 8) | buf[1];

    Serial.println("ValueBIN: " + String(buttons, BIN));
    Serial.println("ValueHEX: " + String(buttons, HEX));

    Serial.flush();
  }
}