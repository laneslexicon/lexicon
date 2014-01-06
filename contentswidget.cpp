#include "contentswidget.h"

ContentsWidget::ContentsWidget(QWidget * parent) : QTreeWidget(parent) {
  m_debug = true;
  setColumnCount(1);
  setHeaderLabels(
                  QStringList() << tr("Contents"));
  header()->setSectionResizeMode(0,QHeaderView::Stretch);
}
void ContentsWidget::loadContents() {
  QSqlQuery query;
  QFile f;
  QTextStream * out;
  if (m_debug) {
    QFileInfo fi(QDir::tempPath(),QString("roots.txt"));
    f.setFileName(fi.filePath());
    if (f.open(QIODevice::WriteOnly)) {
      out = new QTextStream(&f);
      out->setCodec("UTF-8");
    }
  }
  query.prepare("select distinct letter from root order by letter");
  query.exec();

  QSqlQuery rootQuery;
  rootQuery.prepare("select word from root where letter = ? order by word ");

  while(query.next()) {
    QString letter = query.value(0).toString();
    QTreeWidgetItem * item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(letter));
    rootQuery.bindValue(0,letter);
    rootQuery.exec();
    //    if (! rootQuery.first()) {
    //      QLOG_DEBUG() << rootQuery.lastError().text();
    //    }
    while(rootQuery.next()) {
      QString root = rootQuery.value(0).toString();
      if (m_debug && f.isOpen()) {
        *out << root << '\n';
      }
      QTreeWidgetItem * rootitem = new QTreeWidgetItem(item,QStringList(root));
    }
    addTopLevelItem(item);
  }
  if (m_debug) {
    out->flush();
    f.close();
    delete out;
  }
}
QString ContentsWidget::findNextRoot(const QString & root) {
  QString nr;
  QTreeWidgetItem * nextItem;
  int tc = topLevelItemCount();
  int topIndex = -1;
  int childIndex = -1;
  bool found = false;

  for(int i = 0;(i < tc) && ! found;i++) {
    QTreeWidgetItem * topItem = topLevelItem(i);
    int kidCount = topItem->childCount();
    for(int j=0;(j < kidCount) && ! found ;j++) {
      QTreeWidgetItem * child = topItem->child(j);
      if (child->text(0) == root) {
        topIndex = i;
        if (j == (kidCount - 1)) {
          topIndex++;
          childIndex = 0;
        }
        else {
          childIndex = j + 1;
        }
        found = true;
        qDebug() << "found at" << i << j;
      }
    }
  }
  if (topIndex == tc) {
    qDebug() << "At the end";
    return QString();
  }
  /// overkill, but would only matter if there were letters without any roots
  for(int i = topIndex;i < tc; i++) {
      QTreeWidgetItem * item = topLevelItem(i);
      int kidCount = item->childCount();
      if (childIndex < kidCount) {
        QTreeWidgetItem * nr = item->child(childIndex);
        return nr->text(0);
      }
    }
  return QString();
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
