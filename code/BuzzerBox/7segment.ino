#include "portConfig.h"
const uint8_t segments[8] = {2,3,5,6,7,1,0,4}; //A,B,C,D,E,F,G,DP

const byte ZERO = 1<<segments[0] | 1<<segments[1] | 1<<segments[2] | 1<<segments[3] | 1<<segments[4] | 1<<segments[5];
const byte ONE = 1<<segments[1] | 1<<segments[2];
const byte TWO = 1<<segments[0] | 1<<segments[1] | 1<<segments[3] | 1<<segments[4] | 1<<segments[6];
const byte THREE = 1<<segments[0] | 1<<segments[1] | 1<<segments[2] | 1<<segments[3] | 1<<segments[6];
const byte FOUR = 1<<segments[1] | 1<<segments[2]  | 1<<segments[5] | 1<<segments[6];
const byte FIVE = 1<<segments[0] | 1<<segments[2] | 1<<segments[3] | 1<<segments[5] | 1<<segments[6];
const byte SIX = 1<<segments[0] | 1<<segments[2] | 1<<segments[3] | 1<<segments[4] | 1<<segments[5] | 1<<segments[6];
const byte SEVEN = 1<<segments[0] | 1<<segments[1] | 1<<segments[2];
const byte EIGHT = 1<<segments[0] | 1<<segments[1] | 1<<segments[2] | 1<<segments[3] | 1<<segments[4] | 1<<segments[5] | 1<<segments[6];
const byte NINE = 1<<segments[0] | 1<<segments[1] | 1<<segments[2] | 1<<segments[3] | 1<<segments[5] | 1<<segments[6];
const byte numbers[10] = {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE}; 

void displayNumber(uint8_t number) {
  if (number > 9) return;
  setExpanderRegister(expanderId7Seg, outRegisterAId, ~numbers[number]);
}

void clearDisplay() {
  setExpanderRegister(expanderId7Seg, outRegisterAId, 0xFF);
}
