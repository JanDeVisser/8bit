//
// Created by jan on 2021-01-25.
//

#ifndef EMU_REGISTER_H
#define EMU_REGISTER_H

#include "systembus.h"

class Register : public ConnectedComponent {
private:
  byte        value = 0;

public:
  explicit    Register(int, std::string && = "");

  void        setValue(byte val);
  int         getValue() const override { return value; }

  SystemError status() override;
  SystemError reset() override;
  SystemError onRisingClockEdge() override;
  SystemError onHighClock() override;

};

#endif //EMU_REGISTER_H
