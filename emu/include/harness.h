#ifndef EMU_HARNESS_H
#define EMU_HARNESS_H

#include "systembus.h"

class Harness {
private:
  std::vector<ConnectedComponent *>  m_components;

public:
  SystemBus           bus;
  ConnectedComponent *comp = nullptr;
  bool                printStatus = false;
  SystemError         error;

  explicit Harness() : bus(), m_components() {
    m_components.resize(16);
  };

  explicit Harness(ConnectedComponent *c) : Harness() {
    insert(c);
    comp = c;
  };

  void insert(ConnectedComponent *component) {
    component -> bus(&bus);
    m_components[component -> id()] = component;
  }

  ConnectedComponent * component(int ix) {
    return m_components[ix];
  }

  int run(bool debug = false, int cycles = -1) {
    bool oldPrintStatus = printStatus;
    printStatus = debug;
    status("Starting Condition", 0);
    error = NoError;
    int i = 0;
    do {
      error = cycle(i);
      if (error != NoError) {
        goto exit;
      }
      i++;
    } while (bus.halt() && ((cycles == -1) || (i < cycles)));
  exit:
    printStatus = oldPrintStatus;
    return i;
  }

  SystemError cycles(int count) {
    status("Starting Condition", 0);
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
    auto error = bus.status();
    if (error == NoError) {
      for (auto &component : m_components) {
        if (!component) continue;
        error = component->status();
        if (error != NoError) {
          return error;
        }
      }
    }
    return NoError;
  }

  SystemError onRisingClockEdge() {
    for (auto & component : m_components) {
      if (!component) continue;
      auto err = component -> onRisingClockEdge();
      if (err != NoError) {
        return err;
      }
    }
    return NoError;
  }

  SystemError onHighClock() {
    for (auto & component : m_components) {
      if (!component) continue;
      auto error = component -> onHighClock();
      if (error != NoError) {
        return error;
      }
    }
    return NoError;
  }

  SystemError onFallingClockEdge() {
    for (auto & component : m_components) {
      if (!component) continue;
      auto error = component -> onFallingClockEdge();
      if (error != NoError) {
        return error;
      }
    }
    return NoError;
  }

  SystemError onLowClock() {
    for (auto & component : m_components) {
      if (!component) continue;
      auto error = component -> onLowClock();
      if (error != NoError) {
        return error;
      }
    }
    return NoError;
  }

  SystemError cycle(int num) {
    SystemError err = onRisingClockEdge();
    if (err == NoError) {
      err = onHighClock();
      if (err == NoError) {
        status("After onHighClock", num);
        err = onFallingClockEdge();
        if (err == NoError) {
          err = onLowClock();
          status("After onLowClock", num);
        }
      }
    }
    return err;
  }

  SystemError cycle(bool xdata, bool xaddr, byte getReg, byte putReg, byte opflags_val,
                    byte data_bus_val = 0x00, byte addr_bus_val = 0x00) {
    bus.initialize(xdata, xaddr, getReg, putReg, opflags_val, data_bus_val, addr_bus_val);
    return cycle(0);
  }
};

#endif //EMU_HARNESS_H
