#ifndef EMU_BACKPLANE_H
#define EMU_BACKPLANE_H

#include <vector>
#include "clock.h"
#include "systembus.h"

constexpr static int GP_A = 0;
constexpr static int GP_B = 1;
constexpr static int GP_C = 2;
constexpr static int GP_D = 3;
constexpr static int IR = 6;
constexpr static int MEM = 7;
constexpr static int PC = 8;
constexpr static int SP = 9;
constexpr static int Si = 10;
constexpr static int Di = 11;
constexpr static int TX = 12;
constexpr static int MEMADDR = 15;


class BackPlane : public Component {
private:
  std::vector<ConnectedComponent *>  components;
  Clock                              clock;
  SystemBus                         *systemBus;
  SystemError                        error = NoError;

  SystemError                        reportError();

protected:

public:
                          BackPlane();
                          ~BackPlane() override = default;
  void                    run();
  void                    stop() { clock.stop(); }

  void                    add(ConnectedComponent *);
  ConnectedComponent *    componentByID(int id) const;
  SystemBus *             bus() const { return systemBus; }

  SystemError             status() override;
  SystemError             reset() override;
  SystemError             onRisingClockEdge() override;
  SystemError             onHighClock() override;
  SystemError             onFallingClockEdge() override;
  SystemError             onLowClock() override;
};

#endif //EMU_BACKPLANE_H
