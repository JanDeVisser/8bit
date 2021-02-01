#include <iostream>

#include "addressregister.h"

AddressRegister::AddressRegister(int registerID, std::string n) : ConnectedComponent(registerID, n) {
}

void AddressRegister::setValue(word val) {
  value = val;
  sendEvent(EV_VALUECHANGED);
}


SystemError AddressRegister::status() {
  printf("%2s %04x\n", name().c_str(), value);
  return NoError;
}

SystemError AddressRegister::reset() {
  value = 0;
  return NoError;
}

SystemError AddressRegister::onRisingClockEdge() {
  if (bus()->getID() == id()) {
    if (!bus()->xdata()) {
      if (bus()->opflags() & SystemBus::OP_MSB) {
        bus()->putOnDataBus((value & 0xFF00) >> 8);
      } else {
        bus()->putOnDataBus(value & 0x00FF);
      }
    } else if (!bus()->xaddr()) {
      bus()->putOnDataBus(value & 0x00FF);
      bus()->putOnAddrBus((value & 0xFF00) >> 8);
      if (bus()->opflags() & SystemBus::OP_INC) {
        setValue(value+1);
      } else if (bus()->opflags() & SystemBus::OP_DEC) {
        setValue(value-1);
      }
    }
  }
  return NoError;
}

SystemError AddressRegister::onHighClock() {
  if (!bus()->xdata() && (bus()->putID() == id())) {
    if (!(bus()->opflags() & SystemBus::OP_MSB)) {
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
