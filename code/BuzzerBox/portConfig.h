#pragma once

typedef struct LEDConfig {
  uint8_t ledPort;
  uint8_t ledPin;
}ButtonC;

typedef struct buttonBConfig {
  uint8_t number;
  uint8_t ledPort;
  uint8_t ledPin;
}ButtonB;


typedef enum {   
  EMPTY = 0x00,
  SOFT_RELEASE = 0x01,
  SET_BUZZER = 0x02,

} Message_res_t;

const byte NO_BUZZER = 0x7f;

const uint8_t BUTTON_COUNT = 10;
const ButtonB buttonsB [8] = {
  {0,0,0}, //UNUSED
  {3,1,0},
  {0,0,1}, 
  {2,1,1}, 
  {9,0,7}, 
  {8,0,6},
  {1,0,0},
  {7,0,0} // UNUSED
};

const LEDConfig buttons [BUTTON_COUNT] = {
  {0,1}, 
  {0,0},
  {1,1}, 
  {1,0},
  {0,2},
  {0,3}, 
  {0,4}, 
  {0,5}, 
  {0,6},
  {0,7} 
};
const uint8_t resetButton = A0;
const uint8_t leds[10] = {0};
 
const byte expanderIdButtonLEDs = 0x20;
const byte expanderId7Seg = 0x21;
const byte piI2CAddress = 0x22;

const byte ddrRegisterAId = 0x00;
const byte ddrRegisterBId = 0x01;
const byte outRegisterAId = 0x14;
const byte outRegisterBId = 0x15;

byte lastCommand[2] = {0x00, 0x00};
