//
// Created by jan on 2021-01-25.
//

#ifndef EMU_LINES_H
#define EMU_LINES_H

#include "component.h"

class ControlLines {
public:
  byte              put = 0;
  byte              get = 0;
  byte              op = 0;
  bool              _halt = true;
  bool              _sus = true;
  bool              _sack = true;
  bool              _xdata = true;
  bool              _xaddr = true;
  bool              rst = false;
  bool              _io = true;

  ControlLines();
  ControlLines(const ControlLines &);
  void              reset();
  void              copy(const ControlLines &);

  ControlLines & operator = (const ControlLines &);
};

#endif //EMU_LINES_H
