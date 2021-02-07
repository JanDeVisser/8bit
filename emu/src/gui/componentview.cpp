#include "componentview.h"

ComponentView::ComponentView(ConnectedComponent *comp, QWidget *owner) : QWidget(owner) {
  component = comp;
  component -> setListener(this);
  layout = new QHBoxLayout;
  setLayout(layout);
  name = new QLabel(component -> name().c_str());
  name -> setFont(QFont("Impact Label Reversed", 16));
  name -> setStyleSheet("QLabel { color : white; }");
  value = new QLabel(QString("%1").arg(component -> getValue(), w(), 16, QLatin1Char('0')));
  value -> setFont(QFont("DSEG7 Classic", 16));
  value -> setStyleSheet("QLabel { color : red; }");

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

SystemBusView::SystemBusView(SystemBus *bus, QWidget *parent) : QWidget(parent) {
  systemBus= bus;
  systemBus -> setListener(this);
  layout = new QHBoxLayout;
  setLayout(layout);
  auto *lbl = new QLabel("Data");
  lbl -> setFont(QFont("Impact Label Reversed", 16));
  lbl -> setStyleSheet("QLabel { color : white; }");
  layout -> addWidget(lbl);
  data = new QLabel(QString("%1").arg(bus -> readDataBus(), 2, 16, QLatin1Char('0')));
  data -> setFont(QFont("DSEG7 Classic", 16));
  data -> setStyleSheet("QLabel { color : red; }");
  layout -> addWidget(data);

  lbl = new QLabel("Address");
  lbl -> setFont(QFont("Impact Label Reversed", 16));
  lbl -> setStyleSheet("QLabel { color : white; }");
  layout -> addWidget(lbl);
  address = new QLabel(QString("%1").arg(bus -> readAddrBus(), 2, 16, QLatin1Char('0')));
  address -> setFont(QFont("DSEG7 Classic", 16));
  address -> setStyleSheet("QLabel { color : red; }");
  layout -> addWidget(address);
  layout -> addSpacing(20);

  lbl = new QLabel("From");
  lbl -> setFont(QFont("Impact Label Reversed", 16));
  lbl -> setStyleSheet("QLabel { color : white; }");
  layout -> addWidget(lbl);
  get = new QLabel(QString("%1").arg(bus -> getID(), 1, 16, QLatin1Char('0')));
  get -> setFont(QFont("DSEG7 Classic", 16));
  get -> setStyleSheet("QLabel { color : red; }");
  layout -> addWidget(get);

  lbl = new QLabel("To");
  lbl -> setFont(QFont("Impact Label Reversed", 16));
  lbl -> setStyleSheet("QLabel { color : white; }");
  layout -> addWidget(lbl);
  put = new QLabel(QString("%1").arg(bus -> putID(), 1, 16, QLatin1Char('0')));
  put -> setFont(QFont("DSEG7 Classic", 16));
  put -> setStyleSheet("QLabel { color : red; }");
  layout -> addWidget(put);
  layout -> addSpacing(20);

#define LED_SIZE 16
  xdata = new QLed();
  xdata -> setFixedSize(LED_SIZE, LED_SIZE);
  layout -> addWidget(xdata);
  xaddr = new QLed();
  xaddr -> setFixedSize(LED_SIZE, LED_SIZE);
  layout -> addWidget(xaddr);
  layout -> addSpacing(20);
  op = new QLedArray(4);
  op -> setColourForAll(QLed::Green);
  layout -> addWidget(op);
}

void SystemBusView::componentEvent(Component *sender, int ev) {
  if (ev == Component::EV_VALUECHANGED) {
    data->setText(QString("%1").arg(systemBus->readDataBus(), 2, 16, QLatin1Char('0')));
    address->setText(QString("%1").arg(systemBus->readAddrBus(), 2, 16, QLatin1Char('0')));
    get->setText(QString("%1").arg(systemBus->getID(), 1, 16, QLatin1Char('0')));
    put->setText(QString("%1").arg(systemBus->putID(), 1, 16, QLatin1Char('0')));
    xdata->setValue(!(systemBus->xdata()));
    xaddr->setValue(!(systemBus->xaddr()));
    op->setValue(systemBus->opflags());
  }
}


