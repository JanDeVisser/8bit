#include <iostream>

#include "register.h"

SystemError Register::status() {
  printf("%c  %02x\n", "ABCDxxI"[id()], value);
  return NoError;
}

SystemError Register::reset() {
  value = 0;
  return NoError;
}

SystemError Register::onRisingClockEdge() {
  if (!system()->xdata() && (system()->getID() == id())) {
    system()->putOnBus(value);
  }
  return NoError;
}

SystemError Register::onHighClock() {
  if (!system()->xdata() && (system()->putID() == id())) {
    value = system()->readBus();
  }
  return NoError;
}
