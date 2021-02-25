#include <QApplication>
#include <QListView>
#include <QVBoxLayout>

#include "memdump.h"

MemModel::MemModel(const Memory &memory, QObject *parent)
    : QAbstractListModel(parent), m_memory(memory) {
  m_address = memory.ramStart();
}

int MemModel::rowCount(const QModelIndex &parent) const {
  return parent.isValid() ? 0 : m_memory.ramSize() / 8;
}

QVariant MemModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (index.row() >= (m_memory.ramSize() / 8) || index.row() < 0)
    return QVariant();

  if (role == Qt::DisplayRole) {
    return getRow(index.row());
  } else if (role == Qt::BackgroundRole) {
    int batch = (index.row() / 100) % 2;
    if (batch == 0)
      return qApp->palette().base();
    else
      return qApp->palette().alternateBase();
  }
  return QVariant();
}

bool MemModel::canFetchMore(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return false;
  }
  return (m_address < m_memory.ramEnd());
}

void MemModel::fetchMore(const QModelIndex &parent) {
  if (parent.isValid()) {
    return;
  }
  word remainder = m_memory.ramEnd() - m_address;
  int bytesToFetch = qMin(800, (int) remainder);

  if (bytesToFetch <= 0) {
    return;
  }

  beginInsertRows(QModelIndex(), m_address * 8, (m_address * 8) + bytesToFetch - 1);
  m_address += bytesToFetch;
  endInsertRows();

  emit numberPopulated(bytesToFetch / 8);
}

QVariant MemModel::getRow(int row) const {
  word start = (word) (row * 8);
  word end = (word) qMin(start + 8, (int) m_memory.ramEnd());

  auto s = QString("%1   ").arg(start, 4, 16, QLatin1Char('0'));
  for (word ix = start; ix < end; ix++) {
    s += QString(" %1").arg(m_memory[ix], 2, 16, QLatin1Char('0'));
  }
  return QVariant(s);
}

void MemModel::reload() {
  beginResetModel();
  m_address = m_memory.ramStart();
  endResetModel();
}

QModelIndex MemModel::indexOf(word addr) const {
  if (addr == 0xFFFF) {
    addr = m_memory.getValue();
  }
  auto row = (addr - m_memory.ramStart()) / 8;
  auto col = 0;
  return createIndex(row, col);
}

/* ----------------------------------------------------------------------- */

MemDump::MemDump(BackPlane *system, QWidget *parent)
    : QWidget(parent), m_system(system), m_model(*(system -> memory()))  {
  m_view = new QListView;
  m_view->setModel(&m_model);
  QFont font("ibm3270", 12);
  QFontMetrics metrics(font);
  auto width = metrics.horizontalAdvance("0000    00 00 00 00 00 00 00 00");
  m_view->setMinimumWidth(width + 40);
  m_view -> setFont(font);
  m_view -> setFocusPolicy(Qt::ClickFocus);

  m_view -> setStyleSheet("QListView::item { background: black; color : green; } "
                          "QListView::item:selected { background: green; color: black; }");
  auto *layout = new QVBoxLayout;
  layout->addWidget(m_view);
  setLayout(layout);
  setWindowTitle(tr("Memory"));
}

void MemDump::reload() {
  m_model.reload();
}

void MemDump::focusOnAddress(word addr) {
  m_view -> setCurrentIndex(m_model.indexOf(addr));
}

void MemDump::focus() {
  focusOnAddress(0xFFFF);
}

