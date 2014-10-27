#include "graphicsentry.h"
#include "QsLog.h"
#include "xsltsupport.h"
#include "history.h"
#include "place.h"
#include "entryitem.h"
#include "application.h"
#include "notes.h"
#include "definedsettings.h"
#include "laneslexicon.h"
#include "namespace.h"
#include "searchdialogs.h"
#include "searchoptionswidget.h"
#include "externs.h"
ToolButtonData::ToolButtonData(int id) : QToolButton() {
  m_id = id;
  setObjectName("toolbuttondata");
}
ToolButtonData::~ToolButtonData() {

}
/**
 * NOT NEEDED
 *
 * @param scene
 * @param parent

 LaneGraphicsView::LaneGraphicsView(QGraphicsScene * scene,GraphicsEntry * parent) :
 QGraphicsView(scene,parent) {
 setObjectName("lexicongraphicsview");
 setFocusPolicy(Qt::StrongFocus);
 setAlignment(Qt::AlignLeft|Qt::AlignTop);
 }
 /// TODO get rid of this
 void LaneGraphicsView::scrollContentsBy(int dx,int dy) {
 QGraphicsView::scrollContentsBy(dx,dy);
 }
 void LaneGraphicsView::keyPressEvent(QKeyEvent * event) {

 QGraphicsView::keyPressEvent(event);
 }
 void LaneGraphicsView::focusInEvent(QFocusEvent * event) {
 QGraphicsView::focusInEvent(event);
 }
 void LaneGraphicsView::focusOutEvent(QFocusEvent * event) {
 QGraphicsView::focusOutEvent(event);
 }
*/
/**
 * (the QSqlQuery instances rely on the default connection being to
 * to the lexicon)
 *
 * @param parent
 */
GraphicsEntry::GraphicsEntry(QWidget * parent ) : QWidget(parent) {
  setObjectName("graphicsentry");
  setFocusPolicy(Qt::StrongFocus);
  readSettings();
  QVBoxLayout * layout = new QVBoxLayout;
  m_textOption.setTextDirection(Qt::LeftToRight);
  //  m_compiledXsl = 0;
  /// 0 = paging forward, items are appended
  /// 1 = paging backward, items are prepended
  m_pagingDir = 0;
  m_scale = 1.0;

  m_scene = new QGraphicsScene(this);
  m_view = new QGraphicsView(m_scene,this);

  m_view->setFocusPolicy(Qt::StrongFocus);
  //  m_view->setFocusProxy(this);
  m_view->setSceneRect(m_scene->sceneRect());
  m_view->setAlignment(Qt::AlignTop);



  //  connect(m_view,SIGNAL(nextPage()),this,SLOT(nextPageRequested()));
  //  connect(m_view,SIGNAL(backPage()),this,SLOT(prevPageRequested()));



  m_view->setInteractive(true);
  //  m_view->setAlignment(Qt::AlignLeft);
  /// TODO what is this for ?
  m_item = new QGraphicsTextItem("");
  m_item->setTextInteractionFlags(Qt::TextBrowserInteraction);
  m_item->setTextWidth(m_textWidth);
  m_scene->addItem(m_item);

  // add the graphics viwe
  layout->addWidget(m_view);


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
  QLOG_DEBUG() << Q_FUNC_INFO;
  delete m_nodeQuery;
  delete m_rootQuery;
  delete m_pageQuery;
  /// TODO xalan cleanup ?
}
QGraphicsView * GraphicsEntry::getView() const {
  return m_view;
}
void GraphicsEntry::readSettings() {
  QString v;
  bool ok;
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QMap<QString,QString> cmdOptions = app->getOptions();

  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());
  settings->beginGroup("Entry");
  m_debug = settings->value(SID_ENTRY_DEBUG,false).toBool();
  QString css = settings->value(SID_ENTRY_CSS,QString("entry.css")).toString();
  css = readCssFromFile(css);
  if (! css.isEmpty()) {
    m_currentCss = css;
    emit(cssChanged());
  }
  css = settings->value(SID_ENTRY_PRINT_CSS,QString("entry_print.css")).toString();
  css = readCssFromFile(css);
  if (! css.isEmpty()) {
    m_printCss = css;
  }

  m_xsltSource = settings->value(SID_ENTRY_XSLT,QString("entry.xslt")).toString();
  m_textWidth = settings->value(SID_ENTRY_TEXT_WIDTH,300).toInt();
  if (cmdOptions.contains("textwidth")) {
    int w = cmdOptions.value("textwidth").toInt(&ok);
    if (ok) {
      m_textWidth = w;
    }
  }
  m_defaultWidth = m_textWidth;

  m_entryMargin = settings->value(SID_ENTRY_MARGIN,10).toInt();
  /// TODO change this to use color names
  v  = settings->value(SID_ENTRY_SUPPLEMENT_BACKGROUND_COLOR,"lightgray").toString();

  m_supplementBg = QColor(v);
  if (! m_supplementBg.isValid())  {
    m_supplementBg = QColor::fromRgb(211,211,211);
  }
  m_clearScene = settings->value(SID_ENTRY_CLEAR_SCENE,true).toBool();
  /// these are set to empty to disable the feature
  m_moveFocusUpKey = settings->value(SID_ENTRY_MOVE_FOCUS_UP,QString()).toString();
  m_moveFocusDownKey = settings->value(SID_ENTRY_MOVE_FOCUS_DOWN,QString()).toString();
  m_moveForwardKey = settings->value(SID_ENTRY_FORWARD,QString()).toString();
  m_moveBackwardKey = settings->value(SID_ENTRY_BACK,QString()).toString();
  m_zoomInKey = settings->value(SID_ENTRY_ZOOM_IN,QString()).toString();
  m_zoomOutKey = settings->value(SID_ENTRY_ZOOM_OUT,QString()).toString();
  m_reloadKey = settings->value(SID_ENTRY_RELOAD,QString()).toString();

  m_widenKey = settings->value(SID_ENTRY_WIDEN,QString()).toString();
  m_narrowKey = settings->value(SID_ENTRY_NARROW,QString()).toString();
  m_widenStep = settings->value(SID_ENTRY_STEP,50).toInt(&ok);
  if ( ! ok ) {
    m_widenStep = 50;
  }
  m_searchKey = settings->value(SID_ENTRY_FIND,QString()).toString();
  m_searchNextKey = settings->value(SID_ENTRY_FIND_NEXT,QString()).toString();
  m_clearKey = settings->value(SID_ENTRY_CLEAN,QString()).toString();
  m_showKey = settings->value(SID_ENTRY_SHOW,QString()).toString();
  m_homeKey = settings->value(SID_ENTRY_HOME,QString()).toString();
  m_markKey = settings->value(SID_ENTRY_MARK,QString()).toString();


  m_dumpXml = settings->value(SID_ENTRY_DUMP_XML,false).toBool();
  m_dumpHtml = settings->value(SID_ENTRY_DUMP_HTML,false).toBool();
  m_dumpOutputHtml = settings->value(SID_ENTRY_DUMP_OUTPUT_HTML,false).toBool();
  settings->endGroup();

  settings->beginGroup("Notes");
  m_notesEnabled = settings->value(SID_NOTES_ENABLED,true).toBool();
  settings->endGroup();

  settings->beginGroup("Diacritics");
  QStringList keys = settings->childKeys();
  QStringList points;
  for(int i=0;i < keys.size();i++) {
    if (keys[i].startsWith("Char")) {
      v = settings->value(keys[i],QString()).toString();
      points << v;
    }
  }
  m_pattern = QString("[\\x%1]*").arg(points.join("\\x"));

  settings->endGroup();

}
void GraphicsEntry::writeDefaultSettings() {

}
void GraphicsEntry::keyPressEvent(QKeyEvent * event) {
  if (event->key() == Qt::Key_Escape) {
    QWidget * w = this->parentWidget();
    while(w) {
      QTabWidget * tabw = qobject_cast<QTabWidget *>(w);
      if (tabw)  {
        tabw->tabBar()->setFocus();
        return;
      }
      w = w->parentWidget();
    }
  }
  if (! m_reloadKey.isEmpty() && (event->text() == m_reloadKey)) {
    onReload();
    return;
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
    this->search();
    return;
  }
  if (! m_searchNextKey.isEmpty() && (event->text() == m_searchNextKey)) {
    this->searchNext();
    return;
  }
  if (! m_clearKey.isEmpty() && (event->text() == m_clearKey)) {
    this->clearHighlights();
    return;
  }
  if (! m_showKey.isEmpty() && (event->text() == m_showKey)) {
    this->showSelections();
    return;
  }
  if (! m_markKey.isEmpty() && (event->text() == m_markKey)) {
    m_focusNode = m_place.getNode();
    return;
  }
  if (! m_homeKey.isEmpty() && (event->text() == m_homeKey)) {
    this->home();
    return;
  }
  QWidget::keyPressEvent(event);

}
void GraphicsEntry::home() {
  this->focusNode(m_focusNode);
}
QString GraphicsEntry::getHome() const {
  return m_focusNode;
}
void GraphicsEntry::setHome(const QString & node) {
  m_focusNode = node;
}
void GraphicsEntry::moveFocusDown() {
  EntryItem * item = dynamic_cast<EntryItem *>(m_scene->focusItem());
  if (item) {
    int m = m_items.size() - 1;
    for(int i=0;i < m ;i++) {
      if (m_items[i] == item) {
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
Place GraphicsEntry::getPlace(int index) const {
  if (index == -1) {
    EntryItem * item = dynamic_cast<EntryItem *>(m_scene->focusItem());
    if (item)
      return item->getPlace();
  }
  else {
    if (index < m_items.size()) {
      return m_items[index]->getPlace();
    }
  }

  return m_place;
}
/**
 * returns the highest page number
 * @ w = 0, get the highest page
 *       1, get the lowest
 * @return
 */
int GraphicsEntry::getPageNumber(int w) const {
  QList<int> pages;
  int x;
  for(int i=0;i < m_items.size();i++) {
    Place p = m_items[i]->getPlace();
    x = p.getPage();
    if (x != -1) {
      pages << x;
    }
  }
  qSort(pages);
  if (w == 0) {
    return pages.takeLast();
  }
  else {
    return pages.takeFirst();
  }
}
void GraphicsEntry::focusInEvent(QFocusEvent * event) {
  /// giving focus to the graphicsview so keyboard
  //  m_view->setFocus();
  //  QLOG_DEBUG() << Q_FUNC_INFO;
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
  /// TODO need to remove the graphicswidget for the items which have notes
  m_scene->clear();
}
/**
 * redundant
 *
 */

void GraphicsEntry::anchorClicked(const QUrl & link) {
  QLOG_DEBUG() << Q_FUNC_INFO << link.toDisplayString();
  QLOG_DEBUG() << QApplication::keyboardModifiers();
}
/**
 * There are three options here:
 * (1)  a <note> so we tell the user something
 * (2)  a jump to another root, which is how entries like  xxx see yyy,xxx and yyy etc are handled
 * (3)  a jump to another word
 *
 * @param link
 */
void GraphicsEntry::linkActivated(const QString & link) {
  QLOG_DEBUG() << Q_FUNC_INFO << link;

  /// turn history on as the user has clicked on something
  Place p;
  int options = 0;
  if (QApplication::keyboardModifiers() & (Qt::ShiftModifier | Qt::ControlModifier)) {
    options |= Lane::Create_Tab;
  }

  QUrl url(link);
  QLOG_DEBUG() << Q_FUNC_INFO << url;
  if (url.hasQuery()) {
    QString msg = url.query();
    QLOG_DEBUG() << "query" << msg;
    if (msg.startsWith("text=")) {
      msg.remove("text=");
      QMessageBox::information(this, tr("A note from the editor"),
                               msg,
                               QMessageBox::Ok);
      return;
    }
    if (msg.startsWith("root=")) {
      msg.remove("root=");
      QSqlQuery query;
      query.prepare("select word from root where bword = ?");
      query.bindValue(0,msg);
      query.exec();
      if (query.first()) {
        getHistory()->on();
        p.setRoot(query.value(0).toString());
        emit(gotoNode(p,options));
        return;
      }
      else {
        QLOG_WARN() << "Jump to missing root requested" << msg << query.lastError().text();
      }
    }
  }
  else {
    QString node(link);
    /// remove the leading #
    node.remove(0,1);
    p.setNode(node);
    getHistory()->on();
    /// TODO replace this
    /// including move to new tab stuff
    showPlace(p,false,options);
  }
}
void GraphicsEntry::linkHovered(const QString & link) {
  QGraphicsTextItem * gi = static_cast<QGraphicsTextItem *>(QObject::sender());

  if (link.isEmpty()) {
    gi->setCursor(QCursor(Qt::ArrowCursor));
  }
  else {
    gi->setCursor(QCursor(Qt::PointingHandCursor));
    QLOG_DEBUG() << Q_FUNC_INFO << link;
    QLOG_DEBUG() << QApplication::keyboardModifiers();
  }
}
/**
 * redundant
 *
 */
void GraphicsEntry::anchorTest() {
  QString node;
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
/// TODO fix this !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/// TODO how is it supposed to be used ?????????
Place GraphicsEntry::showPlace(const Place & p,bool thisPageOnly,int options) {
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
    QLOG_DEBUG() << "Out of page for node" << node;
    p.setNode(node);
    p.setOptions(options);
    emit(gotoNode(p,options));
    /*
      np = getXmlForRoot(p);
      /// is this right ?
      if (np != p) {
      emit(placeChanged(np));
      m_place = np;
      }
    */
  }
  return np;
}
/**
 *
 *
 */
QString GraphicsEntry::readCssFromFile(const QString & name) {
  QString css;
  QFile f(name);
  if (! f.open(QIODevice::ReadOnly)) {
    QLOG_WARN()  << "Cannot open CSS file for reading: " << name
                 << f.errorString();
    return css;
  }
  QTextStream in(&f);
  css = in.readAll();
  f.close();
  return css;
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


  m_focusNode = node;
  QLOG_DEBUG() << "Focus node set" << node;
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
      //      QLOG_DEBUG() << Q_FUNC_INFO << "exiting 3 with place" << retval.toString();
      return retval;
    }
  }
  int quasi = 0;
  QSqlQuery quasiQuery;
  if (quasiQuery.prepare("select quasi from root where word = ? and datasource = 1")) {
    quasiQuery.bindValue(0,root);
    quasiQuery.exec();
    if (quasiQuery.first()) {
      quasi = quasiQuery.value(0).toInt();
    }
    else {
      QLOG_WARN() << tr("Error quasi query failed") << quasiQuery.lastError().text();
    }
  }
  else {
    QLOG_WARN() << tr("Error quasi query prepare") << quasiQuery.lastError().text();
  }
  QLOG_DEBUG() << Q_FUNC_INFO << __LINE__ << p << p.getType();
  m_rootQuery->bindValue(0,root);
  m_rootQuery->exec();
  if (! m_rootQuery->first()) {
    QLOG_WARN() << QString(tr("Root not found %1")).arg(root);
    return retval;
  }

  QString str = QString("<word type=\"root\" ar=\"%1\" quasi=\"%2\"></word>").arg(root).arg(quasi);
  QLOG_DEBUG() << str;
  EntryItem * rootItem  = createEntry(str);
  /// will be null if the XSLT/XML has not parsed correctly
  if (rootItem == NULL) {
    return p;
  }
  ///
  /// write a history record because we are leaving this page
  ///
  if (getHistory()->isOn()) {
    //    QLOG_DEBUG() << "History old place" << m_place;
    //    QLOG_DEBUG() << "New place" << dp;
    if (dp.getType() == Place::User) {
      getHistory()->add(m_place);
      /// this allows mainwindow to update the history list
      emit(historyAddition(dp));
    }
  }

  if (m_clearScene) {
    onClearScene();
  }
  rootItem->setRoot(root,true);
  rootItem->setSupplement(m_rootQuery->value(8).toInt());
  rootItem->setPage(m_rootQuery->value(5).toInt());

  items << rootItem;


  /// by default we will center on the root item
  centerItem = rootItem;
  /// now add all the entries for the root
  do  {
    supplement = m_rootQuery->value(8).toInt();
    QString t  = QString("<word buck=\"%1\" ar=\"%2\" page=\"%3\" itype=\"%4\" supp=\"%5\">")
      .arg(m_rootQuery->value(3).toString())
      .arg(m_rootQuery->value(2).toString())
      .arg(m_rootQuery->value(5).toInt())
      .arg(m_rootQuery->value(6).toString())
      .arg(m_rootQuery->value(8).toInt());
    t += m_rootQuery->value(4).toString();
    t += "</word>";
    if (m_dumpXml) {
      QFileInfo fi(QDir::tempPath(),QString("%1.xml").arg(m_rootQuery->value(7).toString()));
      QFile f(fi.filePath());
      if (f.open(QIODevice::WriteOnly)) {
        QTextStream out(&f);
        out.setCodec("UTF-8");
        out << t;
      }
    }
    EntryItem * item  = createEntry(t);
    if (item != NULL) {
      if (m_dumpOutputHtml) {
        QFileInfo fi(QDir::tempPath(),QString("%1-out.html").arg(m_rootQuery->value(7).toString()));
        QFile f(fi.filePath());
        if (f.open(QIODevice::WriteOnly)) {
          QTextStream out(&f);
          out.setCodec("UTF-8");
          out << item->getOutputHtml();
        }
      }
      Place p;
      p.setSupplement(supplement);
      p.setNode(m_rootQuery->value(7).toString());
      p.setRoot(root);
      p.setWord(m_rootQuery->value(2).toString());
      p.setPage(m_rootQuery->value(5).toInt());
      item->setPlace(p);
      QList<Note *> notes;
      item->setNotes();//getApp()->notes()->find(m_rootQuery->value(2).toString()));
      items << item;
      /// if we asked for a specific word/node, focus on it
      if (! node.isEmpty() && (item->getNode() == node)) {
        centerItem = item;
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
  if (centerItem) {
    this->setCurrentItem(centerItem);
  }
  else {
    QLOG_DEBUG() << Q_FUNC_INFO << "no center item";
  }

  if (dp.getType() == Place::History) {
    emit(historyPositionChanged(dp.getId()));
  }

  /// we're done
  if (m_place.isSamePlace(centerItem->getPlace())) {
    return m_place;
  }
  m_place = centerItem->getPlace();
  m_place.setType(dp.getType());

  //  QLOG_DEBUG() << Q_FUNC_INFO << "exiting 2 with place" << m_place.toString();
  //  m_view->setBackgroundBrush(QBrush(Qt::cyan,Qt::Dense7Pattern));
  /*
    QLOG_DEBUG() << "At exit" << m_view->sceneRect();
    QLOG_DEBUG() << "Widget" << this->geometry();
    QLOG_DEBUG() << "gview" << m_view->geometry();
    QLOG_DEBUG() << "viewport geometry" << m_view->viewport()->geometry();
    m_view->setBackgroundBrush(QBrush(Qt::cyan,Qt::Dense7Pattern));
    QRect viewport_rect(0, 0, m_view->viewport()->width(), m_view->viewport()->height());
    QRectF visible_scene_rect = m_view->mapToScene(viewport_rect).boundingRect();
    QLOG_DEBUG() << "viewport rect" << viewport_rect << "scene rect" << visible_scene_rect;
    QLOG_DEBUG() << visible_scene_rect.width();
  */
  //  if (m_place.isSupplement()) {
  //    m_view->setBackgroundBrush(QBrush(m_supplementBg));
  //  }
  return m_place;
}
Place GraphicsEntry::getPage(const Place & p) {
  QList<EntryItem *> items;
  EntryItem * item;

  EntryItem * focusItem;
  int page = p.getPage();


  //  qStrip << Q_FUNC_INFO << "Page" << page;
  m_pageQuery->bindValue(0,page);
  m_pageQuery->exec();
  if (! m_pageQuery->first()) {
    QLOG_INFO() << "Page not found" << page;
    return p;
  }
  QString node = m_pageQuery->value("nodeid").toString();

  if (m_clearScene) {
    onClearScene();
  }
  QString lastRoot;
  // "select root,broot,word,bword,xml,page,itype,nodeId,supplement from entry where datasource =  // 1 and page = ? order by nodenum asc");


  int rootCount = 0;
  int entryCount = 0;
  do   {
    qStrip << Q_FUNC_INFO << m_pageQuery->value("nodeid").toString();
    int supplement = m_pageQuery->value(8).toInt();
    QString root = m_pageQuery->value(0).toString();
    /// if root has changed add root XML
    if (root != lastRoot) {
      if (! lastRoot.isEmpty()) {
        QSqlQuery quasiQuery;
        int quasi = 0;
        if (quasiQuery.prepare("select quasi from root where word = ? and datasource = 1")) {
          quasiQuery.bindValue(0,root);
          quasiQuery.exec();
          if (quasiQuery.first()) {
            quasi = quasiQuery.value(0).toInt();
          }
          else {
            QLOG_WARN() << tr("Error quasi query failed") << quasiQuery.lastError().text();
          }
        }
        else {
          QLOG_WARN() << tr("Error quasi query prepare") << quasiQuery.lastError().text();
        }
        QString  str = QString("<word type=\"root\" ar=\"%1\" quasi=\"%2\"></word>").arg(root).arg(quasi);

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
    if (m_dumpXml) {
      QFileInfo fi(QDir::tempPath(),QString("%1.xml").arg(m_pageQuery->value(7).toString()));
      QFile f(fi.filePath());
      if (f.open(QIODevice::WriteOnly)) {
        QTextStream out(&f);
        out.setCodec("UTF-8");
        out << t;
      }
    }
    item  = createEntry(t);
    if (item != NULL) {
      if (m_dumpOutputHtml) {
        QFileInfo fi(QDir::tempPath(),QString("%1-out.html").arg(m_pageQuery->value(7).toString()));
        QFile f(fi.filePath());
        if (f.open(QIODevice::WriteOnly)) {
          QTextStream out(&f);
          out.setCodec("UTF-8");
          out << item->getOutputHtml();
        }
      }
      Place p;
      p.setSupplement(supplement);
      p.setNode(m_pageQuery->value(7).toString());
      p.setRoot(root);
      p.setWord(m_pageQuery->value(2).toString());
      p.setPage(m_pageQuery->value(5).toInt());
      item->setPlace(p);
      item->setNotes();//getApp()->notes()->find(m_rootQuery->value(2).toString()));
      items << item;

      entryCount++;
    }
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
  m_view->setSceneRect(m_scene->sceneRect());

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
    transform(ENTRY_XSLT,m_xsltSource,xml);
  if (html.isEmpty()) {
    return NULL;
  }
  EntryItem * gi = new EntryItem("");
  gi->document()->setDefaultStyleSheet(m_currentCss);
  gi->setTextWidth(m_textWidth);
  gi->setHtml(html);
  gi->setOutputHtml(html);
  gi->setXml(xml);
  /// need this otherwise arabic text will be right justified
  gi->document()->setDefaultTextOption(m_textOption);
  gi->setTextInteractionFlags(Qt::TextBrowserInteraction);
  gi->setAcceptHoverEvents(true);
  gi->setBackground(m_supplementBg);
  gi->setNotesEnabled(m_notesEnabled);
  //    if (m_dumpOutputHtml) {
  //      gi->setOutputHtml(html);
  //    }
  connect(gi,SIGNAL(linkActivated(const QString &)),this,SLOT(linkActivated(const QString &)));
  connect(gi,SIGNAL(linkHovered(const QString &)),this,SLOT(linkHovered(const QString &)));
  connect(gi,SIGNAL(showPerseus(const Place &)),this,SLOT(showPerseus(const Place &)));
  connect(gi,SIGNAL(showHtml()),this,SLOT(showHtml()));

  /// TODO this is no longer used
  connect(gi,SIGNAL(placeChanged(const Place &)),this,SLOT(updateCurrentPlace(const Place &)));
  connect(gi,SIGNAL(selectAllItems()),this,SLOT(selectAll()));
  connect(gi,SIGNAL(clearAllItems()),this,SLOT(clearAll()));
  connect(gi,SIGNAL(gotoNode(const Place &,int)),this,SIGNAL(gotoNode(const Place &,int)));
  /// pass through signal for mainwindow to handle
  connect(gi,SIGNAL(bookmarkAdd(const QString &,const Place &)),this,SIGNAL(bookmarkAdd(const QString &,const Place &)));
  connect(gi,SIGNAL(copy()),this,SLOT(copy()));
  connect(gi,SIGNAL(addButton(bool)),this,SLOT(addButtonDecoration(bool)));
  connect(gi,SIGNAL(printNode(const QString &)),this,SIGNAL(printNode(const QString &)));
  connect(gi,SIGNAL(printPage()),this,SIGNAL(printPage()));

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
    if (m_dumpHtml) {
      QFileInfo fi(QDir::tempPath(),QString("%1.html").arg(m_items[i]->getNode()));
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
    if (m_dumpHtml) {
      QFileInfo fi(QDir::tempPath(),QString("%1.html").arg(m_items[i]->getNode()));
      QFile f(fi.filePath());
      if (f.open(QIODevice::WriteOnly)) {
        QTextStream out(&f);
        out.setCodec("UTF-8");
        out << m_items[i]->toHtml();
      }
    }
    sz = m_items[i]->document()->size();
    if (m_items[i]->hasNotes()) {
      QLOG_DEBUG() << "Adding note button for";
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
QString GraphicsEntry::transform(int type,const QString & xsl,const QString & xml) {
  int ok;
  ok = compileStylesheet(type,xsl);
  if (ok == 0) {
    QString html = xsltTransform(type,xml);
    if (! html.isEmpty()) {
      return html;
    }
    else {
      QLOG_WARN() << "Transform returned no HTML";
      return QString();
    }
  }
  /// could be errors in stylesheet or in the xml
  QStringList errors = getParseErrors();
  /// TODO fix this
  errors.prepend("Errors when processing entry styesheet:");
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
  //  QLOG_DEBUG() << Q_FUNC_INFO << p << ix;
  if ((ix != -1) && setFocus) {
    m_scene->setFocusItem(m_items[ix]);
    m_view->ensureVisible(m_items[ix]);
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
    QLOG_DEBUG() << "emit nextRoo" << root;
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
  QLOG_DEBUG() << Q_FUNC_INFO;
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
    emit prevRoot(root);
  }
  return root;
}
void GraphicsEntry::showPerseus(const Place & p) {
  QString node = p.getNode();
  if ( node.isEmpty()) {
    QMessageBox msgBox;
    msgBox.setText(tr("No XML found"));
    msgBox.exec();
    return;
  }
  m_nodeQuery->bindValue(0,node);
  m_nodeQuery->exec();
  QString xml;
  if (m_nodeQuery->first()) {
    xml = m_nodeQuery->value("xml").toString();
  }
  else {
    xml = "No XML for " + node;
  }
  QMessageBox msgBox;
  msgBox.setText(xml);
  msgBox.exec();
}
void GraphicsEntry::showHtml() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  EntryItem * item = qobject_cast<EntryItem *>(QObject::sender());
  if (! item )
    return;


  QString xml = item->getXml();
  QString html = transform(ENTRY_XSLT,m_xsltSource,xml);

  QMessageBox msgBox;
  msgBox.setTextFormat(Qt::PlainText);
  msgBox.setText(html);
  msgBox.exec();
}
void GraphicsEntry::updateCurrentPlace(const Place & /* p */) {
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
  QLOG_DEBUG() << "select entry";
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
  /// TODO set width step in INI file
  m_textWidth += 50;
  //  QLOG_DEBUG() << "Scene rect before widen" << m_view->sceneRect() << m_textWidth;
  for(int i=0;i < m_items.size();i++) {
    m_items[i]->setTextWidth(m_textWidth);
  }
  reposition();
  m_view->setSceneRect(m_scene->sceneRect());
  //  m_view->update();
  //  QLOG_DEBUG() << "Scene rect after" << m_view->sceneRect();
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
  //  QLOG_DEBUG() << "Scene rect before narrow" << m_view->sceneRect() << m_textWidth;
  for(int i=0;i < m_items.size();i++) {
    m_items[i]->setTextWidth(m_textWidth);
  }
  reposition();
  m_view->setSceneRect(m_scene->sceneRect());
  //  QLOG_DEBUG() << "Scene rect after" << m_view->sceneRect();
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
void GraphicsEntry::clearHighlights(bool keepResults) {
  int pos_x = m_view->horizontalScrollBar()->value();
  int pos_y = m_view->verticalScrollBar()->value();
  QGraphicsItem * item = m_scene->focusItem();
  for(int i=0;i < m_items.size();i++) {
    m_items[i]->clearHighlights(keepResults);
  }
  if (item) {
    m_view->ensureVisible(item);
    m_scene->setFocusItem(item);
  }
  m_view->horizontalScrollBar()->setValue(pos_x);
  m_view->verticalScrollBar()->setValue(pos_y);
  m_highlightCount = 0;
}
void GraphicsEntry::shiftFocus() {
  QGraphicsItem * item = m_scene->focusItem();
  //  EntryItem * ei = qobject_cast<EntryItem *>(item);
  if ( ! item  && (m_items.size() > 0)) {
    item = m_items[0];
  }
  if (item) {
    QLOG_DEBUG() << "ensuring current node is visible";
    //    this->setFocus();
    this->setCurrentItem(item);
    //m_view->ensureVisible(item);
    //    m_scene->setFocusItem(item);
  }
  return;
}
bool GraphicsEntry::focusNode(const QString & node) {
  if (node.isEmpty()) {
    return true;
  }
  QLOG_DEBUG() << Q_FUNC_INFO << node;
  for(int i=0;i < m_items.size();i++) {
    Place p = m_items[i]->getPlace();
    if (p.getNode() == node) {
      //      m_view->ensureVisible(m_items[i]);
      //      m_view->setFocus();
      //      m_scene->setFocus();
      //      m_scene->setFocusItem(m_items[i]);
      //      m_items[i]->setSelected(true);
      this->setCurrentItem(m_items[i]);
      return true;
    }
  }
  QLOG_DEBUG() << "focusNode failed, cannot find node" << node;
  return false;
}
/**
 * Will return true if the given node is on this page
 *
 * @param node the node to find (nxxxx)
 *
 * @return true if found, otherwise false
 */
bool GraphicsEntry::hasNode(const QString & node) const {
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
  QLOG_DEBUG() << Q_FUNC_INFO;
  while(m_items.size() > 0) {
    EntryItem * item = m_items.takeFirst();
    delete item;
  }
  QWidget::closeEvent(event);
}
void GraphicsEntry::notesButtonPressed() {
  ToolButtonData * btn = qobject_cast<ToolButtonData *>(QObject::sender());
  if (btn) {
    //QLOG_DEBUG() << "index" << btn->getIndex();
    EntryItem * item = m_items[btn->getIndex()];
    item->showNote();

  }
}
void GraphicsEntry::addButtonDecoration(bool ok) {
  EntryItem * item = qobject_cast<EntryItem *>(QObject::sender());
  if ( ! item )
    return;

  if (!ok) {
    return;
  }
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
      m_items[i]->setProxy(pushButton);
      Place p = item->getPlace();
      item->setNotes();//getApp()->notes()->find(p.getWord()));
    }
  }

}

void GraphicsEntry::focusPlace() {
  Place p = this->getPlace();
  if (! p.getNode().isEmpty()) {
    QLOG_DEBUG() << Q_FUNC_INFO << p.getNode();
    this->focusNode(p.getNode());
    return;
  }
  else {
    QLOG_DEBUG() << "setting focus through shiftFocus()";
    this->shiftFocus();
  }
}
void GraphicsEntry::setCurrentItem(QGraphicsItem * item) {
  m_view->setFocus();
  m_view->ensureVisible(item);
  m_scene->setFocusItem(item);
  EntryItem * entry = dynamic_cast<EntryItem *>(item);
  if (entry)
    m_place = entry->getPlace();

}
/**
 * This uses different default search options from the other search routines
 *
 *
 * @return
 */
int GraphicsEntry::search() {
  QString target = "and";
  int count = 0;


  SearchOptions options;
  options.setSearchScope(SearchOptions::Local);
  // int max = m_items.size() * step;
  QString v;

  QLOG_DEBUG() << Q_FUNC_INFO;
  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());
  settings->beginGroup("LocalSearch");
  v  = settings->value("Type",QString("normal")).toString();
  if (v == "normal") {
    options.setSearchType(SearchOptions::Normal);
  }
  else {
    options.setSearchType(SearchOptions::Regex);
  }

  options.setIgnoreDiacritics(settings->value("Ignore diacritics",true).toBool());
  options.setWholeWordMatch(settings->value("Whole word",true).toBool());
  options.setForceLTR(settings->value("Force LTR",false).toBool());

  ArabicSearchDialog * d = new ArabicSearchDialog(SearchOptions::Local,this);
  d->setOptions(options);
  QString t;
  if (d->exec()) {
    t = d->getText();
    if ( t.isEmpty()) {
      return -1;
    }
    d->getOptions(options);
    m_currentSearchOptions = options;
  }
  else {
    return -1;
  }
  m_findCount = 0;
  m_highlightCount = 0;
  m_currentFind = 0;
  this->clearHighlights(false);
  m_searchItemPtr = 0;
  m_searchIndex = 0;
  /// TODO remove these
  m_currentSearchPosition = -1;
  m_currentSearchIndex = -1;
  m_searchItemIndexes.clear();
  QRegExp rx = SearchOptionsWidget::buildRx(t,m_pattern,options);
  QLOG_DEBUG() << "Search pattern" << rx.pattern();
  m_currentSearchRx = rx;
  m_currentSearchTarget = t;
  QGraphicsItem * focusItem = m_scene->focusItem();
  //  this->m_items[0]->ensureVisible();
  int pos = 0;
  bool found;
  qDebug() << "Show all" << options.showAll();
  for(int i=1;i < m_items.size();i++) {
    pos = 0;
    found = false;
    while(pos != -1) {
      pos++;
      m_currentSearchPosition = m_items[i]->find(rx,pos,options.showAll());
      if (m_currentSearchPosition != -1) {
        found = true;
        count++;
        if (options.showAll()) {
          m_highlightCount++;
        }
      }
      pos = m_currentSearchPosition;
    }
    if (found) {
      m_searchItemIndexes << i;
    }
  }
  m_findCount = count;

  if (count  == 0) {
    QMessageBox msgBox;
    msgBox.setObjectName("wordnotfound");
    msgBox.setTextFormat(Qt::RichText);
    QString style;
    /// TODO get this from INI
    if (UcdScripts::isScript(m_currentSearchTarget,"Arabic")) {
      t = (qobject_cast<Lexicon *>(qApp))->spanArabic(t,"wordnotfound");
    }
    msgBox.setText(QString(tr("Word not found: %1")).arg(t));
    msgBox.exec();
    if (focusItem) {
      m_scene->setFocusItem(focusItem);
    }
  }
  else {
    if (! m_currentSearchOptions.showAll()) {
      this->centerOnSearchResult(m_searchItemIndexes[m_searchItemPtr],0);
      emit(searchStarted());
    }
    else {
      m_currentFind = m_findCount;
      statusMessage(QString("Find count: %1").arg(m_findCount));
      emit(searchFinished());
    }
  }
  return count;
}
/**
 * m_currentSearchIndex points to the current search EntryItem
 * m_currentSearchPosition is the current position within the current EntryItem
 */
void GraphicsEntry::searchNext() {
  //  QLOG_DEBUG() << Q_FUNC_INFO << m_searchItemPtr << m_searchIndex;
  int pos = m_searchItemIndexes[m_searchItemPtr];
  int findCount = m_items[pos]->findCount();
  m_searchIndex++;
  if (m_searchIndex >= findCount) {
    m_searchItemPtr++;
    m_searchIndex = 0;
  }
  if (m_searchItemPtr >= m_searchItemIndexes.size()) {
    QMessageBox msgBox;
    msgBox.setObjectName("wordnotfound");
    msgBox.setTextFormat(Qt::RichText);
    QString style;
    /// TODO get this from INI
    if (UcdScripts::isScript(m_currentSearchTarget,"Arabic")) {
      style = "font-family : Amiri;font-size : 18pt";
    }
    msgBox.setText(QString(tr("No more occurrences of : <span style=\"%1\">%2</span>")).arg(style).arg(m_currentSearchTarget));
    msgBox.exec();
    emit(searchFinished());
    return;
  }
  int i = m_searchItemIndexes[m_searchItemPtr];
  this->centerOnSearchResult(i,m_searchIndex);
  emit(searchFoundNext());
  return;
}
void GraphicsEntry::centerOnSearchResult(int itemIndex,int ix) {
  int pos = m_items[itemIndex]->showHighlight(ix);
  m_highlightCount++;
  m_currentFind++;
  if (m_currentSearchOptions.showAll()) {
    statusMessage(QString("Find count: %1").arg(m_findCount));
  }
  else {
    statusMessage(QString("Showing %1 of %2").arg(m_currentFind).arg(m_findCount));
  }
  m_items[itemIndex]->ensureVisible();
  if (m_items[itemIndex]->boundingRect().height() > m_view->height()) {
    int charCount = m_items[itemIndex]->document()->characterCount();
    qreal h = m_items[itemIndex]->boundingRect().height();
    qreal dy = (h * (qreal)pos)/(qreal)charCount;
    QPointF p = m_items[itemIndex]->scenePos();
    /*
    qDebug() << m_items[itemIndex]->boundingRect().height() << m_view->height();
    qDebug() << "Character count" << charCount;
    qDebug() << "Position" << pos;
    qDebug() << "Scene pos of item" << p;
    QLOG_DEBUG() << "adjusting pos" << dy;
    */
    p.setY(p.y() + dy);
    m_view->centerOn(p);
  }
}
void GraphicsEntry::showSelections() {
  for(int i=0;i < m_items.size();i++) {
    m_items[i]->showHighlights();
  }
}
int GraphicsEntry::getFindCount() const {
  return m_findCount;
}
/*
int GraphicsEntry::getHighlightCount() const {
  return m_highlightCount;
}
int GraphicsEntry::getCurrentFind() const {
  return m_currentFind;
}
*/
bool GraphicsEntry::hasMoreFinds() const {

  return (m_currentFind < m_findCount);
}
bool GraphicsEntry::hasHighlights() const {
  return (m_highlightCount > 0);
}
QString GraphicsEntry::getOutputFilename(const QString & pdfdir,const QString & method,const QString & node) {
  QString base;
  if (method == "node") {
    if (!node.isEmpty()) {
      base = node;
    }
    else {
      for(int i=0;(i < m_items.size()) && base.isEmpty();i++) {
        base = m_items[i]->getNode();
      }
    }
  }
  else if (method == "arabic") {
    base = m_place.getRoot();
  }
  else {
    base = QDateTime::currentDateTime().toString(Qt::ISODate);
  }
  if (base.isEmpty()) {
    base = QDateTime::currentDateTime().toString(Qt::ISODate);
  }
  QFileInfo fi(QDir(pdfdir),QString("%1.pdf").arg(base));
  return fi.absoluteFilePath();
}
#define ROW(a,b)    "<tr><td width=\"30%\">" + QString(a) + "</td><td width=\"%70\">" + QString(b) + "</td></tr>"

QString GraphicsEntry::getPageInfo(bool summary) {
  Place firstPlace;
  QString firstNode;
  for(int i=0;(i < m_items.size()) && ! firstPlace.isValid();i++) {
    firstPlace = m_items[i]->getPlace();
  }
  for(int i=0;(i < m_items.size()) && firstNode.isEmpty();i++) {
    firstNode = m_items[i]->getPlace().getNode();
  }
  QString html;
  html += "<table class=\"pageinfo\" style=\"width:100%\">";
  html += ROW("<b>" + tr("Entry information") + "</b>","");
  html += ROW("","");

  html += ROW(tr("Root"),QString("<span class=\"arabic\">%1</span>").arg(firstPlace.getRoot()));
  html += ROW(tr("Volume"),QString("%1").arg(firstPlace.getVol()));
  html += ROW(tr("First page"),QString("%1").arg(firstPlace.getPage()));

  Place lastPlace;
  for(int i=m_items.size() - 1;(i >= 0) && ! lastPlace.isValid();i--) {
    lastPlace = m_items[i]->getPlace();
  }

  html += ROW(tr("Last page"),QString("%1").arg(lastPlace.getPage()));
  html += ROW(tr("Nodes"),QString("%1 - %2").arg(firstNode).arg(lastPlace.getNode()));
  html += ROW(tr("Date"),QDateTime::currentDateTime().toString(Qt::ISODate));
  QSqlQuery query;
  if (query.prepare("select createversion,createdate,xmlversion,dbid from lexicon")) {
    query.exec();
    if (query.first()) {
      html += ROW("","");
      html += ROW("<b>" + tr("System information") + "</b>","");
      html += ROW("","");
      html += ROW(tr("Creation date"),query.value("createdate").toString());
      html += ROW(tr("Script version"),query.value("createversion").toString());
      html += ROW(tr("Xml version"),query.value("xmlversion").toString());
      html += ROW(tr("Dbid"),query.value("dbid").toString());
    }
  }

  Place p;
  if (! summary ) {
    html += ROW("","");
    html += ROW("<b>" + tr("Node details") + "</b>","");
    html += ROW("","");
    for(int i=1; i < m_items.size();i++) {
      p = m_items[i]->getPlace();
      html += ROW(p.getNode(),QString("<span class=\"arabic\">%1</span>").arg(p.getWord()));
    }

  }
  html += "</table>";
  return html;
}
void GraphicsEntry::print(QPrinter & printer,const QString & node) {
  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());
  settings->beginGroup("Printer");
  bool pdfoutput = settings->value(SID_PRINTER_OUTPUT_PDF).toBool();

  if (pdfoutput) {
    QString pdfdir = settings->value(SID_PRINTER_PDF_DIRECTORY).toString();
    QString filename = getOutputFilename(pdfdir,settings->value(SID_PRINTER_AUTONAME_METHOD).toString(),node);
    printer.setOutputFileName(filename);
  }
  QString html; // = "<html><body>";
  for(int i=0;i < m_items.size();i++) {
    if (node.isEmpty() || (node == m_items[i]->getPlace().getNode())) {
      /// without the trimmed() the printing of the root causes problems
      QString t = m_items[i]->getOutputHtml().trimmed();
      t.remove("<html><body>");
      t.remove("</body></html>");
      html += t;
    }
  }

  QTextDocument doc;
  doc.setDefaultStyleSheet(m_printCss);
  doc.setHtml(html); // + "</body></html>");
  doc.setDefaultTextOption(m_textOption);
  if (1) {
    QString note;
    for(int i=0;i < m_items.size();i++) {
      if (node.isEmpty() || (node == m_items[i]->getPlace().getNode())) {
        QList<Note *> notes = m_items[i]->getNotes();
        for(int j=0;j < notes.size();j++) {
          note += QString("<div class=\"arabic\">%1</div>").arg(notes[j]->getWord());
          note += QString("<div class=\"note\">%1</div>").arg(notes[j]->getNote());
          note += "<br/>";
        }
      }
    }
    if (! note.isEmpty()) {
      QTextCursor cursor(&doc);
      cursor.movePosition(QTextCursor::End);
      QTextBlockFormat format;
      format.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysBefore);
      cursor.insertBlock(format);
      cursor.insertHtml(note);
    }
  }
  /// TODO this needs to be dependant on something
  if (1) {
    QTextCursor cursor(&doc);
    cursor.movePosition(QTextCursor::End);
    QTextBlockFormat format;
    format.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysBefore);
    cursor.insertBlock(format);
    //    html += "<hr/>";
    cursor.insertHtml(getPageInfo(false));
  }
  doc.print(&printer);
  if (printer.outputFormat() == QPrinter::NativeFormat) {
    QString name = printer.printerName();
    if (! name.isEmpty()) {
      statusMessage(QString(tr("Document printed to %1")).arg(name));
    }
    else {
      statusMessage(QString(tr("Document printed")));
    }
  }
  else {
    statusMessage(QString(tr("PDF created: %1")).arg(printer.outputFileName()));
  }
}
void GraphicsEntry::onReload() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  /// TODO reload CSS
  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());
  settings->beginGroup("Entry");
  m_debug = settings->value(SID_ENTRY_DEBUG,false).toBool();
  QString css = settings->value(SID_ENTRY_CSS,QString("entry.css")).toString();
  css = readCssFromFile(css);
  if (! css.isEmpty()) {
    m_currentCss = css;
    emit(cssChanged());
  }
  css = settings->value(SID_ENTRY_PRINT_CSS,QString("entry_print.css")).toString();
  css = readCssFromFile(css);
  if (! css.isEmpty()) {
    m_printCss = css;
  }

  m_xsltSource = settings->value(SID_ENTRY_XSLT,QString("entry.xslt")).toString();

  QString html;
  for (int i=0;i < m_items.size();i++) {
    html = transform(ENTRY_XSLT_RECOMPILE,m_xsltSource,m_items[i]->getXml());
    m_items[i]->document()->clear();
    m_items[i]->document()->setDefaultStyleSheet(m_currentCss);
    m_items[i]->setTextWidth(m_textWidth);
    m_items[i]->setHtml(html);
    m_items[i]->setOutputHtml(html);
  }
  statusMessage(tr("Reloaded page"));
}
/**
 * This reload function should be used when dynamic versions of css/xslt have changed.
 * It will not reload from file.
 *
 * Written to be used by dialog that allows user to amend xslt/css online
 *
 * @param css stylesheet to apply. If empty use current CSS
 * @param xslt proc to use. If empty, force recompile, otherwise use supplied xslt
 */
void GraphicsEntry::onReload(const QString & css,const QString & xslt) {
  QLOG_DEBUG() << Q_FUNC_INFO;
  QString html;
  for (int i=0;i < m_items.size();i++) {
    if (xslt.isEmpty()) {
      html = transform(ENTRY_XSLT,m_xsltSource,m_items[i]->getXml());
    }
    else {
      html = transform(TEST_XSLT,xslt,m_items[i]->getXml());
    }
    m_items[i]->document()->clear();
    if (css.isEmpty()) {
      m_items[i]->document()->setDefaultStyleSheet(m_currentCss);
    }
    else {
      m_items[i]->document()->setDefaultStyleSheet(css);
    }
    m_items[i]->setTextWidth(m_textWidth);
    m_items[i]->setHtml(html);
    m_items[i]->setOutputHtml(html);
  }
  statusMessage(tr("Reloaded page"));
}
