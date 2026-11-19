// Runs the firmware's buzzer logic on the host with mocked registers:
//   g++ -std=gnu++11 -Imock -I../../src test_buzzers.cpp -o test_buzzers && ./test_buzzers
#include <cstdio>
#include <vector>
#include "Arduino.h"

uint8_t PINB = 0xFF, PINC = 0xFF, PIND = 0xFF, DDRB, DDRC, DDRD, PORTB, PORTC, PORTD;
uint8_t PCMSK0, PCMSK1, PCMSK2, PCICR, PCIFR;
uint32_t mockMillis = 1;

#include "rpi_communication.hpp"
#include "7segment.hpp"
#include "i2c_communication.hpp"

// what the firmware sent to the Pi, and the commands the Pi sends it
std::vector<int> sent;
std::vector<PiCmd> commands;
RpiComm::RpiComm(int heartbeatTimeout) { this->heartbeatTimeout = heartbeatTimeout; connection_state = CONNECTED; }
boolean RpiComm::checkData() { if (commands.empty()) return false; current_command = commands.front(); commands.erase(commands.begin()); return true; }
PiCmd RpiComm::getLastCmd() { return current_command; }
void RpiComm::sendUnlock() { sent.push_back('q'); }
void RpiComm::sendButtonSelected(uint8_t buttonID) { sent.push_back('0' + buttonID); }
void RpiComm::sendRawData(byte data) { sent.push_back(data); }
Pi_connection_state_t RpiComm::getConnectionState() { return connection_state; }
void RpiComm::setConnectionState(Pi_connection_state_t state) { connection_state = state; }
void RpiComm::heartbeatDetected() {}
bool RpiComm::isTimedOut() { return false; }
SevenSeg::SevenSeg(bool, I2CComm*) {}
void SevenSeg::displayNumber(uint8_t, bool) {}
void SevenSeg::clearDisplay(bool) {}
I2CComm::I2CComm() {}
void I2CComm::turnOffAll() {}
void I2CComm::turnOnSingle(ButtonC) {}

#include "BuzzerBox.cpp"

// where each buzzer is wired, see buttonsB/C/D in portConfig.hpp
struct Pin { uint8_t* reg; uint8_t bit; uint8_t pcie; void (*isr)(); };
Pin pinOf(uint8_t buzzer) {
  const uint8_t* ports[3] = {buttonsB, buttonsC, buttonsD};
  uint8_t* regs[3] = {&PINB, &PINC, &PIND};
  uint8_t selects[3] = {b_select, c_select, d_select};
  void (*isrs[3])() = {PCINT0_vect, PCINT1_vect, PCINT2_vect};
  for (uint8_t p = 0; p < 3; p++)
    for (uint8_t bit = 0; bit < 8; bit++)
      if (((selects[p] >> bit) & 1) && ports[p][bit] == buzzer) return {regs[p], bit, p, isrs[p]};
  return {nullptr, 0, 0, nullptr};
}

void run(uint32_t ms) { for (uint32_t i = 0; i < ms; i++) { mockMillis++; loop(); } }
void edge(Pin pin) { if ((PCICR >> pin.pcie) & 1) pin.isr(); }
void press(uint8_t buzzer) { Pin pin = pinOf(buzzer); *pin.reg &= ~(1 << pin.bit); edge(pin); }
void release(uint8_t buzzer) { Pin pin = pinOf(buzzer); *pin.reg |= (1 << pin.bit); edge(pin); }
void command(byte cmd, byte data) { commands.push_back({cmd, data}); run(1); }
int lastSent() { return sent.empty() ? -1 : sent.back(); }

int failures = 0;
void check(bool ok, const char* what) { printf("%s %s\n", ok ? "ok  " : "FAIL", what); if (!ok) failures++; }

void reset() {
  PINB = PINC = PIND = 0xFF; PCICR = 0;
  if (isLocked) unlock();
  disabledButtons = 0; ignoredUntilReleased = 0;
  enableButtonInterrupts(); sent.clear();
}

int main() {
  setup();
  // buzzers 2 and 3 are on port D, 2 on the lower pin, so it would win a tie
  reset(); command(DISABLE_BUZZER, 2); press(2); run(50);
  check(!isLocked && sent.empty(), "a disabled buzzer doesn't lock");
  check((PCICR & 0b111) == 0b111, "the others can buzz after it");
  press(3); run(50);
  check(isLocked && lastSent() == '3', "an enabled buzzer locks while a disabled one is held on the same port");

  reset(); command(DISABLE_BUZZER, 2);
  PIND &= ~((1 << pinOf(2).bit) | (1 << pinOf(3).bit)); edge(pinOf(2)); run(50);
  check(isLocked && lastSent() == '3', "pressed at the same edge, the enabled buzzer wins");

  command(SOFT_RELEASE, 0); release(2); release(3); run(50);
  press(2); run(50);
  check(!isLocked, "a release keeps the buzzer disabled");

  // buzzer 2 is still held down when all are enabled again
  command(ENABLE_ALL_BUZZERS, 0); run(50);
  check(!isLocked, "enabling doesn't lock a buzzer that is held down");
  press(4); run(50);
  check(isLocked && lastSent() == '4', "a held buzzer doesn't win once it is enabled");
  command(SOFT_RELEASE, 0); release(4); release(2); run(50);
  press(2); run(50);
  check(isLocked && lastSent() == '2', "it counts again once it was released");

  reset(); command(DISABLE_BUZZER, 7); command(SET_BUZZER, 7);
  check(isLocked && lastSent() == '7', "the Pi can still set a disabled buzzer");

  reset(); command(DISABLE_BUZZER, 12); press(9); run(50);
  check(isLocked && lastSent() == '9' && disabledButtons == 0, "IDs above 9 are ignored");

  reset(); command(DISABLE_BUZZER, 1); command(DISABLE_BUZZER, 8); press(1); run(50); release(1); run(50); press(8); run(50);
  check(!isLocked, "several buzzers can be disabled, also on port C next to the release button");
  release(8); command(ENABLE_ALL_BUZZERS, 0); press(8); run(50);
  check(isLocked && lastSent() == '8', "enabling all lets them buzz again");

  printf(failures ? "%d failed\n" : "all passed\n", failures);
  return failures ? 1 : 0;
}
