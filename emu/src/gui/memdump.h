#ifndef EMU_MEMDUMP_H
#define EMU_MEMDUMP_H

#include <QAbstractListModel>
#include <QWindow>

#include "backplane.h"
#include "memory.h"

class MemModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit MemModel(const Memory &, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    void reload();

signals:
    void numberPopulated(int number);

public slots:
//    void setDirPath(const QString &path);

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

signals:

protected:

private:
  BackPlane *m_system;
  MemModel   m_model;
};


#endif //EMU_CPUTHREAD_H

