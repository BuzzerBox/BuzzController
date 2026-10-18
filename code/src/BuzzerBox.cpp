#include <Arduino.h>
#include "portConfig.hpp"
#include <avr/interrupt.h>
#include "7segment.hpp"
#include "i2c_communication.hpp"
#include "rpi_communication.hpp"

#include <avr/wdt.h>
volatile uint8_t port_interrupt_flag = 255;
volatile uint8_t pin_data = 0xFF; // port snapshot taken at the first edge (0 = pressed)

RpiComm* rpiComm;
SevenSeg* sevenSeg;
I2CComm* i2cComm;

uint8_t b_select = B00000111;
uint8_t c_select = B00001010;
uint8_t d_select = B11111000;

boolean isLocked = false;
boolean rPiIsConnected = false;

#define RELEASE_HOLD_MS 50
uint8_t release_was_pressed = 0;
uint32_t release_down_since = 0;
boolean release_armed = false;

uint8_t currentBuzzer = NO_BUZZER;

// Buzzers the Pi turned off, one bit per buzzer ID. Releasing doesn't clear it, only ENABLE_ALL_BUZZERS does; after a
// reset all buzzers count, and the Pi sends the disabled ones again with its heartbeats.
uint16_t disabledButtons = 0;
// Buzzers that were held down while disabled: they count again once released, so a team that keeps its button
// pressed doesn't lock the box the moment it is enabled again.
uint16_t ignoredUntilReleased = 0;

#define DEBOUNCE_MS 20
uint32_t debounce_start = 0;

// uint8_t lookup[] = {7, 0, 5, 1, 6, 4, 3, 2};
uint8_t getBitPosition(uint8_t b) {
  int i=0;
  while( !((b >> i) & 1) ) {
    i++;
  }
  return i;
  //return lookup[((b * 0x1D) >> 4) & 0x7];
}

boolean isDisabled(uint8_t buttonID) {
  return ((disabledButtons | ignoredUntilReleased) >> buttonID) & 1;
}

// The pins of the port whose buzzers count, out of the selected ones
uint8_t countingPins(uint8_t select, const uint8_t* buttonNumbers) {
  uint8_t counting = select;
  for (uint8_t bit = 0; bit < 8; bit++) {
    if (((select >> bit) & 1) && isDisabled(buttonNumbers[bit])) {
      counting &= ~(1 << bit);
    }
  }
  return counting;
}

// Every buzzer that is held down right now, one bit per buzzer ID
uint16_t readPressedButtons() {
  uint16_t pressed = 0;
  uint8_t pins[3] = {(uint8_t) (~PINB & b_select), (uint8_t) (~PINC & c_select), (uint8_t) (~PIND & d_select)};
  const uint8_t* buttonNumbers[3] = {buttonsB, buttonsC, buttonsD};
  for (uint8_t port = 0; port < 3; port++) {
    for (uint8_t bit = 0; bit < 8; bit++) {
      if ((pins[port] >> bit) & 1) {
        pressed |= (uint16_t) 1 << buttonNumbers[port][bit];
      }
    }
  }
  return pressed;
}

void enableButtonInterrupts() {
  cli();
  PCIFR = 0b00000111; // drop edges that happened while disabled
  PCICR |= 0b00000111;
  sei();
}

void setup() {
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

  wdt_enable(WDTO_1S);

  rpiComm = new RpiComm();
  i2cComm = new I2CComm();
  sevenSeg = new SevenSeg(true, i2cComm);
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
  enableButtonInterrupts();
  rpiComm->sendUnlock();
}

void lock(uint8_t buttonID) {
  isLocked = true;
  currentBuzzer = buttonID;
  sevenSeg->displayNumber(buttonID, !rPiIsConnected);
  i2cComm->turnOnSingle(buttons[buttonID]);
  rpiComm->sendButtonSelected(buttonID);
}

void checkCommand(PiCmd command) {
  if(command.command == EMPTY) return;
  rPiIsConnected = true;
  switch(command.command) {
    case SOFT_RELEASE:
      unlock();
      break;
    case SET_BUZZER:
      // also a disabled one, e.g. when the master marks a team by hand
      if(command.data <= 9) {
         lock(command.data);
      }
      break;
    case DISABLE_BUZZER:
      if(command.data <= 9) {
        disabledButtons |= (uint16_t) 1 << command.data;
      }
      break;
    case ENABLE_ALL_BUZZERS:
      // the ones still held down by a team that was disabled wait until they are released
      ignoredUntilReleased |= disabledButtons & readPressedButtons();
      disabledButtons = 0;
      break;
  }
}

void displayCurrentState() {
  if(currentBuzzer == NO_BUZZER) {
    sevenSeg->clearDisplay(!rPiIsConnected);
  } else {
    sevenSeg->displayNumber(currentBuzzer, !rPiIsConnected);
  }
}

void handleRpiConnectionState() {
  Pi_connection_state_t state = rpiComm->getConnectionState();
  if (state == JUST_CONNECTED) {
    rPiIsConnected = true;
    rpiComm->setConnectionState(CONNECTED);
    displayCurrentState();
  } else if (state == JUST_TIMED_OUT) {
    rPiIsConnected = false;
    rpiComm->setConnectionState(NO_CONNECTION);
    displayCurrentState();
  } else if (state == CONNECTED) {
    if (rpiComm->isTimedOut()) {
      rPiIsConnected = false;
      displayCurrentState();
      rpiComm->setConnectionState(NO_CONNECTION);
    }
  }
}

void handleReleaseButton() {
  uint8_t release_pressed = ~PINC & 1;
  if (release_pressed && !release_was_pressed) {
    // Only a press that starts while locked may unlock, so holding the
    // button cannot cancel the next lock.
    release_down_since = millis();
    release_armed = isLocked;
  } else if (release_pressed && release_armed && isLocked
             && millis() - release_down_since >= RELEASE_HOLD_MS) {
    release_armed = false;
    unlock();
  }
  release_was_pressed = release_pressed;
}

// Pick the pressed button on the port that triggered the interrupt.
// Buttons already down at the first edge win over ones pressed during
// the debounce window. Disabled buttons don't count: when only those
// were pressed, nothing locks.
void lockPressedButton() {
  uint8_t pins, select;
  const uint8_t* buttonNumbers;
  if (port_interrupt_flag == 0) {
    pins = PINB; select = b_select; buttonNumbers = buttonsB;
  } else if (port_interrupt_flag == 1) {
    pins = PINC; select = c_select; buttonNumbers = buttonsC;
  } else {
    pins = PIND; select = d_select; buttonNumbers = buttonsD;
  }
  uint8_t pressed = ~pins & countingPins(select, buttonNumbers);
  uint8_t pressedAtEdge = pressed & ~pin_data;
  if (pressedAtEdge) pressed = pressedAtEdge;
  if (pressed) lock(buttonNumbers[getBitPosition(pressed)]);
}

void loop() {
  if (rpiComm->checkData()) {
    checkCommand(rpiComm->getLastCmd());
  }

  handleRpiConnectionState();
  handleReleaseButton();
  if (ignoredUntilReleased) {
    ignoredUntilReleased &= readPressedButtons();
  }

  if(port_interrupt_flag != 255) {
    if (debounce_start == 0) {
      debounce_start = millis();
    } else if (millis() - debounce_start >= DEBOUNCE_MS) {
      if (!isLocked) {
        lockPressedButton();
      }
      port_interrupt_flag = 255;
      debounce_start = 0;
      // Bounce or short tap: nothing locked, so listen again.
      if (!isLocked) {
        enableButtonInterrupts();
      }
    }
  }
  wdt_reset();
}
