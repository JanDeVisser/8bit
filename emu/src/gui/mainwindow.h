#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenuBar>
#include <QPainter>
#include <QProxyStyle>
#include <QPushButton>
#include <QStyleOption>
#include <QThread>

#include "commands.h"

#include "cputhread.h"
#include "backplane.h"
#include "memdump.h"


class MainWindow : public QMainWindow
{
  Q_OBJECT

private slots:
  void cpuStopped();
  void openFile();

public:
  explicit MainWindow(QWidget *parent = nullptr);

private:
  CPU             *cpu = nullptr;
  QAction         *m_exit = nullptr;
  QAction         *m_open = nullptr;
  QLabel          *m_history;
  QLabel          *m_result;
  CommandLineEdit *m_command;

  MemDump         *m_memdump = nullptr;

  CommandLineEdit * makeCommandLine();

private slots:
  void          commandResult(const QString &, bool, const QString &);

protected:
  void          createMenu();
  void          addHistory(const QString &);
  static QVector<QString> fileCompletions(const QStringList &);
};
#endif // MAINWINDOW_H
