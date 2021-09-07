#include "i2c_communication.hpp"

I2CComm::I2CComm() {
  Wire.begin();
  this->initExpanders();
}

void I2CComm::initExpanders() {
  setExpanderRegister(expanderId7Seg, ddrRegisterAId, 0x00);
  setExpanderRegister(expanderIdButtonLEDs, ddrRegisterAId, 0x00);
  setExpanderRegister(expanderIdButtonLEDs, ddrRegisterBId, 0x00);
}

void I2CComm::setExpanderRegister(byte chip, byte register_, byte data) {
  sendI2CPacket(chip, register_, data);
}

void I2CComm::turnOffAll(){
  setExpanderRegister(expanderIdButtonLEDs, outRegisterAId, 0x00);
  setExpanderRegister(expanderIdButtonLEDs, outRegisterBId, 0x00);
}

void I2CComm::turnOnSingle(ButtonC button_){
  byte outReg = outRegisterAId;
  byte offReg = outRegisterBId;
  if (button_.ledPort != 0) {
    outReg = outRegisterBId;
    offReg = outRegisterAId;
  }
  setExpanderRegister(expanderIdButtonLEDs, outReg, 1<<button_.ledPin);
  setExpanderRegister(expanderIdButtonLEDs, offReg, 0x00);
}

void I2CComm::sendI2CPacket(byte address, byte messageType, byte payload) {
  Wire.beginTransmission(address);
  Wire.write(messageType);
  Wire.write(payload);
  Wire.endTransmission();
}
