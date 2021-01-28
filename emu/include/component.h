//
// Created by jan on 2021-01-25.
//

#ifndef EMU_COMPONENT_H
#define EMU_COMPONENT_H

#include <iomanip>
#include <iostream>

typedef unsigned char byte;
typedef unsigned short word;

enum SystemError {
  NoError,
  InvalidComponentID,
  ProtectedMemory,
  InvalidInstruction,
  InvalidMicroCode,
  NoMicroCode,
  GeneralError,
};

class Component {
public:
  virtual ~Component() = default;

  virtual SystemError status() { return NoError; }
  virtual SystemError reset() { return NoError; }
  virtual SystemError onRisingClockEdge() { return NoError; }
  virtual SystemError onHighClock() { return NoError; }
  virtual SystemError onFallingClockEdge() { return NoError; }
  virtual SystemError onLowClock() { return NoError; }
};

std::ostream & printhex(std::ostream &, byte);
std::ostream & printhex(std::ostream &, word);

#endif //EMU_COMPONENT_H
