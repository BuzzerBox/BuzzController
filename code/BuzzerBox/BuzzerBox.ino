#include "Keyboard.h"

const int BUTTON_COUNT = 10;
const int buttonPins[10] = {5,6,7,8,9,10,11,12,13,14}; 
bool previousState[10] = {HIGH}; 

void setup() {
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
     pinMode(buttonPins[i], INPUT_PULLUP);
  }
  Keyboard.begin();
}

void loop() {
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    int currentState = digitalRead(buttonPins[i]);
    if (currentState != previousState[i] && currentState == LOW) {   
        Keyboard.print(String(i));
    }
    previousState[i] = currentState;
  }
}
