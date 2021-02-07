#ifndef EMU_SYSTEMBUS_H
#define EMU_SYSTEMBUS_H

#include "component.h"

class SystemBus : public Component {
private:
  byte              data_bus = 0;
  byte              addr_bus = 0;
  byte              put = 0;
  byte              get = 0;
  byte              op = 0;
  bool              _halt = true;
  bool              _sus = true;
  bool              _sack = true;
  bool              _xdata = true;
  bool              _xaddr = true;
  bool              rst = false;
  bool              _io = true;

  byte              m_flags;

public:
  enum ProcessorFlags {
    Z = 0x01,
    C = 0x02,
    V = 0x04,
  };

  enum OperatorFlags {
    None      = 0x00,
    Inc       = 0x01,
    Dec       = 0x02,
    MSB       = 0x08,
    Mask      = 0x0F,
    Halt      = 0x0F,
    Done      = 0x10,
  };
                  ~SystemBus() override = default;
  byte             readDataBus() const;
  void             putOnDataBus(byte);
  byte             readAddrBus() const;
  void             putOnAddrBus(byte);
  bool             xdata() const { return _xdata; }
  bool             xaddr() const { return _xaddr; }
  bool             halt() const { return _halt; }
  byte             putID() const { return put; }
  byte             getID() const { return get; }
  byte             opflags() const { return op; }
  byte             flags() const { return m_flags; }

  void             initialize(bool, bool, byte, byte, byte, byte = 0x00, byte = 0x00);
  void             xdata(int, int, int);
  void             xaddr(int, int, int);
  void             stop();
  SystemError      reset() override;
  SystemError      status() override;
};

class ConnectedComponent : public Component {
private:
  SystemBus *         systemBus = nullptr;
  int                 ident = -1;
  std::string         componentName = "?";

protected:
                      ConnectedComponent() = default;
  explicit            ConnectedComponent(int id, std::string n) : ident(id) {
    componentName = std::move(n);
  }

public:
  virtual int         id() const          { return ident;         }
  virtual std::string name() const        { return componentName; }
  void                bus(SystemBus *bus) { systemBus = bus;      }
  SystemBus *         bus() const         { return systemBus;     }
  virtual int         getValue() const    { return 0;             }
};

#endif //EMU_SYSTEMBUS_H
