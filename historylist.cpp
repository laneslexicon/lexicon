#include "historylist.h"
#include "place.h"
#include "QsLog.h"
#include "htmldelegate.h"
#include "application.h"
#include "history.h"
#include "externs.h"
#include "definedsettings.h"
#include "columnartablewidget.h"


#define ROOT_COLUMN 0
#define WORD_COLUMN 1
#define VOL_COLUMN 2
#define WHEN_COLUMN 3
#define NODE_COLUMN 4
#define KEY_COLUMN 5
HistoryWidget::HistoryWidget(HistoryMaster * history,QWidget * parent)
  : QDialog(parent) {
  readSettings();
  setWindowTitle(tr("Current History"));
  setObjectName("historywidget");
  QList<HistoryEvent *> events = history->getHistory(true);
  m_list = new ColumnarTableWidget(QStringList()  << tr("Root") << tr("Head") << tr("Vol/Page") << tr("Visited") << tr("Node") << tr("Id"));
  m_list->setObjectName("historylist");

  m_list->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  m_list->setKey(ColumnarTableWidget::STATE,SID_HISTORY_LIST_STATE);
  m_list->setDefaultWidth(100);
  m_list->setFixedRowHeight(40);
  //  m_list->verticalHeader()->setVisible(true);

  QTableWidgetItem * item;
  HistoryEvent * h;
  int row;
  QLabel * label;
  for(int i=0;i < events.size();i++) {
    h = events[i];
    Place p = h->getPlace();
    if (p.isValid()) {
    row = m_list->rowCount();
    m_list->insertRow(row);

    item = new QTableWidgetItem(QString("%1").arg(h->getId()));
    m_list->setItem(row,KEY_COLUMN,item);

    label = m_list->createLabel(p.m_root,"historylist");
    label->setAlignment(Qt::AlignCenter);
    m_list->setCellWidget(row,ROOT_COLUMN,label);


    label = m_list->createLabel(p.m_word,"historylist");
    label->setAlignment(Qt::AlignCenter);
    m_list->setCellWidget(row,WORD_COLUMN,label);

    label = m_list->createLabel(p.format("%V/%P"));
    label->setAlignment(Qt::AlignCenter);
    m_list->setCellWidget(row,VOL_COLUMN,label);

    item = new QTableWidgetItem(h->getWhen().toString());
    m_list->setItem(row,WHEN_COLUMN,item);

    item = new QTableWidgetItem(p.getNode());
    m_list->setItem(row,NODE_COLUMN,item);
    }
  }
  if (m_list->rowCount() > 0) {
    m_list->selectRow(0);
  }
  else {
    m_list->showEmpty(tr("No history items"));
  }
  m_newTab = new QCheckBox(tr("Open in new tab"));
  m_switchTab = new QCheckBox(tr("Switch to new tab"));
  SETTINGS
  settings.beginGroup("History");

  m_list->readConfiguration(settings);

  m_newTab->setChecked(settings.value(SID_HISTORY_NEW_TAB,false).toBool());
  m_switchTab->setChecked(settings.value(SID_HISTORY_GO_TAB,true).toBool());
  m_switchTab->setEnabled(m_newTab->isChecked());
  connect(m_newTab,SIGNAL(stateChanged(int)),this,SLOT(onStateChanged(int)));
  QPushButton * jumpButton = new QPushButton(tr("&Jump to"));
  QPushButton * cancelButton = new QPushButton(tr("&Cancel"));
  if (events.size() == 0) {
    jumpButton->setEnabled(false);
  }
  QDialogButtonBox *   buttonBox = new QDialogButtonBox();//QDialogButtonBox::Cancel);
  buttonBox->addButton(jumpButton,QDialogButtonBox::AcceptRole);
  buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(setPlace()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(m_list,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),this,SLOT(jump(QTableWidgetItem *)));
  //  connect(m_list,SIGNAL(itemClicked(QTableWidgetItem *)),this,SLOT(jump(QTableWidgetItem *)));
  //  connect(m_list,SIGNAL(itemPressed(QTableWidgetItem *)),this,SLOT(jump(QTableWidgetItem *)));
  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addWidget(m_newTab);
  hlayout->addWidget(m_switchTab);
  hlayout->addStretch();
  hlayout->addWidget(buttonBox);
  QVBoxLayout * layout = new QVBoxLayout;

  layout->addWidget(m_list);
  layout->addLayout(hlayout);
  setLayout(layout);
  m_list->installEventFilter(this);
  //  m_list->setFocusPolicy(Qt::StrongFocus);
}
HistoryWidget::~HistoryWidget() {
  writeSettings();
}
/**
 * if the user has pressed space,return or enter, get id the the current
 * and save it to m_mark
 */
void HistoryWidget::setPlace() {
  int row = m_list->currentRow();
  QTableWidgetItem * item = m_list->item(row,KEY_COLUMN);
  if (item) {
    QString t = item->text();
    m_mark = item->text();
  }
  this->accept();
}
void HistoryWidget::jump(QTableWidgetItem * /* item */) {
  //  QLOG_DEBUG() << Q_FUNC_INFO << item->row();
  this->setPlace();
}
bool HistoryWidget::eventFilter(QObject * target ,QEvent * event) {
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
    switch(keyEvent->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space: {
      this->setPlace();
        break;
      }
    default:
      break;
    }
  }
  return QWidget::eventFilter(target,event);
}
QSize HistoryWidget::sizeHint() const {
  return QSize(600,300);
}
void HistoryWidget::readSettings() {
  SETTINGS
  settings.beginGroup("History");
  QString fontString = settings.value(SID_HISTORY_LIST_ARABIC_FONT).toString();
  if ( ! fontString.isEmpty()) {
    m_arFont.fromString(fontString);
  }
  resize(settings.value(SID_HISTORY_VIEWER_SIZE,QSize(650,30)).toSize());
  move(settings.value(SID_HISTORY_VIEWER_POS,QPoint(100,100)).toPoint());
}
void HistoryWidget::writeSettings() {
  SETTINGS
  settings.beginGroup("History");
  settings.setValue(SID_HISTORY_VIEWER_POS,pos());
  settings.setValue(SID_HISTORY_VIEWER_SIZE,size());
  settings.sync();
}

Place HistoryWidget::getSelected() const {
  Place p;
  bool ok;
  int id = m_mark.toInt(&ok);
  if (ok) {
    p = getHistory()->getPlaceById(id);
  }
  return p;
}
bool HistoryWidget::getNewTab() {
  return m_newTab->isChecked();
}
bool HistoryWidget::getSwitchTab() {
  return m_switchTab->isChecked();
}
void HistoryWidget::onStateChanged(int /* state */) {
  m_switchTab->setEnabled(m_newTab->isChecked());
}
