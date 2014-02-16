#include "bookmarkwidget.h"
BookmarkWidget::BookmarkWidget(const QMap<QString,Place> & marks,QWidget * parent)
  : QDialog(parent) {
  setWindowTitle(tr("Current Bookmarks"));
  m_list = new QListWidget;
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
  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(m_list);
  layout->addWidget(buttonBox);
  setLayout(layout);
}
void BookmarkWidget::setPlace() {
  qDebug() << "accept slot";
  QListWidgetItem * item = m_list->currentItem();
  if (item) {
    QString t = item->text();
    QRegExp rx("(\\w+)\\s+");
    if (rx.indexIn(t,0) != -1) {
      m_mark = rx.cap(1);
      qDebug() << "got" << m_mark;
    }
    else {
      qDebug() << "rx failed";
    }
  }
  this->accept();
}
