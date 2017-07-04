#include "tablistdialog.h"
#include "place.h"
#include "QsLog.h"
#include "application.h"
#include "definedsettings.h"
#include "externs.h"
#include "columnartablewidget.h"
#include "laneslexicon.h"
#include "graphicsentry.h"
#define TAB_COLUMN 0
#define TITLE_COLUMN 1
#define ROOT_COLUMN 2
#define HEAD_COLUMN 3
#define NODE_COLUMN 4
TabListDialog::TabListDialog(QWidget * parent)
  : QDialog(parent) {
  setWindowTitle(tr("Current Tabs"));
  setObjectName("tablistdialog");


  m_list = new ColumnarTableWidget(QStringList() << tr("Tab") << tr("Title") << tr("Root") << tr("Headword") << tr("Node"));
  m_list->setKey(ColumnarTableWidget::STATE,SID_SYSTEM_TABLIST_STATE);
  m_list->setDefaultWidth(100);
  m_list->setObjectName("tablist");
  m_list->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);

  QDialogButtonBox *   buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  connect(m_list,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(switchTab(int,int)));

  QVBoxLayout * layout = new QVBoxLayout;

  layout->addWidget(m_list);
  layout->addWidget(buttonBox);

  setLayout(layout);
  m_list->installEventFilter(this);
  this->loadTabs();

  /// load the column configuration
  SETTINGS
  settings.beginGroup("System");
  m_list->readConfiguration(settings);
  settings.endGroup();
  ///
  settings.beginGroup("Dialog State");
  QSize sz = settings.value(QString("%1 size").arg(this->metaObject()->className())).toSize();
  QPoint p  =settings.value(QString("%1 position").arg(this->metaObject()->className())).toPoint();
  if (! sz.isNull()) {
    this->resize(sz);
  }
  if (! p.isNull()) {
    this->move(p);
  }

}
void TabListDialog::loadTabs() {
  //  QTableWidgetItem * item;
  QLabel * label;
  int row;
  QTabWidget * tabs = getApp()->tabwidget();
  QString str;
  m_list->clearContents();
  m_list->setRowCount(0);
  m_list->setFixedRowHeight(44);
  for(int i=0;i < tabs->count();i++) {
    row = m_list->rowCount();
    m_list->insertRow(row);
    label = m_list->createLabel(QString("%1").arg(i+1));
    label->setAlignment(Qt::AlignCenter);
    m_list->setCellWidget(row,TAB_COLUMN,label);

    str = tabs->tabText(i);
    str.remove(QRegExp("^\\d+"));
    label = m_list->createLabel(str,"tablist");
    label->setAlignment(Qt::AlignCenter);
    m_list->setCellWidget(row,TITLE_COLUMN,label);

    GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(tabs->widget(i));
    if (entry) {
      Place p = entry->getPlace();
      if (p.isValid()) {
        label = m_list->createLabel(p.m_root,"tablist");
        label->setAlignment(Qt::AlignCenter);
        m_list->setCellWidget(row,ROOT_COLUMN,label);


        label = m_list->createLabel(p.m_head,"tablist");
        label->setAlignment(Qt::AlignCenter);
        m_list->setCellWidget(row,HEAD_COLUMN,label);

        label = m_list->createLabel(p.m_node,"tablist");
        label->setAlignment(Qt::AlignCenter);
        m_list->setCellWidget(row,NODE_COLUMN,label);
      }
    }

    if (m_list->rowCount() > 0) {
      m_list->selectRow(0);
      m_list->resizeRowsToContents();
    }
    else {
      m_list->showEmpty(tr("No tabs"));
    }
  }
}
void TabListDialog::showEvent(QShowEvent * /* event */) {
  this->loadTabs();
}

bool TabListDialog::eventFilter(QObject * target ,QEvent * event) {
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
    switch(keyEvent->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space: {
      this->switchTab(m_list->currentRow(),0);
        break;
      }
    default:
      break;
    }
  }
  return QDialog::eventFilter(target,event);
}
QSize TabListDialog::sizeHint() const {
  return QSize(600,800);
}

TabListDialog::~TabListDialog() {
  QLOG_DEBUG() << Q_FUNC_INFO;

  SETTINGS
  settings.beginGroup("Dialog State");
  settings.setValue(QString("%1 position").arg(this->metaObject()->className()),this->pos());
  settings.setValue(QString("%1 size").arg(this->metaObject()->className()),this->size());
}
void TabListDialog::switchTab(int row,int /* col */) {
  QTabWidget * tabs = getApp()->tabwidget();
  tabs->setCurrentIndex(row);
  this->accept();
}
