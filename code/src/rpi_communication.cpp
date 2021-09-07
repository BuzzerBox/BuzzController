#include "rpi_communication.hpp"


RpiComm::RpiComm() {
    Serial.begin(115200);
}

void RpiComm::sendRawData(byte data) {
    Serial.write(data);
}

//TODO make resilent against missing bytes with the third termination byte.
boolean RpiComm::checkData(byte cmd[3]) {
  if (Serial.available() >= 3) {
    this->receiveEvent(3, cmd);
    return true;
  }
  return false;
}


void RpiComm::receiveEvent(int howMany, byte cmd[3]) {
  for (uint8_t i = 0; i < 3; i++) {
    cmd[i] = Serial.read();
  }
}  

void RpiComm::sendUnlock() {
   this->sendRawData('q');
}


void RpiComm::sendButtonSelected(byte buttonID) {
  this->sendRawData(buttonID + 48);
}