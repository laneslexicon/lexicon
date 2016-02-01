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
#include <QCheckBox>
class ContentsWidget;
class ContentsPanel : public QWidget {
  Q_OBJECT

 public:
  ContentsPanel(QWidget * parent = 0);
  ContentsWidget * tree();
  bool isNewTabRequired() const;
  bool activateTab() const;
  private slots:
    void onExpand();
    void onCollapse();
    void onLoad();
    void onFlatten();
    void onToggle();
 private:
  ContentsWidget * m_tree;
  void readSettings();
  QAction * m_expandAction;
  QAction * m_collapseAction;
  QAction * m_loadAction;
  QAction * m_flattenAction;
  QAction * m_newTabAction;
  QAction * m_newBackgroundTabAction;
  QCheckBox * m_newTab;
  QToolButton * m_newTabButton;
  QToolButton * m_newBackgroundTabButton;
};
#endif
