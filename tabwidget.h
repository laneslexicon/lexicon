#ifndef __TABWIDGET_H__
#define __TABWIDGET_H__
#include <QTabWidget>
#include <QKeyEvent>
#include <QDebug>

class TabWidget : public QTabWidget {
  Q_OBJECT
 public:
  TabWidget(QWidget  * parent = 0);
 private:
  void keyPressEvent(QKeyEvent *);
};
#endif
