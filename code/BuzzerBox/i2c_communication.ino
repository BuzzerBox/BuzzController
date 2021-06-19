#include "portConfig.h"
struct packetType {
  byte messageType;
  byte payload1;
  byte payload2;
};


void setExpanderRegister(byte chip, byte register_, byte data) {
  sendI2CPacket(chip, register_, data);
}

void sendPacketToPi(byte messageType, byte payload) {
  sendI2CPacket(piI2CAddress, messageType, payload);
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
