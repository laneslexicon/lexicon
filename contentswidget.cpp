#include "contentswidget.h"

ContentsWidget::ContentsWidget(QWidget * parent) : QTreeWidget(parent) {
  setColumnCount(1);
  setHeaderLabels(
                  QStringList() << tr("Letter/Root"));
  header()->setSectionResizeMode(0,QHeaderView::Stretch);
}
void ContentsWidget::loadContents() {
  QSqlQuery query;
  query.prepare("select distinct letter from root order by letter");
  query.exec();

  QSqlQuery rootQuery;
  rootQuery.prepare("select word from root where letter = ? order by word ");

  QFont f;
  f.fromString("Amiri,12,-1,5,50,0,0,0,0,0");
  while(query.next()) {
    QString letter = query.value(0).toString();
    QTreeWidgetItem * item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(letter));
    item->setFont(0,f);
    rootQuery.bindValue(0,letter);
    rootQuery.exec();
    if (! rootQuery.first()) {
      QLOG_DEBUG() << rootQuery.lastError().text();
    }
    while(rootQuery.next()) {
      QString root = rootQuery.value(0).toString();
      QTreeWidgetItem * rootitem = new QTreeWidgetItem(item,QStringList(root));
      rootitem->setFont(0,f);
    }
    addTopLevelItem(item);
  }
}
void ContentsWidget::keyPressEvent(QKeyEvent * event) {
  switch (event->key()) {
  case Qt::Key_Space: {
    QTreeWidgetItem * item = this->currentItem();
    if (item) {
      /// it is a top level item so expand it
      if (item->childCount() > 0) {
        if (item->isExpanded()) {
          collapseItem(item);
        }
        else {
          expandItem(item);
        }
      }
      else {
        emit(itemActivated(item,0));
      }
    }
    break;
  }
  default:
    QTreeWidget::keyPressEvent(event);
  }
}
