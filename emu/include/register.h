//
// Created by jan on 2021-01-25.
//

#ifndef EMU_REGISTER_H
#define EMU_REGISTER_H

#include "system.h"

class Register : public OwnedComponent {
private:
  int     regId;
  byte    value = 0;

public:
  Register(System *s, int registerID) : OwnedComponent(s), regId(registerID) { }

  int id() const override { return regId; }
  void setValue(byte val) { value = val; }
  byte getValue() const { return value; }

  SystemError status() override;
  SystemError reset() override;
  SystemError onRisingClockEdge() override;
  SystemError onHighClock() override;

};

#endif //EMU_REGISTER_H
