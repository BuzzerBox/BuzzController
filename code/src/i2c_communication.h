#ifndef i2c_h
#define i2c_h

#include <Arduino.h>
#include "portConfig.h"
#include <Wire.h>

void receiveEvent(int howMany, byte cmd[3]);
void initExpanders();
void setExpanderRegister(byte chip, byte register_, byte data);
void turnOffLEDs();
void turnOnSingleLED(ButtonC button_);
void sendI2CPacket(byte address, byte messageType, byte payload);

#endif