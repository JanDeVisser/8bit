#include "backplane.h"
#include "cputhread.h"
#include "memory.h"

#include <QFile>

CPU::CPU(QObject *parent) : QObject(parent), m_running(false), m_status(), m_keys(), m_kbdMutex() {
  m_system = new BackPlane();
  m_system -> defaultSetup();
  m_system -> setOutputStream(m_status);
  m_keyboard = new IOChannel(0x00, "KEY", [this]() {
    byte ret = 0x00;
    m_kbdMutex.lock();
    if (!m_keys.empty()) {
      ret = m_keys[0];
      m_keys.pop_front();
    }
    m_kbdMutex.unlock();
    return ret;
  });

  m_terminal = new IOChannel(0x01, "OUT", [this](byte out) {
    emit terminalWrite(QString((char) out));
  });

  m_system -> insertIO(m_keyboard);
  m_system -> insertIO(m_terminal);

  m_thread = new Executor(m_system, this);
  connect(m_thread, &QThread::finished, this, &CPU::finished);

  QFile initial("./emu.bin");
  if (initial.exists()) {
    openImage(initial.fileName());
  }
}

void CPU::run(word addr) {
  reset();
  m_thread->startAddress(addr);
  continueExecution();
}

void CPU::continueExecution() {
  start(Controller::Continuous);
}

void CPU::step(word addr) {
  start(Controller::BreakAtInstruction);
}

void CPU::tick(word addr) {
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
//  std::cout << m_status.str();
  if (!m_system->bus().halt()) {
    emit executionEnded(QString::fromStdString(m_status.str()));
  } else {
    emit executionInterrupted(QString::fromStdString(m_status.str()));
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

void CPU::openImage(QFile &img, word addr, bool writable) {
  img.open(QIODevice::ReadOnly);
  auto bytearr = img.readAll();
  m_system -> loadImage(bytearr.size(), (const byte *) bytearr.data(), addr, writable);
}

void CPU::openImage(QFile &&img, word addr, bool writable) {
  openImage(img, addr, writable);
}

void CPU::openImage(const QString &img, word addr, bool writable) {
  openImage(QFile(img), addr, writable);
}

void CPU::keyPressed(QKeyEvent *key) {
  m_kbdMutex.lock();
  emit terminalWrite(key->text());

//  m_keys.emplace_back(key);
//  if (m_keys.size() == 1) {
//    m_system->bus().setNmi();
//  }
  m_kbdMutex.unlock();
}

//#include "cputhread.moc"