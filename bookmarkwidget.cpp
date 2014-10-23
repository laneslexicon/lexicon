#include "bookmarkwidget.h"
#include "place.h"
#include "QsLog.h"
#include "htmldelegate.h"
#include "application.h"
BookmarkWidget::BookmarkWidget(const QMap<QString,Place> & marks,QWidget * parent)
  : QDialog(parent) {
  setWindowTitle(tr("Current Bookmarks"));
  QStringList keys = marks.keys();
  keys.removeOne("-here-");
  m_list = new QTableWidget(keys.size(),2);
  m_list->setObjectName("bookmarklist");
  HtmlDelegate * d = new HtmlDelegate(m_list);
  d->setStyleSheet(".ar { font-family : Amiri;font-size : 16px}");
  m_list->setItemDelegate(d);

  m_list->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);

  m_list->setHorizontalHeaderLabels(QStringList() << tr("Id") << tr("Place"));
  m_list->horizontalHeader()->setStretchLastSection(true);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  QTableWidgetItem * item;
  for(int i=0;i < keys.size();i++) {
    item = new QTableWidgetItem(QString("%1").arg(keys[i]));
      m_list->setItem(i,0,item);
      Place p = marks.value(keys[i]);
      QString html = qobject_cast<Lexicon *>(qApp)->scanAndSpan(p.getText());
      item = new QTableWidgetItem(html);//p.getText());
      m_list->setItem(i,1,item);
  }
  if (keys.size() > 0) {
    m_list->selectRow(0);
  }
  QPushButton * jumpButton = new QPushButton(tr("&Jump to"));
  QPushButton * cancelButton = new QPushButton(tr("&Cancel"));
  if (keys.size() == 0) {
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
void BookmarkWidget::setPlace() {
  int row = m_list->currentRow();
  QLOG_DEBUG() << Q_FUNC_INFO << row;
  QTableWidgetItem * item = m_list->item(row,0);
  if (item) {
    QString t = item->text();
    m_mark = item->text();//rx.cap(1);
  }
  this->accept();
}
void BookmarkWidget::jump(QTableWidgetItem * /* item */) {
  //  QLOG_DEBUG() << Q_FUNC_INFO << item->row();
  this->setPlace();
}
bool BookmarkWidget::eventFilter(QObject * target ,QEvent * event) {
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
QSize BookmarkWidget::sizeHint() const {
  return QSize(600,300);
}
