#include "contentspanel.h"
#include "contentswidget.h"
#include "definedsettings.h"
#include "QsLog.h"
#include "externs.h"
ContentsPanel::ContentsPanel(QWidget * parent) : QWidget(parent) {
  QVBoxLayout * layout = new QVBoxLayout;

  QHBoxLayout * hlayout = new QHBoxLayout;

  m_expandAction = new QAction("Expand",this);
  m_collapseAction = new QAction("Collapse",this);
  m_loadAction = new QAction("Load",this);
  m_flattenAction = new QAction("Collapse all",this);

  connect(m_expandAction,SIGNAL(triggered()),this,SLOT(onExpand()));
  connect(m_collapseAction,SIGNAL(triggered()),this,SLOT(onCollapse()));
  connect(m_loadAction,SIGNAL(triggered()),this,SLOT(onLoad()));
  connect(m_flattenAction,SIGNAL(triggered()),this,SLOT(onFlatten()));

  readSettings();

  QToolButton * tool = new QToolButton;
  tool->setDefaultAction(m_expandAction);
  hlayout->addWidget(tool);

  tool = new QToolButton;
  tool->setDefaultAction(m_collapseAction);
  hlayout->addWidget(tool);

  tool = new QToolButton;
  tool->setDefaultAction(m_flattenAction);
  hlayout->addWidget(tool);
  hlayout->addSpacing(10);

  tool = new QToolButton;
  tool->setDefaultAction(m_loadAction);
  hlayout->addWidget(tool);
    hlayout->addStretch();

  m_tree = new ContentsWidget(this);
  m_tree->setObjectName("treeroots");
  layout->addLayout(hlayout);
  layout->addWidget(m_tree);
  setLayout(layout);
}
void ContentsPanel::readSettings() {
  QDir images(getLexicon()->getResourceFilePath(Lexicon::Image));
  if (!images.exists()) {
    QLOG_WARN() << QString(tr("Theme image directory not found : %1")).arg(images.absolutePath());
  }
  QString filename;
  SETTINGS

  settings.beginGroup("Icons");

  filename = settings.value(SID_ICON_EXPAND,"list-add.svg").toString();
  QFileInfo fi(images,filename);
  if (fi.exists()) {
    QIcon icon(fi.absoluteFilePath());
    if (!icon.isNull()) {
      m_expandAction->setIcon(icon);
    }
  }
  else {
    QLOG_INFO() << QString("Missing icon : %1 - %2").arg(SID_ICON_EXPAND).arg(fi.absoluteFilePath());
  }
  filename = settings.value(SID_ICON_COLLAPSE,"list-remove.svg").toString();
  fi.setFile(images,filename);
  if (fi.exists()) {
    QIcon icon(fi.absoluteFilePath());
    if (!icon.isNull()) {
      m_collapseAction->setIcon(icon);
    }
  }
  else {
    QLOG_INFO() << QString("Missing icon : %1 - %2").arg(SID_ICON_COLLAPSE).arg(fi.absoluteFilePath());
  }
  filename = settings.value(SID_ICON_LOAD,"quickopen.svg").toString();
  fi.setFile(images,filename);
  if (fi.exists()) {
    QIcon icon(fi.absoluteFilePath());
    if (!icon.isNull()) {
      m_loadAction->setIcon(icon);
    }
  }
  else {
    QLOG_INFO() << QString("Missing icon : %1 - %2").arg(SID_ICON_LOAD).arg(fi.absoluteFilePath());
  }
  filename = settings.value(SID_ICON_FLATTEN,"view-calendar-list.svg").toString();
  fi.setFile(images,filename);
  if (fi.exists()) {
    QIcon icon(fi.absoluteFilePath());
    if (!icon.isNull()) {
      m_flattenAction->setIcon(icon);
    }
  }
  else {
    QLOG_INFO() << QString("Missing icon : %1 - %2").arg(SID_ICON_FLATTEN).arg(fi.absoluteFilePath());
  }
}
ContentsWidget * ContentsPanel::tree() {
  return m_tree;
}
void ContentsPanel::onExpand() {
  QLOG_DEBUG() << Q_FUNC_INFO;

  QTreeWidgetItem * item = m_tree->currentItem();
  if (!item) {
    return;
  }
  for(int i=0;i < item->columnCount();i++) {
    qDebug() << item->text(i);
  }
  m_tree->blockSignals(true);
  item->setExpanded(! item->isExpanded());
  if (! item->parent()) {
    qDebug() << "Letter";
  }
  else if (! item->parent()->parent()) {
    qDebug() << "root";
  }
 else {
   qDebug() << "node";
 }
  m_tree->blockSignals(false);

}
void ContentsPanel::onCollapse() {
  QLOG_DEBUG() << Q_FUNC_INFO;

  QTreeWidgetItem * item = m_tree->currentItem();
  if (item) {
    m_tree->blockSignals(true);
    item->setExpanded(! item->isExpanded());
    m_tree->blockSignals(false);
  }

}
void ContentsPanel::onLoad() {
  QLOG_DEBUG() << Q_FUNC_INFO;
}
void ContentsPanel::onFlatten() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  m_tree->blockSignals(true);
  for(int i=0;i < m_tree->topLevelItemCount();i++) {
    m_tree->topLevelItem(i)->setExpanded(false);
  }
  m_tree->blockSignals(false);

}
