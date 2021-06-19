#pragma once

typedef struct buttonConfig {
  uint8_t buttonPin;
  uint8_t ledPort;
  uint8_t ledPin;
}ButtonC;

typedef struct packetType {
  byte messageType;
  byte payload1;
  byte payload2;
} PacketType;

typedef enum {   
  BUZZER_PRESSED = 0, 
  RELEASE_PRESSED = 1, 
  READ_DATA = 2
} Message_req_t;

typedef enum {   
  SET_BUZZER_LEDS = 100,
  SET_SEGMENTS = 101,
  SOFT_RELEASE = 102

} Message_res_t;
 

const uint8_t BUTTON_COUNT = 10;
const ButtonC buttons [BUTTON_COUNT] = {
  {4,0,2},
  {5,0,3}, 
  {6,0,4}, 
  {7,0,5}, 
  {8,0,7}, 
  {9,0,6}, 
  {10,0,0}, 
  {16,0,1}, 
  {14,1,1}, 
  {15,1,0}
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
