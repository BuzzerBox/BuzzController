#include "Keyboard.h"
#include <Wire.h>
#include "portConfig.h"

boolean isLocked = false;

bool previousState[BUTTON_COUNT] = {HIGH}; 
void setup() {
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
     pinMode(buttonPins[i], INPUT_PULLUP);
  }
  pinMode(resetButton, INPUT_PULLUP);
  Keyboard.begin();
  Wire.begin();
  // SET Write Mode to Port A on second Expander
  setExpanderRegister(expanderId7Seg, ddrRegisterId, 0x00);
  clearDisplay();
}

void loop() {
  if(digitalRead(resetButton) == LOW) {
    isLocked = false;
    clearDisplay();
  }  
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    int currentState = digitalRead(buttonPins[i]);
    if (currentState != previousState[i] && currentState == LOW && isLocked == false) {   
        displayNumber(i);
        isLocked = true;
        //Keyboard.print(String(i));
    }
    previousState[i] = currentState;
  }
}
