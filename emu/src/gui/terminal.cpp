#include <QVBoxLayout>

#include "blockcursorstyle.h"
#include "terminal.h"

TerminalView::TerminalView(CPU *cpu, Terminal *terminal)
    : QTextEdit(terminal), m_cpu(cpu), m_terminal(terminal) {
  setStyle(new BlockCursorStyle(style()));
  setFont(QFont("ibm3270", 12));
  setStyleSheet("QTextEdit { color: green; background-color: black; }");
  connect(m_cpu, &CPU::terminalWrite, this, &TerminalView::writeCharacter);
}

void TerminalView::keyPressEvent(QKeyEvent *e) {
  m_cpu->keyPressed(e);
}

void TerminalView::writeCharacter(const QString &key) {
  auto t = toPlainText();
  t += key;
  setPlainText(t);
  auto cursor = textCursor();
  cursor.movePosition(QTextCursor::End);
  setTextCursor(cursor);
  setStyle(new BlockCursorStyle(style()));
}

Terminal::Terminal(CPU *cpu, QWidget *parent) : QWidget(parent), m_cpu(cpu) {
  m_terminal = new TerminalView(cpu,this);
  auto layout = new QVBoxLayout;
  layout->addWidget(m_terminal);
  setLayout(layout);
}

