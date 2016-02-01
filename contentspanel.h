#ifndef _CONTENTSPANEL_H_
#define _CONTENTSPANEL_H_
#include <QtCore>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QIcon>
#include <QAction>
#include <QSettings>
class ContentsWidget;
class ContentsPanel : public QWidget {
  Q_OBJECT

 public:
  ContentsPanel(QWidget * parent = 0);
  ContentsWidget * tree();
  private slots:
    void onExpand();
    void onCollapse();
    void onLoad();
    void onFlatten();
 private:
  ContentsWidget * m_tree;
  void readSettings();
  QAction * m_expandAction;
  QAction * m_collapseAction;
  QAction * m_loadAction;
  QAction * m_flattenAction;
};
#endif
