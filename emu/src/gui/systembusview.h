#ifndef EMU_SYSTEMBUSVIEW_H
#define EMU_SYSTEMBUSVIEW_H

#include <QWidget>

#include "qled.h"

#include "componentview.h"
#include "systembus.h"

class SystemBusView : public QWidget, public ComponentListener {
  Q_OBJECT

signals:
  void valueChanged();

public:
  explicit SystemBusView(SystemBus &bus, QWidget *parent = nullptr);
  void componentEvent(Component *sender, int ev) override;

private slots:
  void refresh();

private:
  SystemBus         &systemBus;
  QLayout           *layout;
  ByteWidget        *data;
  ByteWidget        *address;
  RegisterNameLabel *put;
  RegisterNameLabel *get;
  QLed              *xdata;
  QLed              *xaddr;
  QLedArray         *op;
  QLabel            *z;
  QLabel            *c;
  QLabel            *v;
};

#endif //EMU_SYSTEMBUS_H
