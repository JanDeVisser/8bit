#ifndef EMU_CPUTHREAD_H
#define EMU_CPUTHREAD_H

#include <QThread>

#include "backplane.h"

class CPUThread : public QThread {
  Q_OBJECT

public:
  explicit CPUThread(QObject * = nullptr);
  ~CPUThread() override = default;
  BackPlane *    getSystem() { return m_system; }
  void           setRunMode(Controller::RunMode) const;

signals:
  void executionStart(void);
  void executionEnded(void);

protected:
  void run() override;

private:
  BackPlane *m_system;
};


#endif //EMU_CPUTHREAD_H
