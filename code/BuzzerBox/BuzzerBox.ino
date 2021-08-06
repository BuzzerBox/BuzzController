#include <Wire.h>
#include "portConfig.h"
#include "Keyboard.h"

//#include <avr/wdt.h>


boolean isLocked = false;
boolean rPiIsConnected = false;
boolean rPiJustConnected = false;
//unsigned long lastConnection;
uint8_t last_state[10] = {1};
uint8_t last_release_button_state = 1;

uint8_t currentBuzzer = NO_BUZZER;

uint8_t lookup[] = {7, 0, 5, 1, 6, 4, 3, 2};
uint8_t getBitPosition(uint8_t b) {
  //int i=0;
  //while( !((b >> i++) & 0x01) ) { ; }
  //return i;
  return lookup[((b * 0x1D) >> 4) & 0x7];
}


void setup() {
  //wdt_enable(WDTO_250MS);
  
  DDRB = B00000000;
  PORTB = B00000111 | PORTB;
  
  DDRC = B00000000;
  PORTC = B00001011 | PORTC;
  
  DDRD = B00000000;
  PORTD = B11111000 | PORTD;
  
  Wire.begin(0x19);
  Wire.onReceive(receiveEvent);
  //Keyboard.begin();

  initExpanders();
  clearDisplay();
  turnOffLEDs();
}

void unlock() {
  isLocked = false;
  currentBuzzer = NO_BUZZER;
  clearDisplay();
  turnOffLEDs();
  //Keyboard.print('q');
}

void lock(uint8_t buttonID) {
  isLocked = true;
  currentBuzzer = buttonID;
  displayNumber(buttonID);
  turnOnSingleLED(buttons[buttonID]);
  //Keyboard.print(String(buttonID));
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
  uint8_t pinb = PINB | 0b11111000; //& 0b01111110;
  uint8_t pinc = PINC | 0b11110100; //& 0b01000000;
  uint8_t pind = PIND | 0b00000111; //& 0b10010000;
  
  if((~pinc) & 1) {
    if (isLocked == true && last_release_button_state == 1) {
      unlock();
    }
    last_release_button_state = 0;
  } else {
    last_release_button_state = 1;
     if (!isLocked) {
      if (pinb != 0xFF) {
        lock(buttonsB[getBitPosition(~pinb)].number);
      } else if (pinc != 0xFF) {
        lock(buttonsC[getBitPosition(~pinc)].number);
      } else if (pind != 0xFF) {
        lock(buttonsD[getBitPosition(~pind)].number);
      }
    }
  }
  

 
  //wdt_reset();
}
