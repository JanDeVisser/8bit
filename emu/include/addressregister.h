//
// Created by jan on 2021-01-25.
//

#ifndef EMU_ADDRESSREGISTER_H
#define EMU_ADDRESSREGISTER_H

#include <string>
#include <utility>
#include "system.h"

class AddressRegister : public OwnedComponent {
private:
  int          regId;
  word         value = 0;
  std::string  name;

public:
  AddressRegister(System *s, int registerID, std::string n) : OwnedComponent(s), regId(registerID) {
    name = std::move(n);
  }

  int id() const override { return regId; }
  void setValue(word val) { value = val; }
  word getValue() const { return value; }

  SystemError status() override;
  SystemError reset() override;
  SystemError onRisingClockEdge() override;
  SystemError onHighClock() override;
};

#endif //EMU_ADDRESSREGISTER_H
