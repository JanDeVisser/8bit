//
// Created by jan on 2021-01-26.
//

#ifndef EMU_CONTROLLER_H
#define EMU_CONTROLLER_H

#include <map>
#include "register.h"
#include "system.h"

constexpr static byte OP_NONE = 0x00;
constexpr static byte OP_DONE = 0xF0;
constexpr static byte OP_MASK = 0x0F;
constexpr static byte OP_HALT = 0x0F;

struct MicroCode {
  enum Action {
    XDATA,
    XADDR,
    IO,
    OTHER
  };
  byte   instruction;
  byte   step;
  Action action;
  byte   src;
  byte   target;
  byte   opflags;
};

class Controller : public Register {
private:
  byte       step = 0;
  MicroCode *microCode;
//  std::map<byte, std::vector<MicroCode>> microcode;

  MicroCode * findMicroCode();

public:
  Controller(System *s, MicroCode *);

  SystemError status() override;
  SystemError reset() override;
  SystemError onLowClock() override;
};

#endif //EMU_CONTROLLER_H
