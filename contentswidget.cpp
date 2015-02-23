#include "contentswidget.h"
#include "QsLog.h"
#include "place.h"
#include "application.h"
#include "definedsettings.h"
#include <QDrag>
#include <QMimeData>
#include <QPixmap>
#include "externs.h"
#define ROOT_COLUMN 0
#define WORD_COLUMN 1
#define HEADWORD_COLUMN 2
#define HEAD_SUPPLEMENT_COLUMN 3
#define NODE_COLUMN 4
ContentsWidget::ContentsWidget(QWidget * parent) : QTreeWidget(parent) {
  setObjectName("treeroots");
  readSettings();
  setColumnCount(5);
  //  setDragEnabled(true);
  setHeaderLabels(
                  QStringList() << tr("Letter/Root") << tr("Entry") << tr("Head") << tr("") << tr("Node"));
  setSelectionMode(QAbstractItemView::SingleSelection);
  header()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
  header()->setSectionResizeMode(HEAD_SUPPLEMENT_COLUMN,QHeaderView::ResizeToContents);
  this->setStyleSheet(QString("selection-background-color : %1").arg(m_backgroundColor));

  connect(this,SIGNAL(itemExpanded(QTreeWidgetItem *)),this,SLOT(nodeExpanded(QTreeWidgetItem *)));
  connect(this,SIGNAL(itemCollapsed(QTreeWidgetItem *)),this,SLOT(nodeCollapsed(QTreeWidgetItem *)));
  this->setExpandsOnDoubleClick(false);

  m_itypesText << "I" << "II" << "III" << "IV" << "V" << "VI" << "VII" << "VIII" << "IX" << "X" << "XI" << "XII" << "XIII" << "XIV";
  if (! m_debug)
    this->hideColumn(NODE_COLUMN);

  if (! m_showEntryWord && ! m_showHeadWord ) {
    m_showEntryWord = true;
  }
  if (! m_showEntryWord ) {
    this->hideColumn(WORD_COLUMN);
  }
  if (! m_showHeadWord ) {
    this->hideColumn(HEADWORD_COLUMN);
  }
  if (! m_showSupplement) {
    this->hideColumn(HEAD_SUPPLEMENT_COLUMN);
  }

}
ContentsWidget::~ContentsWidget() {

}
/**
 * The default font is through the stylesheet entry for QTreeWidget#treeRoots
 * but we load it from QSettings anyway. The stylesheet entry will override it
 * if both are set.
 * For non-Arabic columns (itype,node) set via QSettings
 *
 */
void ContentsWidget::readSettings() {
  SETTINGS

  settings.beginGroup("Roots");
  m_backgroundColor = settings.value(SID_CONTENTS_BACKGROUND,"lightgray").toString();
  /// the itype font just controls the numbers
  QString fontString = settings.value(SID_CONTENTS_STANDARD_FONT,QString()).toString();
  if ( ! fontString.isEmpty()) {
    m_itypeFont.fromString(fontString);
  }
  fontString = settings.value(SID_CONTENTS_ARABIC_FONT,QString()).toString();
  if ( ! fontString.isEmpty()) {
    QFont f;
    f.fromString(fontString);
    setFont(f);
  }
  /// this is not really on option since it's not in any of the ini files
  ///
  m_showSupplement = settings.value(SID_CONTENTS_SHOWSUPPLEMENT,true).toBool();
  /// entry is Lane's (multi-)word head,
  /// head is the single word version
  m_showHeadWord = settings.value(SID_CONTENTS_SHOWHEAD,false).toBool();
  m_showEntryWord = settings.value(SID_CONTENTS_SHOWENTRY,false).toBool();
  m_debug = settings.value(SID_CONTENTS_DEBUG,false).toBool();
  m_moveDown = settings.value(SID_CONTENTS_MOVE_DOWN,"s").toString();
  m_moveUp = settings.value(SID_CONTENTS_MOVE_UP,"w").toString();
  m_expand = settings.value(SID_CONTENTS_EXPAND," ").toString();
  m_romanItypes = settings.value(SID_CONTENTS_ROMAN_ITYPES,false).toBool();
  settings.endGroup();

  settings.beginGroup("Icons");
  QString dragicon = settings.value("Insert link","insert-link.png").toString();
  m_dragIconFileName = getLexicon()->getResourceFilePath(Lexicon::Image,dragicon);
}
void ContentsWidget::loadContents() {
  QSqlQuery letterQuery;


  if (! m_entryQuery.prepare("select word,itype,bword,nodeid,supplement,headword from entry where datasource = 1 and root = ? order by nodenum asc")) {
    QLOG_WARN() << QString(tr("Entry SQL prepare failed:%1")).arg(m_entryQuery.lastError().text());
  }

  QString sql = "select distinct letter from root where datasource = 1 order by letter";
  if ( ! letterQuery.prepare(sql)) {
    QLOG_WARN() << QString(tr("Error preparing letter query SQL:%1")).arg(letterQuery.lastError().text());
    return;
  }
  letterQuery.exec();

  QSqlQuery rootQuery;
  sql = "select word,supplement from root where letter = ? and datasource = 1 order by word,supplement";
  if (! rootQuery.prepare(sql)) {
    QLOG_WARN() << QString(tr("Error preparing root query SQL:%1")).arg(rootQuery.lastError().text());
    return;
  }
  while(letterQuery.next()) {
    QString letter = letterQuery.value(0).toString();
    QTreeWidgetItem * item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(letter));
    item->setData(0,Qt::UserRole,"letter");
    rootQuery.bindValue(0,letter);
    rootQuery.exec();
    QString supp;
    QString root;
    while(rootQuery.next()) {
      bool ok = true;
      // we can have root entries that are in the main text
      // and the supplement but we only show one entry
      // merging the children
      if (root != rootQuery.value(0).toString()) {
        //      itype = rootQuery.value(2).toString();
        root = rootQuery.value(0).toString();
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
        if (root == letter) {
          ok = false;
        }
        if (ok) {
          QStringList cols;
          cols << root << "" << "" << supp;
          QTreeWidgetItem * rootItem = new QTreeWidgetItem(cols);
          rootItem->setData(ROOT_COLUMN,Qt::UserRole,"root");
          item->addChild(rootItem);
        }
      }
      addTopLevelItem(item);
    }
  }
  resizeColumnToContents(HEAD_SUPPLEMENT_COLUMN);
}
Place ContentsWidget::findNextPlace(const Place & p) {
  Place np;
  QString nextRoot = this->findNextRoot(p.getRoot());
  np.setRoot(nextRoot);
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
  QList<QTreeWidgetItem *> items = this->findItems(root,Qt::MatchRecursive,ROOT_COLUMN);
  if (items.size() == 0) {
    return QString();
  }
  QTreeWidgetItem * currentItem = items[0];
  QString itemType = currentItem->data(0,Qt::UserRole).toString();
  if (itemType == "letter") {
    if (currentItem->childCount() > 0) {
      return currentItem->child(0)->text(0);
    }
    return QString();
  }
  if (itemType == "root") {
    QTreeWidgetItem * letter = currentItem->parent();
    int ix = letter->indexOfChild(currentItem);
    ix++;
    if (ix < letter->childCount()) {
      return letter->child(ix)->text(0);
    }
    ix = indexOfTopLevelItem(letter);
    ix++;
    if (ix < topLevelItemCount()) {
      return topLevelItem(ix)->text(0);
    }
    else {
      emit(atEnd());
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
  QList<QTreeWidgetItem *> items = this->findItems(root,Qt::MatchRecursive,ROOT_COLUMN);
  if (items.size() == 0) {
    return QString();
  }
  QTreeWidgetItem * currentItem = items[0];
  QString itemType = currentItem->data(0,Qt::UserRole).toString();
  if (itemType == "letter") {
    int ix = indexOfTopLevelItem(currentItem);
    ix--;
    if (ix < 0) {
      emit(atStart());
      return QString();
    }
    currentItem = topLevelItem(ix);
    if (currentItem->childCount() > 0) {
      currentItem = currentItem->child(currentItem->childCount() - 1);
      return currentItem->text(0);
    }
    else {
      return QString();
    }
  }
  if (itemType == "root") {
    QTreeWidgetItem * letter = currentItem->parent();
    int ix = letter->indexOfChild(currentItem);
    ix--;
    if (ix >= 0) {
      return letter->child(ix)->text(0);
    }
    ix = indexOfTopLevelItem(letter);
    if (ix >= 0) {
      return topLevelItem(ix)->text(0);
    }
    else {
      emit(atStart());
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
  Place np;
  QString nextRoot = this->findPrevRoot(p.getRoot());
  np.setRoot(nextRoot);
  return np;
}
void ContentsWidget::keyPressEvent(QKeyEvent * event) {
  if (event->text() == m_moveUp) {
    QTreeWidgetItem * item = this->itemAbove(this->currentItem());
    if (item) {
      this->setCurrentItem(item);
      return;
    }
  }
  if (event->text() == m_moveDown) {
    QTreeWidgetItem * item = this->itemBelow(this->currentItem());
    if (item) {
      this->setCurrentItem(item);
      return;
    }
  }
  if (event->text() == m_expand) {
    this->toggleExpand();
    return;
  }
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
    toggleExpand();
    break;
  }
  default:
    QTreeWidget::keyPressEvent(event);
  }
}
void ContentsWidget::toggleExpand() {
    QTreeWidgetItem * item = this->currentItem();
    if (item) {
      /// if it is an entry item
      if ((item->parent() != 0)  && (item->parent()->parent() != 0)) {
        /// TODO make this configurable
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
          QString root = item->text(ROOT_COLUMN);
          this->addEntries(root,item);
          expandItem(item);
        }
      }
    }
}
/**
 * (1) find the root and add its headwords if this
 * has not already been done.
 * (2) expand the root
 * (3) if we have a word/node, find it
 *
 * @param p              Place we want to find
 * @param select
 */
void ContentsWidget::ensurePlaceVisible(const Place & p, bool select) {
  QLOG_DEBUG() << Q_FUNC_INFO << p << select;
  QString root = p.getRoot();
  int supplement = p.getSupplement();
  QTreeWidgetItem * item;
  QString suppTest;
  if (supplement == 1) {
    suppTest = "*";
  }
  Place rootPlace;
  rootPlace.setRoot(p.getRoot());
  item = this->findPlace(rootPlace);
  if (! item ) {
    QLOG_DEBUG() << Q_FUNC_INFO << "Cannot find root" << p.getRoot();
    return;
  }
  /**
   * this is called when restoring tabs so we need to load
   * the head words for each root we are restoring
   */
  if (addEntries(p.getRoot(),item) == 0) {
    return;
  }
  item->setExpanded(true);
  QModelIndex index = indexFromItem(item);
  if (index.isValid()) {
    scrollTo(index,QAbstractItemView::PositionAtCenter);
  }
  if (select)
    setCurrentItem(item);
  item = this->findPlace(p);
  if (item) {
     index = indexFromItem(item);
     if (index.isValid()) {
       scrollTo(index,QAbstractItemView::PositionAtCenter);
       setCurrentItem(item);
     }
  }
}
QTreeWidgetItem * ContentsWidget::findPlace(const Place & p) const {
  QModelIndex ix;
  QTreeWidgetItem * item = 0;
  QString target;
  int column;


  if (p.isRoot()) {
    column = ROOT_COLUMN;
    target = p.getRoot();
  }
  else {
    column = NODE_COLUMN;
    target = p.getNode();
  }

  QList<QTreeWidgetItem *> items = this->findItems(target,Qt::MatchRecursive,column);
  QLOG_DEBUG() << Q_FUNC_INFO << target << items.size();
  /// TODO multiple items, for supplement ?
  if (items.size() > 0) {
    item = items[0];
  }
  return item;
}
void ContentsWidget::focusInEvent(QFocusEvent * event) {
  /// clearing the style sheets has the effect that the systems default
  /// will be used
  this->setStyleSheet("");
  //  QTreeWidgetItem * item = this->currentItem();
  QTreeWidget::focusInEvent(event);

}
void ContentsWidget::focusOutEvent(QFocusEvent * event) {
  this->setStyleSheet(QString("selection-background-color : %1").arg(m_backgroundColor));
  QTreeWidget::focusOutEvent(event);
}
/**
 * This loads the head words for the given root
 * For head words there are two columns, the itype and word
 *
 * @param root
 * @param parent
 */
int ContentsWidget::addEntries(const QString & root,QTreeWidgetItem * parent) {
  int c = 0;
  QString supplement;
  QString itype;
  QString word;
  QString node;
  QString head;
  QLOG_DEBUG() << Q_FUNC_INFO << root;
  /// if already expanding return
  if (parent->childCount() > 0) {
    return parent->childCount();
  }
  m_entryQuery.bindValue(0,root);
  m_entryQuery.exec();
  bool isSupplementRoot = false;
  if (parent->text(HEAD_SUPPLEMENT_COLUMN) == "*") {
    isSupplementRoot = true;
  }
  while(m_entryQuery.next()) {
    //QLOG_DEBUG() << m_entryQuery->value("bword").toString() << m_entryQuery->value("nodeid").toString();
    supplement.clear();
    if (! isSupplementRoot && (m_entryQuery.value("supplement").toInt() == 1)) {
      supplement = "*";
    }
    itype = itypeText(m_entryQuery.value("itype").toString());

    word = m_entryQuery.value("word").toString();
    node = m_entryQuery.value("nodeid").toString();
    head = m_entryQuery.value("headword").toString();
    QTreeWidgetItem * item = new QTreeWidgetItem(QStringList() << itype << word << head << supplement << node);
    item->setFont(0,m_itypeFont);
    item->setFont(NODE_COLUMN,m_itypeFont);
    item->setFont(HEAD_SUPPLEMENT_COLUMN,m_itypeFont);
    item->setData(0,Qt::UserRole,m_entryQuery.value("nodeid"));//.toString()
    item->setFlags(item->flags() | Qt::ItemIsDragEnabled);
    parent->addChild(item);
    c++;
  }
  if ((c > 0) && ! parent->isExpanded()) {
    parent->setExpanded(true);
  }
  return c;
}
QString ContentsWidget::itypeText(const QString & itype) {
  bool ok;

  if (! m_romanItypes) {
    return itype;
  }
  int n = itype.toInt(&ok);
  if (ok  && (n >= 0) && (n < m_itypesText.size())) {
    return m_itypesText[n];
  }
  else {
    return itype;
  }
}
void ContentsWidget::mousePressEvent(QMouseEvent * event) {
  if (event->button() == Qt::LeftButton) {
    m_startPos = event->pos();
  }
  QTreeWidget::mousePressEvent(event);
}
void ContentsWidget::mouseMoveEvent(QMouseEvent * event) {
  if (event->buttons() & Qt::LeftButton) {
    int distance = (event->pos() - m_startPos).manhattanLength();
    if (distance >= QApplication::startDragDistance()) {
      QTreeWidgetItem * item = currentItem();
      if (item && (item->columnCount() == 5)) {
        QMimeData * mimeData = new QMimeData;
        QString t = QString("%1:%2")
          .arg(item->text(NODE_COLUMN))
          .arg(item->text(WORD_COLUMN));
        mimeData->setText(t);
        QDrag * drag = new QDrag(this);

        drag->setPixmap(QPixmap(m_dragIconFileName));
        drag->setMimeData(mimeData);
        if (drag->exec(Qt::LinkAction) == Qt::LinkAction) {
          QLOG_DEBUG() << "Linked ok";
        }
      }
    }
  }
  QTreeWidget::mouseMoveEvent(event);
}
void ContentsWidget::nodeExpanded(QTreeWidgetItem * /*item */) {
  QLOG_DEBUG() << Q_FUNC_INFO;
}
void ContentsWidget::nodeCollapsed(QTreeWidgetItem * /*item */) {
  QLOG_DEBUG() << Q_FUNC_INFO;
}
Place ContentsWidget::getCurrentPlace() {
  Place p;
  QTreeWidgetItem * item = this->currentItem();
  if (!item) {
    return p;
  }
  for(int i=0;i < item->columnCount();i++) {
    QLOG_DEBUG() << QString("[%1][%21]").arg(i).arg(item->text(i));
  }
  /// at at letter
  if (item->parent() == 0) {
    return p;
  }
  /// at a root
  if (item->childCount() > 0) {
    p.setRoot(item->text(ROOT_COLUMN));
    if (item->text(HEAD_SUPPLEMENT_COLUMN) == "*") {
      p.setSupplement(1);
    }
    else {
      p.setSupplement(0);
    }
    return p;
  }
  /// at node
  p.setNode(item->text(NODE_COLUMN));
  if ((item->text(HEAD_SUPPLEMENT_COLUMN) == "*") ||
      (item->parent()->text(HEAD_SUPPLEMENT_COLUMN) == "*")) {
    p.setSupplement(1);
  }
  else {
    p.setSupplement(0);
  }
  return p;
}
void ContentsWidget::contextMenuEvent(QContextMenuEvent * event) {
  QTreeWidgetItem * item = this->itemAt(event->pos());
  if (!item) {
    return;
  }
  if ((item->parent() == 0) ||                     // at letter
      (item->childCount() == 0)) {                 // at node
    return;
  }
  QString root = item->text(ROOT_COLUMN);
  QLOG_DEBUG() << "at root" << root;

}
