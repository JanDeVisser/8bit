#include "componentview.h"

ComponentView::ComponentView(ConnectedComponent *comp, QWidget *owner) : QWidget(owner) {
  component = comp;
  component -> setListener(this);
  layout = new QHBoxLayout;
  setLayout(layout);
  name = new ImpactLabel(component -> name().c_str());
  name -> setFontSize(20);
  value = new DSegLabel(QString("%1").arg(component -> getValue(), w(), 16, QLatin1Char('0')));

  layout->addWidget(name);
  layout->addWidget(value);
}

void ComponentView::componentEvent(Component *sender, int ev) {
  if (ev == Component::EV_VALUECHANGED) {
    value -> setText(QString("%1").arg(component -> getValue(), w(), 16, QLatin1Char('0')));
  }
}

// -----------------------------------------------------------------------

InstructionRegisterView::InstructionRegisterView(Controller *reg, QWidget *parent)
    : ComponentView(reg, parent) {
  step = new QLabel("0");
  step -> setFont(QFont("DSEG7 Classic", 16));
  step -> setStyleSheet("QLabel { color : red; }");
  layout->addWidget(step);
//  value -> setFont(QFont("Skyfont", 18));
//  value -> setStyleSheet("QLabel { color : white; }");
  value -> setFont(QFont("DSEG14 Classic", 16));
  value -> setText("!!!!!!!!!!");
}

void InstructionRegisterView::componentEvent(Component *sender, int ev) {
  switch (ev) {
    case Component::EV_VALUECHANGED:
    case Controller::EV_STEPCHANGED:
      step -> setText(QString("%1").arg(controller()->getStep()));
      value -> setText(QString("%1").arg(controller()->instruction().c_str(), 10, QLatin1Char('!')));
      break;
    default:
      // Unrecognized
      break;
  }
}

// -----------------------------------------------------------------------

MemoryView::MemoryView(Memory *reg, QWidget *parent) : ComponentView(reg, parent) {
  contents = new QLabel("0");
  contents -> setFont(QFont("DSEG7 Classic", 16));
  contents -> setStyleSheet("QLabel { color : red; }");
  layout->addWidget(contents);
}

void MemoryView::componentEvent(Component *sender, int ev) {
  switch (ev) {
    case Component::EV_VALUECHANGED:
      this -> ComponentView::componentEvent(sender, ev);
    case Memory::EV_CONTENTSCHANGED:
      contents -> setText(QString("%1").arg((*memory())[component -> getValue()], 2, 16, QLatin1Char('0')));
      break;
    default:
      // Unrecognized
      break;
  }
}

// -----------------------------------------------------------------------

SystemBusView::SystemBusView(SystemBus &bus, QWidget *parent)
    : QWidget(parent), systemBus(bus) {
  systemBus.setListener(this);
  auto grid = new QGridLayout;
  layout = grid;
  setLayout(layout);

  auto bp = bus.backplane();

  auto *lbl = new ImpactLabel("Data");
  grid->setColumnMinimumWidth(0, 120);
  grid->setColumnMinimumWidth(1, 20);
  grid -> addWidget(lbl, 0, 0, 1, 2, Qt::AlignHCenter);
  data = new ByteWidget;
  grid -> addWidget(data, 1, 0);
  xdata = new QLed();
  xdata -> setFixedSize(LED_SIZE, LED_SIZE);
  grid -> addWidget(xdata, 1, 1, Qt::AlignHCenter);

  lbl = new ImpactLabel("Address");
  grid->setColumnMinimumWidth(2, 120);
  grid->setColumnMinimumWidth(3, 20);
  grid -> addWidget(lbl, 0, 2, 1, 2, Qt::AlignHCenter);
  address = new ByteWidget;
  grid -> addWidget(address, 1,2);
  xaddr = new QLed();
  xaddr -> setFixedSize(LED_SIZE, LED_SIZE);
  grid -> addWidget(xaddr, 1, 3, Qt::AlignHCenter);
//  layout -> addSpacing(20);

  lbl = new ImpactLabel("From");
  grid -> addWidget(lbl, 0, 4);
  get = new RegisterNameLabel(systemBus);
  grid -> addWidget(get, 1, 4);

  lbl = new ImpactLabel("To");
  grid -> addWidget(lbl, 0, 5);
  put = new RegisterNameLabel(systemBus);
  grid -> addWidget(put, 1, 5);
//  layout -> addSpacing(20);

//  layout -> addSpacing(20);
  lbl = new ImpactLabel("Operation");
  grid -> addWidget(lbl, 0, 6, Qt::AlignHCenter);
  op = new QLedArray(4);
  op -> setColourForAll(QLed::Green);
  grid -> addWidget(op, 1, 6);

  lbl = new ImpactLabel("Flags");
  grid -> addWidget(lbl, 0, 7, Qt::AlignHCenter);
  flags = new QLedArray(3);
  flags -> setColourForAll(QLed::Green);
  grid -> addWidget(flags, 1, 7);

}

void SystemBusView::componentEvent(Component *sender, int ev) {
  if (ev == Component::EV_VALUECHANGED) {
    data -> setValue(systemBus.readDataBus());
    address -> setValue(systemBus.readAddrBus());

    get -> setRegister(systemBus.getID());
    put -> setRegister(systemBus.putID());

    xdata->setValue(!(systemBus.xdata()));
    xaddr->setValue(!(systemBus.xaddr()));
    op->setValue(systemBus.opflags());
    flags -> setValue(systemBus.flags());
  }
}


