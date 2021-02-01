
#include <QDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLayout>
#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QThread>

#include "qled.h"
#include "mainwindow.h"

#include "addressregister.h"
#include "controller.h"
#include "memory.h"
#include "register.h"

class ComponentView : public QWidget, ComponentListener {
  Q_OBJECT

public:
  ~ComponentView() override {
    delete layout;
  }

  void componentEvent(Component *sender, int ev) override {
    if (ev == Component::EV_VALUECHANGED) {
      value -> setText(QString("%1").arg(component -> getValue(), w(), 16, QLatin1Char('0')));
    }
  }

protected:
  ConnectedComponent  *component;
  QLabel              *name;
  QLabel              *value;
  QHBoxLayout         *layout;

  explicit ComponentView(ConnectedComponent *comp, QWidget *owner) : QWidget(owner) {
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

  virtual int w() const {
    return 4;
  }

  virtual int def() const {
    return 0;
  }
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
  ~InstructionRegisterView() override {
    delete step;
  }

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
  ~MemoryView() override {
    delete contents;
  };

  void componentEvent(Component *, int) override;

private:
  QLabel   *contents;

protected:
  Memory * memory() const {
    return dynamic_cast<Memory *>(component);
  }
};

class SystemBusView : public QWidget, public ComponentListener {
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


public:
  SystemBusView(SystemBus *bus, QWidget *parent = nullptr) : QWidget(parent) {
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

  ~SystemBusView() override = default;

  void componentEvent(Component *sender, int ev) override {
    if (ev == Component::EV_VALUECHANGED) {
      data -> setText(QString("%1").arg(systemBus -> readDataBus(), 2, 16, QLatin1Char('0')));
      address -> setText(QString("%1").arg(systemBus -> readAddrBus(), 2, 16, QLatin1Char('0')));
      get -> setText(QString("%1").arg(systemBus -> getID(), 1, 16, QLatin1Char('0')));
      put -> setText(QString("%1").arg(systemBus -> putID(), 1, 16, QLatin1Char('0')));
      xdata -> setValue(!(systemBus -> xdata()));
      xaddr -> setValue(!(systemBus -> xaddr()));
      op -> setValue(systemBus -> opflags());
    }
  }

};


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
      value -> setText(QString("%1").arg(controller()->instruction().c_str(), 10, QLatin1Char('!')));
    case Controller::EV_STEPCHANGED:
      step -> setText(QString("%1").arg(controller()->getStep()));
      break;
    default:
      // Unrecognized
      break;
  }
}

MemoryView::MemoryView(Memory *reg, QWidget *parent)
    : ComponentView(reg, parent) {
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

CPUThread::CPUThread(BackPlane *s, QObject *parent) : QThread(parent) {
  this -> system = s;
}

void CPUThread::run() {
  system -> run();
  emit executionEnded();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
  system = new BackPlane();

  createMenu();

  auto widget = new QWidget;
  auto *layout = new QGridLayout;
  widget->setLayout(layout);

  auto busView = new SystemBusView(system -> bus());
  layout -> addWidget(busView, 0, 0);
  for (int r = 0; r < 4; r++) {
    auto reg = system->componentByID(r);
    auto regView = new RegisterView(dynamic_cast<Register *>(reg), widget);
    layout->addWidget(regView, r / 2 + 1, r % 2);
  }

  auto reg = system->componentByID(IR);
  auto regView = new InstructionRegisterView(dynamic_cast<Controller *>(reg), widget);
  layout->addWidget(regView, 3, 0);

  for (int r = 0; r < 4; r++) {
    auto addr_reg = system->componentByID(8+r);
    auto addr_regView = new AddressRegisterView(dynamic_cast<AddressRegister *>(addr_reg), widget);
    layout->addWidget(addr_regView, r / 2 + 4, r % 2);
  }

  auto mem = system->componentByID(MEMADDR);
  auto memView = new MemoryView(dynamic_cast<Memory *>(mem), widget);
  layout->addWidget(memView, 6, 0);

  run = new QPushButton("&Run");
  connect(run, SIGNAL(clicked()), this, SLOT(runClicked()));

  layout->addWidget(run, 7, 0);
  layout->setMenuBar(menuBar());

  setCentralWidget(widget);
  setWindowTitle(tr("Emu"));
}

void MainWindow::createMenu()
{
  auto fileMenu = new QMenu(tr("&File"), this);

  const QIcon exitIcon = QIcon::fromTheme("application-exit");
  QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
  exitAct->setShortcuts(QKeySequence::Quit);
  exitAct->setStatusTip(tr("Exit the application"));
  menuBar()->addMenu(fileMenu);


//  connect(exitAction, &QAction::triggered, this, &QDialog::accept);
}

void MainWindow::runClicked() {
  run -> setDisabled(true);
  thread = new CPUThread(system, this);
  connect(thread, &CPUThread::executionEnded, this, &MainWindow::enableRun);
  thread -> start();
}

void MainWindow::enableRun() {
  run -> setEnabled(true);
}

MainWindow::~MainWindow() {
  delete system;
}

#include "mainwindow.moc"