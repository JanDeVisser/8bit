//
// Created by jan on 2021-01-26.
//

#ifndef EMU_CONTROLLER_H
#define EMU_CONTROLLER_H

#include <map>
#include "register.h"
#include "systembus.h"

constexpr static byte OP_NONE = 0x00;
constexpr static byte OP_DONE = 0xF0;
constexpr static byte OP_MASK = 0x0F;
constexpr static byte OP_HALT = 0x0F;



struct MicroCode {
  enum Action {
    XDATA = 0x01,
    XADDR = 0x02,
    IO    = 0x04,
    OTHER = 0x08
  };

  struct MicroCodeStep {
    Action action;
    byte   src;
    byte   target;
    byte   opflags;
  };

  byte           opcode;
  const char    *instruction;
  MicroCodeStep  steps[16];
};


class Controller : public Register {
private:
  byte       step = 0;
  MicroCode *microCode;
//  std::map<byte, std::vector<MicroCode>> microcode;

  const MicroCode::MicroCodeStep & findMicroCodeStep();

public:
  explicit    Controller(MicroCode *);
  std::string name() const override { return "IR"; }
  std::string instruction() const;
  int         getStep() const { return step; }

  SystemError status() override;
  SystemError reset() override;
  SystemError onLowClock() override;

  constexpr static int EV_STEPCHANGED = 0x02;
};

#endif //EMU_CONTROLLER_H
