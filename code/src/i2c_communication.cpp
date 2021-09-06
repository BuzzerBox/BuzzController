#include "i2c_communication.h"

void receiveEvent(int howMany, byte cmd[3]) {
  for (uint8_t i = 0; i < 3; i++) {
    cmd[i] = Serial.read();
  }

}

void initExpanders() {
  setExpanderRegister(expanderId7Seg, ddrRegisterAId, 0x00);
  setExpanderRegister(expanderIdButtonLEDs, ddrRegisterAId, 0x00);
  setExpanderRegister(expanderIdButtonLEDs, ddrRegisterBId, 0x00);
}

void setExpanderRegister(byte chip, byte register_, byte data) {
  sendI2CPacket(chip, register_, data);
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

void sendI2CPacket(byte address, byte messageType, byte payload) {
  Wire.beginTransmission(address);
  Wire.write(messageType);
  Wire.write(payload);
  Wire.endTransmission();
}
