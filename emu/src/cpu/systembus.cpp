#include "systembus.h"

SystemError SystemBus::reset() {
  _xdata = true;
  _xaddr = true;
  get = 0;
  put = 0;
  op = 0;
  data_bus = 0;
  addr_bus = 0;
  _sus = true;
  _sack = true;
  rst = false;
  _io = true;
  _halt = true;
  sendEvent(EV_VALUECHANGED);
  return NoError;
}

byte SystemBus::readDataBus() const {
  return data_bus;
}

void SystemBus::putOnDataBus(byte value) {
  data_bus = value;
  sendEvent(EV_VALUECHANGED);
}

byte SystemBus::readAddrBus() const {
  return addr_bus;
}

void SystemBus::putOnAddrBus(byte value) {
  addr_bus = value;
  sendEvent(EV_VALUECHANGED);
}

void SystemBus::initialize(bool xdata, bool xaddr,
                           byte getReg, byte putReg, byte opflags_val,
                           byte data_bus_val, byte addr_bus_val) {
  _xdata = xdata;
  _xaddr = xaddr;
  get = getReg;
  put = putReg;
  op = opflags_val;
  data_bus = data_bus_val;
  addr_bus = addr_bus_val;
  sendEvent(EV_VALUECHANGED);
}

void SystemBus::xdata(int from, int to, int opflags) {
  _xdata = false;
  _xaddr = true;
  get = from;
  put = to;
  op = opflags;
  sendEvent(EV_VALUECHANGED);
}

void SystemBus::xaddr(int from, int to, int opflags) {
  _xdata = true;
  _xaddr = false;
  get = from;
  put = to;
  op = opflags;
  sendEvent(EV_VALUECHANGED);
}

void SystemBus::stop() {
  _halt = false;
  sendEvent(EV_VALUECHANGED);
}


SystemError SystemBus::status() {
  printf("DATA ADDR  GET PUT OP ACT\n");
  printf(" %02x   %02x    %01x   %01x  %01x   %c\n",
         data_bus, addr_bus, get, put, op,
         (_xdata) ? ((_xaddr) ? '_' : 'A') : 'D');
  printf("=========================\n");
  return NoError;
}

