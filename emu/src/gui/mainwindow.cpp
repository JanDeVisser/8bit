
#include <QDialog>
#include <QFileDialog>
#include <QGridLayout>
#include <QObject>
#include <QPushButton>
#include <QStyleOption>
#include <QPainter>

#include "componentview.h"
#include "mainwindow.h"

#include "addressregister.h"
#include "controller.h"
#include "memory.h"
#include "register.h"
#include "registers.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
  createMenu();
  cpu = new CPU(this);
  connect(cpu, &CPU::executionEnded, this, &MainWindow::cpuStopped);
  connect(cpu, &CPU::executionInterrupted, this, &MainWindow::cpuStopped);
  setStyleSheet("MainWindow { background-color: black; }");

  auto widget = new QWidget;
  auto *layout = new QGridLayout;
  widget -> setLayout(layout);

  auto system = cpu -> getSystem();
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

  int row = 6;
  auto mem = system->component(MEMADDR);
  auto memView = new MemoryView(dynamic_cast<Memory *>(mem), widget);
  layout->addWidget(memView, row++, 0);

  history = new QLabel;
  history ->setFont(QFont("ibm3270", 15));
  history -> setStyleSheet("QLabel { background-color: black; color: green; border: none; }");
  layout->addWidget(history, row++, 0, 1, 2);

  result = new QLabel;
  result ->setFont(QFont("ibm3270", 15));
  result -> setStyleSheet("QLabel { background-color: black; color: green; border: none; }");
  layout->addWidget(result, row++, 0, 1, 2);

  auto w = new QWidget;
  auto hbox = new QHBoxLayout;
  w -> setLayout(hbox);
  auto prompt = new QLabel(">");
  prompt->setFont(QFont("ibm3270", 15));
  prompt -> setStyleSheet("QLabel { background-color: black; color: green; }");
  hbox -> addWidget(prompt);

  command = new CommandLineEdit;
  command -> setMaxLength(80);
  command ->setFont(QFont("ibm3270", 15));
  command -> setStyleSheet("QLineEdit { background-color: black; font: ibm3270; font-size: 15; color: green; border: none; }");
  connect(command, SIGNAL(returnPressed()), this, SLOT(commandSubmitted()));
  hbox -> addWidget(command);
  layout->addWidget(w, row++, 0, 1, 2);

  layout->setMenuBar(menuBar());
  setCentralWidget(widget);
  setWindowTitle(tr("Emu"));
}

void MainWindow::paintEvent(QPaintEvent *pe) {
  QStyleOption o;
  o.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(
    QStyle::PE_Widget, &o, &p, this);
}

void MainWindow::createMenu()
{
  auto fileMenu = new QMenu(tr("&File"), this);

  m_open = new QAction(tr("&Open"), this);
  m_open->setShortcuts(QKeySequence::Open);
  m_open->setStatusTip(tr("Open a memory image file"));
  connect(m_open, &QAction::triggered, this, &MainWindow::openFile);
  fileMenu -> addAction(m_open);

  const QIcon exitIcon = QIcon::fromTheme("application-exit");
  m_exit = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
  m_exit->setShortcuts(QKeySequence::Quit);
  m_exit->setStatusTip(tr("Exit the application"));
  menuBar()->addMenu(fileMenu);


//  connect(exitAction, &QAction::triggered, this, &QDialog::accept);
}


void MainWindow::openFile() {
  QString selectedFilter;
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("QFileDialog::getOpenFileName()"),
                                                  ".",
                                                  tr("Binary Files (*.bin);;All Files (*)"),
                                                  &selectedFilter);
  if (!fileName.isEmpty()) {
    cpu -> openImage(fileName);
  }

}

void MainWindow::cpuStopped() {
  //
}

Command::Command(QString &cmd) : line(cmd) {
  args = cmd.split(" ", Qt::SkipEmptyParts);
  if (args.isEmpty()) {
    setError("Syntax error: no command");
  } else {
    command = args[0].toLower();
    args.removeAt(0);
  }
}

void Command::setError(QString &&err) {
  result = err;
  success = false;
}

void Command::setError(QString &err) {
  result = err;
  success = false;
}

void Command::setResult(QString &&res) {
  result = res;
  success = true;
}

void Command::setResult(QString &res) {
  result = res;
  success = true;
}

typedef std::function<void(Command &)> CommandHandler;

void MainWindow::commandSubmitted() {
  static QHash<QString, CommandHandler> handlers;

  if (handlers.isEmpty()) {
    handlers["run"] = [this](Command &cmd) {
      if (!cpu->isRunning()) {
        cpu->run();
        cmd.success = true;
      } else {
        cmd.setError("CPU running");
      }
    };

    handlers["reset"] = [this](Command &cmd) {
      if (!cpu -> isRunning()) {
        cpu -> reset();
      } else {
        cmd.setError("CPU running");
      }
    };

    handlers["step"] = [this](Command &cmd) {
      if (cpu -> isRunning()) {
        cmd.setError("CPU running");
      } else if (cpu -> isHalted()) {
        cmd.setError("CPU halted");
      } else {
        cpu -> step();
      }
    };

    handlers["tick"] = [this](Command &cmd) {
      if (cpu -> isRunning()) {
        cmd.setError("CPU running");
      } else if (cpu -> isHalted()) {
        cmd.setError("CPU halted");
      } else {
        cpu -> tick();
      }
    };

    handlers["continue"] = [this](Command &cmd) {
      if (cpu -> isRunning()) {
        cmd.setError("CPU running");
      } else if (cpu -> isHalted()) {
        cmd.setError("CPU halted");
      } else {
        cpu -> continueExecution();
      }
    };

    handlers["clock"] = [this](Command &cmd) {
      bool ok;
      double speed;
      switch (cmd.args.size()) {
        case 0:
          cmd.setResult(QString("%1 kHz").arg(cpu->getSystem()->clockSpeed()));
          break;
        case 1:
          speed = cmd.args[1].toDouble(&ok);
          if (ok) {
            if (!cpu->getSystem()->setClockSpeed(speed)) {
              cmd.setError(QString("Frequency %1 kHz out of bounds").arg(speed));
            }
          } else {
            cmd.setError(QString("Requested frequency '%1' is not a number").arg(cmd.args[1]));
          }
          break;
        default:
          cmd.setError("Syntax error: too many arguments");
          break;
      }
    };
  }

  QString cmd = command -> text();
  addHistory(cmd);
  Command c(cmd);
  if (c.success) {
    if (handlers.contains(c.command)) {
      auto handler = handlers[c.command];
      handler(c);
    } else {
      c.setError("Unrecognized command");
    }
  }

  result->setText((c.result != "") ? c.result : (c.success) ? "OK" : "ERROR");
  result->setStyleSheet(QString("color: %1").arg((c.success) ? "green" : "red"));
  command->setText("");
}

void MainWindow::addHistory(QString &cmd) {
  history->setText(cmd);
}

//#include "mainwindow.moc"
