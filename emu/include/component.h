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

class Component;

class ComponentListener {
public:
  virtual      ~ComponentListener() = default;
  virtual void  componentEvent(Component *sender, int ev) = 0;
};


class Component {
private:
  ComponentListener *  listener = nullptr;

protected:
  void                 sendEvent(int);
public:
  virtual             ~Component() = default;

  constexpr static int EV_VALUECHANGED = 0;

  ComponentListener *  setListener(ComponentListener *);
  virtual SystemError  status() { return NoError; }
  virtual SystemError  reset() { return NoError; }
  virtual SystemError  onRisingClockEdge() { return NoError; }
  virtual SystemError  onHighClock() { return NoError; }
  virtual SystemError  onFallingClockEdge() { return NoError; }
  virtual SystemError  onLowClock() { return NoError; }
};

#endif //EMU_COMPONENT_H
