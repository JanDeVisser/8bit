#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QPushButton>
#include <QThread>

#include "backplane.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CPUThread : public QThread {
Q_OBJECT

public:
  CPUThread(BackPlane *, QObject * = nullptr);
  ~CPUThread() override = default;

signals:
  void executionEnded(void);

protected:
  void run() override;

private:
  BackPlane *system;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

private slots:
    void runClicked();
    void enableRun();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
  CPUThread    *thread = nullptr;
  BackPlane    *system;
  QPushButton  *run;

protected:
  void       createMenu();

};
#endif // MAINWINDOW_H
