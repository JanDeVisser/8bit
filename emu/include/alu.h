#ifndef EMU_ALU_H
#define EMU_ALU_H

#include <functional>
#include "register.h"

class ALU;

typedef std::function<word(ALU *)> Operator;

class ALU : public Register {
  Operator         m_operator = nullptr;
  Register        *m_lhs;

private:
  void    setOverflow(word);

public:
  enum Operations {
    ADD = 0x00,
    ADC = 0x01,
    SUB = 0x02,
    SBB = 0x03,
    AND = 0x08,
    OR = 0x09,
    XOR = 0x0A,
    NOT = 0x0B,
    SHL = 0x0C,
    SHR = 0x0D,
    CLR = 0x0E,
  };

  ALU(int, Register *lhs);
  Register * lhs() const { return m_lhs; }

  SystemError onHighClock() override;
};


#endif //EMU_ALU_H
