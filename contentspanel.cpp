#include "contentspanel.h"
#include "contentswidget.h"
#include "QsLog.h"
ContentsPanel::ContentsPanel(QWidget * parent) : QWidget(parent) {
  QVBoxLayout * layout = new QVBoxLayout;

  QHBoxLayout * hlayout = new QHBoxLayout;
  QPushButton * button;

  button = new QPushButton("+");
  hlayout->addWidget(button);
  connect(button,SIGNAL(clicked()),this,SLOT(onExpand()));

  button = new QPushButton("-");
  hlayout->addWidget(button);
  connect(button,SIGNAL(clicked()),this,SLOT(onCollapse()));

  m_tree = new ContentsWidget(this);
  m_tree->setObjectName("treeroots");
  layout->addLayout(hlayout);
  layout->addWidget(m_tree);
  setLayout(layout);
}
ContentsWidget * ContentsPanel::tree() {
  return m_tree;
}
void ContentsPanel::onExpand() {
  QLOG_DEBUG() << Q_FUNC_INFO;

  QTreeWidgetItem * item = m_tree->currentItem();
  if (item) {
    item->setExpanded(true);
  }

}
void ContentsPanel::onCollapse() {
  QLOG_DEBUG() << Q_FUNC_INFO;

  QTreeWidgetItem * item = m_tree->currentItem();
  if (item) {
    item->setExpanded(false);
  }

}
