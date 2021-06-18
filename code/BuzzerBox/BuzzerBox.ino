#include "Keyboard.h"
#include <Wire.h>

const int BUTTON_COUNT = 10;
const int buttonPins[BUTTON_COUNT] = {5,6,7,8,9,10,11,12,13,14}; 
const int segments[8] = {2,3,5,6,7,1,0,4}; //A,B,C,D,E,F,G,DP
const byte ZERO = 1<<segments[0] | 1<<segments[1] | 1<<segments[2] | 1<<segments[3] | 1<<segments[4] | 1<<segments[5];
const byte ONE = 1<<segments[1] | 1<<segments[2];
const byte TWO = 1<<segments[0] | 1<<segments[1] | 1<<segments[3] | 1<<segments[4] | 1<<segments[6];
const byte THREE = 1<<segments[0] | 1<<segments[1] | 1<<segments[2] | 1<<segments[3] | 1<<segments[6];
const byte FOUR = 1<<segments[1] | 1<<segments[2]  | 1<<segments[5] | 1<<segments[6];
const byte FIVE = 1<<segments[0] | 1<<segments[2] | 1<<segments[3] | 1<<segments[5] | 1<<segments[6];
const byte SIX = 1<<segments[0] | 1<<segments[2] | 1<<segments[3] | 1<<segments[4] | 1<<segments[5] | 1<<segments[6];
const byte SEVEN = 1<<segments[0] | 1<<segments[1] | 1<<segments[2];
const byte EIGHT = 1<<segments[0] | 1<<segments[1] | 1<<segments[2] | 1<<segments[3] | 1<<segments[4] | 1<<segments[5] | 1<<segments[6];
const byte NINE = 1<<segments[0] | 1<<segments[1] | 1<<segments[2] | 1<<segments[3] | 1<<segments[5] | 1<<segments[6];
const byte numbers[10] = {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE}; 
 
bool previousState[BUTTON_COUNT] = {HIGH}; 
void setup() {
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
     pinMode(buttonPins[i], INPUT_PULLUP);
  }
  Keyboard.begin();
  Wire.begin();
  Wire.beginTransmission(0x21); // transmit to device #4
  Wire.write(0x00);        // sends five bytes
  Wire.write(0x00);              // sends one byte  
  Wire.endTransmission();    // stop transmitting
}

void sendByte(byte data) {
  Wire.beginTransmission(0x21); // transmit to device #4
  Wire.write(0x14);        // sends five bytes
  Wire.write(data);              // sends one byte  
  Wire.endTransmission();    // stop transmitting
}

void displayNumber(uint8_t number) {
  if (number > 9) return;
  sendByte(~numbers[number]);
}

void displayOff() {
  sendByte(0xFF);
}

void loop() {
  displayOff();
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    int currentState = digitalRead(buttonPins[i]);
    if (currentState != previousState[i] && currentState == LOW) {   
        Keyboard.print(String(i));
    }
    previousState[i] = currentState;
  }
}
