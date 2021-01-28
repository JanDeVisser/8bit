#ifndef EMU_MOCKSYSTEM_H
#define EMU_MOCKSYSTEM_H

#include "system.h"

class MockSystem : public System {
public:
  byte data_bus =0x00;
  byte addr_bus = 0x00;
  bool _xdata = true;
  bool _xaddr = true;
  byte get = 0x0;
  byte put = 0x0;
  byte op = 0x0;

  Component *reg = nullptr;

  MockSystem() = default;

  byte readBus() const override {
    return data_bus;
  }

  void putOnBus(byte value) override {
    data_bus = value;
  }

  byte addrBus() const override {
    return addr_bus;
  }

  void putOnAddrBus(byte value) override {
    addr_bus = value;
  }

  bool xdata() const override {
    return _xdata;
  }

  bool xaddr() const override {
    return _xaddr;
  }

  byte putID() const override {
    return put;
  }

  byte getID() const override {
    return get;
  }

  byte opflags() const override {
    return op;
  }

  void xdata(int from, int to, int opflags) override {
    _xdata = false;
    _xaddr = true;
    get = from;
    put = to;
    op = opflags;
  }

  void xaddr(int from, int to, int opflags) override {
    _xdata = true;
    _xaddr = false;
    get = from;
    put = to;
    op = opflags;
  }


  void run() override {
  }

  void stop() override {
  }

  SystemError cycle(bool xdata, bool xaddr, byte getReg, byte putReg, byte opflags_val,
                    byte data_bus_val = 0x00, byte addr_bus_val = 0x00) {
    _xdata = xdata;
    _xaddr = xaddr;
    get = getReg;
    put = putReg;
    op = opflags_val;
    data_bus = data_bus_val;
    addr_bus = addr_bus_val;

    SystemError err = reg -> onRisingClockEdge();
    if (err == NoError) {
      err = reg -> onHighClock();
      if (err == NoError) {
        err = reg -> onFallingClockEdge();
        if (err == NoError) {
          err = reg -> onLowClock();
        }
      }
    }
    return err;
  }
};

#endif //EMU_MOCKSYSTEM_H
