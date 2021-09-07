#ifndef rpi_comm_h
#define rpi_comm_h

#include <Arduino.h>
class RpiComm {
 public:
  RpiComm();
  void init();
  boolean checkData(byte cmd[3]);
  void receiveEvent(int howMany, byte cmd[3]);
  void sendUnlock();
  void sendButtonSelected(byte buttonID);
 private:
   void sendRawData(byte data);
};

#endif