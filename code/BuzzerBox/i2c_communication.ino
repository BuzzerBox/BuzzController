#include "portConfig.h"

void initExpanders() {
  setExpanderRegister(expanderId7Seg, ddrRegisterAId, 0x00);
  setExpanderRegister(expanderIdButtonLEDs, ddrRegisterAId, 0x00);
  setExpanderRegister(expanderIdButtonLEDs, ddrRegisterBId, 0x00);
}

void setExpanderRegister(byte chip, byte register_, byte data) {
  sendI2CPacket(chip, register_, data);
}

void sendPacketToPi(Message_req_t messageType, byte payload) {
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

PacketType getDataFromPi() {
  Wire.requestFrom(piI2CAddress, (uint8_t)3);
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
