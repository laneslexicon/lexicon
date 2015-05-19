#include "QsLog.h"
#include "tabwidget.h"
#include "graphicsentry.h"
#include "headsearch.h"
#include "fullsearch.h"
#include "application.h"
#include "definedsettings.h"
#include "externs.h"
TabWidget::TabWidget(QWidget * parent) : QTabWidget(parent) {
  setObjectName("tabwidget");
  tabBar()->setObjectName("arabicpagetab");
  readSettings();
  connect(this,SIGNAL(tabBarClicked(int)),this,SLOT(onTabBarClicked(int)));
  if (m_numberTabs) {
    connect(tabBar(),SIGNAL(tabMoved(int,int)),this,SIGNAL(tabsChanged()));
  }
  //  tabBar()->installEventFilter(this);
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
void TabWidget::onTabBarClicked(int /* ix */) {
  /*
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(this->widget(ix));
  if (entry) {
    QLOG_DEBUG() << "showing place for tab" << ix;
    entry->focusPlace();
  }
  */
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
void TabWidget::readSettings() {
  SETTINGS
  settings.beginGroup("System");
  m_numberTabs = settings.value(SID_SYSTEM_TABBAR_NUMBER,true).toBool();
  setMovable(settings.value(SID_SYSTEM_TABBAR_MOVEABLE,true).toBool());
}
void TabWidget::tabContentsChanged() {
  if (m_numberTabs) {
    emit(tabsChanged());
  }
}
void TabWidget::focusOutEvent(QFocusEvent * /* event */) {
  //  QLOG_DEBUG() << Q_FUNC_INFO;
}
bool TabWidget::numberTab() const {
  return m_numberTabs;
}
