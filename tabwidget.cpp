#include "tabwidget.h"
#include "graphicsentry.h"
#include "headsearch.h"
#include "fullsearch.h"
TabWidget::TabWidget(QWidget * parent) : QTabWidget(parent) {
  setObjectName("entrywidget");
  tabBar()->setObjectName("entrytabs");
  connect(this,SIGNAL(tabBarClicked(int)),this,SLOT(onTabBarClicked(int)));
}
void TabWidget::keyPressEvent(QKeyEvent * event) {
    switch (event->key()) {
    case Qt::Key_Enter :
    case Qt::Key_Space :
    case Qt::Key_Return : {
      GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(this->currentWidget());
      if (entry) {
        entry->focusPlace();
        return;
      }
      HeadSearchWidget * search = qobject_cast<HeadSearchWidget *>(this->currentWidget());
      if (search) {
        search->focusTable();
        return;
      }
      FullSearchWidget * fullsearch = qobject_cast<FullSearchWidget *>(this->currentWidget());
      if (fullsearch) {
        fullsearch->focusTable();
        return;
      }
      break;
    }

    default:
      return;
    }
}
void TabWidget::onTabBarClicked(int ix) {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(this->widget(ix));
  if (entry) {
    entry->focusPlace();
  }
}
//    if (event->key() == Qt::Key_Escape) {
//      QLOG_DEBUG() << Q_FUNC_INFO << "escape";
//    }
