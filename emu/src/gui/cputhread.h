#ifndef EMU_CPUTHREAD_H
#define EMU_CPUTHREAD_H

#include <deque>
#include <mutex>

#include <QKeyEvent>
#include <QThread>

#include "backplane.h"
#include "iochannel.h"

class CPU : public QObject {
  Q_OBJECT

public:
  explicit CPU(QObject * = nullptr);
  ~CPU() override = default;
  BackPlane *    getSystem() { return m_system; }
  void           setRunMode(Controller::RunMode) const;
  void           openImage(const QString &);
  void           openImage(QFile &);
  void           openImage(QFile &&);

  void           run();
  void           continueExecution();
  void           step();
  void           tick();
  void           reset();
  bool           isRunning() const;
  bool           isHalted() const;
  bool           isSuspended() const;
  void           keyPressed(QKeyEvent *);

signals:
  void executionStart();
  void executionEnded(const QString &);
  void executionInterrupted(const QString &);
  void terminalWrite(const QString &);


private:
  QThread           *m_thread;
  BackPlane         *m_system;
  IOChannel         *m_keyboard;
  IOChannel         *m_terminal;
  bool               m_running;
  std::stringstream  m_status;
  std::deque<int>    m_keys;
  std::mutex         m_kbdMutex;

  void start(Controller::RunMode);

private slots:
  void finished();
};


#endif //EMU_CPUTHREAD_H
