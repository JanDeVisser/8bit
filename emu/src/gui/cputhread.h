#ifndef EMU_CPUTHREAD_H
#define EMU_CPUTHREAD_H

#include <QThread>

#include "backplane.h"

class CPU : public QObject {
  Q_OBJECT

public:
  explicit CPU(QObject * = nullptr);
  ~CPU() override = default;
  BackPlane *    getSystem() { return m_system; }
  void           setRunMode(Controller::RunMode) const;
  void           openImage(QString &&);
  void           openImage(QString &);

  void           run();
  void           continueExecution();
  void           step();
  void           tick();
  void           reset();
  bool           isRunning() const;
  bool           isHalted() const;
  bool           isSuspended() const;

signals:
  void executionStart();
  void executionEnded();
  void executionInterrupted();


private:
  QThread   *m_thread;
  BackPlane *m_system;
  bool       m_running;

  void start(Controller::RunMode);
private slots:
  void finished();
};


#endif //EMU_CPUTHREAD_H
