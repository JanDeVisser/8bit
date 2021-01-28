#ifndef EMU_REALSYSTEM_H
#define EMU_REALSYSTEM_H

#include "system.h"

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


class RealSystem : public System {
private:
  std::vector<Component *>  components;
  Clock                     clock;
  byte                      bus = 0x00;
  byte                      addr_bus = 0x00;
  ControlLines              controlLines;
  SystemError               error = NoError;

  SystemError               reportError();

protected:

public:
  RealSystem();
  ~RealSystem() override = default;
  void                      run() override;
  void                      stop() override { clock.stop(); }
  byte                      readBus() const override;
  void                      putOnBus(byte) override;
  byte                      addrBus() const override;
  void                      putOnAddrBus(byte) override;
  ControlLines &            control();
  void                      setControl(ControlLines &);
  bool                      xdata() const override { return controlLines._xdata; }
  bool                      xaddr() const override { return controlLines._xaddr; }
  byte                      putID() const override { return  controlLines.put; }
  byte                      getID() const override { return  controlLines.get; }
  byte                      opflags() const override { return controlLines.op; }
  void                      xdata(int, int, int) override;
  void                      xaddr(int, int, int) override;

  const Component *         componentByID(int id) const;

  SystemError               status() override;
  SystemError               reset() override;
  SystemError               onRisingClockEdge() override;
  SystemError               onHighClock() override;
  SystemError               onFallingClockEdge() override;
  SystemError               onLowClock() override;
};

#endif //EMU_REALSYSTEM_H
