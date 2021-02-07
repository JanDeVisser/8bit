#include <iostream>

#include "addressregister.h"

AddressRegister::AddressRegister(int registerID, std::string n) : ConnectedComponent(registerID, n) {
}

void AddressRegister::setValue(word val) {
  value = val;
  sendEvent(EV_VALUECHANGED);
}


SystemError AddressRegister::status() {
  printf("%1x. %2s %04x\n", id(), name().c_str(), value);
  return NoError;
}

SystemError AddressRegister::reset() {
  value = 0;
  sendEvent(EV_VALUECHANGED);
  return NoError;
}

SystemError AddressRegister::onRisingClockEdge() {
  if (bus()->getID() == id()) {
    if (!bus()->xdata()) {
      if (bus()->opflags() & SystemBus::MSB) {
        bus()->putOnDataBus((value & 0xFF00) >> 8);
      } else {
        bus()->putOnDataBus(value & 0x00FF);
      }
    } else if (!bus()->xaddr()) {
      if (bus()->opflags() & SystemBus::Dec) {
        setValue(value - 1);
      }
      bus()->putOnDataBus(value & 0x00FF);
      bus()->putOnAddrBus((value & 0xFF00) >> 8);
      if (bus()->opflags() & SystemBus::Inc) {
        setValue(value+1);
      }
    }
  }
  return NoError;
}

SystemError AddressRegister::onHighClock() {
  if (!bus()->xdata() && (bus()->putID() == id())) {
    if (!(bus()->opflags() & SystemBus::MSB)) {
      value &= 0xFF00;
      setValue(value | bus()->readDataBus());
    } else {
      value &= 0x00FF;
      setValue(value | ((word) bus()->readDataBus()) << 8);
    }
  } else if (!bus()->xaddr() && (bus()->putID() == id())) {
    setValue((word) ((bus()->readAddrBus() << 8) | bus()->readDataBus()));
  }
  return NoError;
}