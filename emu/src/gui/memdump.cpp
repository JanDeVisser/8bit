#include <QApplication>
#include <QComboBox>
#include <QListView>
#include <QVBoxLayout>

#include "memdump.h"

MemModel::MemModel(const Memory &memory, QObject *parent)
    : QAbstractListModel(parent), m_memory(memory), m_bank() {
  auto banks = memory.banks();
  if (!banks.empty()) {
    m_bank = *banks.begin();
    m_bankIx = 0;
  } else {
    m_bankIx = -1;
  }
  m_address = m_bank.start();
}

int MemModel::rowCount(const QModelIndex &parent) const {
  return parent.isValid() ? 0 : m_bank.size() / 8;
}

QVariant MemModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (index.row() >= (m_bank.size() / 8) || index.row() < 0)
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
  return (m_address < m_bank.end());
}

void MemModel::fetchMore(const QModelIndex &parent) {
  if (parent.isValid()) {
    return;
  }
  word remainder = m_bank.end() - m_address;
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
  word end = (word) qMin(start + 8, (int) m_bank.end());

  auto s = QString("%1   ").arg(start, 4, 16, QLatin1Char('0'));
  for (word ix = start; ix < end; ix++) {
    s += QString(" %1").arg(m_memory[ix], 2, 16, QLatin1Char('0'));
  }
  return QVariant(s);
}

void MemModel::reset(word addr) {
  beginResetModel();
  m_bank = m_memory.bank(addr);
  if (!m_bank.valid()) {
    m_bank = *m_memory.banks().begin();
    m_bankIx = 0;
    m_address = m_bank.start();
  } else {
    int ix = 0;
    for (auto &b : m_memory.ban  ks()) {
      if (b.mapped(addr)) {
        m_bankIx = ix;
        break;
      }
      ix++;
    }
    m_address = addr;
  }
  endResetModel();
}

void MemModel::reset(int ix, word addr) {
  beginResetModel();
  m_bank = m_memory.bank(addr);
  m_bankIx = ix;
  m_address = m_bank.start();
  endResetModel();
}

void MemModel::reload() {
  reset(m_address);
}

QModelIndex MemModel::indexOf(word addr) const {
  if (addr == 0xFFFF) {
    addr = m_memory.getValue();
  }
  auto row = (addr - m_bank.start()) / 8;
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

  m_banks = new QComboBox;
  for (auto &bank : system->memory()->banks()) {
    m_banks->addItem(QString(bank.name().c_str()), bank.start());
  }
  connect(m_banks, SIGNAL(currentIndexChanged(int)), this, SLOT(selectBank(int)));

  auto *layout = new QVBoxLayout;
  layout->addWidget(m_view);
  layout->addWidget(m_banks);
  setLayout(layout);
  setWindowTitle(tr("Memory"));
}

void MemDump::reload() {
  m_model.reload();
  m_banks->clear();
  for (auto &bank : getSystem()->memory()->banks()) {
    m_banks->addItem(QString(bank.name().c_str()), bank.start());
  }
  m_banks->setCurrentIndex(m_model.currentBankIndex());
}

void MemDump::focusOnAddress(word addr) {
  m_model.reset(addr);
  if (m_banks->currentIndex() != m_model.currentBankIndex()) {
    m_banks->setCurrentIndex(m_model.currentBankIndex());
  }
  m_view -> setCurrentIndex(m_model.indexOf(addr));
}

void MemDump::focus() {
  focusOnAddress(0x0000);
}

void MemDump::selectBank(int ix) {
  auto addr = m_banks->currentData().toInt();
  m_model.reset(ix, addr);
}
