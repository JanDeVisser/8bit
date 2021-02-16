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

#define LED_SIZE 16

class ImpactLabel : public QLabel {
public:
  explicit ImpactLabel(const QString &label) : QLabel(label) {
    setFont(QFont("Impact Label Reversed", 16));
    setStyleSheet("QLabel { color : white; }");
  }

  void setFontSize(int sz) {
    setFont(QFont("Impact Label Reversed", sz));
  }
};

class DSegLabel : public QLabel {
public:
  enum Style {
    SevenSegment,
    FourteenSegment,
  };

  explicit DSegLabel(const QString &label, const char *color = "red") : QLabel(label) {
    setDSegStyle(SevenSegment);
    setStyleSheet(QString("QLabel { color : %1; }").arg(color));
  }

  void setDSegStyle(Style style) {
    switch (style) {
      case SevenSegment:
        setFont(QFont("DSEG7 Classic", 16));
        break;
      case FourteenSegment:
        auto font = QFont("DSEG14 Classic", 16);
        setFont(font);
        break;
    }
  }
};

class RegisterNameLabel : public DSegLabel {
private:
  SystemBus &m_bus;
public:
  explicit RegisterNameLabel(SystemBus &bus) : DSegLabel("!!!!"), m_bus(bus) {
    setDSegStyle(FourteenSegment);
  }

  void setRegister(int id) {
    auto bp = m_bus.backplane();
    auto n = bp.name(id);
    while (n.size() < 4) n += "!";
    setText(n.c_str());
  }
};

class ByteWidget : public QWidget {
private:
  QLayout   *m_layout;
  QLedArray *m_leds;
  DSegLabel *m_label;
public:
  ByteWidget(QWidget *parent = nullptr) : QWidget(parent) {
    m_layout = new QHBoxLayout;
    setLayout(m_layout);
    m_leds = new QLedArray(8, 12);
    m_layout->addWidget(m_leds);
    m_label = new DSegLabel("!!");
    m_layout->addWidget(m_label);
    setValue(0);
  }

  void setValue(byte value) {
    m_leds->setValue(value);
    m_label->setText(QString("%1").arg(value, 2, 16, QLatin1Char('0')));
  }
};

class ComponentView : public QWidget, ComponentListener {
  Q_OBJECT

public:
  void componentEvent(Component *sender, int ev) override;

protected:
  ConnectedComponent  *component;
  ImpactLabel         *name;
  DSegLabel           *value;
  QHBoxLayout         *layout;

  explicit ComponentView(ConnectedComponent *, QWidget *);
  virtual int w() const { return 4; }
};

class RegisterView : public ComponentView {
  Q_OBJECT

public:
  explicit RegisterView(Register *reg, QWidget *parent = nullptr)
    : ComponentView(reg, parent) { }

protected:
  int w() const override { return 2; }
};


class AddressRegisterView : public ComponentView {
  Q_OBJECT

public:
  explicit AddressRegisterView(AddressRegister *reg, QWidget *parent = nullptr)
    : ComponentView(reg, parent) { }
};


class InstructionRegisterView : public ComponentView {
  Q_OBJECT

public:
  explicit InstructionRegisterView(Controller *reg, QWidget *parent = nullptr);
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
  explicit MemoryView(Memory *reg, QWidget *parent = nullptr);
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
  explicit SystemBusView(SystemBus &bus, QWidget *parent = nullptr);
  void componentEvent(Component *sender, int ev) override;

private:
  SystemBus         &systemBus;
  QLayout           *layout;
  ByteWidget        *data;
  ByteWidget        *address;
  RegisterNameLabel *put;
  RegisterNameLabel *get;
  QLed              *xdata;
  QLed              *xaddr;
  QLedArray         *op;
  QLedArray         *flags;
};

#endif //EMU_COMPONENTVIEW_H
