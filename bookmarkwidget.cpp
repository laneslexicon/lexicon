#include "bookmarkwidget.h"
#include "place.h"
#include "QsLog.h"
BookmarkWidget::BookmarkWidget(const QMap<QString,Place> & marks,QWidget * parent)
  : QDialog(parent) {
  setWindowTitle(tr("Current Bookmarks"));
  m_list = new QListWidget;
  m_list->setObjectName("bookmarklist");
  QStringList keys = marks.keys();
  keys.removeOne("-here-");
  for(int i=0;i < keys.size();i++) {
      Place p = marks.value(keys[i]);
      QString t = QString("%1    %2").arg(keys[i],p.getText());
      QListWidgetItem * item = new QListWidgetItem(t,m_list);
  }
  QDialogButtonBox *   buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(setPlace()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(m_list,SIGNAL(itemDoubleClicked(QListWidgetItem *)),this,SLOT(jump(QListWidgetItem *)));
  connect(m_list,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(jump(QListWidgetItem *)));
  connect(m_list,SIGNAL(itemPressed(QListWidgetItem *)),this,SLOT(jump(QListWidgetItem *)));
  QVBoxLayout * layout = new QVBoxLayout;
  m_list->setMinimumWidth(m_list->sizeHintForColumn(0));
  layout->addWidget(m_list);
  layout->addWidget(buttonBox);
  setLayout(layout);
  m_list->installEventFilter(this);
}
void BookmarkWidget::setPlace() {
  QListWidgetItem * item = m_list->currentItem();
  if (item) {
    QString t = item->text();
    QRegExp rx("(\\w+)\\s+");
    if (rx.indexIn(t,0) != -1) {
      m_mark = rx.cap(1);
      //      QLOG_DEBUG() << "got" << m_mark;
    }
    else {
      QLOG_DEBUG() << Q_FUNC_INFO <<  "regex failed" << t;
    }
  }
  this->accept();
}
void BookmarkWidget::jump(QListWidgetItem * item) {
  qDebug() << Q_FUNC_INFO;
  this->setPlace();
}
bool BookmarkWidget::eventFilter(QObject * target ,QEvent * event) {
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
    switch(keyEvent->key()) {
    case Qt::Key_Return:
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
