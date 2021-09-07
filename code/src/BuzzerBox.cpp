#include <Arduino.h>
#include "portConfig.hpp"
#include <avr/interrupt.h>
#include "7segment.hpp"
#include "i2c_communication.hpp"
#include "rpi_communication.hpp"

#include <avr/wdt.h>
uint8_t port_interrupt_flag = 255;
uint8_t pin_data = 0xFF;

RpiComm* rpiComm;
SevenSeg* sevenSeg;
I2CComm* i2cComm;

byte lastCommand[3] = {0x00, 0x00, 0x00};


uint8_t b_select = B00000111;
uint8_t c_select = B00001010;
uint8_t d_select = B11111000;

boolean isLocked = false;
boolean rPiIsConnected = false;
boolean rPiJustConnected = false;
//unsigned long lastConnection;
uint8_t last_state[10] = {1};
uint8_t release_down_count = 0;
uint8_t release_was_high = 1;

uint8_t currentBuzzer = NO_BUZZER;

// uint8_t lookup[] = {7, 0, 5, 1, 6, 4, 3, 2};
uint8_t getBitPosition(uint8_t b) {
  int i=0;
  while( !((b >> i) & 1) ) { 
    i++;  
  }
  return i;
  //return lookup[((b * 0x1D) >> 4) & 0x7];
}


void setup() {
  wdt_enable(WDTO_1S);

  DDRB = B00000000;
  DDRC = B00000000;
  DDRD = B00000000;

  PORTB |= b_select;
  PORTC |= (c_select | 1); // Also configure Release
  PORTD |= d_select;

  cli();
  PCMSK0 |= b_select;
  PCMSK1 |= c_select;
  PCMSK2 |= d_select; // enable Interrupts on active pins for port d
  PCICR  |= 0b00000111; // enable Interrupt on all 3 ports
  sei();
  
  rpiComm = new RpiComm();
  i2cComm = new I2CComm();
  sevenSeg = new SevenSeg(!rPiIsConnected, i2cComm);
  i2cComm->turnOffAll();
}

ISR(PCINT0_vect)
{
  if (port_interrupt_flag == 255) {
    pin_data = PINB | ~b_select; 
    if (pin_data != 0xFF) {
      PCICR  &= ~0b00000111;
      port_interrupt_flag = 0;
    }
  }
}

ISR(PCINT1_vect)
{
  if (port_interrupt_flag == 255) {
    pin_data = PINC | ~c_select; 
    if (pin_data != 0xFF) {
      PCICR  &= ~0b00000111;
      port_interrupt_flag = 1;
    }
  }
}

ISR(PCINT2_vect)
{
  if (port_interrupt_flag == 255) {
    pin_data = PIND | ~d_select; 
    if (pin_data != 0xFF) {
      PCICR  &= ~0b00000111;
      port_interrupt_flag = 2;
    }
  }
}


void unlock() {
  isLocked = false;
  currentBuzzer = NO_BUZZER;
  sevenSeg->clearDisplay(!rPiIsConnected);
  i2cComm->turnOffAll();
  cli();
  PCICR  |= 0b00000111;
  sei();
  rpiComm->sendUnlock();
}

void lock(uint8_t buttonID) {
  isLocked = true;
  currentBuzzer = buttonID;
  sevenSeg->displayNumber(buttonID, !rPiIsConnected);
  i2cComm->turnOnSingle(buttons[buttonID]);
  rpiComm->sendButtonSelected(buttonID);
}

void checkCommand(byte command[3]) {
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
    sevenSeg->clearDisplay(!rPiIsConnected);
  } else {
    sevenSeg->displayNumber(currentBuzzer, !rPiIsConnected);
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
  if (rpiComm->checkData(lastCommand)) {
    rPiJustConnected = true;
    checkCommand(lastCommand);  
  }
  
  handleRpiConnectionState();
  uint8_t release_pressed = ~PINC & 1;
  if (release_pressed && release_was_high == 1) {
      if (isLocked == true && release_down_count > 10) {
        unlock();
        release_was_high = 0;
      }
      release_down_count++;
  } else {
    if (!release_pressed) release_was_high = 1;
    release_down_count = 0;
  }
    
  
  if(port_interrupt_flag != 255) {
     if (!isLocked) {
      if (port_interrupt_flag == 0) {
        lock(buttonsB[getBitPosition(~pin_data)].number);
      } else if (port_interrupt_flag == 1) {
        lock(buttonsC[getBitPosition(~pin_data)].number);
      } else if (port_interrupt_flag == 2) {
        lock(buttonsD[getBitPosition(~pin_data)].number);
      }
    }
    port_interrupt_flag = 255;
  }
 
  wdt_reset();
}
