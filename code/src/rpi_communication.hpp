#ifndef rpi_comm_h
#define rpi_comm_h

#include <Arduino.h>

typedef enum {   
  NO_CONNECTION = 0,
  JUST_CONNECTED = 1,
  CONNECTED = 2,
  JUST_TIMED_OUT = 3,
} Pi_connection_state_t;

class RpiComm {
 public:
  RpiComm(int heartbeatTimeout = 6000);
  boolean checkData(byte cmd[3]);
  void receiveEvent(int howMany, byte cmd[3]);
  void sendUnlock();
  void sendButtonSelected(uint8_t buttonID);
  void sendRawData(byte data);
  Pi_connection_state_t getConnectionState();
  Pi_connection_state_t advanceConnectionState();
  void setConnectionState(Pi_connection_state_t state);
  void heartbeatDetected();
  bool isTimedOut();

 private:
  Pi_connection_state_t connection_state;
  unsigned long lastConnection;
  int heartbeatTimeout;

};

#endif