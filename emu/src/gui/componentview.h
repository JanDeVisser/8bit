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
private:
  QLatin1Char       m_fill;
  int               m_width;

public:
  enum Style {
    SevenSegment,
    FourteenSegment,
    IBM3270,
    SkyFont,
  };

  explicit DSegLabel(const QString &label, int width = 4) : QLabel(label), m_fill(' '), m_width(width) {
    setDSegStyle(IBM3270);
  }

  void setDSegStyle(Style style) {
    switch (style) {
      case SevenSegment:
        setStyleSheet("color: green; font-family: \"DSEG7 Classic\"; font-size: 16pt;");
        m_fill = QLatin1Char('!');
        break;
      case FourteenSegment:
        setStyleSheet("color: green; font-family: \"DSEG14 Classic\"; font-size: 16pt;");
        m_fill = QLatin1Char('!');
        break;
      case IBM3270:
        setStyleSheet("font-family: ibm3270; font-size: 20pt; color: green;");;
        m_fill = QLatin1Char(' ');
        break;
      case SkyFont:
        setStyleSheet("font-family: SkyFont; font-size: 20pt; color: green;");;
        m_fill = QLatin1Char(' ');
        break;
    }
  }

  void setValue(int value) {
    setText(QString("%1").arg(value, m_width, 16, QLatin1Char('0')));
  }

  void setValue(QString &value) {
    setText(QString("%1").arg(value, m_width, m_fill));
  }

  void setValue(QString &&value) {
    setText(QString("%1").arg(value, m_width, m_fill));
  }

  void setValue(std::string &value) {
    setText(QString("%1").arg(value.c_str(), m_width, m_fill));
  }

  void setValue(std::string &&value) {
    setText(QString("%1").arg(value.c_str(), m_width, m_fill));
  }

  void erase() {
    setText("");
  }
};

class RegisterNameLabel : public DSegLabel {
private:
  SystemBus &m_bus;
public:
  explicit RegisterNameLabel(SystemBus &bus) : DSegLabel("", 4), m_bus(bus) {
    erase();
  }

  void setRegister(int id) {
    auto bp = m_bus.backplane();
    auto n = bp.name(id);
    while (n.size() < 4) n += " ";
    setText(n.c_str());
  }
};

class ByteWidget : public QWidget {
private:
  QLayout   *m_layout;
  QLedArray *m_leds;
  DSegLabel *m_label;
public:
  explicit ByteWidget(QWidget *parent = nullptr) : QWidget(parent) {
    m_layout = new QHBoxLayout;
    setLayout(m_layout);
    m_leds = new QLedArray(8, 12);
    m_layout->addWidget(m_leds);
    m_label = new DSegLabel("", 2);
    m_label -> setValue(0);
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

  explicit ComponentView(ConnectedComponent *, int = 4, QWidget * = nullptr);

  void paintEvent(QPaintEvent *pe) override;
};

class RegisterView : public ComponentView {
  Q_OBJECT

public:
  explicit RegisterView(Register *reg, QWidget *parent = nullptr)
    : ComponentView(reg, 2, parent) { }
};


class AddressRegisterView : public ComponentView {
  Q_OBJECT

public:
  explicit AddressRegisterView(AddressRegister *reg, QWidget *parent = nullptr)
    : ComponentView(reg, 4, parent) { }
};


class InstructionRegisterView : public ComponentView {
  Q_OBJECT

public:
  explicit InstructionRegisterView(Controller *reg, QWidget *parent = nullptr);
  void componentEvent(Component *, int) override;

private:
  DSegLabel  *step;

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
  DSegLabel   *contents;
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
