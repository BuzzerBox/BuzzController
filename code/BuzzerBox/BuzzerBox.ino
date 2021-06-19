#include <Wire.h>
#include "portConfig.h"

boolean isLocked = false;
boolean standAlone = false;

bool previousState[BUTTON_COUNT] = {HIGH}; 
void setup() {
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
     pinMode(buttons[i].buttonPin, INPUT_PULLUP);
  }
  pinMode(resetButton, INPUT_PULLUP);
  Wire.begin();
  initExpanders();
  clearDisplay();
  if(digitalRead(resetButton) == LOW) {
    standAlone = true;
  }
}

void loop() {
  if(digitalRead(resetButton) == LOW) {
    isLocked = false;
    clearDisplay();
    turnOffLEDs();
    sendPacketToPi(RELEASE_PRESSED, 0x00);
  }  
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    int currentState = digitalRead(buttons[i].buttonPin);
    if (currentState != previousState[i] && currentState == LOW && isLocked == false) {   
        displayNumber(i);
        turnOnSingleLED(buttons[i]);
        isLocked = true;
        sendPacketToPi(BUZZER_PRESSED, i);
    }
    previousState[i] = currentState;
  }
}
