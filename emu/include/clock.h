//
// Created by jan on 2021-01-25.
//

#ifndef EMU_CLOCK_H
#define EMU_CLOCK_H

#include "component.h"

class Clock {
public:
  enum Cycle { High, Low };
  enum State { Running, Stopped };

private:
  float            khz;
  Component       *owner;
  Cycle            cycle = Low;
  State            state = Stopped;

  void             sleep() const;


public:
  Clock(Component *o, float speed_khz) : khz(speed_khz), owner(o) { }
  virtual ~Clock() = default;

  unsigned long  tick() const;
  SystemError    start();
  void           stop();
};

#endif //EMU_CLOCK_H
