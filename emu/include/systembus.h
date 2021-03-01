#ifndef EMU_SYSTEMBUS_H
#define EMU_SYSTEMBUS_H

#include <vector>
#include "component.h"

class ComponentContainer;

class SystemBus : public Component {
private:
  ComponentContainer &m_backplane;
  byte                data_bus = 0;
  byte                addr_bus = 0;
  byte                put = 0;
  byte                get = 0;
  byte                op = 0;
  bool                _halt = true;
  bool                _sus = true;
  bool                _sack = true;
  bool                _xdata = true;
  bool                _xaddr = true;
  bool                rst = false;
  bool                _io = true;

  byte                m_flags = 0x0;

  void                _reset();

public:
  enum ProcessorFlags {
    Clear = 0x00,
    Z     = 0x01,
    C     = 0x02,
    V     = 0x04,
  };

  enum OperatorFlags {
    None      = 0x00,
    IOIn      = 0x01,
    Inc       = 0x01,
    Dec       = 0x02,
    Flags     = 0x04,
    MSB       = 0x08,
    IOOut     = 0x08,
    Mask      = 0x0F,
    Halt      = 0x0F,
    Done      = 0x10,
  };
                   explicit SystemBus(ComponentContainer &);
                  ~SystemBus() override = default;
  byte             readDataBus() const;
  void             putOnDataBus(byte);
  byte             readAddrBus() const;
  void             putOnAddrBus(byte);
  bool             xdata() const { return _xdata; }
  bool             xaddr() const { return _xaddr; }
  bool             io() const { return _io; }
  bool             halt() const { return _halt; }
  bool             sus() const { return _sus; }
  void             clearSus() { _sus = true; }
  byte             putID() const { return put; }
  byte             getID() const { return get; }
  byte             opflags() const { return op; }

  void             initialize(bool, bool, bool, byte, byte, byte, byte = 0x00, byte = 0x00);
  void             xdata(int, int, int);
  void             xaddr(int, int, int);
  void             io(int, int, int);
  void             stop();
  void             suspend();
  SystemError      reset() override;
  std::ostream &   status(std::ostream &) override;

  void             setFlag(ProcessorFlags, bool = true);
  void             clearFlag(ProcessorFlags);
  void             clearFlags();
  byte             flags() const { return m_flags; }
  bool             isSet(ProcessorFlags) const;
  std::string      flagsString() const;

  ComponentContainer & backplane() {
    return m_backplane;
  }
};

class ConnectedComponent : public Component {
private:
  SystemBus     *systemBus = nullptr;
  int            ident = -1;
  std::string    componentName = "?";

protected:
                      ConnectedComponent() = default;
  explicit            ConnectedComponent(int id, std::string n) : ident(id) {
    componentName = std::move(n);
  }

public:
  virtual int         id() const          { return ident;         }
  virtual int         alias() const       { return id();          }
  virtual std::string name() const        { return componentName; }
  void                bus(SystemBus *bus) { systemBus = bus;      }
  SystemBus *         bus() const         { return systemBus;     }
  virtual int         getValue() const    { return 0;             }
};

class ComponentContainer : public Component {
private:
  std::vector<ConnectedComponent *> m_components;
  std::vector<int>                  m_aliases;
  std::vector<ConnectedComponent *> m_io;

protected:
  SystemBus                         m_bus;

  explicit ComponentContainer()
      : m_bus(*this), m_components(), m_aliases() {
    m_components.resize(16);
    m_aliases.resize(16);
    m_io.resize(16);
  };

  explicit ComponentContainer(ConnectedComponent *c) : ComponentContainer() {
    insert(c);
  };

  virtual SystemError reportError() {
    return error();
  }

public:
  virtual ~ComponentContainer() override = default;

  void insert(ConnectedComponent *component) {
    component->bus(&m_bus);
    m_components[component->id()] = component;
    m_aliases[component->alias()] = component->id();
  }

  ConnectedComponent * component(int ix) const {
    return m_components[m_aliases[ix]];
  }

  void insertIO(ConnectedComponent *component) {
    component->bus(&m_bus);
    m_io[component->id()] = component;
  }

  SystemBus & bus() {
    return m_bus;
  }

  std::string name(int ix) const {
    switch (ix) {
      case 0x7:
        return "MEM";
      case 0xF:
        return "ADDR";
      default:
        ConnectedComponent *c = component(ix);
        return (c) ? c->name() : "";
    }
  }

  SystemError forAllComponents(const ComponentHandler &handler) {
    for (auto &component : m_components) {
      if (!component) continue;
      handler(component);
      error(component->error());
      if (error() != NoError) {
        return error();
      }
    }
    return NoError;
  }

  SystemError forAllChannels(const ComponentHandler &handler) {
    for (auto &channel : m_io) {
      if (!channel) continue;
      handler(channel);
      error(channel->error());
      if (error() != NoError) {
        return error();
      }
    }
    return NoError;
  }

};


#endif //EMU_SYSTEMBUS_H
