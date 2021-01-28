#ifndef EMU_SYSTEM_H
#define EMU_SYSTEM_H

#include <vector>
#include "clock.h"
#include "lines.h"

constexpr static byte OP_INC = 0x01;
constexpr static byte OP_DEC = 0x02;
constexpr static byte OP_MSB = 0x08;



class System : public Component {
public:
                  ~System() override = default;
  virtual byte     readBus() const = 0;
  virtual void     putOnBus(byte) = 0;
  virtual byte     addrBus() const = 0;
  virtual void     putOnAddrBus(byte) = 0;
  virtual bool     xdata() const = 0;
  virtual bool     xaddr() const = 0;
  virtual byte     putID() const = 0;
  virtual byte     getID() const = 0;
  virtual byte     opflags() const = 0;
  virtual void     xdata(int, int, int) = 0;
  virtual void     xaddr(int, int, int) = 0;
  virtual void     run() = 0;
  virtual void     stop() = 0;
};

class OwnedComponent : public Component {
private:
  System *      owner;

protected:
  explicit OwnedComponent(System *s) : owner(s) { }

public:
  System * system() const { return owner; }
  virtual int id() const { return -1; }
};

#endif //EMU_SYSTEM_H
