#include "backplane.h"
#include "cputhread.h"

CPUThread::CPUThread(QObject *parent) : QThread(parent) {
  m_system = new BackPlane();
  m_system -> defaultSetup();
}

void CPUThread::run() {
  emit executionStart();
  m_system -> run();
  emit executionEnded();
}

void CPUThread::setRunMode(Controller::RunMode runMode) const {
  m_system -> setRunMode(runMode);
}