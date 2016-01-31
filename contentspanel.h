#ifndef _CONTENTSPANEL_H_
#define _CONTENTSPANEL_H_
#include <QtCore>
#include <QtGui>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

class ContentsWidget;
class ContentsPanel : public QWidget {
  Q_OBJECT

 public:
  ContentsPanel(QWidget * parent = 0);
  ContentsWidget * tree();
  private slots:
    void onExpand();
    void onCollapse();
 private:
  ContentsWidget * m_tree;
};
#endif
