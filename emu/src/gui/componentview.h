#ifndef EMU_COMPONENTVIEW_H
#define EMU_COMPONENTVIEW_H

#include <QLabel>
#include <QLayout>
#include <QWidget>

#include "qled.h"

#include "addressregister.h"
#include "controller.h"
#include "memory.h"
#include "register.h"

class ComponentView : public QWidget, ComponentListener {
  Q_OBJECT

public:
  void componentEvent(Component *sender, int ev) override;

protected:
  ConnectedComponent  *component;
  QLabel              *name;
  QLabel              *value;
  QHBoxLayout         *layout;

  explicit ComponentView(ConnectedComponent *, QWidget *);
  virtual int w() const { return 4; }
};

class RegisterView : public ComponentView {
  Q_OBJECT

public:
  RegisterView(Register *reg, QWidget *parent = nullptr)
    : ComponentView(reg, parent) { }

protected:
  int w() const override { return 2; }
};


class AddressRegisterView : public ComponentView {
  Q_OBJECT

public:
  AddressRegisterView(AddressRegister *reg, QWidget *parent = nullptr)
    : ComponentView(reg, parent) { }
};


class InstructionRegisterView : public ComponentView {
  Q_OBJECT

public:
  InstructionRegisterView(Controller *reg, QWidget *parent = nullptr);
  void componentEvent(Component *, int) override;

private:
  QLabel     *step;

protected:
  Controller * controller() const {
    return dynamic_cast<Controller *>(component);
  }
};


class MemoryView : public ComponentView {
  Q_OBJECT

public:
  MemoryView(Memory *reg, QWidget *parent = nullptr);
  void componentEvent(Component *, int) override;

protected:
  Memory * memory() const {
    return dynamic_cast<Memory *>(component);
  }

private:
  QLabel   *contents;
};

class SystemBusView : public QWidget, public ComponentListener {
  Q_OBJECT
public:
  SystemBusView(SystemBus *bus, QWidget *parent = nullptr);
  void componentEvent(Component *sender, int ev) override;

private:
  SystemBus       *systemBus;
  QHBoxLayout     *layout;
  QLabel          *data;
  QLabel          *address;
  QLabel          *put;
  QLabel          *get;
  QLed            *xdata;
  QLed            *xaddr;
  QLedArray       *op;
};

#endif //EMU_COMPONENTVIEW_H
