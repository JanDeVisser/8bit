#ifndef EMU_BACKPLANE_H
#define EMU_BACKPLANE_H

#include <functional>
#include <vector>
#include "clock.h"
#include "controller.h"
#include "memory.h"
#include "systembus.h"


class BackPlane : public ComponentContainer {
private:
  enum ClockPhase {
    SystemClock = 0x00,
    IOClock = 0x01,
  };
  Clock               clock;
  ClockPhase          m_phase = SystemClock;
  std::ostream       *m_output = nullptr;

  SystemError         onClockEvent(const ComponentHandler&);

protected:
  SystemError         reportError() override;

public:
                      BackPlane();
                      ~BackPlane() override = default;
  void                run(word = 0x0000);
  void                stop() { clock.stop(); }
  Controller::RunMode runMode() const;
  void                setRunMode(Controller::RunMode runMode) const;
  Controller *        controller() const;
  Memory *            memory() const;
  void                loadImage(word, const byte *, word addr = 0, bool writable = true);
  void                setOutputStream(std::ostream &os) { m_output = &os; }

  std::ostream &      status(std::ostream &) override;
  SystemError         reset() override;
  SystemError         onRisingClockEdge() override;
  SystemError         onHighClock() override;
  SystemError         onFallingClockEdge() override;
  SystemError         onLowClock() override;

  bool                setClockSpeed(double);
  double              clockSpeed();

  void                defaultSetup();
};

#endif //EMU_BACKPLANE_H
