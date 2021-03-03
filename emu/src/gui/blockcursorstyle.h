#ifndef EMU_BLOCKCURSORSTYLE_H
#define EMU_BLOCKCURSORSTYLE_H

#include <QProxyStyle>

class BlockCursorStyle : public QProxyStyle {
public:
  explicit BlockCursorStyle(QStyle * = nullptr);
  int      pixelMetric(PixelMetric, const QStyleOption *, const QWidget *) const override;
};

#endif //EMU_BLOCKCURSORSTYLE_H
