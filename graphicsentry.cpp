#include "graphicsentry.h"
#include "laneslexicon.h"

extern LanesLexicon * getApp();
extern NoteMaster * getNotes();
ToolButtonData::ToolButtonData(int id) : QToolButton() {
  m_id = id;
  setObjectName("toolbuttondata");
}
ToolButtonData::~ToolButtonData() {
  qDebug() << Q_FUNC_INFO;
}
/**
 *
 *
 * @param scene
 * @param parent
 */
LaneGraphicsView::LaneGraphicsView(QGraphicsScene * scene,GraphicsEntry * parent) :
  QGraphicsView(scene,parent) {
  setObjectName("lexicongraphicsview");
  setFocusPolicy(Qt::StrongFocus);
  //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  //  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
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
  //  QLOG_DEBUG() << Q_FUNC_INFO << "got key" << event->modifiers() << event->key();
  QGraphicsView::keyPressEvent(event);
}
void LaneGraphicsView::focusInEvent(QFocusEvent * event) {
  //  qDebug() << Q_FUNC_INFO << this->styleSheet();
  //  this->setStyleSheet(" border : 1px red solid");
  QGraphicsView::focusInEvent(event);
}
void LaneGraphicsView::focusOutEvent(QFocusEvent * event) {
  QGraphicsView::focusOutEvent(event);
  //  this->setStyleSheet("");
}
/**
 *
 *
 * @param parent
 */
GraphicsEntry::GraphicsEntry(QWidget * parent ) : QWidget(parent) {
  setObjectName("graphicsentry");
  setFocusPolicy(Qt::StrongFocus);
  readSettings();
  QVBoxLayout * layout = new QVBoxLayout;
  m_textOption.setTextDirection(Qt::LeftToRight);
  m_showNodeOnly = false;
  //  m_compiledXsl = 0;
  /// 0 = paging forward, items are appended
  /// 1 = paging backward, items are prepended
  m_pagingDir = 0;
  m_scale = 1.0;

  m_scene = new QGraphicsScene(this);
  m_view = new LaneGraphicsView(m_scene,this);
  m_view->setSceneRect(m_scene->sceneRect());



  connect(m_view,SIGNAL(nextPage()),this,SLOT(nextPageRequested()));
  connect(m_view,SIGNAL(backPage()),this,SLOT(prevPageRequested()));



  m_view->setInteractive(true);
  //  m_view->setAlignment(Qt::AlignLeft);
  /// TODO what is this for ?
  m_item = new QGraphicsTextItem("");
  m_item->setTextInteractionFlags(Qt::TextBrowserInteraction);
  m_item->setTextWidth(m_textWidth);
  m_scene->addItem(m_item);

  // add the graphics viwe
  layout->addWidget(m_view);

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
  m_transform = m_view->transform();
}
GraphicsEntry::~GraphicsEntry() {
  qDebug() << Q_FUNC_INFO;
  delete m_nodeQuery;
  delete m_rootQuery;
  delete m_pageQuery;
  /// TODO xalan cleanup ?
}
void GraphicsEntry::readSettings() {
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  bool ok;
  settings->setIniCodec("UTF-8");
  settings->beginGroup("Entry");
  QString css = settings->value("CSS",QString("entry.css")).toString();
  readCssFromFile(css);
  m_xsltSource = settings->value("XSLT",QString("entry.xslt")).toString();
  m_textWidth = settings->value("Text width",300).toInt();
  m_defaultWidth = m_textWidth;

  m_entryMargin = settings->value("Margin",10).toInt();

  QString bg = settings->value("Supplement background color",QString("rgb(255,254,253)")).toString();
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
  m_clearScene = settings->value("Clear scene",true).toBool();
  /// these are set to empty to disable the feature
  m_moveFocusUpKey = settings->value("Move focus up",QString()).toString();
  m_moveFocusDownKey = settings->value("Move focus down",QString()).toString();
  m_moveForwardKey = settings->value("Forward",QString()).toString();
  m_moveBackwardKey = settings->value("Back",QString()).toString();
  m_zoomInKey = settings->value("Zoom in",QString()).toString();
  m_zoomOutKey = settings->value("Zoom out",QString()).toString();

  m_widenKey = settings->value("Widen",QString()).toString();
  m_narrowKey = settings->value("Narrow",QString()).toString();
  m_widenStep = settings->value("Step",50).toInt(&ok);
  if ( ! ok ) {
    m_widenStep = 50;
  }
  m_searchKey = settings->value("Find",QString()).toString();
  m_clearKey = settings->value("Clean",QString()).toString();


  settings->endGroup();

  settings->beginGroup("Debug");
  m_dumpXML = settings->value("Dump XML",false).toBool();
  m_dumpHTML = settings->value("Dump HTML",false).toBool();
  m_dumpOutputHTML = settings->value("Dump output HTML",false).toBool();
  settings->endGroup();

  settings->beginGroup("Notes");
  m_notesEnabled = settings->value("Enabled",true).toBool();
  settings->endGroup();
  delete settings;
}
void GraphicsEntry::writeDefaultSettings() {

}
void GraphicsEntry::keyPressEvent(QKeyEvent * event) {
  // qDebug() << Q_FUNC_INFO << event->modifiers() << event->key() << event->text();
  if (event->key() == Qt::Key_Escape) {
    QWidget * w = this->parentWidget();

    while(w) {
      qDebug() << w->metaObject()->className();
      QTabWidget * tabw = qobject_cast<QTabWidget *>(w);
      if (tabw)  {
        qDebug() << "setting focus" << tabw->tabBar()->hasFocus();
        tabw->tabBar()->setFocus();
        return;
      }
      w = w->parentWidget();
    }
  }
  if (! m_zoomInKey.isEmpty() && (event->text() == m_zoomInKey)) {
    onZoomIn();
    return;
  }
  if (! m_zoomOutKey.isEmpty() && (event->text() == m_zoomOutKey)) {
    onZoomOut();
    return;
  }
  if (! m_moveFocusDownKey.isEmpty() && (event->text() ==  m_moveFocusDownKey))  {
    moveFocusDown();
    return;
  }
  if (! m_moveFocusUpKey.isEmpty() && (event->text() ==  m_moveFocusUpKey)) {
    moveFocusUp();
    return;
  }
  if (! m_moveForwardKey.isEmpty() && (event->text() ==  m_moveForwardKey)) {
    moveForward();
    return;
  }
  if (! m_moveBackwardKey.isEmpty() && (event->text() ==  m_moveBackwardKey)) {
    moveBackward();
    return;
  }
  if (! m_widenKey.isEmpty() && (event->text() ==  m_widenKey)) {
    onWiden();
    return;
  }
  if (! m_narrowKey.isEmpty() && (event->text() ==  m_narrowKey)) {
    onNarrow();
    return;
  }
  if (! m_searchKey.isEmpty() && (event->text() == m_searchKey)) {
    emit(searchPage());
    return;
  }
  if (! m_clearKey.isEmpty() && (event->text() == m_clearKey)) {
    emit(clearPage());
    return;
  }
  QWidget::keyPressEvent(event);

}
void GraphicsEntry::moveFocusDown() {
  EntryItem * item = dynamic_cast<EntryItem *>(m_scene->focusItem());
  if (item) {
    int m = m_items.size() - 1;
    for(int i=0;i < m ;i++) {
      if (m_items[i] == item) {
        //        m_view->centerOn(m_items[i+1]);
        //        m_view->ensureVisible(m_items[i+1]);
        //        m_scene->setFocusItem(m_items[i+1]);
        //        qDebug() << "focus moved from" << m_items[i]->getPlace().getText();
        //        qDebug() << "focus moved to  " << m_items[i + 1]->getPlace().getText();
        setCurrentItem(m_items[i+1]);
        return;
      }
    }
  }
}
void GraphicsEntry::moveFocusUp() {
  EntryItem * item = dynamic_cast<EntryItem *>(m_scene->focusItem());
  if (item) {
    int m = m_items.size();
    for(int i=1;i < m ;i++) {
      if (m_items[i] == item) {
        //        m_view->centerOn(m_items[i-1]);
        //        m_view->ensureVisible(m_items[i-1]);
        //        m_scene->setFocusItem(m_items[i-1]);
        //        m_place = m_items[i-1]->getPlace();
        //        qDebug() << "focus moved from" << m_items[i]->getPlace().getText();
        //        qDebug() << "focus moved to  " << m_items[i - 1]->getPlace().getText();
        setCurrentItem(m_items[i-1]);
        return;
      }
    }
  }
}
/**
 * Returns either the Place of the focus item (i.e the node) or, if no item has focus,
 * the Place of page (i.e the root)
 *
 * @return
 */
Place GraphicsEntry::getPlace() const {
  EntryItem * item = dynamic_cast<EntryItem *>(m_scene->focusItem());
  if (item)
    return item->getPlace();

 return m_place;
}
void GraphicsEntry::focusInEvent(QFocusEvent * event) {
  /// giving focus to the graphicsview so keyboard
  //  m_view->setFocus();
  qDebug() << Q_FUNC_INFO;
  this->focusPlace();
  QWidget::focusInEvent(event);

}
void GraphicsEntry::onClearScene() {
  for(int i=0;i < m_items.size();i++) {
    m_scene->removeItem(m_items[i]);
  }
  while(m_items.size() > 0) {
    delete m_items.takeFirst();
  }
  /// need to remove the graphicswidget for the items which have notes
  m_scene->clear();
}
/**
 * redundant
 *
 */

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
  bool newTab = false;
  if (QApplication::keyboardModifiers() & Qt::ShiftModifier) {
    newTab = true;
  }

  showPlace(p,newTab);
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
Place GraphicsEntry::showPlace(const Place & p,bool thisPageOnly) {
  /// check if the node is on this page
  QString node = p.getNode();
  for(int i=0;i < m_items.size();i++) {
    EntryItem * item = m_items[i];
    if (item->getNode() == node) {
      m_scene->setFocusItem(item);
       m_view->centerOn(item);
       /// update the place
       m_place = item->getPlace();
       return m_place;
    }
  }
  /// trying out of page jump
  /// TODO this does not return Place needs fixing
  Place np;
  if (! thisPageOnly ) {
    Place p;
    p.setNode(node);
    np = getXmlForRoot(p);
    /// is this right ?
    if (np != p) {
      emit(placeChanged(np));
      m_place = np;
    }
  }
  return np;
}
/**
 * lines beginning with - are omitted
 *
 */
bool GraphicsEntry::readCssFromFile(const QString & name) {
  QFile f(name);
  if (! f.open(QIODevice::ReadOnly)) {
    QLOG_WARN()  << "Cannot open CSS file for reading: " << name
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
    m_currentCSS = css;
    emit(cssChanged());
  }
  return true;
}



bool GraphicsEntry::prepareQueries() {
  bool ok;
  m_pageQuery = new QSqlQuery;
  ok = m_pageQuery->prepare("select root,broot,word,bword,xml,page,itype,nodeId,supplement from entry where datasource = 1 and page = ? order by nodenum asc");
  if (! ok ) {
    QLOG_WARN() << "page SQL prepare failed" << m_pageQuery->lastError();
  }
  m_nodeQuery = new QSqlQuery;
  ok = m_nodeQuery->prepare("select * from entry where datasource = 1 and nodeId = ?");
  if (! ok ) {
    QLOG_WARN() << "node SQL prepare failed" << m_nodeQuery->lastError();
  }
  m_rootQuery = new QSqlQuery;
  ok = m_rootQuery->prepare("select root,broot,word,bword,xml,page,itype,nodeId,supplement from entry where datasource = 1  and root = ? order by nodenum");
  if (! ok ) {
    QLOG_WARN() << "root SQL prepare failed" << m_rootQuery->lastError();
  }
  return ok;
}
/*
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
*/
/**
 * TODO if this is called as part of search for node it does not return
 *      the Place that matches the node, but the root
 * @param root
 * @param node the id of the entry we want to focus on
 */
Place GraphicsEntry::getXmlForRoot(const Place & dp) {
  QList<EntryItem *> items;
  Place retval;
  int supplement;

  EntryItem * centerItem;

  QString root = dp.getRoot();
  int supp = dp.getSupplement();
  QString node = dp.getNode();
  //  bool nodeOnly = dp.getNodeOnly();

  Place p = dp;
  /**
   * if we asked for the node, look up the root
   *
   */

  if (root.isEmpty() && ! node.isEmpty() ) {
    m_nodeQuery->bindValue(0,node);
    m_nodeQuery->exec();
    if (m_nodeQuery->first()) {
      //QString xml = m_nodeQuery->value("xml").toString();
      //    QLOG_DEBUG() << "got " << xml;
      root = m_nodeQuery->value("root").toString();
      supplement = m_nodeQuery->value("supplement").toInt();
      p.setNode(node);
      p.setRoot(root);
      p.setWord(m_nodeQuery->value("word").toString());
      p.setSupplement(supplement);
    }
    else {
      QLOG_WARN() << "Node not found" << node;
      //      qDebug() << Q_FUNC_INFO << "exiting 3 with place" << retval.toString();
      return retval;
    }
  }

  QLOG_DEBUG() << Q_FUNC_INFO << __LINE__ << p << p.getType();
  m_rootQuery->bindValue(0,root);
  m_rootQuery->exec();
  if (! m_rootQuery->first()) {
    if (m_showNodeOnly) {
      QLOG_WARN() << QString(tr("Root not found for word %1 (node %2)")).arg(p.getWord()).arg(p.getNode());
    }
    else {
      QLOG_WARN() << QString(tr("Root not found %1")).arg(root);
    }
    qDebug() << Q_FUNC_INFO << "exiting 4 with place" << retval.toString();
    return retval;
  }
  /// this will be set to the right word if a node has been supplied

  /// add the root item unless nodeOnly is set
  QString str = QString("<word type=\"root\" ar=\"%1\"></word>").arg(root);
  EntryItem * rootItem  = createEntry(str);
  /// will be null if the XSLT/XML has not parsed correctly
  if (rootItem == NULL) {
    //    qDebug() << Q_FUNC_INFO << "exiting 5 with place";
    return p;
  }
  ///
  /// write a history record because we are leaving this page
  ///
  if (getHistory()->isOn()) {
    //    qDebug() << "History old place" << m_place;
    //    qDebug() << "New place" << dp;
    if (dp.getType() == Place::User) {
      getHistory()->add(m_place);
      /// this allows mainwindow to update the history list
      emit(historyAddition());
    }
  }

  if (m_clearScene) {
    onClearScene();
  }
  rootItem->setRoot(root,true);
  rootItem->setSupplement(m_rootQuery->value(8).toInt());
  rootItem->setPage(m_rootQuery->value(5).toInt());

  items << rootItem;
  if ( m_showNodeOnly ) {
    rootItem->hide();
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
      //      ok = false;
    }
    //
    // "select root,broot,word,bword,xml,page,itype,nodeId,supplement from entry where root = ? order by nodenum");
    //
    if (ok) {
      QString t  = QString("<word buck=\"%1\" ar=\"%2\" page=\"%3\" itype=\"%4\" supp=\"%5\">")
        .arg(m_rootQuery->value(3).toString())
        .arg(m_rootQuery->value(2).toString())
        .arg(m_rootQuery->value(5).toInt())
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
        Place p;
        p.setSupplement(supplement);
        p.setNode(m_rootQuery->value(7).toString());
        p.setRoot(root);
        p.setWord(m_rootQuery->value(2).toString());
        p.setPage(m_rootQuery->value(5).toInt());
        item->setNotes(getApp()->notes()->find(m_rootQuery->value(2).toString()));
        item->setPlace(p);
        items << item;
        /// if we asked for a specific word/node, focus on it
        if (! node.isEmpty() && (item->getNode() == node)) {
          centerItem = item;
        }
        if (m_showNodeOnly) {
            item->hide();
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
  /// TODO there has to be a better way than this
  /// TODO if node specified we need to center on it
  //  if ( node.isEmpty()) {
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
  centerItem->ensureVisible();
      //      int h =  m_view->height();
      //      QPointF pt = centerItem->pos();
      //      pt.setY(pt.y() + h/2 - 10);
      //      m_view->centerOn(pt);
      //  }

  if (dp.getType() == Place::History) {
      emit(historyPositionChanged(dp.getId()));
  }

  /// we're done
  if (m_place.isSamePlace(centerItem->getPlace())) {
    //    qDebug() << Q_FUNC_INFO << "exiting 1 with place" << m_place.toString();
    return m_place;
  }
  m_place = centerItem->getPlace();
  m_place.setType(dp.getType());

  //  qDebug() << Q_FUNC_INFO << "exiting 2 with place" << m_place.toString();
  //  m_view->setBackgroundBrush(QBrush(Qt::cyan,Qt::Dense7Pattern));
  /*
  qDebug() << "At exit" << m_view->sceneRect();
  qDebug() << "Widget" << this->geometry();
  qDebug() << "gview" << m_view->geometry();
  qDebug() << "viewport geometry" << m_view->viewport()->geometry();
  m_view->setBackgroundBrush(QBrush(Qt::cyan,Qt::Dense7Pattern));
  QRect viewport_rect(0, 0, m_view->viewport()->width(), m_view->viewport()->height());
  QRectF visible_scene_rect = m_view->mapToScene(viewport_rect).boundingRect();
  qDebug() << "viewport rect" << viewport_rect << "scene rect" << visible_scene_rect;
  qDebug() << visible_scene_rect.width();
  */
  return m_place;
}
Place GraphicsEntry::getPage(const Place & p) {
  QList<EntryItem *> items;
  EntryItem * item;
  QString arRoot;
  QString str;
  EntryItem * focusItem;
  int page = p.getPage();



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
      item->setNotes(getApp()->notes()->find(m_rootQuery->value(2).toString()));
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
  /**
   * we need to know whether we got here by accessing the history button
   * or not
   *
   */
  /// TODO fix this
  if (getHistory()->isOn()) {
    getHistory()->add(m_place);
  }
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
    QString html =
      transform(m_xsltSource,xml);
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
    gi->setNotesEnabled(m_notesEnabled);
    if (m_dumpOutputHTML) {
      gi->setOutputHTML(html);
    }
    connect(gi,SIGNAL(linkActivated(const QString &)),this,SLOT(linkActivated(const QString &)));
    connect(gi,SIGNAL(linkHovered(const QString &)),this,SLOT(linkHovered(const QString &)));
    connect(gi,SIGNAL(showPerseus(const Place &)),this,SLOT(showPerseus(const Place &)));
    connect(gi,SIGNAL(placeChanged(const Place &)),this,SLOT(updateCurrentPlace(const Place &)));
    connect(gi,SIGNAL(selectAllItems()),this,SLOT(selectAll()));
    connect(gi,SIGNAL(clearAllItems()),this,SLOT(clearAll()));
    connect(gi,SIGNAL(gotoNode(const Place &,bool)),this,SIGNAL(gotoNode(const Place &,bool)));
    /// pass through signal for mainwindow to handle
    connect(gi,SIGNAL(bookmarkAdd(const QString &,const Place &)),this,SIGNAL(bookmarkAdd(const QString &,const Place &)));
    connect(gi,SIGNAL(copy()),this,SLOT(copy()));
    connect(gi,SIGNAL(addButton()),this,SLOT(addButtonDecoration()));
    connect(gi,SIGNAL(deleteNotes()),this,SLOT(deleteNotes()));
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

    if (m_items[i]->hasNotes()) {
      qreal btnx;
      qreal btny;
      btnx = xpos + m_items[i]->boundingRect().width();
      btny = ypos;// + sz.height();

      ToolButtonData  * notesBtn = new ToolButtonData(i);
      notesBtn->setIcon(QIcon("notes-0.xpm"));
      notesBtn->setStyleSheet("padding :0px;border : 0px;margin : 0px");
      QGraphicsWidget *pushButton = m_scene->addWidget(notesBtn);
      pushButton->setPos(btnx,btny);
      m_items[i]->setProxy(pushButton);
      connect(notesBtn,SIGNAL(clicked()),this,SLOT(notesButtonPressed()));
    }
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
    if (m_items[i]->hasNotes()) {
      qDebug() << "Adding note button for";
      qreal btnx;
      qreal btny;
      btnx = xpos + m_items[i]->boundingRect().width();
      btny = ypos;// + sz.height();

      ToolButtonData  * notesBtn = new ToolButtonData(i);
      notesBtn->setIcon(QIcon("notes-0.xpm"));
      notesBtn->setStyleSheet("padding :0px;border : 0px;margin : 0px");
      QGraphicsWidget *pushButton = m_scene->addWidget(notesBtn);
      pushButton->setPos(btnx,btny);
      m_items[i]->setProxy(pushButton);
      connect(notesBtn,SIGNAL(clicked()),this,SLOT(notesButtonPressed()));
    }
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
void GraphicsEntry::setScale(qreal v, bool use) {
  m_scale = v;
  if (use) {
    m_view->setTransform(m_transform);
    m_view->scale(m_scale,m_scale);
  }
}
/**
 *
 *
 * @param p
 * @param type search type
 * @param setFocus
 *
 * @return
 */
int GraphicsEntry::hasPlace(const Place & p,int type,bool setFocus) {
  int max = m_items.size();
  int ix = -1;
  int supp = p.getSupplement();
  QString root = p.getRoot();
  for(int i=0;i < max;i++) {
    if (type == GraphicsEntry::NodeSearch) {
      if (m_items[i]->getNode() == p.getNode()) {
          ix = i;
          i = max;
      }
    }
    else if (type == GraphicsEntry::RootSearch) {
      if (m_items[i]->isRoot() &&
          ((m_items[i]->getRoot() == root) &&
           (m_items[i]->getSupplement() == supp))) {
        ix = i;
        i = max;
      }
    }
    else if (type == GraphicsEntry::WordSearch) {
      /// TODO word search
    }
    else {
      QLOG_WARN() << "Unknown search type for place" << p;
    }

  }
  //  qDebug() << Q_FUNC_INFO << p << ix;
  if ((ix != -1) && setFocus) {
    m_scene->setFocusItem(m_items[ix]);
    m_view->ensureVisible(m_items[ix]);
    //    m_place = p;
    //    m_showPlace->setPlace(m_place);

    //    m_currentRoot = root;
    //    emit(rootChanged(root,QString()));
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
  //  qDebug() << Q_FUNC_INFO << "item" << ix << "pos" << cursor.position() << cursor.hasSelection();
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
void GraphicsEntry::updateCurrentPlace(const Place & p) {
  //  m_place = p;
}
/**
 *
 *
 */
void GraphicsEntry::copy() {
  QString txt;
  QClipboard *clipboard = QApplication::clipboard();

  for(int i=0;i < m_items.size();i++) {
   txt += m_items[i]->textCursor().selectedText();
  }
  clipboard->setText(txt);
}
void GraphicsEntry::selectAll() {
  for(int i=0;i < m_items.size();i++) {
    m_items[i]->selectAll();
  }
}
void GraphicsEntry::clearAll() {
  for(int i=0;i < m_items.size();i++) {
    m_items[i]->clearSelection();
  }
}
void GraphicsEntry::selectEntry() {
  qDebug() << "select entry";
  EntryItem * item = dynamic_cast<EntryItem *>(m_scene->focusItem());
  if (item) {
    item->selectAll();
  }
}
void GraphicsEntry::moveForward() {
  emit(next(m_place));
}
void GraphicsEntry::moveBackward() {
  emit(prev(m_place));
}
/**
 *
 */
void GraphicsEntry::onWiden() {
  m_textWidth += 50;
  //  qDebug() << "Scene rect before widen" << m_view->sceneRect() << m_textWidth;
  for(int i=0;i < m_items.size();i++) {
      m_items[i]->setTextWidth(m_textWidth);
  }
  reposition();
  m_view->setSceneRect(m_scene->sceneRect());
  //  m_view->update();
  //  qDebug() << "Scene rect after" << m_view->sceneRect();
  QGraphicsItem * item = m_scene->focusItem();
  if ( ! item ) {
    item = m_items[0];
  }
  if (item) {
    m_view->centerOn(item);//x,pos.y());
  }
}
void GraphicsEntry::onNarrow() {
  m_textWidth -= 50;
  //  qDebug() << "Scene rect before narrow" << m_view->sceneRect() << m_textWidth;
  for(int i=0;i < m_items.size();i++) {
      m_items[i]->setTextWidth(m_textWidth);
  }
  reposition();
  m_view->setSceneRect(m_scene->sceneRect());
  //  qDebug() << "Scene rect after" << m_view->sceneRect();
  QGraphicsItem * item = m_scene->focusItem();
  //  m_view->setAlignment(Qt::AlignCenter);
  if ( ! item ) {
    item = m_items[0];
  }
  if (item) {
    m_view->centerOn(item);
  }
}
void GraphicsEntry::reposition() {
  qreal ypos = 0;
  qreal xpos = 0;
  QRectF r;
  QSizeF sz;
  qreal maxwidth = 0;
  for(int i=0;i < m_items.size();i++) {
    m_items[i]->setPos(xpos,ypos);
    sz = m_items[i]->document()->size();
    ypos += sz.height() + m_entryMargin;
    if (sz.width() > maxwidth) {
      maxwidth = sz.width();
    }
  }
  m_scene->setSceneRect(QRectF(0,0,maxwidth,m_scene->height()));
}
int GraphicsEntry::search() {
  QString target = "and";
  int count = 0;
  int step = 10;
  int max = m_items.size() * step;
  QProgressDialog progress("Searching...", "Cancel search", 0,  max, this);
  progress.setWindowModality(Qt::WindowModal);
  progress.show();
  for(int i=0;i < m_items.size();i++) {
    //    QApplication::processEvents();
    progress.setValue(i * step);
    m_items[i]->highlight(target,Qt::yellow);
    count += m_items[i]->findCount();
    /// TODO check this spelling
    if (progress.wasCanceled())
      break;
  }
  progress.setValue(max);
  qDebug() << "Found" << count;
  return count;
}
void GraphicsEntry::clearHighlights() {
  for(int i=0;i < m_items.size();i++) {
    m_items[i]->clearHighlights();
  }
}
void GraphicsEntry::shiftFocus() {
  QGraphicsItem * item = m_scene->focusItem();
  //  EntryItem * ei = qobject_cast<EntryItem *>(item);
  if ( ! item  && (m_items.size() > 0)) {
    item = m_items[0];
  }
  if (item) {
    qDebug() << "ensuring current node is visible";
    //    this->setFocus();
    this->setCurrentItem(item);
    //m_view->ensureVisible(item);
    //    m_scene->setFocusItem(item);
  }
  return;
}
void GraphicsEntry::focusNode(const QString & node) {
  if (node.isEmpty()) {
    return;
  }
  qDebug() << Q_FUNC_INFO << node;
  for(int i=0;i < m_items.size();i++) {
    Place p = m_items[i]->getPlace();
    if (p.getNode() == node) {
      //      m_view->ensureVisible(m_items[i]);
      //      m_view->setFocus();
      //      m_scene->setFocus();
      //      m_scene->setFocusItem(m_items[i]);
      //      m_items[i]->setSelected(true);
      this->setCurrentItem(m_items[i]);
      return;
    }
  }
}
bool GraphicsEntry::hasNode(const QString & node) {
  if (node.isEmpty()) {
    return true;
  }
  for(int i=0;i < m_items.size();i++) {
    Place p = m_items[i]->getPlace();
    if (p.getNode() == node) {
      return true;
    }
  }
  return false;
}
void GraphicsEntry::closeEvent(QCloseEvent * event) {
  qDebug() << Q_FUNC_INFO;
  while(m_items.size() > 0) {
    EntryItem * item = m_items.takeFirst();
    delete item;
  }
  QWidget::closeEvent(event);
}
void GraphicsEntry::notesButtonPressed() {
  ToolButtonData * btn = qobject_cast<ToolButtonData *>(QObject::sender());
  if (btn) {
    //qDebug() << "index" << btn->getIndex();
    EntryItem * item = m_items[btn->getIndex()];
    item->showNote();

  }
}
void GraphicsEntry::addButtonDecoration() {
  EntryItem * item = qobject_cast<EntryItem *>(QObject::sender());
  if ( ! item )
    return;

  for(int i=0;i < m_items.size();i++) {
    if (m_items[i] == item) {
      qreal btnx;
      qreal btny;
      QPointF pos = item->pos();
      btnx = pos.x() + item->boundingRect().width();
      btny = pos.y();// + sz.height();

      ToolButtonData  * notesBtn = new ToolButtonData(i);
      notesBtn->setIcon(QIcon("notes-0.xpm"));
      notesBtn->setStyleSheet("padding :0px;border : 0px;margin : 0px");
      QGraphicsWidget *pushButton = m_scene->addWidget(notesBtn);
      pushButton->setPos(btnx,btny);
      connect(notesBtn,SIGNAL(clicked()),this,SLOT(notesButtonPressed()));
    }
  }

}
void GraphicsEntry::deleteNotes() {
  qDebug() << Q_FUNC_INFO;
  EntryItem * item = qobject_cast<EntryItem *>(QObject::sender());
  if ( ! item )
    return;

  NoteMaster * m = getNotes();
  QList<Note *> notes = item->getNotes(true);
  qDebug() << "Delete" << notes.size() << "notes";
  while(notes.size() > 0) {
    Note * n = notes.takeFirst();
    m->remove(n);
    delete n;
  }
}
void GraphicsEntry::focusPlace() {
  Place p = this->getPlace();
  if (! p.getNode().isEmpty()) {
    qDebug() << Q_FUNC_INFO << p.getNode();
    this->focusNode(p.getNode());
    return;
  }
  //  this->shiftFocus();
}
void GraphicsEntry::setCurrentItem(QGraphicsItem * item) {
  qDebug() << Q_FUNC_INFO;
  m_view->setFocus();
  m_view->ensureVisible(item);
  m_scene->setFocusItem(item);
  EntryItem * entry = dynamic_cast<EntryItem *>(item);
  if (entry)
    m_place = entry->getPlace();

}
