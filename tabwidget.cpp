#include "tabwidget.h"
#include "graphicsentry.h"
#include "headsearch.h"
#include "fullsearch.h"
#include "application.h"
#include "definedsettings.h"
TabWidget::TabWidget(QWidget * parent) : QTabWidget(parent) {
  setObjectName("entrywidget");
  tabBar()->setObjectName("entrytabs");
  readSettings();
  connect(this,SIGNAL(tabBarClicked(int)),this,SLOT(onTabBarClicked(int)));
  if (m_numberTabs) {
    connect(tabBar(),SIGNAL(tabMoved(int,int)),this,SIGNAL(tabsChanged()));
  }
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
void TabWidget::tabRemoved(int index) {
  QTabWidget::tabRemoved(index);
  if (m_numberTabs) {
    emit(tabsChanged());
  }
}
void TabWidget::tabInserted(int index) {
  QTabWidget::tabInserted(index);
  if (m_numberTabs) {
    emit(tabsChanged());
  }
}
//    if (event->key() == Qt::Key_Escape) {
//      QLOG_DEBUG() << Q_FUNC_INFO << "escape";
//    }
void TabWidget::readSettings() {
  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());
  settings->beginGroup("TabBar");
  m_numberTabs = settings->value(SID_TABBAR_NUMBER,true).toBool();
  setMovable(settings->value(SID_TABBAR_MOVEABLE,true).toBool());
}
void TabWidget::tabContentsChanged() {
  if (m_numberTabs) {
    emit(tabsChanged());
  }
}
