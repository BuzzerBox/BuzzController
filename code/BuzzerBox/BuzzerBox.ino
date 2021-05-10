#include "Keyboard.h"

const int BUTTON_COUNT = 10;
const int buttonPins[] = {PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA15}; // Dont reuse any RX/TX/USB/Pins
const int ledPins[] = {PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9}; 
const int unlockPin = PB11;
bool locked = false;

void setup() {
  pinMode(unlockPin, INPUT_PULLUP);
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
     pinMode(buttonPins[i], INPUT_PULLUP);
  }
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
     pinMode(ledPins[i], OUTPUT);
  }
  Keyboard.begin();
}


void loop() {
  if (digitalRead(unlockPin) == LOW) {
    locked = false;
  }
  
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    if (digitalRead(buttonPins[i]) == LOW) {
      if (locked == false) {        
        Keyboard.print(String(i));
        locked = true;
      }
    }
  }
}
