#include "graphicsentry.h"
EntryItem::EntryItem(const QString & text, QGraphicsItem * parent) : QGraphicsTextItem(text,parent) {
}
EntryItem::EntryItem(QGraphicsItem * parent) :QGraphicsTextItem(parent) {

}
void EntryItem::contextMenuEvent(QGraphicsSceneContextMenuEvent * event ) {
  //  QGraphicsTextItem::contextMenuEvent(event);

  QMenu menu("Fuck off you bastards");
  menu.setObjectName("entry");
  QAction *markAction = menu.addAction("Bookmark");
  QAction *searchAction = menu.addAction("Find");
  connect(searchAction,SIGNAL(triggered()),this,SLOT(searchItem()));

  if (this->textCursor().hasSelection()) {
    QAction *copyAction = menu.addAction("Copy");
    connect(copyAction,SIGNAL(triggered()),this,SLOT(copy()));
  }
  QAction * perseusAction = menu.addAction("Show Perseus XML");
  Place p = this->getPlace();
  QAction *selectedAction = menu.exec(event->screenPos());
  if (selectedAction == perseusAction) {
    emit(showPerseus(p));
  }
}
void EntryItem::searchItem() {
  qDebug() << Q_FUNC_INFO;
}
void EntryItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event) {
  //  QLOG_DEBUG() << "hover" << this->getNode() << this->isRoot();
}
void EntryItem::copy() {
  QString txt = this->textCursor().selectedText();
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(txt);
}
void EntryItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *o, QWidget *w) {
  QPen pen = painter->pen();
  QBrush brush = painter->brush();
  painter->setPen(Qt::NoPen);
  if ( ! m_place.isSupplement()) {
    painter->setBrush(Qt::white);
  }
  else {
    painter->setBrush(m_backgroundColor);
  }
  painter->drawRect(boundingRect());
  painter->setPen(pen);
  QGraphicsTextItem::paint(painter, o, w);
}
QTextCursor EntryItem::highlight(const QString & target) {
  int pos;
  QTextCursor cursor;
  QTextCursor firstPos;
  QTextDocument * doc = this->document();
  QTextCharFormat fmt;
  /// TODO get from QSettings
  fmt.setBackground(Qt::yellow);
  cursor = doc->find(target,QTextDocument::FindWholeWords);
  firstPos = cursor;
  while(! cursor.isNull()) {
    pos =  cursor.position();
    //    qDebug() << Q_FUNC_INFO << "found at" << pos;
    cursor.setPosition(pos - target.size(), QTextCursor::MoveAnchor);
    cursor.setPosition(pos, QTextCursor::KeepAnchor);
    cursor.setCharFormat(fmt);
    cursor = doc->find(target,pos,QTextDocument::FindWholeWords);
  }
  return firstPos;
}
/**
 * this is the one that is used by default
 *
 * @param target
 *
 * @return
 */
QTextCursor EntryItem::highlightRx(const QString & target) {
  int pos;
  QTextCursor cursor;
  QTextCursor firstPos;
  QTextDocument * doc = this->document();
  QTextCharFormat fmt;
  QRegExp rx;
  /// TODO this needs to be fixed to allow for e.g. full stop
  /// and then made the default
  rx.setPattern("\\b" + target + "\\b");
  /// TODO get from QSettings
  fmt.setBackground(Qt::yellow);
  cursor = doc->find(rx,QTextDocument::FindWholeWords);
  firstPos = cursor;
  while(! cursor.isNull()) {
    pos =  cursor.position();
    cursor.setPosition(pos - target.size(), QTextCursor::MoveAnchor);
    cursor.setPosition(pos, QTextCursor::KeepAnchor);
    cursor.setCharFormat(fmt);
    cursor = doc->find(target,pos,QTextDocument::FindWholeWords);
  }
  return firstPos;
}
void EntryItem::setWord(const QString & word) {
  m_place.setWord(word);
}
void EntryItem::setRoot(const QString & root,bool isroot) {
  m_place.setRoot(root);
  m_place.setIsRoot(isroot);
}
void EntryItem::setNode(const QString & node) {
  m_place.setNode(node);
}
void EntryItem::setSupplement(int supplement) {
  m_place.setSupplement(supplement);
}
void EntryItem::setPage(const int page) {
  m_place.setPage(page);
}
Place EntryItem::getPlace() {
  return m_place;
}
/**
 *
 *
 * @param scene
 * @param parent
 */
LaneGraphicsView::LaneGraphicsView(QGraphicsScene * scene,GraphicsEntry * parent) :
  QGraphicsView(scene,parent) {
  //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}
void LaneGraphicsView::scrollContentsBy(int dx,int dy) {
  QGraphicsView::scrollContentsBy(dx,dy);
  /*
  qDebug() << Q_FUNC_INFO << dx << dy;
  QScrollBar * b = this->verticalScrollBar();
  if (b->value() == b->maximum()) {
    qDebug() << "nextPage emit";
    emit(nextPage());
  }
  else if (b->value() == b->minimum()) {
    qDebug() << "backPage emit";
    emit(backPage());
  }
  else {

  }
  */
}
void LaneGraphicsView::keyPressEvent(QKeyEvent * event) {
  //  QLOG_DEBUG() << "got key";
  QGraphicsView::keyPressEvent(event);
}
GraphicsEntry::GraphicsEntry(QWidget * parent ) : QWidget(parent) {
  readSettings();
  QVBoxLayout * layout = new QVBoxLayout;
  m_textOption.setTextDirection(Qt::LeftToRight);
  //  m_compiledXsl = 0;
  /// 0 = paging forward, items are appended
  /// 1 = paging backward, items are prepended
  m_pagingDir = 0;
  m_scale = 1.0;

  QHBoxLayout * btnslayout = new QHBoxLayout;
  m_zoomIn = new QPushButton(QIcon("./images/zoom-in-32.png"),tr(""));
  m_zoomIn->setToolTip(tr("Zoom in"));
  m_zoomOut = new QPushButton(QIcon("./images/zoom-out-32.png"),tr(""));
  m_zoomOut->setToolTip(tr("Zoom out"));
  m_clearSceneBtn = new QPushButton(QIcon("./images/32/cleaning-erase-eraser-icone-4970-32.png"),tr("Clear"));
  m_clearSceneBtn->setToolTip("Clear contents");
  btnslayout->addWidget(m_zoomIn);
  btnslayout->addWidget(m_zoomOut);
  btnslayout->addWidget(m_clearSceneBtn);
  layout->addLayout(btnslayout);

  connect(m_zoomIn,SIGNAL(clicked()),this,SLOT(onZoomIn()));
  connect(m_zoomOut,SIGNAL(clicked()),this,SLOT(onZoomOut()));
  connect(m_clearSceneBtn,SIGNAL(clicked()),this,SLOT(onClearScene()));


  m_scene = new QGraphicsScene(this);
  m_view = new LaneGraphicsView(m_scene,this);
  m_view->setSceneRect(m_scene->sceneRect());

  //  m_view->setBackgroundBrush(QBrush(Qt::red, Qt::SolidPattern));
  connect(m_view,SIGNAL(nextPage()),this,SLOT(nextPageRequested()));
  connect(m_view,SIGNAL(backPage()),this,SLOT(prevPageRequested()));


  connect(this,SIGNAL(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)),
          this,SLOT(nodeChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)));

  m_view->setInteractive(true);
  m_item = new QGraphicsTextItem("");
  m_item->setTextInteractionFlags(Qt::TextBrowserInteraction);
  m_item->setTextWidth(m_textWidth);
  m_scene->addItem(m_item);

  // add the graphics viwe
  layout->addWidget(m_view,1);

  //  m_showPlace = new PlaceWidget(this);
  //  layout->addWidget(m_showPlace,0);
  m_nodeQuery = 0;
  setLayout(layout);

  connect(m_item,SIGNAL(linkActivated(const QString &)),this,SLOT(linkActivated(const QString &)));
  connect(m_item,SIGNAL(linkHovered(const QString &)),this,SLOT(linkHovered(const QString &)));

  connect(m_scene,SIGNAL(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)),
          this,SIGNAL(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)));

  //  m_xalan = getXalan();
  initXslt();
  prepareQueries();
}
GraphicsEntry::~GraphicsEntry() {
  qDebug() << Q_FUNC_INFO;
  delete m_nodeQuery;
  delete m_rootQuery;

  delete m_pageQuery;
  /// TODO xalan cleanup ?
}
void GraphicsEntry::readSettings() {
  QSettings settings;
  settings.beginGroup("Entry");
  QString css = settings.value("CSS",QString("entry.css")).toString();
  readCssFromFile(css);
  m_xsltSource = settings.value("XSLT",QString("entry.xslt")).toString();
  m_textWidth = settings.value("Text Width",300).toInt();
  m_entryMargin = settings.value("Margin",10).toInt();

  QString bg = settings.value("Supplement Background Color",QString("rgb(255,254,253)")).toString();
  QRegExp rx("^rgb\\((\\d+),\\s*(\\d+),\\s*(\\d+)\\s*\\)");
  if (rx.indexIn(bg) != -1) {
    //    qDebug() << bg << "color" << rx.cap(1) << rx.cap(2) << rx.cap(3);
    m_supplementBg = QColor::fromRgb(rx.cap(1).toInt(),rx.cap(2).toInt(),rx.cap(3).toInt());
    if (! m_supplementBg.isValid())  {
      m_supplementBg = QColor::fromRgb(255,255,255);
    }
  }
  else {
    m_supplementBg = QColor::fromRgb(255,255,255);
  }
  m_clearScene = settings.value("Clear",true).toBool();
  settings.endGroup();

  settings.beginGroup("Debug");
  m_dumpXML = settings.value("Dump XML",false).toBool();
  m_dumpHTML = settings.value("Dump HTML",false).toBool();
  m_dumpOutputHTML = settings.value("Dump Output HTML",false).toBool();
  settings.endGroup();
}
void GraphicsEntry::writeDefaultSettings() {
  QSettings settings;
  settings.beginGroup("Entry");
  settings.setValue("CSS","entry.css");
  settings.setValue("XSLT","entry.xslt");
  settings.setValue("Text Width",300);
  settings.setValue("Clear",true);
  settings.setValue("Margin",0);
  settings.setValue("Supplement Background Color","rgb(255,254,253)");
}
void GraphicsEntry::keyPressEvent(QKeyEvent * event) {
  switch(event->key()) {
  case Qt::Key_Plus: {
    onZoomIn();
    break;
  }
  case Qt::Key_Minus: {
    onZoomOut();
    break;
  }
  default:
    QWidget::keyPressEvent(event);
  }

}
void GraphicsEntry::focusInEvent(QFocusEvent * event) {
  QLOG_DEBUG() << "Got input focus";
  m_view->setFocus();
  QWidget::focusInEvent(event);

}
void GraphicsEntry::onClearScene() {
  for(int i=0;i < m_items.size();i++) {
    m_scene->removeItem(m_items[i]);
  }
  while(m_items.size() > 0) {
    delete m_items.takeFirst();
  }
}
void GraphicsEntry::anchorClicked(const QUrl & link) {
  QLOG_DEBUG() << link.toDisplayString();
  QLOG_DEBUG() << QApplication::keyboardModifiers();
}
void GraphicsEntry::linkActivated(const QString & link) {
  /// turn history on as the user has clicked on something
  getHistory()->on();
  QString node(link);
  /// remove the leading #
  node.remove(0,1);
  Place p;
  p.setNode(node);
  showPlace(p);
}
void GraphicsEntry::linkHovered(const QString & link) {
  QGraphicsTextItem * gi = static_cast<QGraphicsTextItem *>(QObject::sender());

  if (link.isEmpty()) {
    gi->setCursor(QCursor(Qt::ArrowCursor));
  }
  else {
    gi->setCursor(QCursor(Qt::PointingHandCursor));
    QLOG_DEBUG() << "Link hovered" << link;
    QLOG_DEBUG() << QApplication::keyboardModifiers();
  }
}
/**
 * redundant
 *
 */
void GraphicsEntry::anchorTest() {
  QString node;
  //  QList<QGraphicsItem *> items = m_scene->items();
  for(int i=0;i < m_items.size();i++) {
    EntryItem * item = m_items[i];
    if (item) {
      if (item->getNode() == node) {
        m_scene->setFocusItem(item);
        m_view->ensureVisible(item);
        return;
      }
    }
  }
}
bool GraphicsEntry::showPlace(const Place & p,bool thisPageOnly) {
  /// check if the node is on this page
  QString node = p.getNode();
  for(int i=0;i < m_items.size();i++) {
    EntryItem * item = m_items[i];
    if (item->getNode() == node) {
      QLOG_DEBUG() << "Found local node" << node;
      m_scene->setFocusItem(item);
      //m_view->ensureVisible(item);
       m_view->centerOn(item);
      //     m_scene->clearFocus();
      return true;
    }
  }
  QLOG_DEBUG() << "Not found on page" << node;
  /// trying out of page jump
  /// TODO this does not return Place needs fixing
  if (! thisPageOnly ) {
    QLOG_DEBUG() << "Out of page jump" << node;
    getXmlForNode(node);
  }
  return false;
}
/**
 *
 *
 */
bool GraphicsEntry::readCssFromFile(const QString & name) {
  QFile f(name);
  if (! f.open(QIODevice::ReadOnly)) {
    QLOG_WARN()  << "Cannot open file for reading: "
                 << f.errorString();
    return false;

  }
  QTextStream in(&f);
  QString css;
  while( ! in.atEnd()) {
    if (! css.startsWith("-")) {
      css += in.readLine();
    }
  }
  f.close();
  if (! css.isEmpty()) {
    m_currentCSS= css;
    emit(cssChanged());
  }
  return true;
}



bool GraphicsEntry::prepareQueries() {
  bool ok;
  m_pageQuery = new QSqlQuery;
  ok = m_pageQuery->prepare("select root,broot,word,bword,xml,page,itype,nodeId,supplement from entry where page = ? order by nodenum asc");
  if (! ok ) {
    QLOG_WARN() << "page SQL prepare failed" << m_pageQuery->lastError();
  }
  m_nodeQuery = new QSqlQuery;
  ok = m_nodeQuery->prepare("select * from entry where nodeId = ?");
  if (! ok ) {
    QLOG_WARN() << "node SQL prepare failed" << m_nodeQuery->lastError();
  }
  m_rootQuery = new QSqlQuery;
  ok = m_rootQuery->prepare("select root,broot,word,bword,xml,page,itype,nodeId,supplement from entry where root = ? order by nodenum");
  if (! ok ) {
    QLOG_WARN() << "root SQL prepare failed" << m_rootQuery->lastError();
  }
  return ok;
}
Place GraphicsEntry::getXmlForPlace(const Place & p) {
  Place np = getXmlForRoot(p);
  if (np.isValid()) {
    if (np.getRoot() != m_place.getRoot()) {
      emit(placeChanged(np));
    }
    m_place = np;
    //    m_showPlace->setPlace(m_place);
  }
  return np;
}
/**
 * get the root,supplement for the given node and show it
 * nodeId are unique
 *
 * @param node
 *
 */
Place GraphicsEntry::getXmlForNode(const QString  & node,bool nodeOnly) {
  Place p;
  QLOG_DEBUG() << "Search for node" << node << nodeOnly;
  m_nodeQuery->bindValue(0,node);
  m_nodeQuery->exec();

  if (m_nodeQuery->first()) {
    //QString xml = m_nodeQuery->value("xml").toString();
    //    QLOG_DEBUG() << "got " << xml;
    QString root = m_nodeQuery->value("root").toString();
    int supplement = m_nodeQuery->value("supplement").toInt();
    p = Place(root,supplement);
    p.setNode(node);
    p.setWord(m_nodeQuery->value("word").toString());
    p.setNodeOnly(nodeOnly);
    Place np = getXmlForPlace(p);
    /// focus on the node, set thisPageOnly to true just in case something has gone horribly wrong
    /// otherwise we'll be looping forever
    showPlace(np,true);
  }
  else {
    QLOG_DEBUG() << "Error" << m_nodeQuery->lastError().text();
  }
  return p;
}
/**
 *
 *
 * @param root
 * @param node the id of the entry we want to focus on
 */
Place GraphicsEntry::getXmlForRoot(const Place & dp) {
  QList<EntryItem *> items;
  QString arRoot;
  Place p;
  int supplement;
  QString str;
  EntryItem * centerItem;

  QString root = dp.getRoot();
  int supp = dp.getSupplement();
  QString node = dp.getNode();
  bool nodeOnly = dp.getNodeOnly();

  QLOG_DEBUG() << Q_FUNC_INFO << root << supp << node << "nodeOnly = " << nodeOnly;
  m_rootQuery->bindValue(0,root);
  m_rootQuery->exec();
  if (! m_rootQuery->first()) {
    QLOG_INFO() << "root not found" << root;
    return p;
  }
  if (m_clearScene) {
    onClearScene();
  }
  /// this will be set to the right word if a node has been supplied
  QString showWord;
  QString startNode = node;

  /// add the root item unless nodeOnly is set
  str = QString("<word type=\"root\" ar=\"%1\"></word>").arg(root);
  EntryItem * rootItem  = createEntry(str);
  /// will be null if the XSLT/XML has not parsed correctly
  if (rootItem == NULL) {
    return p;
  }
  rootItem->setRoot(root,true);
  rootItem->setSupplement(m_rootQuery->value(8).toInt());
  rootItem->setPage(m_rootQuery->value(5).toInt());

  if (! nodeOnly ) {
    items << rootItem;
  }
  /// by default we will center on the root item
  centerItem = rootItem;
  /// now add all the entries for the root
  bool ok;
  do  {
    ok = true;
    supplement = m_rootQuery->value(8).toInt();
    /**
     * if we've asked to see the entry for the supplement then just show that
     *
     */
    if ((supp == 1) && (supplement == 0)) {
      ok = false;
    }
    if (ok) {
      arRoot = m_rootQuery->value(0).toString();
      QString t  = QString("<word buck=\"%1\" ar=\"%2\" page=\"%3\" itype=\"%4\" supp=\"%5\">")
        .arg(m_rootQuery->value(3).toString())
        .arg(m_rootQuery->value(2).toString())
        .arg(m_rootQuery->value(5).toString())
        .arg(m_rootQuery->value(6).toString())
        .arg(m_rootQuery->value(8).toInt());
      t += m_rootQuery->value(4).toString();
      t += "</word>";
      if (m_dumpXML) {
        QFileInfo fi(QDir::tempPath(),QString("/tmp/%1.xml").arg(m_rootQuery->value(7).toString()));
        QFile f(fi.filePath());
        if (f.open(QIODevice::WriteOnly)) {
          QTextStream out(&f);
          out.setCodec("UTF-8");
          out << t;
        }
      }
      EntryItem * item  = createEntry(t);
      if (item != NULL) {
        if (m_dumpOutputHTML) {
          QFileInfo fi(QDir::tempPath(),QString("/tmp/%1-out.html").arg(m_rootQuery->value(7).toString()));
          QFile f(fi.filePath());
          if (f.open(QIODevice::WriteOnly)) {
            QTextStream out(&f);
            out.setCodec("UTF-8");
            out << item->getOutputHTML();
          }
        }
        item->setSupplement(supplement);
        item->setNode(m_rootQuery->value(7).toString());
        item->setRoot(arRoot);
        item->setWord(m_rootQuery->value(2).toString());
        item->setPage(m_rootQuery->value(5).toInt());
        if (startNode.isEmpty()) {
          startNode = m_rootQuery->value(7).toString();
        }
        else if ( startNode == item->getNode()) {
          showWord = item->getWord();
          /// if a node has been passed, then center on the node
          centerItem = item;
          if ( nodeOnly ) {
            items << item;
          }
        }
        if (! nodeOnly ) {
          items << item;
        }
      }
    }
  } while(m_rootQuery->next());

  /// TODO we've only got a root item
  if (items.size() == 1) {
  }
  /**
   * we have all the new items to show.
   * If we are going forward, add them to the end
   *  "  "  "   "    backwards, prepend them in reverse order
   *
   */

  if (m_pagingDir == 1) {
    /// where the old items begin will be the number of items we're adding
    int x = items.size();
    while(items.size() > 0) {
      EntryItem * item = items.takeLast();
      m_items.prepend(item);
    }
    /// added the entries to the scene and recalculate positions
    /// parameter tells it where the new entries stop
    prependEntries(x);
  }
  else {
   /// get the position of the last item in the scene
    int itemCount = m_items.size();
    while(items.size() > 0) {
      EntryItem * item = items.takeFirst();
      m_items.append(item);
    }
    appendEntries(itemCount);
  }
  ///
  /// all items have been added and positions calculated
  ///
  m_view->setFocus();
  m_transform = m_view->transform();

  //QLOG_DEBUG() << "Scene rect" << m_scene->sceneRect();
  /// without thus centerOn() does not work properly for
  /// items added to the scene
  /// TODO check this
  m_view->setSceneRect(m_scene->sceneRect());
  if (m_currentRoot != arRoot) {
    emit rootChanged(arRoot,node);
    m_currentRoot = arRoot;
  }
  /**
   * we need to know whether we got here by accessing the history button
   * or not
   *
   */
  if (getHistory()->isOn()) {
    HistoryEvent * event = new HistoryEvent;
    event->setRoot(arRoot);
    event->setNode(node);
    event->setWord(showWord);
    getHistory()->add(event);
  }
  /// TODO there has to be a better way than this
  /// TODO if node specified we need to center on it
  if ( node.isEmpty()) {
    /*
    QTextCursor cursor(centerItem->document());
    QTextBlock b = cursor.block();
    QTextLayout * layout = b.layout();
    QTextLine line = layout->lineForTextPosition(cursor.position());
    if (line.isValid()) {
      qDebug() << Q_FUNC_INFO << "text line pos" << line.position() << line.textLength();
        m_view->centerOn(line.position());
    }
    */

      m_scene->setFocusItem(centerItem);
      int h =  m_view->height();
      QPointF p = centerItem->pos();
      p.setY(p.y() + h/2 - 10);
      m_view->centerOn(p);
  }
  if (nodeOnly) {
    delete rootItem;
  }
  m_place = centerItem->getPlace();
  return m_place;
}
Place GraphicsEntry::getPage(const int page) {
  QList<EntryItem *> items;
  EntryItem * item;
  QString arRoot;
  QString str;
  EntryItem * focusItem;
  Place p;

  p.setPage(page);


  m_pageQuery->bindValue(0,page);
  m_pageQuery->exec();
  if (! m_pageQuery->first()) {
    QLOG_INFO() << "Page not found" << page;
    return p;
  }
  if (m_clearScene) {
    onClearScene();
  }
  QString lastRoot;
  /*
  int supplement = m_pageQuery->value(8).toInt();
  /// add the root item unless nodeOnly is set
  str = QString("<word type=\"root\" ar=\"%1\" />").arg(root);
  item  = createEntry(str);
  rootItem->setRoot(root,true);
  rootItem->setSupplement(supplement);
  p.setRoot(root);
  p.setSupplement(supplement);
  if (! nodeOnly ) {
    items << rootItem;
  }
  /// by default we will center on the root item
  centerItem = rootItem;
  /// now add all the entries for the root
  */
  int rootCount = 0;
  int entryCount = 0;
  do   {
    int supplement = m_pageQuery->value(8).toInt();
    QString root = m_pageQuery->value(0).toString();
    /// if root has changed add root XML
    if (root != lastRoot) {
      if (! lastRoot.isEmpty()) {
        QString  str = QString("<word type=\"root\" ar=\"%1\" ></word>").arg(root);
        item = createEntry(str);
        if (item != NULL) {
          items  << item;
          rootCount++;
        }
      }
      lastRoot = root;
    }
    QString t  = QString("<word buck=\"%1\" ar=\"%2\" page=\"%3\" itype=\"%4\" supp=\"%5\">")
      .arg(m_pageQuery->value(3).toString())
      .arg(m_pageQuery->value(2).toString())
      .arg(m_pageQuery->value(5).toInt())
      .arg(m_pageQuery->value(6).toString())
      .arg(m_pageQuery->value(8).toInt());
    t += m_pageQuery->value(4).toString();
    t += "</word>";
    if (m_dumpXML) {
      QFileInfo fi(QDir::tempPath(),QString("/tmp/%1.xml").arg(m_pageQuery->value(7).toString()));
      QFile f(fi.filePath());
      if (f.open(QIODevice::WriteOnly)) {
        QTextStream out(&f);
        out.setCodec("UTF-8");
        out << t;
      }
    }
    item  = createEntry(t);
    if (item != NULL) {
      if (m_dumpOutputHTML) {
        QFileInfo fi(QDir::tempPath(),QString("/tmp/%1-out.html").arg(m_pageQuery->value(7).toString()));
        QFile f(fi.filePath());
        if (f.open(QIODevice::WriteOnly)) {
          QTextStream out(&f);
          out.setCodec("UTF-8");
          out << item->getOutputHTML();
        }
      }
      item->setSupplement(supplement);
      item->setNode(m_pageQuery->value(7).toString());
      item->setRoot(root);
      item->setWord(m_pageQuery->value(2).toString());
      item->setPage(m_pageQuery->value(5).toInt());
      items << item;

      entryCount++;
    }
    /*
    if (startNode.isEmpty()) {
      startNode = m_pageQuery->value(7).toString();
    }
    else if ( startNode == item->getNode()) {
      showWord = item->getWord();
      /// if a node has been passed, then center on the node
      centerItem = item;
      p.setNode(startNode);
      if ( nodeOnly ) {
        items << item;
      }
    }
    */

  } while(m_pageQuery->next());
  if (items.size() == 0) {
    QLOG_INFO() << "No entries found for page" << page;
    return p;
  }
  QLOG_DEBUG() << "Page" << page << "Roots" << rootCount << "Entries" << entryCount;
  /**
   * we have all the new items to show.
   * If we are going forward, add them to the end
   *  "  "  "   "    backwards, prepend them in reverse order
   *
   */

  if (m_pagingDir == 1) {
    /// where the old items begin will be the number of items we're adding
    int x = items.size();
    while(items.size() > 0) {
      EntryItem * item = items.takeLast();
      m_items.prepend(item);
    }
    /// added the entries to the scene and recalculate positions
    /// parameter tells it where the new entries stop
    prependEntries(x);
    focusItem = m_items[x];
  }
  else {
   /// get the position of the last item in the scene
    int itemCount = m_items.size();
    while(items.size() > 0) {
      EntryItem * item = items.takeFirst();
      m_items.append(item);
    }
    appendEntries(itemCount);
    focusItem = m_items[itemCount];
  }
  ///
  /// all items have been added and positions calculated
  ///
  m_view->setFocus();
  m_transform = m_view->transform();

  //QLOG_DEBUG() << "Scene rect" << m_scene->sceneRect();
  /// without thus centerOn() does not work properly for
  /// items added to the scene
  /// TODO check this
  m_view->setSceneRect(m_scene->sceneRect());

  if (m_currentRoot != lastRoot) {
    emit rootChanged(lastRoot,QString());
    m_currentRoot = lastRoot;
  }
  /**
   * we need to know whether we got here by accessing the history button
   * or not
   *
   */
  /// TODO fix this
  if (getHistory()->isOn()) {
    HistoryEvent * event = new HistoryEvent;
    event->setRoot(lastRoot);
    //    event->setNode(node);
    //    event->setWord(showWord);
    getHistory()->add(event);
  }
  /// TODO there has to be a better way than this
  /// TODO if node specified we need to center on it

    /*
    QTextCursor cursor(centerItem->document());
    QTextBlock b = cursor.block();
    QTextLayout * layout = b.layout();
    QTextLine line = layout->lineForTextPosition(cursor.position());
    if (line.isValid()) {
      qDebug() << Q_FUNC_INFO << "text line pos" << line.position() << line.textLength();
        m_view->centerOn(line.position());
    }
    */

  m_scene->setFocusItem(focusItem);
  int h =  m_view->height();
  QPointF pt = focusItem->pos();
  pt.setY(pt.y() + h/2 - 10);
  m_view->centerOn(pt);


  m_place = focusItem->getPlace();
  return m_place;
}
/**
 * create the QTextGraphicsItem by transforming the passed xml
 *
 * @param xml
 *
 * @return
 */
EntryItem * GraphicsEntry::createEntry(const QString & xml) {
    QString html =transform(m_xsltSource,xml);
    if (html.isEmpty()) {
      return NULL;
    }
    EntryItem * gi = new EntryItem("");
    gi->document()->setDefaultStyleSheet(m_currentCSS);
    gi->setTextWidth(m_textWidth);
    gi->setHtml(html);
    /// need this otherwise arabic text will be right justified
    gi->document()->setDefaultTextOption(m_textOption);
    gi->setTextInteractionFlags(Qt::TextBrowserInteraction);
    gi->setAcceptHoverEvents(true);
    gi->setBackground(m_supplementBg);
    if (m_dumpOutputHTML) {
      gi->setOutputHTML(html);
    }
    connect(gi,SIGNAL(linkActivated(const QString &)),this,SLOT(linkActivated(const QString &)));
    connect(gi,SIGNAL(linkHovered(const QString &)),this,SLOT(linkHovered(const QString &)));
    connect(gi,SIGNAL(showPerseus(const Place &)),this,SLOT(showPerseus(const Place &)));
    return gi;
}
/**
 * add the graphics items in m_items to the scene starting with the item
 * at startPos calculating the position as it goes
 *
 * @param startPos
 */
void GraphicsEntry::appendEntries(int startPos) {
  qreal ypos = 0;
  qreal xpos = 0;
  QRectF r;
  QSizeF sz;
  /// calculate the y-position of the last item currently in the scene
  if (startPos > 0) {
    QPointF p = m_items[startPos - 1]->pos();
    r = m_items[startPos - 1]->boundingRect();
    ypos =  p.y() + r.height() + m_entryMargin;
  }
  /// add items updating the ypos as we go
  for(int i=startPos;i < m_items.size();i++) {
    m_items[i]->setPos(xpos,ypos);
    m_scene->addItem(m_items[i]);
    r = m_items[i]->boundingRect();
    if (m_dumpHTML) {
      QFileInfo fi(QDir::tempPath(),QString("/tmp/%1.html").arg(m_items[i]->getNode()));
      QFile f(fi.filePath());
      if (f.open(QIODevice::WriteOnly)) {
        QTextStream out(&f);
        out.setCodec("UTF-8");
        out << m_items[i]->toHtml();
      }
    }
    sz = m_items[i]->document()->size();
    //    QGraphicsRectItem * ri = m_scene->addRect(r);
    //    ri->setPos(xpos,ypos);
    //    ri->setPen(QPen(Qt::NoPen));
    //    ri->setBrush(Qt::cyan);
    ypos += sz.height() + m_entryMargin;
  }
}
/**
 * add the graphics items in m_items to the scene starting with the item
 * at the beginning and stopping at startPos
 *
 * recalculates the position of all the items
 *
 * @param startPos the starting position of items that have already been added
 */
void GraphicsEntry::prependEntries(int startPos) {
  qreal ypos = 0;
  qreal xpos = 0;
  QRectF r;
  QSizeF sz;

  for(int i=0;i < m_items.size();i++) {
    m_items[i]->setPos(xpos,ypos);
    /// check if this is a new item and add it the scene
    if (i < startPos) {
      m_scene->addItem(m_items[i]);
    }
    r = m_items[i]->boundingRect();
    if (m_dumpHTML) {
      QFileInfo fi(QDir::tempPath(),QString("/tmp/%1.html").arg(m_items[i]->getNode()));
      QFile f(fi.filePath());
      if (f.open(QIODevice::WriteOnly)) {
        QTextStream out(&f);
        out.setCodec("UTF-8");
        out << m_items[i]->toHtml();
      }
    }
    sz = m_items[i]->document()->size();
    ypos += sz.height() + m_entryMargin;
  }
}
QString GraphicsEntry::transform(const QString & xsl,const QString & xml) {
  int ok = compileStylesheet(xsl);
  if (ok == 0) {
    QString html = xsltTransform(xml);
    if (! html.isEmpty()) {
      return html;
    }
  }
  /// could be errors in stylesheet or in the xml
  QStringList errors = getParseErrors();
  if (ok != 0) {
    errors.prepend("Errors when processing stylesheet:");
  }
  else {
    errors.prepend("Errors when processing entry:");
  }
  QMessageBox msgBox;
  msgBox.setText(errors.join("\n"));
  msgBox.exec();
  clearParseErrors();
  return QString();
}
void GraphicsEntry::onZoomIn() {
  m_view->setTransform(m_transform);
  m_scale += .1;
  m_view->scale(m_scale,m_scale);
}
void GraphicsEntry::onZoomOut() {
  m_view->setTransform(m_transform);
  m_scale -= .1;
  m_view->scale(m_scale,m_scale);
}
/**
 * search for the given root in the items list
 *
 * @param root         arabic root to search for
 * @param setFocus     center on the root if found
 * @return index in list of item or -1
 */
int GraphicsEntry::hasRoot(const QString & root,bool setFocus) {
  int max = m_items.size();
  int ix = -1;

  for(int i=0;i < max;i++) {
    if (m_items[i]->isRoot() &&
        (m_items[i]->getRoot() == root)) {
          ix = i;
          i = max;
    }
  }
  qDebug() << Q_FUNC_INFO << root << setFocus << ix;
  if ((ix != -1) && setFocus) {
    m_scene->setFocusItem(m_items[ix]);
    m_view->ensureVisible(m_items[ix]);
    m_currentRoot = root;
    emit(rootChanged(root,QString()));
  }
  return ix;
}
int GraphicsEntry::hasPlace(const Place & p,bool setFocus) {
  int max = m_items.size();
  int ix = -1;
  int supp = p.getSupplement();
  QString root = p.getRoot();
  for(int i=0;i < max;i++) {
    if (m_items[i]->isRoot() &&
        ((m_items[i]->getRoot() == root) &&
         (m_items[i]->getSupplement() == supp))) {
          ix = i;
          i = max;
    }
  }
  qDebug() << Q_FUNC_INFO << root << supp << setFocus << ix;
  if ((ix != -1) && setFocus) {
    m_scene->setFocusItem(m_items[ix]);
    m_view->ensureVisible(m_items[ix]);
    m_place = p;
    //    m_showPlace->setPlace(m_place);

    m_currentRoot = root;
    emit(rootChanged(root,QString()));
  }
  return ix;
}
QString GraphicsEntry::lastRoot() {
    /// find the last root item
    int ix = m_items.size() - 1;
    QString root;
    while((ix >= 0) && root.isEmpty()) {
      if (m_items[ix]->isRoot()) {
        root = m_items[ix]->getRoot();
      }
      ix--;
    }
    if (root.isEmpty()) {
      QLOG_DEBUG() << "Cannot find root on current page";
    }
    else {
      qDebug() << "emit nextRoo" << root;
      emit nextRoot(root);
    }
    return root;
}
/**
 * find the first root in the current scene
 *
 *
 * @return the text of the root
 */
QString GraphicsEntry::firstRoot() {
  /// find the first root item (should always be the first
  /// element in m_items
  qDebug() << Q_FUNC_INFO;
  int ix = 0;
  int max = m_items.size();
  QString root;
    while((ix < max) && root.isEmpty()) {
      if (m_items[ix]->isRoot()) {
        root = m_items[ix]->getRoot();
      }
      ix++;
    }
    if (root.isEmpty()) {
      QLOG_DEBUG() << "Cannot find root on current page";
    }
    else {
      qDebug() << "emit prevRoot" << root;
      emit prevRoot(root);
    }
    return root;
}
/**
 * find the last root on the page
 * which will emit(nextRoot(<root>))
 */
void GraphicsEntry::nextPageRequested() {
  this->lastRoot();
}

void GraphicsEntry::prevPageRequested() {
  this->firstRoot();
}
void GraphicsEntry::highlight(const QString & target) {
  int ix = -1;
  QTextCursor cursor;
  for(int i=0;i < m_items.size();i++ ) {
    EntryItem * item = m_items[i];
    QTextCursor c = item->highlightRx(target);
    /// get the cursor for the first match
    if (! c.isNull() && (ix == -1)) {
      cursor = c;
      ix = i;
    }
  }
  qDebug() << Q_FUNC_INFO << "item" << ix << "pos" << cursor.position() << cursor.hasSelection();
  /// unselect the text otherwise it will be in the select color
  /// center the view on the word
  if (ix != -1) {
    if ( ! cursor.isNull()) {
      cursor.clearSelection();
      QTextBlock b = cursor.block();
      QTextLayout * layout = b.layout();
      QTextLine line = layout->lineForTextPosition(cursor.position());
      if (line.isValid()) {
        //        qDebug() << "text line pos" << line.position() << line.textLength();
        m_view->centerOn(line.position());
      }
    }
    m_items[ix]->setTextCursor(cursor);
  }
}
void GraphicsEntry::nodeChanged(QGraphicsItem * newFocus, QGraphicsItem * oldFocus, Qt::FocusReason /* reason */) {

  EntryItem * item = dynamic_cast<EntryItem *>(newFocus);
  if (item) {
    QString node = item->getNode();
    QString word = item->getWord();
    QString root = item->getRoot();

    //    m_showPlace->setNode(node);
    //    m_showPlace->setWord(word);
    //    m_showPlace->setRoot(root);
  }
}
void GraphicsEntry::showPerseus(const Place & p) {
  QString node = p.getNode();
  if ( node.isEmpty()) {
    /// TODO
    return;
  }
  m_nodeQuery->bindValue(0,node);
  m_nodeQuery->exec();
  QString xml;
  if (m_nodeQuery->first()) {
    xml = m_nodeQuery->value("xml").toString();
    qDebug() << xml;
  }
  else {
    xml = "No XML for " + node;
  }
  QMessageBox msgBox;
  msgBox.setText(xml);
  msgBox.exec();
}
