#include "contentswidget.h"

ContentsWidget::ContentsWidget(QWidget * parent) : QTreeWidget(parent) {
  setColumnCount(1);
  setHeaderLabels(
                  QStringList() << tr("Contents"));
  header()->setSectionResizeMode(0,QHeaderView::Stretch);
}
void ContentsWidget::loadContents() {
  QSqlQuery query;
  query.prepare("select distinct letter from root order by letter");
  query.exec();

  QSqlQuery rootQuery;
  rootQuery.prepare("select word from root where letter = ? order by word ");

  while(query.next()) {
    QString letter = query.value(0).toString();
    QTreeWidgetItem * item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(letter));
    rootQuery.bindValue(0,letter);
    rootQuery.exec();
    if (! rootQuery.first()) {
      QLOG_DEBUG() << rootQuery.lastError().text();
    }
    while(rootQuery.next()) {
      QString root = rootQuery.value(0).toString();
      QTreeWidgetItem * rootitem = new QTreeWidgetItem(item,QStringList(root));
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
