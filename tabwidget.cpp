#include "tabwidget.h"
#include "graphicsentry.h"
#include "headsearch.h"
#include "fullsearch.h"
#include "application.h"
#include "definedsettings.h"
TabWidget::TabWidget(QWidget * parent) : QTabWidget(parent) {
  setObjectName("tabwidget");
  tabBar()->setObjectName("tabwidgettabs");
  readSettings();
  connect(this,SIGNAL(tabBarClicked(int)),this,SLOT(onTabBarClicked(int)));
  if (m_numberTabs) {
    connect(tabBar(),SIGNAL(tabMoved(int,int)),this,SIGNAL(tabsChanged()));
  }
  //  tabBar()->installEventFilter(this);
}
void TabWidget::keyPressEvent(QKeyEvent * event) {
    switch (event->key()) {
      /*
    case Qt::Key_Tab: {
      qDebug() << Q_FUNC_INFO << "tab pressed";
    }
    case Qt::Key_Right: {
      qDebug() << Q_FUNC_INFO << "right pressed";
    }
      */
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
void TabWidget::focusOutEvent(QFocusEvent * /* event */) {
  QLOG_DEBUG() << Q_FUNC_INFO;
}
/*
bool TabWidget::eventFilter(QObject * target,QEvent * event) {
    if (event->type() == QEvent::KeyPress) {
    QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
    switch(keyEvent->key()) {
      case Qt::Key_Tab:
        qStrip << Q_FUNC_INFO << "got tab";
        break;
      case Qt::Key_Left:
        qStrip << Q_FUNC_INFO << "got left";
        break;
        /*
      case Qt::Key_T: {
        if (keyEvent->modifiers() && Qt::ControlModifier) {
          m_tree->setFocus();
          return true;
        }
        break;
      }
      case Qt::Key_E: {
        if (keyEvent->modifiers() && Qt::ControlModifier) {
          if (target == m_tree)
          m_tabs->currentWidget()->setFocus();
          return true;
        }
        break;
      }
        */
    default:
      break;
    }
  }

  return QTabWidget::eventFilter(target,event);
}
*/
