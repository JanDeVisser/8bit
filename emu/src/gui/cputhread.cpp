#include "backplane.h"
#include "cputhread.h"
#include "memory.h"

#include <QFile>

class Executor : public QThread {
  Q_OBJECT
  BackPlane *m_system;

public:
  explicit Executor(BackPlane *system, QObject *parent = nullptr)
      : QThread(parent), m_system(system) { }

protected:
  void run() override {
    m_system -> run();
  }
};

CPU::CPU(QObject *parent) : QObject(parent), m_running(false) {
  m_system = new BackPlane();
  m_system -> defaultSetup();
  m_thread = new Executor(m_system, this);
  connect(m_thread, &QThread::finished, this, &CPU::finished);
}

void CPU::run() {
  reset();
  continueExecution();
}

void CPU::continueExecution() {
  start(Controller::Continuous);
}

void CPU::step() {
  start(Controller::BreakAtInstruction);
}

void CPU::tick() {
  start(Controller::BreakAtClock);
}

void CPU::reset() {
  if (!m_running) {
    m_system->reset();
  }
}

void CPU::start(Controller::RunMode runMode) {
  if (!m_running && m_system -> bus().halt()) {
    setRunMode(runMode);
    emit executionStart();
    m_running = true;
    m_thread->start();
  }
}

void CPU::finished() {
  m_running = false;
  if (!m_system->bus().halt()) {
    emit executionEnded();
  } else {
    emit executionInterrupted();
  }
}

bool CPU::isRunning() const {
  return m_running;
}

bool CPU::isHalted() const {
  return !m_system -> bus().halt();
}

bool CPU::isSuspended() const {
  return !m_system -> bus().sus();
}

void CPU::setRunMode(Controller::RunMode runMode) const {
  m_system -> setRunMode(runMode);
}

void CPU::openImage(QString &img) {
  QFile f(img, this);
  f.open(QIODevice::ReadOnly);
  auto bytearr = f.readAll();
  m_system -> loadImage(bytearr.size(), (const byte *) bytearr.data());
}

void CPU::openImage(QString &&img) {
  openImage(img);
}

#include "cputhread.moc"