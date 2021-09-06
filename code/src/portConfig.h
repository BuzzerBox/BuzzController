#ifndef port_h
#define port_h

typedef struct LEDConfig {
  uint8_t ledPort;
  uint8_t ledPin;
}ButtonC;

typedef struct buttonBConfig {
  uint8_t number;
  uint8_t ledPort;
  uint8_t ledPin;
}ButtonPort;


typedef enum {   
  EMPTY = 0x00,
  SOFT_RELEASE = 0x01,
  SET_BUZZER = 0x02,

} Message_res_t;

const byte NO_BUZZER = 0x7f;

const uint8_t BUTTON_COUNT = 10;
const ButtonPort buttonsB [8] = {
  {3,1,0}, //Done
  {1,0,0}, //Done
  {0,0,1},  //Done
  {0,0,0}, //UNUSED
  {0,0,0}, //UNUSED
  {0,0,0},//UNUSED
  {0,0,0},//UNUSED
  {0,0,0} // UNUSED
};
const ButtonPort buttonsC [8] = {
  {0,0,0}, // UNUSED
  {2,1,1}, // Done
  {0,0,1}, // UNUSED
  {9,0,7}, //Done
  {9,0,7}, // UNUSED
  {8,0,6},// UNUSED
  {1,0,0},// UNUSED
  {7,0,0} // UNUSED
};
const ButtonPort buttonsD [8] = {
  {0,0,0}, // UNUSED
  {3,1,0},// UNUSED
  {9,0,7}, // UNUSED
  {8,0,6}, //Done 
  {7,0,5},//Done
  {6,0,4}, //Done
  {5,0,3},// Done
  {4,0,2} //Done
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

#endif