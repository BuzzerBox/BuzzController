#pragma once
const int BUTTON_COUNT = 10;
const int buttonPins[BUTTON_COUNT] = {5,6,7,8,9,10,11,12,13,14}; 
const int resetButton = A0;

 
const byte expanderIdButtons = 0x20;
const byte expanderId7Seg = 0x21;
const byte piI2CAddress = 0x22;

const byte ddrRegisterId = 0x00;
const byte outRegisterAId = 0x14;
const byte outRegisterBId = 0x15;
