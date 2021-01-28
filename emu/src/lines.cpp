#include "lines.h"

ControlLines::ControlLines() {
  reset();
}

ControlLines::ControlLines(const ControlLines &other) {
  copy(other);
}

void ControlLines::reset() {
  put = 0;
  get = 0;
  op = 0;
  _halt = true;
  _sus = true;
  _sack = true;
  _xdata = true;
  _xaddr = true;
  rst = false;
  _io = true;
}

void ControlLines::copy(const ControlLines &other) {
  put = other.put;
  get = other.get;
  op = other.get;
  _halt = other._halt;
  _sus = other._sus;
  _sack = other._sack;
  _xdata = other._xdata;
  _xaddr = other._xaddr;
  rst = other.rst;
  _io = other._io;
}

ControlLines & ControlLines::operator = (const ControlLines &rhs) {
  if (&rhs != this) {
    copy(rhs);
  }
  return *this;
}

