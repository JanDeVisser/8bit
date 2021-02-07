#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QPushButton>
#include <QThread>

#include "cputhread.h"
#include "backplane.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT

private slots:
  void runClicked();
  void cycleClicked();
  void instrClicked();
  void enableButtons();
  void disableButtons();

public:
  explicit MainWindow(QWidget *parent = nullptr);

private:
  CPUThread    *thread = nullptr;
  QPushButton  *run;
  QPushButton  *cycle;
  QPushButton  *instr;

protected:
  void       createMenu();
};
#endif // MAINWINDOW_H
