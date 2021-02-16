#ifndef EMU_BACKPLANE_H
#define EMU_BACKPLANE_H

#include <functional>
#include <vector>
#include "clock.h"
#include "controller.h"
#include "systembus.h"


class BackPlane : public Component, public ComponentContainer {
private:
  enum ClockPhase {
    SystemClock = 0x00,
    IOClock = 0x01,
  };
  Clock                              clock;
  ClockPhase                         m_phase = SystemClock;


  SystemError   onClockEvent(const ComponentHandler&);

protected:
  SystemError   reportError() override;

public:
                          BackPlane();
                          ~BackPlane() override = default;
  void                    run();
  void                    stop() { clock.stop(); }
  Controller::RunMode     runMode() const;
  void                    setRunMode(Controller::RunMode runMode) const;
  Controller *            controller() const;

  SystemError             status() override;
  SystemError             reset() override;
  SystemError             onRisingClockEdge() override;
  SystemError             onHighClock() override;
  SystemError             onFallingClockEdge() override;
  SystemError             onLowClock() override;

  void                    defaultSetup();

};

#endif //EMU_BACKPLANE_H
