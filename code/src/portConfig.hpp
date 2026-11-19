#ifndef port_h
#define port_h

typedef struct LEDConfig {
  uint8_t ledPort;
  uint8_t ledPin;
}ButtonC;

typedef enum {
  EMPTY = 0x00,
  SOFT_RELEASE = 0x01,
  SET_BUZZER = 0x02,
  // the buzzer (data = buzzer ID 0-9) neither locks the box nor lights up until all are enabled again
  DISABLE_BUZZER = 0x03,
  // all buzzers count again (data = stuffing byte)
  ENABLE_ALL_BUZZERS = 0x04,
} Message_res_t;

const byte NO_BUZZER = 0x7f;

const uint8_t BUTTON_COUNT = 10;
// Buzzer ID per port pin. UNUSED pins are excluded by the *_select masks.
const uint8_t buttonsB [8] = {
  7, //Done
  9, //Done
  0, //Done
  0, //UNUSED
  0, //UNUSED
  0, //UNUSED
  0, //UNUSED
  0  //UNUSED
};
const uint8_t buttonsC [8] = {
  0, //UNUSED (Release button)
  8, //Done
  0, //UNUSED
  1, //Done
  0, //UNUSED (I2C SDA)
  0, //UNUSED (I2C SCL)
  0, //UNUSED
  0  //UNUSED
};
const uint8_t buttonsD [8] = {
  0, //UNUSED (UART RX)
  0, //UNUSED (UART TX)
  0, //UNUSED
  2, //Done
  3, //Done
  4, //Done
  5, //Done
  6  //Done
};

const LEDConfig buttons [BUTTON_COUNT] = {
  {0,1}, 
  {0,7},
  {0,6}, 
  {0,5},
  {0,4},
  {0,3}, 
  {0,2}, 
  {1,0}, 
  {1,1},
  {0,0} 
};
const uint8_t resetButton = A0;

const byte expanderIdButtonLEDs = 0x20;
const byte expanderId7Seg = 0x21;

const byte ddrRegisterAId = 0x00;
const byte ddrRegisterBId = 0x01;
const byte outRegisterAId = 0x14;
const byte outRegisterBId = 0x15;

#endif