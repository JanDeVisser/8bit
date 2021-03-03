#include "blockcursorstyle.h"

BlockCursorStyle::BlockCursorStyle(QStyle *style) : QProxyStyle(style) {
}

int BlockCursorStyle::pixelMetric(PixelMetric metric,
                                  const QStyleOption *option,
                                  const QWidget *widget) const {
  if (metric == QStyle::PM_TextCursorWidth) {
    return 10;
  }
  return QProxyStyle::pixelMetric(metric, option, widget);
}
