#include "rpi_communication.hpp"
#define noop

RpiComm::RpiComm(int heartbeatTimeout) {
    this->heartbeatTimeout = heartbeatTimeout;
    Serial.begin(115200);
    connection_state = NO_CONNECTION;
}

void RpiComm::sendRawData(byte data) {
    Serial.write(data);
}

boolean RpiComm::checkData() {
  boolean hasCompleteMessage = false;
  if (Serial.available()) {
    byte readByte = Serial.read();
    if (readByte == 0xFF) {
      heartbeatDetected();
      if (cmd_fill == 2) {
        hasCompleteMessage = true;
      }
      cmd_fill = 0;
    } else if (cmd_fill == 0) {
      current_command.command = readByte;
      cmd_fill = 1;
    } else if (cmd_fill == 1) {
      current_command.data = readByte;
      cmd_fill = 2;
    } else {
      // Frame too long: discard everything until the next 0xFF
      cmd_fill = 3;
    }
  }
  return hasCompleteMessage;
}

PiCmd RpiComm::getLastCmd() {
  return current_command;
}

void RpiComm::sendUnlock() {
   this->sendRawData('q');
}


void RpiComm::sendButtonSelected(byte buttonID) {
  this->sendRawData(buttonID + 48);
}

void RpiComm::heartbeatDetected() {
  switch(connection_state) {
    case NO_CONNECTION : 
      connection_state = JUST_CONNECTED;
      break;
    case JUST_TIMED_OUT :
      connection_state = JUST_CONNECTED;
      break;
    default: 
      noop;
  }
  lastConnection = millis();
}

Pi_connection_state_t RpiComm::getConnectionState() {
  return connection_state;
}

void RpiComm::setConnectionState(Pi_connection_state_t state) {
  connection_state = state;
}

bool RpiComm::isTimedOut() {
  if (connection_state == CONNECTED) {
    if (millis() - lastConnection > (unsigned long)heartbeatTimeout) {
      connection_state = JUST_TIMED_OUT;
      return true;
    }
  }
  return false;
}
