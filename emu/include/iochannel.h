#ifndef EMU_IOCHANNEL_H
#define EMU_IOCHANNEL_H

#include "systembus.h"

typedef std::function<byte()> Input;
typedef std::function<void(byte)> Output;

class IOChannel : public ConnectedComponent {
private:
  Input          m_input = nullptr;
  Output         m_output = nullptr;
  Reset          m_reset = nullptr;
  Status         m_status = nullptr;
  ClockEvent     m_fallingEdge = nullptr;
  ClockEvent     m_lowClock = nullptr;

public:
  explicit       IOChannel(int, std::string &&, Input &);
  explicit       IOChannel(int, std::string &&, Output &);
  explicit       IOChannel(int, std::string &&, Input &&);
  explicit       IOChannel(int, std::string &&, Output &&);

  void           setReset(Reset &reset) { m_reset = std::move(reset); }
  void           setStatus(Status &status) { m_status = std::move(status); }
  void           setFallingEdgeHandler(ClockEvent &handler) { m_fallingEdge = std::move(handler); }
  void           setLowClockHandler(ClockEvent &handler) { m_lowClock = std::move(handler); }

  void           setValue(byte val);
  int            getValue();

  std::ostream & status(std::ostream &) override;
  SystemError    reset() override;
  SystemError    onRisingClockEdge() override;
  SystemError    onHighClock() override;
//  SystemError    onFallingClockEdge() override;
//  SystemError    onLowClock() override;

  constexpr static int EV_INPUTREAD = 0x10;
  constexpr static int EV_OUTPUTWRITTEN = 0x11;

};

#endif //EMU_IOCHANNEL_H
