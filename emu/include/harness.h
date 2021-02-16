#ifndef EMU_HARNESS_H
#define EMU_HARNESS_H

#include "systembus.h"

class Harness : public ComponentContainer {
public:
  bool                printStatus = false;

  explicit Harness() : ComponentContainer() {
  };

  explicit Harness(ConnectedComponent *c) : ComponentContainer(c) {
  }

  int run(bool debug = false, int cycles = -1) {
    bool oldPrintStatus = printStatus;
    printStatus = debug;
//    status("Starting Condition", 0);
    error(NoError);
    int i = 0;
    do {
      error(cycle(i));
      if (error() != NoError) {
        goto exit;
      }
      i++;
    } while (bus().halt() && ((cycles == -1) || (i < cycles)));
  exit:
    printStatus = oldPrintStatus;
    return i;
  }

  SystemError cycles(int count) {
//    status("Starting Condition", 0);
    for (int i = 0; i < count; i++) {
      auto err = cycle(i);
      if (err != NoError) {
        return err;
      }
    }
    return NoError;
  }

  SystemError status(const std::string &msg, int num) {
    if (!printStatus) return NoError;
    std::cout << "Cycle " << num << " " << msg << std::endl;
    auto err = bus().status();
    if (err == NoError) {
      err = forAllComponents([](Component *c) -> SystemError {
        return c -> status();
      });
    }
    return err;
  }

  SystemError onRisingClockEdge() {
    return forAllComponents([](Component *c) -> SystemError {
      return c -> onRisingClockEdge();
    });
  }

  SystemError onHighClock() {
    return forAllComponents([](Component *c) -> SystemError {
      return c -> onHighClock();
    });
  }

  SystemError onFallingClockEdge() {
    return forAllComponents([](Component *c) -> SystemError {
      return c -> onFallingClockEdge();
    });
  }

  SystemError onLowClock() {
    return forAllComponents([](Component *c) -> SystemError {
      return c -> onLowClock();
    });
  }

  SystemError cycle(int num) {
    SystemError err = onRisingClockEdge();
    if (err == NoError) {
      err = onHighClock();
      if (err == NoError) {
//        status("After onHighClock", num);
        err = onFallingClockEdge();
        if (err == NoError) {
          err = onLowClock();
//          status("After onLowClock", num);
        }
      }
    }
    return err;
  }

  SystemError cycle(bool xdata, bool xaddr, byte getReg, byte putReg, byte opflags_val,
                    byte data_bus_val = 0x00, byte addr_bus_val = 0x00) {
    bus().initialize(xdata, xaddr, getReg, putReg, opflags_val, data_bus_val, addr_bus_val);
    return cycle(0);
  }
};

#endif //EMU_HARNESS_H
