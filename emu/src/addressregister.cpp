#include <iostream>

#include "addressregister.h"

SystemError AddressRegister::status() {
  printf("%2s %04x\n", name.c_str(), value);
  return NoError;
}

SystemError AddressRegister::reset() {
  value = 0;
  return NoError;
}

SystemError AddressRegister::onRisingClockEdge() {
  if (system()->getID() == id()) {
    if (!system()->xdata()) {
      if (system()->opflags() & OP_MSB) {
        system()->putOnBus((value & 0xFF00) >> 8);
      } else {
        system()->putOnBus(value & 0x00FF);
      }
    } else if (!system()->xaddr()) {
      system()->putOnBus(value & 0x00FF);
      system()->putOnAddrBus((value & 0xFF00) >> 8);
      if (system()->opflags() & OP_INC) {
        value++;
      } else if (system()->opflags() & OP_DEC) {
        value--;
      }
    }
  }
  return NoError;
}

SystemError AddressRegister::onHighClock() {
  if (!system()->xdata() && (system()->putID() == id())) {
    if (!(system()->opflags() & OP_MSB)) {
      value &= 0xFF00;
      value |= system()->readBus();
    } else {
      value &= 0x00FF;
      value |= ((word) system()->readBus()) << 8;
    }
  } else if (!system()->xaddr() && (system()->putID() == id())) {
    value = (word) ((system()->addrBus() << 8) | system()->readBus());
  }
  return NoError;
}
