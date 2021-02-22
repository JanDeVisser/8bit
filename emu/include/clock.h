//
// Created by jan on 2021-01-25.
//

#ifndef EMU_CLOCK_H
#define EMU_CLOCK_H

#include "component.h"

enum ClockCycleEvent {
  RisingEdge,
  High,
  FallingEdge,
  Low,
};



class ClockListener {
public:
  enum ClockEvent {
    Started,
    Stopped,
    Error,
    FreqChange,
  };
  virtual void clockEvent(ClockEvent event) = 0;
};

class Clock {
public:
  enum Cycle { High, Low };
  enum State { Running, Stopped };

private:
  double           khz;
  Component       *owner;
  Cycle            cycle = Low;
  State            state = Stopped;
  ClockListener   *m_listener = nullptr;

  void             sendEvent(ClockListener::ClockEvent);
  void             sleep() const;


public:
  Clock(Component *o, double speed_khz) : khz(speed_khz), owner(o) { }
  virtual ~Clock() = default;

  double         frequency() { return khz; }
  unsigned long  tick() const;
  SystemError    start();
  void           stop();
  bool           setSpeed(double);

  ClockListener * setListener(ClockListener *);
};

#endif //EMU_CLOCK_H
