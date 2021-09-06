#include "7segment.hpp"
I2CComm* i2c_comm;

SevenSeg::SevenSeg(bool setDot, I2CComm* i2cComm) {
    i2c_comm = i2cComm;
    clearDisplay(setDot);
}

void SevenSeg::displayNumber(uint8_t number, bool setDot) {
  if (number > 9) return;
  byte output = numbers[number];
  if (setDot) output = output | 1<<segments[7];
  i2c_comm->setExpanderRegister(expanderId7Seg, outRegisterAId, ~output);
}

void SevenSeg::clearDisplay(bool setDot) {
  byte output = 0x00;
  if (setDot) output = output | 1<<segments[7];
  i2c_comm->setExpanderRegister(expanderId7Seg, outRegisterAId, ~output);
}
