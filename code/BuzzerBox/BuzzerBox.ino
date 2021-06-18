#include "Keyboard.h"
#include <Wire.h>

const byte expanderIdButtons = 0x20;
const byte expanderId7Seg = 0x21;

const int BUTTON_COUNT = 10;
const int buttonPins[BUTTON_COUNT] = {5,6,7,8,9,10,11,12,13,14}; 
const int resetButton = A0;
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

uint8_t example_counter = 0;
boolean last_button_state = HIGH;

bool previousState[BUTTON_COUNT] = {HIGH}; 
void setup() {
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
     pinMode(buttonPins[i], INPUT_PULLUP);
  }
  pinMode(resetButton, INPUT_PULLUP);
  Keyboard.begin();
  Wire.begin();
  // SET Write Mode to Port A on second Expander
  setExpanderRegister(expanderId7Seg, 0x00, 0x00);
}


void setExpanderRegister(byte chip, byte register_, byte data) {
  Wire.beginTransmission(chip);
  Wire.write(register_);
  Wire.write(data);
  Wire.endTransmission();
}

void displayNumber(uint8_t number) {
  if (number > 9) return;
  setExpanderRegister(expanderId7Seg, 0x14, ~numbers[number]);
}

void displayOff() {
  setExpanderRegister(expanderId7Seg, 0x14, 0xFF);
}

void loop() {
  if(digitalRead(resetButton) == LOW && last_button_state == HIGH) {
      example_counter = (example_counter +1) % 10;
      last_button_state = LOW;
  }  else {
    last_button_state = HIGH;
  }
  displayNumber(example_counter);
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    int currentState = digitalRead(buttonPins[i]);
    if (currentState != previousState[i] && currentState == LOW) {   
        Keyboard.print(String(i));
    }
    previousState[i] = currentState;
  }
}
