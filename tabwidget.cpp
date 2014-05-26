#include "tabwidget.h"
#include "graphicsentry.h"
TabWidget::TabWidget(QWidget * parent) : QTabWidget(parent) {
  setObjectName("entrywidget");
  tabBar()->setObjectName("entrytabs");
}
void TabWidget::keyPressEvent(QKeyEvent * event) {
    switch (event->key()) {
    case Qt::Key_Enter :
    case Qt::Key_Space :
    case Qt::Key_Return : {
      GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(this->currentWidget());
      if (entry) {
        entry->focusPlace();
      }
      return;
    }
    default:
      return;
    }
}
//    if (event->key() == Qt::Key_Escape) {
//      QLOG_DEBUG() << Q_FUNC_INFO << "escape";
//    }
