#include "7segment.h"

void displayNumber(uint8_t number, bool setDot) {
  if (number > 9) return;
  byte output = numbers[number];
  if (setDot) output = output | 1<<segments[7];
  setExpanderRegister(expanderId7Seg, outRegisterAId, ~output);
}

void clearDisplay(bool setDot) {
  byte output = 0x00;
  if (setDot) output = output | 1<<segments[7];
  setExpanderRegister(expanderId7Seg, outRegisterAId, ~output);
}
