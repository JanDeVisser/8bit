#ifndef EMU_MEMDUMP_H
#define EMU_MEMDUMP_H

#include <QAbstractListModel>
#include <QListView>
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

signals:
    void numberPopulated(int number);

public slots:
  void         reload();

protected:
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

private:
  word          m_address;
  const Memory &m_memory;

  QVariant      getRow(int) const;
};


class MemDump : public QWidget {
  Q_OBJECT

public:
  explicit MemDump(BackPlane *, QWidget * = nullptr);
  ~MemDump()     override = default;
  BackPlane *    getSystem() const  { return m_system; }
  void           reload();
  void           focusOnAddress(word addr=0xFFFF);

public slots:
  void focus();

signals:

protected:

private:
  BackPlane *m_system;
  MemModel   m_model;
  QListView *m_view;
};


#endif //EMU_CPUTHREAD_H

