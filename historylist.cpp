#include "historylist.h"
#include "place.h"
#include "QsLog.h"
#include "htmldelegate.h"
#include "application.h"
#include "history.h"
#include "externs.h"
#include "definedsettings.h"
#include "columnartablewidget.h"
HistoryWidget::HistoryWidget(HistoryMaster * history,QWidget * parent)
  : QDialog(parent) {
  readSettings();
  setWindowTitle(tr("Current History"));
  setObjectName("historywidget");
  QList<HistoryEvent *> events = history->getHistory();
  m_list = new ColumnarTableWidget(QStringList() << tr("Id") << tr("Root") << tr("Head") << tr("Vol/Page") << tr("Visited"));
  m_list->setObjectName("historylist");
  //  HtmlDelegate * d = new HtmlDelegate(m_list);
  //  d->setStyleSheet(".ar { font-family : Amiri;font-size : 16px}");
  //  m_list->setItemDelegate(d);

  m_list->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  m_list->setKey(ColumnarTableWidget::STATE,SID_HISTORY_LIST_STATE);
  m_list->setDefaultWidth(100);

  //  m_list->setHorizontalHeaderLabels(QStringList() << tr("Id") << tr("Root") << tr("Head") << tr("Vol/Page") << tr("Visited"));
  //  m_list->horizontalHeader()->setStretchLastSection(true);
  //  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  QTableWidgetItem * item;
  HistoryEvent * h;
  int row;
  for(int i=0;i < events.size();i++) {
    h = events[i];
    Place p = h->getPlace();
    if (p.isValid()) {
    row = m_list->rowCount();
    m_list->insertRow(row);

    item = new QTableWidgetItem(QString("%1").arg(h->getId()));
    m_list->setItem(row,0,item);

    item = new QTableWidgetItem(p.getRoot());
    item->setFont(m_arFont);
    m_list->setItem(row,1,item);
    item = new QTableWidgetItem(p.getWord());
    item->setFont(m_arFont);
    m_list->setItem(row,2,item);
    item = new QTableWidgetItem(QString(tr("V%1/%2")).arg(p.getVol()).arg(p.getPage()));
    m_list->setItem(row,3,item);
    item = new QTableWidgetItem(h->getWhen().toString());
    m_list->setItem(row,4,item);
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
  //  m_list->setMinimumWidth(m_list->sizeHintForColumn(0));
  layout->addWidget(m_list);
  layout->addLayout(hlayout);
  setLayout(layout);
  m_list->installEventFilter(this);
}
HistoryWidget::~HistoryWidget() {
  writeSettings();
}
void HistoryWidget::setPlace() {
  int row = m_list->currentRow();
  QTableWidgetItem * item = m_list->item(row,0);
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
