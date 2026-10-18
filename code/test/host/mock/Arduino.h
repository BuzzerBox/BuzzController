// Just enough of the Arduino core and the ATmega328P registers to run BuzzerBox.cpp on the host, see test_buzzers.cpp
#ifndef mock_arduino_h
#define mock_arduino_h
#include <stdint.h>
#include <stddef.h>
// the binary constants BuzzerBox.cpp uses, from the core's binary.h
#define B00000111 7
#define B00001010 10
#define B11111000 248
#define B00000000 0

typedef uint8_t byte;
typedef bool boolean;

extern uint8_t PINB, PINC, PIND, DDRB, DDRC, DDRD, PORTB, PORTC, PORTD;
extern uint8_t PCMSK0, PCMSK1, PCMSK2, PCICR, PCIFR;
extern uint32_t mockMillis;
inline uint32_t millis() { return mockMillis; }
inline void cli() {}
inline void sei() {}
const uint8_t A0 = 14;
#endif
