#ifndef i2c_h
#define i2c_h

#include <Arduino.h>
#include "portConfig.hpp"
#include <Wire.h>

class I2CComm {
 public:
    I2CComm();
    void initExpanders();
    void setExpanderRegister(byte chip, byte register_, byte data);
    void turnOffAll();
    void turnOnSingle(ButtonC button_);
    void sendI2CPacket(byte address, byte messageType, byte payload);
};


#endif