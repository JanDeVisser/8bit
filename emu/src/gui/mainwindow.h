#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenuBar>
#include <QProxyStyle>
#include <QPushButton>
#include <QThread>

#include "cputhread.h"
#include "backplane.h"
#include "memdump.h"


class BlockCursorStyle : public QProxyStyle {
public:
  explicit BlockCursorStyle(QStyle *style = nullptr) : QProxyStyle(style) {  }

  int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const override {
    if (metric == QStyle::PM_TextCursorWidth) {
      return 10;
    }
    return QProxyStyle::pixelMetric(metric, option, widget);
  }
};


class CommandLineEdit : public QLineEdit {
  Q_OBJECT

public:
  explicit CommandLineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {
    setStyle(new BlockCursorStyle(style()));
  }
};

struct Command {
  QString     line;
  QString     command;
  QStringList args;
  QString     result = "";
  bool        success = true;

  explicit Command(QString &);
  void     setError(QString &&);
  void     setError(QString &);
  void     setResult(QString &&);
  void     setResult(QString &);
};

class MainWindow : public QMainWindow
{
  Q_OBJECT

private slots:
  void cpuStopped();
  void openFile();
  void commandSubmitted();

public:
  explicit MainWindow(QWidget *parent = nullptr);

private:
  CPU             *cpu = nullptr;
  QAction         *m_exit = nullptr;
  QAction         *m_open = nullptr;
  QLabel          *history;
  QLabel          *result;
  CommandLineEdit *command;

  MemDump         *m_memdump = nullptr;

protected:
  void          createMenu();
  void          paintEvent(QPaintEvent *) override;
  void          addHistory(QString &);
};
#endif // MAINWINDOW_H
