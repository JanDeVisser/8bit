#include <iostream>

#include "register.h"

Register::Register(int registerID)
    : ConnectedComponent(registerID, std::string(1, 'A' + registerID)) {
}

void Register::setValue(byte val) {
  value = val;
  sendEvent(EV_VALUECHANGED);
}


SystemError Register::status() {
  printf("%s  %02x\n", name().c_str(), value);
  return NoError;
}

SystemError Register::reset() {
  value = 0;
  return NoError;
}

SystemError Register::onRisingClockEdge() {
  if (!bus()->xdata() && (bus()->getID() == id())) {
    bus()->putOnDataBus(value);
  }
  return NoError;
}

SystemError Register::onHighClock() {
  if (!bus()->xdata() && (bus()->putID() == id())) {
    setValue(bus()->readDataBus());
  }
  return NoError;
}
