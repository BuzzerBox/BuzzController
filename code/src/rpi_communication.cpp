#include "rpi_communication.hpp"


RpiComm::RpiComm() {
    Serial.begin(115200);
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

void RpiComm::sendData(byte data) {
    Serial.write(data);
}