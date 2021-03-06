#ifndef EMU_MEMDUMP_H
#define EMU_MEMDUMP_H

#include <QAbstractListModel>
#include <QListView>
#include <QComboBox>
#include <QWindow>

#include "backplane.h"
#include "memory.h"

class MemModel : public QAbstractListModel {
    Q_OBJECT

public:
  explicit     MemModel(const Memory &, QObject *parent = nullptr);

  int          rowCount(const QModelIndex &parent) const override;
  QVariant     data(const QModelIndex &index, int role) const override;
  QModelIndex  indexOf(word addr=0xFFFF) const;
  word         currentAddress() const { return m_address; }
  int          currentBankIndex() const { return m_bankIx; }

signals:
  void         numberPopulated(int number);

public slots:
  void         reset(word addr);
  void         reset(int, word);
  void         reload();

protected:
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

private:
  word           m_address;
  int            m_bankIx;
  MemoryBank     m_bank;
  const Memory & m_memory;

  QVariant       getRow(int) const;
};


class MemDump : public QWidget {
  Q_OBJECT

public:
  explicit       MemDump(BackPlane *, QWidget * = nullptr);
                 ~MemDump() override = default;
  BackPlane *    getSystem() const  { return m_system; }
  void           reload();
  void           focusOnAddress(word addr=0xFFFF);

public slots:
  void           focus();
  void           selectBank(int ix);

signals:

protected:

private:
  BackPlane *m_system;
  MemModel   m_model;
  QListView *m_view;
  QComboBox *m_banks;
};


#endif //EMU_CPUTHREAD_H

