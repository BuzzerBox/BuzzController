#include <Wire.h>
#include "portConfig.h"
//#include <avr/wdt.h>


boolean isLocked = false;
boolean rPiIsConnected = false;
boolean rPiJustConnected = false;
//unsigned long lastConnection;

uint8_t currentBuzzer = NO_BUZZER;

uint8_t lookup[] = {7, 0, 5, 1, 6, 4, 3, 2};
uint8_t getBitPosition(uint8_t b) {
  return lookup[((b * 0x1D) >> 4) & 0x7];
}


void setup() {
  //wdt_enable(WDTO_250MS);
  
  DDRB = B00000000;
  PORTB = B01111110 | PORTB;
  
  DDRC = B00000000;
  PORTC = B01000000 | PORTC;
  
  DDRD = B00000000;
  PORTD = B10010000 | PORTD;
  
  DDRE = B00000000;
  PORTE = B01000000 | PORTE;
  
  DDRF = B00000000;
  PORTF = B10000000 | PORTF;
  
  Wire.begin(0x19);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

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
  rPiIsConnected = true;
  switch(command[0]) {
    case SOFT_RELEASE:
      unlock();
      break;
    case SET_BUZZER:
      if(command[1]<= 9) {
         lock(command[1]);
      }
      break;
  }
  command[0] = EMPTY;
}

void displayCurrentState() {
  if(currentBuzzer == NO_BUZZER) {
    clearDisplay();
  } else {
    displayNumber(currentBuzzer);
  }
}

void handleRpiConnectionState() {
  if (!rPiIsConnected  && rPiJustConnected) {
    rPiJustConnected = false;
    rPiIsConnected = true;
    displayCurrentState();
  }
//    lastConnection = millis();
//    displayCurrentState();
//  } else {
//    if (lastConnection + 2000 < millis()) {
//      rPiIsConnected = false;
//      displayCurrentState();
//    }
//  }
}

void loop() {
  handleRpiConnectionState();
  checkCommand(lastCommand);
  if((~(PINF >> 7) & 1)) {
    unlock();
  }  

  uint8_t pinb = PINB | 0b10000001; //& 0b01111110;
  uint8_t pinc = PINC | 0b10111111; //& 0b01000000;
  uint8_t pind = PIND | 0b01101111; //& 0b10010000;

  if((~(PINE >> 6) & 1)) {
    lock(7);
  } else if (pinb != 0xFF) {
    lock(buttonsB[getBitPosition(~pinb)].number);
  } else if (pinc != 0xFF) {
    lock(5);
  } else if (pind>>7 == 0) {
    lock(6);
  } else if ((pind & 0b00010000)>>4 == 0) {
    lock(4);
  }
  //wdt_reset();
}
