#ifndef __TABWIDGET_H__
#define __TABWIDGET_H__
#include <QTabWidget>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QDebug>
#include <QInputDialog>
class TabWidget : public QTabWidget {
  Q_OBJECT
 public:
  TabWidget(QWidget  * parent = 0);
  bool numberTab() const;
  public slots:
    void onTabBarClicked(int);
    void onTabBarDoubleClicked(int);
    void tabContentsChanged();
 protected:
    void tabRemoved(int index);
    void tabInserted(int index);
    void focusOutEvent(QFocusEvent *);
    void contextMenuEvent(QContextMenuEvent *);
 private:
    void readSettings();
    void keyPressEvent(QKeyEvent *);
    bool m_numberTabs;
    //    bool eventFilter(QObject *,QEvent *);
 signals:
  void tabsChanged();
  void closeOtherTab(int);
  void closeThisTab(int);
  void duplicateTab(int);
  void saveTabs();
};
#endif
