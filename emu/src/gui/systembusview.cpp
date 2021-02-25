#include <QHBoxLayout>
#include <QVBoxLayout>
#include "systembusview.h"

SystemBusView::SystemBusView(SystemBus &bus, QWidget *parent)
    : QWidget(parent), systemBus(bus) {
  systemBus.setListener(this);
  auto grid = new QHBoxLayout;
  layout = grid;
  setLayout(layout);

  auto bp = bus.backplane();

  auto busData = [this](QString &&label, ByteWidget *&bw, QLed *&led) {
    auto w = new StyledWidget;
    auto l = new QVBoxLayout;
    auto *lbl = new ImpactLabel(label);
    l -> addWidget(lbl, 0, Qt::AlignCenter);
    auto dataLayout = new QHBoxLayout;
    bw = new ByteWidget;
    dataLayout -> addWidget(bw);
    led = new QLed();
    led -> setFixedSize(LED_SIZE, LED_SIZE);
    dataLayout -> addWidget(led);
    l->addLayout(dataLayout);
    w->setLayout(l);
    w->setStyleSheet("StyledWidget { border: 1px solid grey; border-radius: 5px; }");
    return w;
  };

  grid->addWidget(busData("Data", data, xdata));
  grid->addWidget(busData("Address", address, xaddr));
  grid -> addSpacing(20);

  auto regData = [this](QString &&label, RegisterNameLabel *&reg) {
    auto w = new StyledWidget;
    auto l = new QVBoxLayout;
    auto *lbl = new ImpactLabel(label);
    l -> addWidget(lbl, 0, Qt::AlignCenter);
    reg = new RegisterNameLabel(systemBus);
    l -> addWidget(reg);
    w->setLayout(l);
    w->setMinimumWidth(100);
    w->setStyleSheet("StyledWidget { border: 1px solid grey; border-radius: 5px; }");
    return w;
  };

  grid -> addWidget(regData("From", get));
  grid -> addWidget(regData("To", put));
  grid -> addSpacing(20);

  auto w = new StyledWidget;
  auto l = new QVBoxLayout;
  auto *lbl = new ImpactLabel("Operation");
  l -> addWidget(lbl, 0, Qt::AlignCenter);
  op = new QLedArray(4);
  op -> setColourForAll(QLed::Red);
  l -> addWidget(op, 0, Qt::AlignCenter);
  w->setLayout(l);
  w->setStyleSheet("StyledWidget { border: 1px solid grey; border-radius: 5px; }");
  grid -> addWidget(w);
  grid -> addSpacing(20);

  w = new StyledWidget;
  l = new QVBoxLayout;
  lbl = new ImpactLabel("Flags");
  l -> addWidget(lbl, 0, Qt::AlignCenter);
  auto flagBox = new QHBoxLayout;

  auto buildFlag = [this](QLabel *&lbl, QString &&flag) {
    lbl = new QLabel(flag);
    lbl->setFont(QFont("ibm3270", 12));
    lbl -> setStyleSheet("QLabel { color: lightgrey; }");
    return lbl;
  };

  flagBox -> addWidget(buildFlag(z, "Z"));
  flagBox -> addWidget(buildFlag(c, "C"));
  flagBox -> addWidget(buildFlag(v, "V"));
  l->addLayout(flagBox);
  w->setLayout(l);
  w->setStyleSheet("StyledWidget { border: 1px solid grey; border-radius: 5px; }");
  grid -> addWidget(w);
  grid -> addSpacing(20);
  connect(this, &SystemBusView::valueChanged, this, &SystemBusView::refresh);
}

void SystemBusView::componentEvent(Component *sender, int ev) {
  if (ev == Component::EV_VALUECHANGED) {
    emit valueChanged();
  }
}

void SystemBusView::refresh() {
  data -> setValue(systemBus.readDataBus());
  address -> setValue(systemBus.readAddrBus());

  get -> setRegister(systemBus.getID());
  put -> setRegister(systemBus.putID());

  xdata->setValue(!(systemBus.xdata()));
  xaddr->setValue(!(systemBus.xaddr()));
  op->setValue(systemBus.opflags());

  auto sheet = [this](SystemBus::ProcessorFlags flag) {
    return QString("QLabel { color: %1; }").arg((systemBus.flags() & flag) ? "red" : "lightgrey");
  };

  z -> setStyleSheet(sheet(SystemBus::Z));
  c -> setStyleSheet(sheet(SystemBus::C));
  v -> setStyleSheet(sheet(SystemBus::V));
}


