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
  m_newTabAction = new QAction("New tab",this);
  m_newBackgroundTabAction = new QAction("Make current",this);

  connect(m_expandAction,SIGNAL(triggered()),this,SLOT(onExpand()));
  connect(m_collapseAction,SIGNAL(triggered()),this,SLOT(onCollapse()));
  connect(m_loadAction,SIGNAL(triggered()),this,SLOT(onLoad()));
  connect(m_flattenAction,SIGNAL(triggered()),this,SLOT(onFlatten()));
  connect(m_newTabAction,SIGNAL(triggered()),this,SLOT(onToggle()));
  connect(m_newBackgroundTabAction,SIGNAL(triggered()),this,SLOT(onToggle()));

  readSettings();

  /// the buttons are defaulting to 15x15
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

  m_newTabButton = new QToolButton;
  m_newTabButton->setDefaultAction(m_newTabAction);
  m_newTabButton->setCheckable(true);

  hlayout->addWidget(m_newTabButton);

  m_newBackgroundTabButton = new QToolButton;
  m_newBackgroundTabButton->setDefaultAction(m_newBackgroundTabAction);
  m_newBackgroundTabButton->setCheckable(true);

  hlayout->addWidget(m_newBackgroundTabButton);

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
  filename = settings.value(SID_ICON_NEW_TAB,"tab-new.svg").toString();
  fi.setFile(images,filename);
  if (fi.exists()) {
    QIcon icon(fi.absoluteFilePath());
    if (!icon.isNull()) {
      m_newTabAction->setIcon(icon);
    }
  }
  else {
    QLOG_INFO() << QString("Missing icon : %1 - %2").arg(SID_ICON_FLATTEN).arg(fi.absoluteFilePath());
  }

  filename = settings.value(SID_ICON_NEW_BACKGROUND_TAB,"tab-new-background.svg").toString();
  fi.setFile(images,filename);
  if (fi.exists()) {
    QIcon icon(fi.absoluteFilePath());
    if (!icon.isNull()) {
      m_newBackgroundTabAction->setIcon(icon);
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

  if (item->isExpanded()) {
    item->setExpanded(false);
    return;
  }
  if (! item->parent()) {    /// its a letter
    item->setExpanded(true);
    return;
  }
  if (! item->parent()->parent()) { /// its a root
    m_tree->blockSignals(true);
    m_tree->addEntries(item->text(0),item);
    item->setExpanded(true);
    m_tree->blockSignals(false);
    qDebug() << "root";
    return;
  }


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

  QTreeWidgetItem * item = m_tree->currentItem();
  if (! item ) {
    return;
  }
  if (! item->parent()) {
    return;
  }
  for(int i=0;i < item->columnCount();i++) {
    qDebug() << i << item->text(i);
  }
  m_tree->itemActivated(item,0);

}
void ContentsPanel::onFlatten() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  m_tree->blockSignals(true);
  for(int i=0;i < m_tree->topLevelItemCount();i++) {
    m_tree->topLevelItem(i)->setExpanded(false);
  }
  m_tree->blockSignals(false);

}
bool ContentsPanel::isNewTabRequired() const {
  return m_newTabButton->isChecked();
}
bool ContentsPanel::activateTab() const {
  return  m_newBackgroundTabButton->isChecked();
}
void ContentsPanel::onToggle() {
  QLOG_DEBUG() << Q_FUNC_INFO;

  QAction * action = qobject_cast<QAction *>(sender());
  if (! action ) {
    return;
  }
  QToolButton * button = NULL;
  if (action == m_newTabAction) {
    button = m_newTabButton;
  }
  else if (action == m_newBackgroundTabAction) {
    button = m_newBackgroundTabButton;
  }
  if (button) {
    button->setDown(! button->isChecked());
    button->setChecked(!button->isChecked());
  }
}
