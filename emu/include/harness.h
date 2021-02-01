#ifndef EMU_HARNESS_H
#define EMU_HARNESS_H

#include "systembus.h"

class Harness {
public:
  SystemBus           bus;
  ConnectedComponent *comp = nullptr;

  explicit Harness(ConnectedComponent *c) : bus() {
    comp = c;
    comp->bus(&bus);
  };

  SystemError cycle(bool xdata, bool xaddr, byte getReg, byte putReg, byte opflags_val,
                    byte data_bus_val = 0x00, byte addr_bus_val = 0x00) {
    bus.initialize(xdata, xaddr, getReg, putReg, opflags_val, data_bus_val, addr_bus_val);
    SystemError err = comp -> onRisingClockEdge();
    if (err == NoError) {
      err = comp -> onHighClock();
      if (err == NoError) {
        err = comp -> onFallingClockEdge();
        if (err == NoError) {
          err = comp -> onLowClock();
        }
      }
    }
    return err;
  }
};

#endif //EMU_HARNESS_H
