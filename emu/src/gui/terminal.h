#ifndef EMU_TERMINAL_H
#define EMU_TERMINAL_H

#include <QKeyEvent>
#include <QTextEdit>
#include <QWidget>

#include "cputhread.h"

class Terminal;

class TerminalView : public QTextEdit {
private:
Q_OBJECT;
  CPU      *m_cpu;
  Terminal *m_terminal;

protected:
  void keyPressEvent(QKeyEvent *) override;

public:
  TerminalView(CPU *cpu, Terminal *terminal);

public slots:
  void writeCharacter(const QString &);
};


class Terminal : public QWidget {
Q_OBJECT
  CPU          *m_cpu;
  TerminalView *m_terminal;

protected:
//  void keyPressEvent(QKeyEvent *) override;

public:
  explicit Terminal(CPU *cpu, QWidget * = nullptr);

public slots:
//  void     writeCharacter(int);
};

#endif //EMU_TERMINAL_H