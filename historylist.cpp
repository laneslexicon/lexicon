#include "historylist.h"
#include "place.h"
#include "QsLog.h"
#include "htmldelegate.h"
#include "application.h"
#include "history.h"
HistoryWidget::HistoryWidget(HistoryMaster * history,QWidget * parent)
  : QDialog(parent) {
  readSettings();
  setWindowTitle(tr("Current History"));
  QList<HistoryEvent *> events = history->getHistory();
  m_list = new QTableWidget(events.size(),4);
  m_list->setObjectName("historylist");
  //  HtmlDelegate * d = new HtmlDelegate(m_list);
  //  d->setStyleSheet(".ar { font-family : Amiri;font-size : 16px}");
  //  m_list->setItemDelegate(d);

  m_list->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);

  m_list->setHorizontalHeaderLabels(QStringList() << tr("Id") << tr("Root") << tr("Entry") << tr("Vol/Page"));
  m_list->horizontalHeader()->setStretchLastSection(true);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  QTableWidgetItem * item;
  HistoryEvent * h;
  for(int i=0;i < events.size();i++) {
    h = events[i];
    item = new QTableWidgetItem(QString("%1").arg(h->getId()));
    m_list->setItem(i,0,item);
    Place p = h->getPlace();
      //      QString html = qobject_cast<Lexicon *>(qApp)->scanAndSpan(p.getText());
    item = new QTableWidgetItem(p.getRoot());//p.getText());
    item->setFont(m_arFont);
    m_list->setItem(i,1,item);
    item = new QTableWidgetItem(p.getWord());//p.getText());
    item->setFont(m_arFont);
    m_list->setItem(i,2,item);
    item = new QTableWidgetItem(QString(tr("V%1/%2")).arg(p.getVol()).arg(p.getPage()));
    m_list->setItem(i,3,item);
  }
  if (events.size() > 0) {
    m_list->selectRow(0);
  }
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
  connect(m_list,SIGNAL(itemClicked(QTableWidgetItem *)),this,SLOT(jump(QTableWidgetItem *)));
  connect(m_list,SIGNAL(itemPressed(QTableWidgetItem *)),this,SLOT(jump(QTableWidgetItem *)));
  QVBoxLayout * layout = new QVBoxLayout;
  //  m_list->setMinimumWidth(m_list->sizeHintForColumn(0));
  layout->addWidget(m_list);
  layout->addWidget(buttonBox);
  setLayout(layout);
  m_list->installEventFilter(this);
}
void HistoryWidget::setPlace() {
  int row = m_list->currentRow();
  QLOG_DEBUG() << Q_FUNC_INFO << row;
  QTableWidgetItem * item = m_list->item(row,0);
  if (item) {
    QString t = item->text();
    m_mark = item->text();//rx.cap(1);
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
  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());
  settings->beginGroup("History");
  settings->beginGroup("List");
  QString fontString = settings->value("Arabic font").toString();
  if ( ! fontString.isEmpty()) {
    m_arFont.fromString(fontString);
  }
  settings->endGroup();
  settings->endGroup();
}
