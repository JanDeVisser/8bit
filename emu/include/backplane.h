#ifndef EMU_BACKPLANE_H
#define EMU_BACKPLANE_H

#include <vector>
#include "clock.h"
#include "controller.h"
#include "systembus.h"


class BackPlane : public Component {
private:
  std::vector<ConnectedComponent *>  components;
  Clock                              clock;
  SystemBus                         *systemBus;
  SystemError                        error = NoError;

  SystemError     reportError();

protected:

public:
                          BackPlane();
                          ~BackPlane() override = default;
  void                    run();
  void                    stop() { clock.stop(); }
  Controller::RunMode     runMode() const;
  void                    setRunMode(Controller::RunMode runMode) const;
  Controller *            controller() const;

  void                    insert(ConnectedComponent *);
  ConnectedComponent *    componentByID(int id) const;
  SystemBus *             bus() const { return systemBus; }

  SystemError             status() override;
  SystemError             reset() override;
  SystemError             onRisingClockEdge() override;
  SystemError             onHighClock() override;
  SystemError             onFallingClockEdge() override;
  SystemError             onLowClock() override;

  void                    defaultSetup();
};

#endif //EMU_BACKPLANE_H
