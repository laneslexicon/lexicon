#include "contentswidget.h"

ContentsWidget::ContentsWidget(QWidget * parent) : QTreeWidget(parent) {
  readSettings();
  setColumnCount(2);
  setHeaderLabels(
                  QStringList() << tr("Letter/Root") << tr("") );
  setSelectionMode(QAbstractItemView::SingleSelection);
  header()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
  this->setStyleSheet(QString("selection-background-color : %1").arg(m_backgroundColor));

  connect(this,SIGNAL(itemExpanded(QTreeWidgetItem *)),this,SLOT(nodeExpanded(QTreeWidgetItem *)));
  connect(this,SIGNAL(itemCollapsed(QTreeWidgetItem *)),this,SLOT(nodeCollapsed(QTreeWidgetItem *)));
  this->setExpandsOnDoubleClick(false);
}
ContentsWidget::~ContentsWidget() {
  qDebug() << Q_FUNC_INFO;
}
void ContentsWidget::readSettings() {
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  settings->setIniCodec("UTF-8");
  settings->beginGroup("Roots");
  m_backgroundColor = settings->value("Background","lightgray").toString();
  QString fontString = settings->value("Itype font",QString()).toString();
  if ( ! fontString.isEmpty()) {
    m_itypeFont.fromString(fontString);
  }
  m_debug = settings->value("Debug",false).toBool();
  delete settings;
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
  QString sql = "select distinct letter from root order by letter";
  if ( ! query.prepare(sql)) {
    QLOG_FATAL() << "Error preparing SQL:" << sql;
    return;
  }
  query.exec();

  QSqlQuery rootQuery;
  sql = "select word,supplement from root where letter = ? order by word,supplement";
  if (! rootQuery.prepare(sql)) {
    QLOG_FATAL() << "Error preparing SQL:" << sql;
    return;
  }

  while(query.next()) {
    QString letter = query.value(0).toString();
    QTreeWidgetItem * item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(letter));
    rootQuery.bindValue(0,letter);
    rootQuery.exec();
    //    if (! rootQuery.first()) {
    //      QLOG_DEBUG() << rootQuery.lastError().text();
    //    }
    QString supp;
    QString root;
    while(rootQuery.next()) {
      bool ok = true;
      root = rootQuery.value(0).toString();
      //      itype = rootQuery.value(2).toString();

      if (rootQuery.value(1).toInt() == 1) {
        supp = "*";
      }
      else {
        supp = "";
      }
      /// skipping the letter entries from the supplement
      if ((supp == "*") && (root.size() == 1)) {
        ok = false;
      }
      if (ok) {
        if (m_debug && f.isOpen()) {
          *out << root << '\n';
        }
        QStringList cols;
        cols << root << supp;
        QTreeWidgetItem * rootitem = new QTreeWidgetItem(item,cols);
      }
    }
    addTopLevelItem(item);
  }
  if (m_debug) {
    out->flush();
    f.close();
    delete out;
  }
  m_entryQuery = new QSqlQuery;
  bool ok = m_entryQuery->prepare("select word,itype,bword,nodeId from entry where root = ? order by nodenum asc");
  if ( ! ok ) {
    QLOG_WARN() << "Entry SQL prepare failed" << m_entryQuery->lastError();
  }

}
Place ContentsWidget::findNextPlace(const Place & p) {
  QTreeWidgetItem * currentItem;
  int tc = topLevelItemCount();
  int topIndex = -1;
  int childIndex = -1;
  bool found = false;

  QString root = p.getRoot();
  int supp = p.getSupplement();
  QString suppTest;
  Place np;
  if (supp == 1) {
    suppTest = "*";
  }
  qDebug() << Q_FUNC_INFO << root << supp;
  for(int i = 0;(i < tc) && ! found;i++) {
    QTreeWidgetItem * topItem = topLevelItem(i);
    int kidCount = topItem->childCount();
    for(int j=0;(j < kidCount) && ! found ;j++) {
      QTreeWidgetItem * child = topItem->child(j);
      //      qDebug() << child->text(0) << child->text(1);
      if ((child->text(0) == root) && (child->text(1) == suppTest)) {
        currentItem = child;
        topIndex = i;
        if (j == (kidCount - 1)) {
          topIndex++;
          childIndex = 0;
        }
        else {
          childIndex = j + 1;
        }
        found = true;
      }
    }
  }
  if ((topIndex == -1) || (childIndex == -1 )) {
    QLOG_WARN() << "Root not found" << root;
    return np;
  }
  if (topIndex == tc) {
    emit(atEnd());
    return np;
  }
  /// overkill, but would only matter if there were letters without any roots
  for(int i = topIndex;i < tc; i++) {
      QTreeWidgetItem * item = topLevelItem(i);
      int kidCount = item->childCount();
      if (childIndex < kidCount) {
        QTreeWidgetItem * nextItem = item->child(childIndex);
        currentItem->setSelected(false);
        nextItem->setSelected(true);
        if (nextItem->text(1) == "*") {
          supp = 1;
        }
        else {
          supp = 0;
        }
        np.setRoot(nextItem->text(0));
        np.setSupplement(supp);
        return np;
      }
    }
  return np;
}
/**
 * Called by LanesLexicon when it gets a next root signal
 *
 *
 * @param root start root
 *
 * @return the next root in sequence
 */
QString ContentsWidget::findNextRoot(const QString & root) {
  QTreeWidgetItem * currentItem;
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
        currentItem = child;
        topIndex = i;
        if (j == (kidCount - 1)) {
          topIndex++;
          childIndex = 0;
        }
        else {
          childIndex = j + 1;
        }
        found = true;
      }
    }
  }
  if (topIndex == tc) {
    emit(atEnd());
    return QString();
  }
  /// overkill, but would only matter if there were letters without any roots
  for(int i = topIndex;i < tc; i++) {
      QTreeWidgetItem * item = topLevelItem(i);
      int kidCount = item->childCount();
      if (childIndex < kidCount) {
        QTreeWidgetItem * nextItem = item->child(childIndex);
        currentItem->setSelected(false);
        nextItem->setSelected(true);
        return nextItem->text(0);
      }
    }
  return QString();
}
/**
 * Called by LanesLexicon when it gets a next root signal
 *
 *
 * @param root start root
 *
 * @return the next root in sequence
 */
QString ContentsWidget::findPrevRoot(const QString & root) {
  QTreeWidgetItem * currentItem;
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
        currentItem = child;
        /// if first child, we want the last root of the prev letter
        topIndex = i;
        if (j == 0) {
          topIndex--;
          childIndex = -1;
        }
        else {
          childIndex = j - 1;
        }
        found = true;
      }
    }
  }
  if (topIndex == -1) {
    emit(atStart());
    return QString();
  }
  /// overkill, but would only matter if there were letters without any roots
  for(int i = topIndex;i >= 0; i--) {
      QTreeWidgetItem * item = topLevelItem(i);
      int kidCount = item->childCount();
      if (kidCount > childIndex) {
        if (childIndex == -1) {
          childIndex = kidCount - 1;
        }
        QTreeWidgetItem * nextItem = item->child(childIndex);
        currentItem->setSelected(false);
        nextItem->setSelected(true);
        return nextItem->text(0);
      }
    }
  return QString();
}
/**
 * Called by LanesLexicon when it gets a next root signal
 *
 *
 * @param root start root
 *
 * @return the next root in sequence
 */
Place ContentsWidget::findPrevPlace(const Place & p) {
  QTreeWidgetItem * currentItem;
  int tc = topLevelItemCount();
  int topIndex = -1;
  int childIndex = -1;
  bool found = false;
  QString root = p.getRoot();
  int supp = p.getSupplement();
  QString suppTest;
  Place np;
  if (supp == 1) {
    suppTest = "*";
  }

  for(int i = 0;(i < tc) && ! found;i++) {
    QTreeWidgetItem * topItem = topLevelItem(i);
    int kidCount = topItem->childCount();
    for(int j=0;(j < kidCount) && ! found ;j++) {
      QTreeWidgetItem * child = topItem->child(j);
      if ((child->text(0) == root) && (child->text(1) == suppTest)) {
        currentItem = child;
        /// if first child, we want the last root of the prev letter
        topIndex = i;
        if (j == 0) {
          topIndex--;
          childIndex = -1;
        }
        else {
          childIndex = j - 1;
        }
        found = true;
      }
    }
  }
  if (topIndex == -1) {
    emit(atStart());
    return np;
  }
  /// overkill, but would only matter if there were letters without any roots
  for(int i = topIndex;i >= 0; i--) {
      QTreeWidgetItem * item = topLevelItem(i);
      int kidCount = item->childCount();
      if (kidCount > childIndex) {
        if (childIndex == -1) {
          childIndex = kidCount - 1;
        }
        QTreeWidgetItem * nextItem = item->child(childIndex);
        currentItem->setSelected(false);
        nextItem->setSelected(true);
        if (nextItem->text(1) == "*") {
          supp = 1;
        }
        else {
          supp = 0;
        }
        np.setRoot(nextItem->text(0));
        np.setSupplement(supp);
        return np;
      }
    }
  return np;
}
void ContentsWidget::keyPressEvent(QKeyEvent * event) {
  switch (event->key()) {
  case Qt::Key_Return : {
    QTreeWidgetItem * item = this->currentItem();
    /// this catches
    if ((item->parent() != 0) && (item->parent()->parent() != 0)) {
      emit(itemActivated(item,0));
    }
    break;
  }
  case Qt::Key_Space: {
    QTreeWidgetItem * item = this->currentItem();
    if (item) {
      /// if it is an entry item
      if ((item->parent() != 0)  && (item->parent()->parent() != 0)) {
        emit(itemActivated(item,0));
      }
      else {
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
          emit(itemDoubleClicked(item,0));
        }
      }
    }
    break;
  }
  default:
    QTreeWidget::keyPressEvent(event);
  }
}
void ContentsWidget::ensurePlaceVisible(const Place & p, bool select) {
  qDebug() << Q_FUNC_INFO << p.getRoot() << p.getSupplement() << select;
  ensureVisible(p.getRoot(),p.getSupplement(),select);
}
void ContentsWidget::ensureVisible(const QString & root, int supplement,bool select) {
  QTreeWidgetItem * item;
  QTreeWidgetItem * topItem;
  int tc = topLevelItemCount();
  int topIndex = -1;
  int childIndex = -1;
  bool found = false;
  QString suppTest;
  if (supplement == 1) {
    suppTest = "*";
  }
  for(int i = 0;(i < tc) && ! found;i++) {
    topItem = topLevelItem(i);
    int kidCount = topItem->childCount();
    for(int j=0;(j < kidCount) && ! found ;j++) {
      item = topItem->child(j);
      if ((item->text(0) == root) && (item->text(1) == suppTest)) {
        topIndex = i;
        childIndex = j;
        found = true;
      }
    }
  }
  if (!item) {
    return;
  }
  if ((topIndex == -1) || (childIndex == -1)) {
    QLOG_WARN() << "Ensure visible error";
    return;
  }
  if ( ! topItem->isExpanded() )
    topItem->setExpanded(true);

  if (select) {
    qDebug() << Q_FUNC_INFO << "setCurrentItem";
    setCurrentItem(item);
    addEntries(root,item);
  }
  QModelIndex index = indexFromItem(item);
  if (index.isValid()) {
    scrollTo(index);
  }
}
void ContentsWidget::focusInEvent(QFocusEvent * event) {
  /// clearing the style sheets has the effect that the systems default
  /// will be used
  this->setStyleSheet("");
  QTreeWidgetItem * item = this->currentItem();
  QTreeWidget::focusInEvent(event);

}
void ContentsWidget::focusOutEvent(QFocusEvent * event) {
  this->setStyleSheet(QString("selection-background-color : %1").arg(m_backgroundColor));
  QTreeWidget::focusOutEvent(event);
}
void ContentsWidget::addEntries(const QString & root,QTreeWidgetItem * parent) {
  if (parent->childCount() > 0) {
    return;
  }
  m_entryQuery->bindValue(0,root);
  m_entryQuery->exec();
  while(m_entryQuery->next()) {
    //qDebug() << m_entryQuery->value("bword").toString() << m_entryQuery->value("nodeId").toString();
    QTreeWidgetItem * item = new QTreeWidgetItem(QStringList() << m_entryQuery->value("itype").toString() << m_entryQuery->value("word").toString());
    item->setFont(0,m_itypeFont);
    item->setData(0,Qt::UserRole,m_entryQuery->value("nodeId"));//.toString()
    parent->addChild(item);

  }
  if ( ! parent->isExpanded()) {
    qDebug() << "Expanding parent";
    parent->setExpanded(true);
  }
}
void ContentsWidget::nodeExpanded(QTreeWidgetItem * /*item */) {
  qDebug() << Q_FUNC_INFO;
}
void ContentsWidget::nodeCollapsed(QTreeWidgetItem * /*item */) {
  qDebug() << Q_FUNC_INFO;
}
