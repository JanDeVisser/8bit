
#include <QDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLayout>
#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QThread>

#include "componentview.h"
#include "mainwindow.h"

#include "addressregister.h"
#include "controller.h"
#include "memory.h"
#include "register.h"
#include "registers.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
  thread = new CPUThread(this);
  connect(thread, &CPUThread::executionEnded, this, &MainWindow::enableButtons);
  createMenu();

  auto widget = new QWidget;
  auto *layout = new QGridLayout;
  widget->setLayout(layout);

  auto system = thread -> getSystem();
  auto busView = new SystemBusView(system -> bus());
  layout -> addWidget(busView, 0, 0, 1, 2);
  for (int r = 0; r < 4; r++) {
    auto reg = system->component(r);
    auto regView = new RegisterView(dynamic_cast<Register *>(reg), widget);
    layout->addWidget(regView, r / 2 + 1, r % 2);
  }

  auto reg = system->component(IR);
  auto regView = new InstructionRegisterView(dynamic_cast<Controller *>(reg), widget);
  layout->addWidget(regView, 3, 0);

  for (int r = 0; r < 4; r++) {
    auto addr_reg = system->component(8+r);
    auto addr_regView = new AddressRegisterView(dynamic_cast<AddressRegister *>(addr_reg), widget);
    layout->addWidget(addr_regView, r / 2 + 4, r % 2);
  }

  auto mem = system->component(MEMADDR);
  auto memView = new MemoryView(dynamic_cast<Memory *>(mem), widget);
  layout->addWidget(memView, 6, 0);

  auto w = new QWidget;
  auto hbox = new QHBoxLayout;
  w -> setLayout(hbox);
  run = new QPushButton("&Run");
  connect(run, SIGNAL(clicked()), this, SLOT(runClicked()));
  hbox -> addWidget(run);

  cycle = new QPushButton("&Cycle");
  connect(cycle, SIGNAL(clicked()), this, SLOT(cycleClicked()));
  hbox -> addWidget(cycle);

  instr = new QPushButton("&Instruction");
  connect(instr, SIGNAL(clicked()), this, SLOT(instrClicked()));
  hbox -> addWidget(instr);

  layout->addWidget(w, 7, 0);

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
  disableButtons();
  thread -> setRunMode(Controller::Continuous);
  thread -> start();
}

void MainWindow::cycleClicked() {
  disableButtons();
  thread -> setRunMode(Controller::BreakAtClock);
  thread -> start();
}

void MainWindow::instrClicked() {
  disableButtons();
  thread -> setRunMode(Controller::BreakAtInstruction);
  thread -> start();
}

void MainWindow::disableButtons() {
  run -> setEnabled(false);
  cycle -> setEnabled(false);
  instr -> setEnabled(false);
}

void MainWindow::enableButtons() {
  run -> setEnabled(true);
  cycle -> setEnabled(true);
  instr -> setEnabled(true);
}
