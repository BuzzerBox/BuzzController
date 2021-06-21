#include <Wire.h>
#include "portConfig.h"
#include <avr/wdt.h>


boolean isLocked = false;
boolean standAlone = false;
uint8_t currentBuzzer = NO_BUZZER;

bool previousState[BUTTON_COUNT] = {HIGH}; 
void setup() {
  wdt_enable(WDTO_250MS);
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
     pinMode(buttons[i].buttonPin, INPUT_PULLUP);
  }
  pinMode(resetButton, INPUT_PULLUP);
  Wire.begin(0x19);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);      
  if(digitalRead(resetButton) == LOW) {
    standAlone = true;
  }
  initExpanders();
  clearDisplay();
  turnOffLEDs();
}

void unlock() {
  isLocked = false;
  currentBuzzer = NO_BUZZER;
  clearDisplay();
  turnOffLEDs();
}

void lock(uint8_t buttonID) {
  isLocked = true;
  currentBuzzer = buttonID;
  displayNumber(buttonID);
  turnOnSingleLED(buttons[buttonID]);
}

void checkCommand(byte command[2]) {
  if(command[0] == EMPTY) return;
  switch(command[0]) {
    case EMPTY:
      break;
    case SOFT_RELEASE:
      unlock();
      break;
    case SET_BUZZER:
      lock(command[1]);
      break;
  }
  command[0] = EMPTY;
}

void loop() {
  checkCommand(lastCommand);
  if(digitalRead(resetButton) == LOW) {
    unlock();
  }  
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    int currentState = digitalRead(buttons[i].buttonPin);
    if (currentState != previousState[i] && currentState == LOW && (isLocked == false)) {   
        lock(i);
    }
    previousState[i] = currentState;
  }
  wdt_reset();
}
