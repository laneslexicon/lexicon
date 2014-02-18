#ifndef __BOOKMARKMENU_H__
#define __BOOKMARKMENU_H__
#include <QMenuBar>
#include <QMouseEvent>
#include <QDebug>
#include <QWidget>
#include <QAction>
#include <QMenu>
class AppMenu : public QMenuBar {
  Q_OBJECT
 public:
  AppMenu(QWidget * parent = 0);
 protected:
  void mousePressEvent(QMouseEvent *);
 signals:
  void rebuildBookmarks();
};
#endif
