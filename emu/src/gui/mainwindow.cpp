
#include <QDialog>
#include <QFileDialog>
#include <QGridLayout>
#include <QMessageBox>
#include <QObject>
#include <QPainter>
#include <QTabWidget>

#include "componentview.h"
#include "mainwindow.h"
#include "systembus.h"

#include "addressregister.h"
#include "controller.h"
#include "memory.h"
#include "register.h"
#include "registers.h"
#include "systembusview.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
  createMenu();
  cpu = new CPU(this);
  connect(cpu, &CPU::executionEnded, this, &MainWindow::cpuStopped);
  connect(cpu, &CPU::executionInterrupted, this, &MainWindow::cpuStopped);
  setStyleSheet("MainWindow { background-color: black; }");

  auto widget = new QWidget;
  auto mainLayout = new QHBoxLayout;
  auto layout = new QGridLayout;
  mainLayout -> addLayout(layout);
  widget -> setLayout(mainLayout);

  auto system = cpu -> getSystem();

  auto tabs = new QTabWidget();
  m_memdump = new MemDump(system);
  m_memdump -> setFocusPolicy(Qt::NoFocus);
  tabs->addTab(m_memdump, "Memory");
  m_status = new QTextEdit();
  m_status -> setFont(QFont("ibm3270", 10));
  m_status -> setStyleSheet("QTextEdit { color: green; background-color: black; }");
  tabs -> addTab(m_status, "Log");

  mainLayout->addWidget(tabs);

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
  connect(memView, &ComponentView::valueChanged, m_memdump, &MemDump::focus);
  connect(memView, &MemoryView::imageLoaded, m_memdump, &MemDump::reload);
  connect(memView, &MemoryView::contentsChanged, m_memdump, &MemDump::reload);

  m_history = new QLabel;
  m_history ->setFont(QFont("ibm3270", 15));
  m_history -> setStyleSheet("QLabel { background-color: black; color: green; border: none; }");
  layout->addWidget(m_history, row++, 0, 1, 2);

  m_result = new QLabel;
  m_result ->setFont(QFont("ibm3270", 15));
  m_result -> setStyleSheet("QLabel { background-color: black; color: green; border: none; }");
  layout->addWidget(m_result, row++, 0, 1, 2);

  auto w = new QWidget;
  auto hbox = new QHBoxLayout;
  w -> setLayout(hbox);
  auto prompt = new QLabel(">");
  prompt->setFont(QFont("ibm3270", 15));
  prompt -> setStyleSheet("QLabel { background-color: black; color: green; }");
  hbox -> addWidget(prompt);

  m_command = makeCommandLine();
  hbox -> addWidget(m_command);
  layout->addWidget(w, row++, 0, 1, 2);
  m_command -> setFocus();

  layout->setMenuBar(menuBar());
  setCentralWidget(widget);
  setWindowTitle(tr("Emu"));
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

void MainWindow::cpuStopped(const QString &status) {
  auto t = m_status->toPlainText();
  if (!t.isEmpty()) {
    t += "\n";
  }
  t += status;
  m_status->setPlainText(t);
}

CommandLineEdit * MainWindow::makeCommandLine() {
  auto ret = new CommandLineEdit();
  ret -> setMaxLength(80);
  ret ->setFont(QFont("ibm3270", 15));
  ret -> setStyleSheet("QLineEdit { background-color: black; font: ibm3270; font-size: 15; color: green; border: none; }");
  connect(ret, SIGNAL(result(const QString &,bool,const QString &)), this, SLOT(commandResult(const QString &, bool, const QString &)));

  ret->addCommandDefinition(ret, "quit", 0, 0,
    [this, ret](Command &cmd) {
#if 0
      if (QMessageBox::question(this, "Are you sure", "Are you sure you want to quit?") == QMessageBox::Yes) {
        close();
      }
#endif
#if 1
      if (ret -> query("Are you sure you want to exit? (Y/N)", "yn") == "y") {
        close();
      }
#endif
    });

  ret->addCommandDefinition(ret, "run", 0, 0,
    [this](Command &cmd) {
      if (!cpu->isRunning()) {
        cpu->run();
        cmd.setSuccess();
      } else {
        cmd.setError("CPU running");
      }
    });

  ret->addCommandDefinition(ret, "reset", 0, 0,
    [this](Command &cmd) {
      if (!cpu->isRunning()) {
        cpu->reset();
      } else {
        cmd.setError("CPU running");
      }
    });

  ret->addCommandDefinition(ret, "step", 0, 0,
    [this](Command &cmd) {
      if (cpu->isRunning()) {
        cmd.setError("CPU running");
      } else if (cpu->isHalted()) {
        cmd.setError("CPU halted");
      } else {
        cpu->step();
      }
    });

  ret->addCommandDefinition(ret, "tick", 0, 0,
    [this](Command &cmd) {
      if (cpu->isRunning()) {
        cmd.setError("CPU running");
      } else if (cpu->isHalted()) {
        cmd.setError("CPU halted");
      } else {
        cpu->tick();
      }
    });

  ret->addCommandDefinition(ret, "continue", 0, 0,
    [this](Command &cmd) {
      if (cpu->isRunning()) {
        cmd.setError("CPU running");
      } else if (cpu->isHalted()) {
        cmd.setError("CPU halted");
      } else {
        cpu->continueExecution();
      }
    });

  ret->addCommandDefinition(ret, "clock", 0, 1,
    [this](Command &cmd) {
      bool ok;
      double speed;
      switch (cmd.numArgs()) {
        case 0:
          cmd.setResult(QString("%1 kHz").arg(cpu->getSystem()->clockSpeed()));
          break;
        case 1:
          speed = cmd.arg(0).toDouble(&ok);
          if (ok) {
            if (!cpu->getSystem()->setClockSpeed(speed)) {
              cmd.setError(QString("Frequency %1 kHz out of bounds").arg(speed));
            }
          } else {
            cmd.setError(QString("Requested frequency '%1' is not a number").arg(cmd.arg(0)));
          }
          break;
      }
    });

  ret->addCommandDefinition(ret, "load", 1, 1,
    [this](Command &cmd) {
      if (!QFile::exists(cmd.arg(0))) {
        cmd.setError(QString("File %1/%2 does not exist").arg(QDir::currentPath()).arg(cmd.arg(0)));
      } else if (!(QFile::permissions(cmd.arg(0)) | QFileDevice::ReadUser)) {
        cmd.setError(QString("File %1/%2 is not readable").arg(QDir::currentPath()).arg(cmd.arg(0)));
      } else {
        cpu->openImage(cmd.arg(0));
      }
    },
    [this](const QStringList &args) {
      return fileCompletions(args);
    });

  ret->addCommandDefinition(ret, "mem", 1, 2,
    [this](Command &cmd) {
      byte value;
      word addr;
      bool ok;
      bool poke = false;
      byte v;
      int  opcode;

      switch (cmd.numArgs()) {
        case 2:
          value = (byte) cmd.arg(1).toInt(&ok, 0);
          if (!ok) {
            opcode = cpu->getSystem()->controller()->opcodeForInstruction(
              cmd.arg(1).replace(QChar('_'), QChar(' ')).toStdString());
            if (opcode == -1) {
              cmd.setError(QString("Syntax error: unparsable value '%1").arg(cmd.arg(1)));
              return;
            }
            value = (byte) opcode;
          }
          poke = true;
          // Fall through
        case 1:
          addr = (word) cmd.arg(0).toInt(&ok, 0);
          if (!ok) {
            cmd.setError(QString("Syntax error: unparsable address '%1").arg(cmd.arg(0)));
            return;
          }
          if (poke) {
            (*cpu->getSystem()->memory())[addr] = value;
          }
          m_memdump->focusOnAddress(addr);
          v = (*cpu->getSystem()->memory())[addr];
          if ((v >= 32) && (v <= 126)) {
            cmd.setResult(QString::asprintf("*0x%04x = 0x%02x '%c' %s",
                                            addr, v,
                                            ((v >= 32) && (v <= 126)) ? (char) v : '.',
                                            cpu->getSystem()->controller()->instructionWithOpcode(v).c_str()));
          } else {
            cmd.setResult(QString::asprintf("*0x%04x = 0x%02x %s",
                                            addr, v,
                                            cpu->getSystem()->controller()->instructionWithOpcode(v).c_str()));
          }
          break;
        default:
          cmd.setError("Syntax error: too many arguments");
          break;
      }
    });

  return ret;
}

void MainWindow::commandResult(const QString &line, bool ok, const QString &result) {
  addHistory(line);
  m_result->setText((result != "") ? result : (ok) ? "OK" : "ERROR");
  m_result->setStyleSheet(QString("color: %1").arg((ok) ? "green" : "red"));
}

void MainWindow::addHistory(const QString &cmd) {
  m_history->setText(cmd);
}

QVector<QString> MainWindow::fileCompletions(const QStringList &args) {
  QVector<QString> ret;
  if (args.size() != 2) {
    return ret;
  }
  auto path = args[1].split("/");
  QString dirPath(path.mid(0, path.size() -1 ).join("/"));
  QString entry(path[path.size()-1]);
  QDir dir(dirPath);
  for (auto &e : dir.entryInfoList()) {
    if (((entry == "") || e.fileName().startsWith(entry)) && (e.fileName() != ".")) {
      ret.append(QString("%1 %2%3").arg(args[0], e.filePath(), (e.isDir()) ? "/" : ""));
    }
  }
  return ret;
}


//#include "mainwindow.moc"
