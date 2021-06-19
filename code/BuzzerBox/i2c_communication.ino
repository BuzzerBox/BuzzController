#include "portConfig.h"
struct packetType {
  byte messageType;
  byte payload1;
  byte payload2;
};

void initExpanders() {
  setExpanderRegister(expanderId7Seg, ddrRegisterAId, 0x00);
  setExpanderRegister(expanderIdButtonLEDs, ddrRegisterAId, 0x00);
  setExpanderRegister(expanderIdButtonLEDs, ddrRegisterBId, 0x00);
}

void setExpanderRegister(byte chip, byte register_, byte data) {
  sendI2CPacket(chip, register_, data);
}

void sendPacketToPi(byte messageType, byte payload) {
  sendI2CPacket(piI2CAddress, messageType, payload);
}

void turnOffLEDs(){
  setExpanderRegister(expanderIdButtonLEDs, outRegisterAId, 0x00);
  setExpanderRegister(expanderIdButtonLEDs, outRegisterBId, 0x00);
}

void turnOnSingleLED(ButtonC button_){
  byte outReg = outRegisterAId;
  byte offReg = outRegisterBId;
  if (button_.ledPort != 0) {
    outReg = outRegisterBId;
    offReg = outRegisterAId;
  }
  setExpanderRegister(expanderIdButtonLEDs, outReg, 1<<button_.ledPin);
  setExpanderRegister(expanderIdButtonLEDs, offReg, 0x00);
}

struct packetType getDataFromPi() {
  Wire.requestFrom(piI2CAddress, 3);
  packetType newPacket;
  newPacket.messageType = Wire.read();
  newPacket.payload1 = Wire.read();
  newPacket.payload2 = Wire.read();
  return newPacket;
}

void sendI2CPacket(byte address, byte messageType, byte payload) {
  Wire.beginTransmission(address);
  Wire.write(messageType);
  Wire.write(payload);
  Wire.endTransmission();
}
