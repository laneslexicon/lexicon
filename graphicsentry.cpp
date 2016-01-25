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
#include "searchdialogs.h"
#include "searchoptionswidget.h"
#include "externs.h"
#include "nodeinfo.h"
#include "definedsql.h"
#include "linkcheckdialog.h"
#include "showxmldialog.h"
ToolButtonData::ToolButtonData(int id) : QToolButton() {
  m_id = id;
  setObjectName("toolbuttondata");
}
ToolButtonData::~ToolButtonData() {

}
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
  //  m_scale = 1.5;

  m_scene = new QGraphicsScene(this);
  m_view = new QGraphicsView(m_scene,this);
  m_view->setObjectName("entryview");
  m_view->setFocusPolicy(Qt::StrongFocus);
  m_view->setSceneRect(m_scene->sceneRect());
  m_view->setAlignment(Qt::AlignTop);
  m_highlightCount = 0;
  m_findCount = 0;

  m_view->setInteractive(true);
  /*  TODO what is this for ? Commented out on 16th Jan
      (adding the dummy item)

  m_item = new QGraphicsTextItem("");
  m_item->setTextInteractionFlags(Qt::TextBrowserInteraction);
  m_item->setTextWidth(m_textWidth);
  m_scene->addItem(m_item);
  */
  // add the graphics viwe
  layout->addWidget(m_view);


  setLayout(layout);

  connect(m_scene,SIGNAL(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)),
          this,SLOT(onFocusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)));

  initXslt();
  m_transform = m_view->transform();
}
GraphicsEntry::~GraphicsEntry() {
  QLOG_DEBUG() << Q_FUNC_INFO;

  QList<SearchOptions::SearchScope_t> keys = m_lastSearchOptions.keys();
  for(int i=0;i < keys.size();i++) {
    SearchOptions * opts = m_lastSearchOptions.take(keys[i]);
    if (opts) {
      delete opts;
    }
  }
}
QGraphicsView * GraphicsEntry::getView() const {
  return m_view;
}
void GraphicsEntry::readSettings() {
  QString v;
  QVariant vn;
  bool ok;
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QMap<QString,QString> cmdOptions = app->getOptions();

  SETTINGS


  settings.beginGroup("System");
  // 0 - no link check
  // 1 - show link dialog and do not follow link
  // 2 - show link dialog and then follow link
  m_linkCheckMode = settings.value(SID_SYSTEM_LINK_CHECK_MODE,0).toInt();
  settings.endGroup();

  settings.beginGroup("Entry");
  QString css = settings.value(SID_ENTRY_CSS,QString("entry.css")).toString();
  css = readCssFromFile(css);
  /// if there are errors, readCssFromFile will show them
  if (! css.isEmpty()) {
    m_currentCss = css;
    emit(cssChanged());
  }

  css = settings.value(SID_ENTRY_PRINT_CSS,QString("entry_print.css")).toString();
  css = readCssFromFile(css);
  if (! css.isEmpty()) {
    m_printCss = css;
  }
  m_textWidth = settings.value(SID_ENTRY_TEXT_WIDTH,400).toInt();
  if (m_textWidth == 0) {
    m_textWidth = 400;
  }

  if (cmdOptions.contains("textwidth")) {
    int w = cmdOptions.value("textwidth").toInt(&ok);
    if (ok) {
      m_textWidth = w;
    }
    else {
      m_textWidth = 400;
    }
  }
  m_defaultWidth = m_textWidth;

  m_entryMargin = settings.value(SID_ENTRY_MARGIN,10).toInt();

  m_clearScene = settings.value(SID_ENTRY_CLEAR_SCENE,true).toBool();
  m_offPageMovement = settings.value(SID_ENTRY_OFF_PAGE,false).toBool();

  /// these are set to empty to disable the feature
  m_moveFocusUpKey = settings.value(SID_ENTRY_MOVE_FOCUS_UP,QString()).toString();
  m_moveFocusDownKey = settings.value(SID_ENTRY_MOVE_FOCUS_DOWN,QString()).toString();
  m_moveForwardKey = settings.value(SID_ENTRY_FORWARD,QString()).toString();
  m_moveBackwardKey = settings.value(SID_ENTRY_BACK,QString()).toString();
  m_zoomInKey = settings.value(SID_ENTRY_ZOOM_IN,QString()).toString();
  m_zoomOutKey = settings.value(SID_ENTRY_ZOOM_OUT,QString()).toString();
  m_reloadKey = settings.value(SID_ENTRY_RELOAD,QString()).toString();

  m_widenKey = settings.value(SID_ENTRY_WIDEN,QString()).toString();
  m_narrowKey = settings.value(SID_ENTRY_NARROW,QString()).toString();
  m_widenStep = settings.value(SID_ENTRY_WIDEN_STEP,50).toInt(&ok);
  if ( ! ok ) {
    m_widenStep = 50;
  }
  m_scaleStep = settings.value(SID_ENTRY_SCALE_STEP,.1).toDouble(&ok);
  if (!ok) {
    m_scaleStep = .1;
  }
  m_searchKey = settings.value(SID_ENTRY_FIND,QString()).toString();
  m_searchNextKey = settings.value(SID_ENTRY_FIND_NEXT,QString()).toString();
  m_clearKey = settings.value(SID_ENTRY_CLEAN,QString()).toString();
  m_showKey = settings.value(SID_ENTRY_SHOW,QString()).toString();
  m_homeKey = settings.value(SID_ENTRY_HOME,QString()).toString();
  m_markKey = settings.value(SID_ENTRY_MARK,QString()).toString();
  m_helpKey = settings.value(SID_ENTRY_KEY_HELP,QString()).toString();

  m_dumpXml = settings.value(SID_ENTRY_DUMP_XML,false).toBool();
  m_dumpHtml = settings.value(SID_ENTRY_DUMP_HTML,false).toBool();
  m_dumpOutputHtml = settings.value(SID_ENTRY_DUMP_OUTPUT_HTML,false).toBool();

  m_showLinkWarning = settings.value(SID_ENTRY_SHOW_LINK_WARNING,true).toBool();

  m_scale = settings.value(SID_ENTRY_SCALE,1.0).toDouble();

  m_highlightColorName = settings.value(SID_ENTRY_HIGHLIGHT_COLOR,"yellow").toString();

  m_nodeinfoClose = settings.value(SID_ENTRY_NODEINFO_CLOSE,true).toBool();

  settings.endGroup();

  m_entryXslt = getLexicon()->getXsltFileName();

  settings.beginGroup("Notes");
  m_notesEnabled = settings.value(SID_NOTES_ENABLED,true).toBool();
  settings.endGroup();

  settings.beginGroup("Diacritics");
  QStringList keys = settings.childKeys();
  QStringList points;
  for(int i=0;i < keys.size();i++) {
    if (keys[i].startsWith("Char")) {
      v = settings.value(keys[i],QString()).toString();
      points << v;
    }
  }
  m_pattern = QString("[\\x%1]*").arg(points.join("\\x"));

  settings.endGroup();

  settings.beginGroup("System");
  m_linksUseCurrentTab  = settings.value(SID_SYSTEM_OPEN_LINK,true).toBool();
  m_activateLink = settings.value(SID_SYSTEM_ACTIVATE_LINK,true).toBool();
  settings.endGroup();

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

  if (event->modifiers() && Qt::ControlModifier) {
    //    return;
  }
  QLOG_DEBUG() << Q_FUNC_INFO << event->modifiers() << event->key() << event->text();
  if (! m_reloadKey.isEmpty() && (event->text() == m_reloadKey)) {
    statusMessage(tr("Reload"));
    onReload();
    return;
  }
  if (! m_zoomInKey.isEmpty() && (event->text() == m_zoomInKey)) {
    statusMessage(tr("Zoom in"));
    onZoomIn();
    return;
  }
  if (! m_zoomOutKey.isEmpty() && (event->text() == m_zoomOutKey)) {
    statusMessage(tr("Zoom out"));
    onZoomOut();
    return;
  }
  if (! m_moveFocusDownKey.isEmpty() && (event->text() ==  m_moveFocusDownKey))  {
    statusMessage(tr("Move down"));
    moveFocusDown();
    return;
  }
  if (! m_moveFocusUpKey.isEmpty() && (event->text() ==  m_moveFocusUpKey)) {
    statusMessage(tr("Move up"));
    moveFocusUp();
    return;
  }
  if (! m_moveForwardKey.isEmpty() && (event->text() ==  m_moveForwardKey)) {
    statusMessage(tr("Next root"));
    moveForward();
    return;
  }
  if (! m_moveBackwardKey.isEmpty() && (event->text() ==  m_moveBackwardKey)) {
    statusMessage(tr("Previous root"));
    moveBackward();
    return;
  }
  if (! m_widenKey.isEmpty() && (event->text() ==  m_widenKey)) {
    statusMessage(tr("Widen text"));
    onWiden();
    return;
  }
  if (! m_narrowKey.isEmpty() && (event->text() ==  m_narrowKey)) {
    statusMessage(tr("Narrow text"));
    onNarrow();
    return;
  }
  if (! m_searchKey.isEmpty() && (event->text() == m_searchKey)) {
    statusMessage(tr("Search this page"));
    this->search();
    return;
  }
  if (! m_searchNextKey.isEmpty() && (event->text() == m_searchNextKey)) {
    statusMessage(tr("Find next on this page"));
    this->searchNext();
    return;
  }
  if (! m_clearKey.isEmpty() && (event->text() == m_clearKey)) {
    statusMessage(tr("Clear highlights"));
    this->clearHighlights();
    return;
  }
  if (! m_showKey.isEmpty() && (event->text() == m_showKey)) {
    statusMessage(tr("Show last search results"));
    this->showSelections();
    return;
  }
  if (! m_markKey.isEmpty() && (event->text() == m_markKey)) {
    statusMessage(tr("Mark"));
    m_focusNode = m_place.getNode();
    return;
  }
  if (! m_homeKey.isEmpty() && (event->text() == m_homeKey)) {
    statusMessage(tr("Go to to mark"));
    this->home();
    return;
  }
  if (! m_helpKey.isEmpty() && (event->text() == m_helpKey)) {
    statusMessage(tr("Key help"));
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
    for(int i=0;i <= m ;i++) {
      if (m_items[i] == item) {
        if (i < m) {
          setCurrentItem(m_items[i+1]);
          return;
        }
        else {
          if (m_offPageMovement) {
            moveForward();
          }
        }
      }
    }
  }
}
void GraphicsEntry::moveFocusUp() {
  EntryItem * item = dynamic_cast<EntryItem *>(m_scene->focusItem());
  if (item) {
    int m = m_items.size();
    for(int i=0;i < m ;i++) {
      if (m_items[i] == item) {
        if (i > 0) {
          setCurrentItem(m_items[i-1]);
          return;
        }
        else {
          if (m_offPageMovement) {
            emit(prevHead(m_items[0]->getPlace()));
          }
          return;
        }
      }
    }
  }
}
void GraphicsEntry::focusLast() {
  if (m_items.size() > 0) {
    setCurrentItem(m_items[m_items.size() - 1]);
  }

}
/**
 * If no index is given:
 * Returns:
 *    either the Place of the current scene focus item
 *    the Place of home node
 *    the place for the current page
 * @return
 */
Place GraphicsEntry::getPlace(int index) const {
  QLOG_DEBUG() << Q_FUNC_INFO << index;
  if ((index >= 0) && (index < m_items.size())) {
    QLOG_DEBUG() << "get_place_1";
      return m_items[index]->getPlace();
  }
  Place p;
  EntryItem * item = dynamic_cast<EntryItem *>(m_scene->focusItem());
  if (item) {
      p = item->getPlace();
      if (p.isValid()) {
        //        QLOG_DEBUG() << "get_place_4";
        return p;
      }
  }
  /// TODO this is not always updated - it may be outdated
  if (! m_focusNode.isEmpty()) {
    QSqlQuery sql;
    if (sql.prepare(SQL_ENTRY_FOR_NODE)) {
      sql.bindValue(0,m_focusNode);
      if (sql.exec()  && sql.first()) {
        //        QLOG_DEBUG() << "get_place_2";
        return Place::fromEntryRecord(sql.record());
      }
    }
    //    QLOG_DEBUG() << "get_place_3";
    return Place::fromNode(m_focusNode);
  }
  if (m_focusPlace.isValid()) {
    //    QLOG_DEBUG() << "get_place_5";
    return m_focusPlace;
  }
  if (m_items.size() > 1) {
    //    QLOG_DEBUG() << "get_place_6";
    return m_items[1]->getPlace();
  }
  //  QLOG_DEBUG() << "get_place_7";
  return m_focusPlace;
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
  QLOG_DEBUG() << Q_FUNC_INFO << link.toDisplayString();;
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
  bool createTab =  m_linksUseCurrentTab;
  bool activateTab = m_activateLink;

  ///
  ///  shift or ctrl, toggles create tab behaviour
  ///
  Place p;
  if (QApplication::keyboardModifiers() & (Qt::ShiftModifier | Qt::ControlModifier)) {
    createTab = ! createTab;
  }

  QUrl url(link);
  QLOG_DEBUG() << Q_FUNC_INFO << url;
  if (url.hasQuery()) {
    QString msg = url.query();
    if (msg.startsWith("text=")) {
      msg.remove("text=");
      msg = getLexicon()->scanAndStyle(msg,"messagebox");
      QMessageBox mbox;
      mbox.setWindowTitle(tr("Text Note"));
      mbox.setTextFormat(Qt::RichText);
      mbox.setText("<html><head/><body><p>" + msg + "</p></body></html>");

      mbox.setIcon(QMessageBox::Information);
      mbox.exec();
      return;
    }
    if (msg.startsWith("golink")) {
      msg.remove("golink=");
      if (m_linkCheckMode > 0) {
        this->checkLink(msg);
        if (m_linkCheckMode == 1)
          return;
      }
      QSqlQuery query;
      query.prepare(SQL_LINKTO_NODE);
      query.bindValue(0,msg);
      query.exec();
      if (query.first()) {
        p.setAction(Place::Link);
        p.setNode(query.value(0).toString());
        if (this->hasNode(p.node())) {
          this->focusNode(p.node());
          return;
        }
        emit(gotoNode(p,createTab,activateTab));
      }
      else {
        QLOG_WARN() << QString(tr("Missing link record for linkid %1")).arg(msg);
      }
      return;
    }
    if (msg.startsWith("nolink=")) {
      msg.remove("nolink=");
      if (m_linkCheckMode > 0) {
        this->checkLink(msg);
        if (m_linkCheckMode == 1)
          return;
      }
      QString id = msg;
      msg = QString(tr("The target for this cross-reference has not been set.\nDetails of how to set it can be found in the documentation."));
      QMessageBox::information(this, QString(tr("No target for link %1")).arg(id),
                               msg,
                               QMessageBox::Ok);
      return;
    }
    if (msg.startsWith("root=")) {
      msg.remove("root=");
      QSqlQuery query;
      query.prepare(SQL_LINK_ROOT_FIND);
      query.bindValue(0,msg);
      query.exec();
      if (query.first()) {
        p.setAction(Place::Link);
        p.setRoot(query.value(0).toString());
        /// TODO fix the boolean parameters
        emit(gotoNode(p,createTab,activateTab));
        return;
      }
      else {
        QLOG_WARN() << QString(tr("Error in Jump to root requested :%1, error %2")).arg(msg).arg(query.lastError().text());
      }
    }
  }
  else {
    /// TODO check this is no longer used
    QLOG_DEBUG() << Q_FUNC_INFO << __LINE__ << "SHOULD NOT BE HERE";
    QString node(link);
    /// remove the leading #
    node.remove(0,1);
    p.setAction(Place::Link);
    p.setNode(node);
    /// TODO replace this
    /// including move to new tab stuff
    /// showPlace(p,createTab,activateTab);
  }
}
void GraphicsEntry::linkHovered(const QString & link) {
  //  QGraphicsTextItem * gi = static_cast<QGraphicsTextItem *>(QObject::sender());
  EntryItem * gi = static_cast<EntryItem *>(QObject::sender());

  if (!gi) {
    return;
  }
  if (link.isEmpty()) {
    gi->setCursor(QCursor(Qt::ArrowCursor));
    gi->setToolTip(gi->getPlace().getText());
    return;
  }
  // "127.0.0.0?golink=n1234-4"
  // "127.0.0.0?nolink=n1234-4"
  QRegularExpression rx("(.+)\?(no|go)link=(.+)");
  QRegularExpressionMatch m = rx.match(link);
  if (! m.hasMatch()) {
    QLOG_INFO() << QString(tr("Invalid cross-reference:%1")).arg(link);
    return;
  }
  QString action = m.captured(2);
  QString orthid = m.captured(3);

  if (action == "no") {
    gi->setToolTip(QString(tr("The target for this cross-reference (id:%1) has not been set.")).arg(orthid));
    gi->setCursor(QCursor(Qt::ArrowCursor));
    return;
  }

  QSqlRecord rec = this->findLinkRecord(orthid);
  if (! rec.isEmpty()) {
    Place n = Place::fromEntryRecord(rec);
    gi->setCursor(QCursor(Qt::PointingHandCursor));
    QString t = QString("Cross ref to: root %1, headword : %2\nVolume %3, page %4\nXref id : %5")
      .arg(n.getRoot())
      .arg(n.getWord())
      .arg(n.getVol())
      .arg(n.getPage())
      .arg(orthid);

    gi->setToolTip(t);
  }
  else {
    QLOG_INFO() << QString(tr("Cannot find cross-reference information for %1")).arg(orthid);
  }

}
/**
 * Called from context menu entry "What's This" created by EntryItem
 *
 * @param link
 */
void GraphicsEntry::showLinkDetails(const QString  & link) {
  QLOG_DEBUG() << Q_FUNC_INFO << link;
  Place p;
  QString node;
  QString t(link);
  if (link.startsWith("nolink")) {
    /// TODO link fixup routine
    return;
  }
  if (! link.startsWith("golink")) {
    return;
  }
  t.remove("golink=");
  QSqlQuery query;
  query.prepare(SQL_LINKTO_NODE);
  query.bindValue(0,t);
  query.exec();
  if (query.first()) {
    node = query.value(0).toString();
  }
  else {
    QLOG_WARN() << QString("Missing link record for linkid %1").arg(t);
    return;
  }
  QSqlQuery nodeQuery;
  bool ok = nodeQuery.prepare(SQL_FIND_ENTRY_BY_NODE);
  if (! ok ) {
    QLOG_WARN() << QString(tr("Node SQL prepare failed for target:%1, error %2")).arg(link).arg(nodeQuery.lastError().text());
    return;
  }

  nodeQuery.bindValue(0,node);
  nodeQuery.exec();
  if (nodeQuery.first()) {
    p = Place::fromEntryRecord(nodeQuery.record());
    //    p.setRoot(nodeQuery.value("root").toString());
    //    p.setSupplement(nodeQuery.value("supplement").toInt());
    //    p.setWord(nodeQuery.value("word").toString());
    //    p.setPage(nodeQuery.value("page").toInt());
    QString html =    transform(NODE_XSLT,m_entryXslt,nodeQuery.value("xml").toString());
    NodeInfo * info = new NodeInfo(this);
    info->setWindowTitle(tr("Link target"));
    info->setPlace(p);
    info->setCss(m_currentCss);
    info->setHtml(html);
    info->setCloseOnLoad(m_nodeinfoClose);
    connect(info,SIGNAL(openNode(const QString &)),this,SIGNAL(showNode(const QString &)));
    int ret = info->exec();
    delete info;
    //    if (ret == QDialog::Accepted) {
    //      emit(gotoNode(Place::fromNode(node),true,true));
    //    }
  }
  else {
    QLOG_WARN() << QString(tr("Requested link node not found : %1")).arg(node);
    return;
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
/**
 *
 *
 * @param p
 * @param thisPageOnly
 * @param createTab
 * @param activateTab
 *
 * @return
 */
/*
Place GraphicsEntry::showPlace(const Place & p,bool thisPageOnly,bool createTab,bool activateTab) {
    QLOG_DEBUG() << Q_FUNC_INFO << __LINE__ << "SHOULD NOT BE HERE";
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
  if (! thisPageOnly ) {
    /// TODO Fix this
    emit(gotoNode(p,createTab,activateTab));
  }
  return p;
}
*/
/**
 *
 *
 */
QString GraphicsEntry::readCssFromFile(const QString & name) {
  QString css;
  if (name.isEmpty()) {
    return css;
  }
  QString filename = getLexicon()->getResourceFilePath(Lexicon::Stylesheet,name);
  if (filename.startsWith("Error:")) {
    QStringList errors = filename.split(":");
    QString msg;
    if (errors.size() >= 2) {

      msg = QString(tr("<p>Cannot find file: %1</p> \
                        <p>Directory is:%2</p> \
                        <p>Please review Preferences -> Layout</p>")).arg(errors[2]).arg(errors[1]);
    }
    else {
      msg = QString(tr("Cannot find file: %1")).arg(name);
    }
    QMessageBox::warning(this,tr("Missing Entry Stylesheet"),msg,QMessageBox::Ok);
    QLOG_WARN() << QString(tr("Missing Entry CSS  file: %1")).arg(name);
    return css;
  }

  QFile f(filename);
  if (! f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QLOG_WARN()  << QString(tr("I/O Error opening CSS file for reading: %1 - %2"))
      .arg(filename)
      .arg(f.errorString());
    return css;
  }
  QTextStream in(&f);
  css = in.readAll();
  f.close();
  return css;
}

void GraphicsEntry::dumpInfo(EntryItem * item , const QString & node) {
  SETTINGS

  settings.beginGroup("Entry");
  QString p = settings.value(SID_ENTRY_OUTPUT_PATH,QDir::tempPath()).toString();

  QFileInfo d(p);
  if (! d.isDir()) {
    p = QDir::tempPath();
  }
  QString prefix;
  if (item->isRoot()) {
    prefix = "root-";
  }
  if (m_dumpXml) {
    QFileInfo fi(p,QString("%1%2.xml").arg(prefix).arg(node));
    QFile f(fi.filePath());
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QTextStream out(&f);
      out.setCodec("UTF-8");
      out << item->getXml();
    }
  }
  if (m_dumpOutputHtml) {
    QFileInfo fi(p,QString("%1%2-in.html").arg(prefix).arg(node));
        QFile f(fi.filePath());
        if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
          QTextStream out(&f);
          out.setCodec("UTF-8");
          out << item->getOutputHtml();
        }
  }
  if (m_dumpHtml) {
    QFileInfo fi(p,QString("%1%2-qt.html").arg(prefix).arg(node));
        QFile f(fi.filePath());
        if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
          QTextStream out(&f);
          out.setCodec("UTF-8");
          out << item->toHtml();
        }
  }
}
void GraphicsEntry::setItemPlace(EntryItem * item,const QSqlQuery & query) {
  Place p;

  p.setSupplement(query.value("supplement").toInt());
  p.setNode(query.value(7).toString());
  p.setRoot(query.value(0).toString());
  p.setWord(query.value(2).toString());
  p.setPage(query.value(5).toInt());
  p.setHead(query.value(9).toString());
  item->setPlace(p);
}
/**
 * TODO if this is called as part of search for node it does not return
 *      the Place that matches the node, but the root
 * @param root
 * @param node the id of the entry we want to focus on
 * returns the Place of the item that is centered on; if a particular node
 * was requested then that place is returned, otherwise the root place
 *
 * set the focusNode used by "home" function
 */
Place GraphicsEntry::getXmlForRoot(const Place & dp) {
  QList<EntryItem *> items;
  Place noplace;
  int supplement;
  bool ok = false;
  SETTINGS
  settings.beginGroup("Entry");
  m_scale  = settings.value(SID_ENTRY_SCALE,1.0).toDouble();
  if (m_scale == 0) {
    m_scale = 1.0;
  }

  EntryItem * centerItem;

  QString root = dp.getRoot();
  QString node = dp.getNode();
  Place p = dp;
  /**
   * if we don't have the root, but have the node, find the root
   *
   */
  QSqlQuery nodeQuery;
  ok = nodeQuery.prepare(SQL_FIND_ENTRY_BY_NODE);
  if (! ok ) {
    QLOG_WARN() << "node SQL prepare failed" << nodeQuery.lastError();
  }

  if (root.isEmpty() && ! node.isEmpty() ) {
    nodeQuery.bindValue(0,node);
    nodeQuery.exec();
    if (nodeQuery.first()) {
      root = nodeQuery.value("root").toString();
      supplement = nodeQuery.value("supplement").toInt();
      p.setNode(node);
      p.setRoot(root);
      p.setWord(nodeQuery.value("word").toString());
      p.setSupplement(supplement);
    }
    else {
      QLOG_WARN() << QString(tr("Requested node not found : %1")).arg(node);
      return noplace;
    }
  }
  QSqlQuery rootQuery;
  rootQuery.prepare(SQL_FIND_ENTRY_FOR_ROOT);

  int quasi = 0;
  QSqlQuery quasiQuery;
  if (quasiQuery.prepare(SQL_QUASI_FIND_BY_WORD)) {
    quasiQuery.bindValue(0,root);
    quasiQuery.exec();
    if (quasiQuery.first()) {
      quasi = quasiQuery.value(0).toInt();
    }
  }
  else {
    QLOG_WARN() << QString(tr("Error preparing quasi query:%1")).arg(quasiQuery.lastError().text());
  }
  rootQuery.bindValue(0,root);
  rootQuery.exec();
  if (! rootQuery.first()) {
    QLOG_WARN() << QString(tr("Root not found %1")).arg(root);
    return noplace;
  }


  QString str = QString("<word type=\"root\" ar=\"%1\" quasi=\"%2\"></word>").arg(root).arg(quasi);

  EntryItem * rootItem  = createEntry(str);
  /// will be null if the XSLT/XML has not parsed correctly
  if (rootItem == NULL) {
    QLOG_WARN() << QString("Error build root item for:[%1]").arg(str);
    return p;
  }
  Place rootPlace;
  rootPlace.setRoot(root);
  rootPlace.setSupplement(rootQuery.value(8).toInt());
  rootPlace.setPage(rootQuery.value(5).toInt());

  rootItem->setPlace(rootPlace);
  dumpInfo(rootItem,rootQuery.value(7).toString());
  ///
  /// this is always set. Originally planned to allow build ever longer pages
  ///
  if (m_clearScene) {
    onClearScene();
  }
  m_userTitle.clear();
  items << rootItem;

  /// by default we will center on the root item
  centerItem = rootItem;

  /// now add all the entries for the root
  do  {
    //    supplement = rootQuery.value(8).toInt();
    QString t  = QString("<word buck=\"%1\" ar=\"%2\" page=\"%3\" itype=\"%4\" supp=\"%5\">")
      .arg(rootQuery.value("bword").toString())
      .arg(rootQuery.value("word").toString())
      .arg(rootQuery.value("page").toInt())
      .arg(rootQuery.value("itype").toString())
      .arg(rootQuery.value("supplement").toInt());
    t += rootQuery.value("xml").toString();
    t += "</word>";
    EntryItem * item  = createEntry(t);
    if (item != NULL) {
      setItemPlace(item,rootQuery);
      dumpInfo(item,rootQuery.value("nodeid").toString());
      item->setNotes();
      items << item;
      /// if we asked for a specific word/node, focus on it
      if (! node.isEmpty() && (item->getNode() == node)) {
        centerItem = item;
        /// this is used by the 'home' key to reset focus
        m_focusNode = node;
      }
    }
    else {
      QLOG_DEBUG() << "Failed to create entry for:" << t;
    }
  } while(rootQuery.next());

  /**
   * Now that we have all the entries, add them to the list and
   * and calculate their positions
   *
   * NOTE: paging direction is now always forward. This code was written
   *       to allow for a single page/tab to have more than one root + entries
   *       with entries either being prepended or append depending on whether the
   *       the user is going forward or backward.
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

  this->setScale(m_scale);

  /// without this call to setSceneRect centerOn() does not work properly for
  /// items added to the scene
  ///
  m_view->setSceneRect(m_scene->sceneRect());
  if (centerItem) {
    this->setCurrentItem(centerItem);
  }

  m_place = centerItem->getPlace();
  m_place.setAction(dp.getAction());
  if (m_place.isValid()) {
    if (m_place.getAction() == Place::History) {
      emit(historyPositionChanged(dp.getId()));
    }
    else if (m_place.getAction() != Place::Bookmark) {
      if (getHistory()->enabled()) {
        if (getHistory()->add(m_place)) {
        /// this allows mainwindow to update the history list
          emit(historyAddition(m_place));
        }
      }
    }
  }


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
  return m_place;
}
Place GraphicsEntry::getPage(const Place & p) {
  QList<EntryItem *> items;
  EntryItem * item;

  EntryItem * focusItem;
  int page = p.getPage();

  QSqlQuery pageQuery;
  bool ok = pageQuery.prepare(SQL_ROOT_FOR_PAGE);
  if (! ok ) {
    QLOG_WARN() << "page SQL prepare failed" << pageQuery.lastError();
  }

  pageQuery.bindValue(0,page);
  pageQuery.exec();
  if (! pageQuery.first()) {
    QLOG_INFO() << "Page not found" << page;
    return p;
  }
  QString node = pageQuery.value("nodeid").toString();


  if (m_clearScene) {
    onClearScene();
  }
  SETTINGS
  settings.beginGroup("Entry");
  m_scale  = settings.value(SID_ENTRY_SCALE,1.0).toDouble();
  if (m_scale == 0) {
    m_scale = 1.0;
  }

  QString lastRoot;

  int rootCount = 0;
  int entryCount = 0;
  do   {
    qStrip << Q_FUNC_INFO << pageQuery.value("nodeid").toString();
    int supplement = pageQuery.value(8).toInt();
    QString root = pageQuery.value(0).toString();
    /// if root has changed add root XML
    if (root != lastRoot) {
      if (! lastRoot.isEmpty()) {
        QSqlQuery quasiQuery;
        int quasi = 0;
        if (quasiQuery.prepare(SQL_QUASI_FIND_BY_WORD)) {
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
      .arg(pageQuery.value(3).toString())
      .arg(pageQuery.value(2).toString())
      .arg(pageQuery.value(5).toInt())
      .arg(pageQuery.value(6).toString())
      .arg(pageQuery.value(8).toInt());
    t += pageQuery.value(4).toString();
    t += "</word>";
    if (m_dumpXml) {
      QFileInfo fi(QDir::tempPath(),QString("%1.xml").arg(pageQuery.value(7).toString()));
      QFile f(fi.filePath());
      if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&f);
        out.setCodec("UTF-8");
        out << t;
      }
    }
    item  = createEntry(t);
    if (item != NULL) {
      if (m_dumpOutputHtml) {
        QFileInfo fi(QDir::tempPath(),QString("%1-out.html").arg(pageQuery.value(7).toString()));
        QFile f(fi.filePath());
        if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
          QTextStream out(&f);
          out.setCodec("UTF-8");
          out << item->getOutputHtml();
        }
      }
      Place p;
      p.setSupplement(supplement);
      p.setNode(pageQuery.value(7).toString());
      p.setRoot(root);
      p.setWord(pageQuery.value(2).toString());
      p.setPage(pageQuery.value(5).toInt());
      item->setPlace(p);
      item->setNotes();//getApp()->notes()->find(m_rootQuery->value(2).toString()));
      items << item;

      entryCount++;
    }
  } while(pageQuery.next());
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
    transform(ENTRY_XSLT,m_entryXslt,xml);
  if (html.isEmpty()) {
    return NULL;
  }
  EntryItem * gi = new EntryItem("");
  if (! m_currentCss.isEmpty()) {
    gi->document()->setDefaultStyleSheet(m_currentCss);
  }
  gi->setTextWidth(m_textWidth);
  gi->setHtml(html);
  gi->setOutputHtml(html);
  gi->setXml(xml);
  /// need this otherwise arabic text will be right justified
  gi->document()->setDefaultTextOption(m_textOption);
  gi->setTextInteractionFlags(Qt::TextBrowserInteraction);
  gi->setAcceptHoverEvents(true);
  gi->setNotesEnabled(m_notesEnabled);
  gi->setHighlightColor(m_highlightColorName);
  //    if (m_dumpOutputHtml) {
  //      gi->setOutputHtml(html);
  //    }
  connect(gi,SIGNAL(linkActivated(const QString &)),this,SLOT(linkActivated(const QString &)));
  connect(gi,SIGNAL(linkHovered(const QString &)),this,SLOT(linkHovered(const QString &)));
  connect(gi,SIGNAL(showXml(const Place &)),this,SLOT(showPerseus(const Place &)));
  connect(gi,SIGNAL(showHtml()),this,SLOT(showHtml()));
  connect(gi,SIGNAL(searchPage()),this,SIGNAL(searchPage()));
  connect(gi,SIGNAL(fixLink(const QStringList &,bool)),this,SLOT(fixLink(const QStringList &,bool)));
  /// placeChanged is emitted whenever an EntryItem gets a focusInEvent
  connect(gi,SIGNAL(placeChanged(const Place &)),this,SLOT(updateCurrentPlace(const Place &)));

  connect(gi,SIGNAL(selectAllItems()),this,SLOT(selectAll()));
  connect(gi,SIGNAL(clearAllItems()),this,SLOT(clearAll()));
  connect(gi,SIGNAL(gotoNode(const Place &,bool,bool)),this,SIGNAL(gotoNode(const Place &,bool,bool)));
  connect(gi,SIGNAL(showLinkDetails(const QString &)),this,SLOT(showLinkDetails(const QString &)));
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
    sz = m_items[i]->document()->size();

    if (m_items[i]->hasNotes()) {
      qreal btnx;
      qreal btny;
      btnx = xpos + m_items[i]->boundingRect().width();
      btny = ypos;// + sz.height();

      ToolButtonData  * notesBtn = new ToolButtonData(i);
      notesBtn->setIcon(QIcon(QPixmap(":/qrc/notes.png")));//m_notesIcon));
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
      if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
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
      notesBtn->setIcon(QIcon(QPixmap(":/qrc/notes.png")));//m_notesIcon));
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
 * This is horrible. But:
 * (1) We are using p { line-height : 45px } to specify line-height
 * (2) when setHtml(html) is called, our attributes are merged with ones used internally
 *     by Qt and placed in <p style="xxxxxx line-height:45px">
 * (3) Our css is replaced by p { some attrubutes used by qt}
 *     and our line height spec is removed the css specification
 * (4) If the html contains a block element (eg. <table>) then after this tag is closed
 *      a  <p> tag will be inserted when it finds text outside of tags.
 *
 *      You get this:
 *
 *      Some text                        <p style="line-height:45px">Some text
 *      <table>                          <table>
 *      </table>                         </table>
 *
 *      more text                        <p>more text</p>
 *                                       </p>
 *
 *
 *  (5) the line-height is lost.
 * @return
 */
QString GraphicsEntry::fixHtml(const QString & t) {
  QString html = t;


  /*
  QRegularExpression rxStart("<!--insert_start_(\\w+)-->");
  QRegularExpressionMatch m = rxStart.match(html);
  if (m.hasMatch()) {
    html.replace(m.captured(0),QString("<%1>").arg(m.captured(1)));
  }
  QRegularExpression rxEnd("<!--insert_end_(\\w+)-->");
  m = rxEnd.match(html);
  if (m.hasMatch()) {
    html.replace(m.captured(0),QString("</%1>").arg(m.captured(1)));
  }
  html = html.remove("\n");
  */
  QRegularExpression rxInsert("<!--insert{([^}]+)}-->",QRegularExpression::MultilineOption);
  QRegularExpressionMatchIterator iter = rxInsert.globalMatch(html);
  while(iter.hasNext()) {
    QRegularExpressionMatch m = iter.next();
    html.replace(m.captured(0),m.captured(1));
  }
  return html;
}

/**
 *
 *
 * @param type
 * @param xsl
 * @param xml
 *
 * @return
 */
QString GraphicsEntry::transform(int type,const QString & xsl,const QString & xml) {
  int ok;
  if (xsl.isEmpty()) {
    QLOG_WARN() << tr("No XSLT supplied");
    return QString();
  }
  if (type == ENTRY_XSLT && ! QFileInfo::exists(xsl)) {
    QLOG_WARN() << QString(tr("Cannot find XSLT file: %1")).arg(QDir::current().relativeFilePath(xsl));
    return QString();
  }

  ok = compileStylesheet(type,xsl);
  if (ok == 0) {
    QString html = xsltTransform(type,xml);
    if (! html.isEmpty()) {

      return fixHtml(html);
    }
    else {
      QLOG_WARN() << "Transform returned no HTML";
      return QString();
    }
  }

  QStringList errors = getParseErrors();
  /// TODO fix this
  errors.prepend("Errors when processing entry styesheet:");
  QMessageBox msgBox;
  msgBox.setText(errors.join("\n"));
  msgBox.exec();
  clearParseErrors();
  return QString();
}
/**
 * This is called when the user is setting the default scale
 *
 * @param v
 */
void GraphicsEntry::onZoom(double v) {
  m_view->setTransform(m_transform);
  m_scale = v;
  m_view->scale(m_scale,m_scale);
  return;
}
qreal GraphicsEntry::onZoomIn() {
  m_view->setTransform(m_transform);
  m_scale += m_scaleStep;
  m_view->scale(m_scale,m_scale);
  return m_scale;
}
qreal GraphicsEntry::onZoomOut() {
  m_view->setTransform(m_transform);
  /// TODO shouldn't this be from scaleStep
  m_scale -= m_scaleStep;
  m_view->scale(m_scale,m_scale);
  return m_scale;
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
    switch(type) {
    case GraphicsEntry::NodeSearch:
      if (m_items[i]->getNode() == p.getNode()) {
        ix = i;
        i = max;
      }
      break;
    case GraphicsEntry::RootSearch:
      if (m_items[i]->isRoot() &&
          ((m_items[i]->getRoot() == root) &&
           (m_items[i]->getSupplement() == supp))) {
        ix = i;
        i = max;
      }
      break;
    case GraphicsEntry::PageSearch:
      if (m_items[i]->getPage() == p.getPage()) {
        ix = i;
        i = max;
      }
      break;
    case GraphicsEntry::WordSearch:
      /// TODO word search
      break;

    default :
      QLOG_WARN() << QString("Unknown search type %1 for place %2").arg(type).arg(p.toString());
      break;
    }
  }
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
  QString xml;
  QString node;
  EntryItem * item = qobject_cast<EntryItem *>(sender());
  if (item) {
    //    xml = item->getXml();
  }
  if (xml.isEmpty()) {
    node = p.getNode();
    if ( node.isEmpty()) {
      QMessageBox msgBox;
      msgBox.setText(tr("No XML found"));
      msgBox.exec();
      return;
    }
    QSqlQuery nodeQuery;
    bool ok = nodeQuery.prepare(SQL_FIND_ENTRY_BY_NODE);
    if (! ok ) {
      QLOG_WARN() << "node SQL prepare failed" << nodeQuery.lastError();
    }

    nodeQuery.bindValue(0,node);
    nodeQuery.exec();

    if (nodeQuery.first()) {
      xml = nodeQuery.value("xml").toString();
    }
    else {
      xml = "No XML for " + node;
    }
  }
  ShowXmlDialog d(node);
  d.setXml(xml);
  d.exec();
}
void GraphicsEntry::showHtml() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  EntryItem * item = qobject_cast<EntryItem *>(QObject::sender());
  if (! item )
    return;


  QString xml = item->getXml();
  QString html = transform(ENTRY_XSLT,m_entryXslt,xml);

  ShowXmlDialog d;
  d.setXml(html);
  d.exec();
}
void GraphicsEntry::updateCurrentPlace(const Place &  p ) {
  if (p.node() != m_place.node()) {
    m_place = p;
    emit(placeChanged(p));
  }
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
  QRegularExpression rx("[\u2000-\u202e]");
  txt.remove(rx);
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
  SETTINGS

  settings.beginGroup("Entry");
  int step = settings.value(SID_ENTRY_WIDEN_STEP,50).toInt();

  this->setTextWidth(m_textWidth + step,true);
  /*
  QLOG_DEBUG() << Q_FUNC_INFO;
  QGraphicsItem * item = m_scene->focusItem();

  for(int i=0;i < m_items.size();i++) {
    if (m_items[i]->hasFocus()) {
      qDebug() << Q_FUNC_INFO << "item" << i << "has focus";
    }
  }
  m_textWidth += 50;
  for(int i=0;i < m_items.size();i++) {
    m_items[i]->setTextWidth(m_textWidth);
  }
  reposition();
  m_view->setSceneRect(m_scene->sceneRect());

  if ( ! item ) {

  }
  if (item) {
    m_view->centerOn(item);//x,pos.y());
    item->setFocus();
  }
  */
}
void GraphicsEntry::onNarrow() {
  SETTINGS

  settings.beginGroup("Entry");
  int step = settings.value(SID_ENTRY_WIDEN_STEP,50).toInt();

  this->setTextWidth(m_textWidth - step,true);
}
void GraphicsEntry::setTextWidth(int w,bool apply) {
  QLOG_DEBUG() << Q_FUNC_INFO << w << apply;
  m_textWidth = w;
  if (! apply ) {
    return;
  }
 QGraphicsItem * item = m_scene->focusItem();
   for(int i=0;i < m_items.size();i++) {
     m_items[i]->setTextWidth(w);
  }
  reposition();
  m_view->setSceneRect(m_scene->sceneRect());
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
  //  QLOG_DEBUG() << Q_FUNC_INFO;
  QGraphicsItem * item = m_scene->focusItem();
  if ( !item  && (m_items.size() > 0)) {
    item = m_items[0];
  }
  if (item) {
    this->setCurrentItem(item);
  }
  return;
}
/**
 * Sets the current item to the specified node.
 * If not found, does nothing
 * @param node
 *
 * @return true when node found, false otherwise
 */
bool GraphicsEntry::focusNode(const QString & node) {
  if (node.isEmpty()) {
    return true;
  }
  QLOG_DEBUG() << Q_FUNC_INFO << node;
  for(int i=0;i < m_items.size();i++) {
    Place p = m_items[i]->getPlace();
    if (p.getNode() == node) {
      this->setCurrentItem(m_items[i]);
      return true;
    }
  }
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
    QLOG_DEBUG() << "index" << btn->getIndex();
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
      btny = pos.y();

      ToolButtonData  * notesBtn = new ToolButtonData(i);
      notesBtn->setIcon(QIcon(QPixmap(":/qrc/notes.png")));
      notesBtn->setStyleSheet("padding :0px;border : 0px;margin : 0px");
      QGraphicsProxyWidget *pushButton = m_scene->addWidget(notesBtn);
      /// at the moment if a page has > 1 toolbuttons, clicking on one
      /// then on another, the second is not activated, Instead focus is
      /// set on the owning item of the first button. To activate the 2nd
      /// button you need to click on its owner first.
      pushButton->setFocusPolicy(Qt::StrongFocus);
      pushButton->widget()->setFocusPolicy(Qt::StrongFocus);
      notesBtn->setFocusPolicy(Qt::StrongFocus);
      pushButton->setPos(btnx,btny);
      connect(notesBtn,SIGNAL(clicked()),this,SLOT(notesButtonPressed()));
      m_items[i]->setProxy(pushButton);
      item->setNotes();
    }
  }

}
/**
 * Will set the current item (gives focus etc) to the pages Place
 * If is is a headword/node, that gets focus other the first item
 * on the page gets it. (Which should be the root)
 */
void GraphicsEntry::focusPlace() {
  Place p = this->getPlace();
  if (! p.getNode().isEmpty()) {
    this->focusNode(p.getNode());
    return;
  }
  else {
    this->shiftFocus();
  }
}
/**
 * gives the item the focus, makes sure its in the view and,
 * if the contents are linked, keeps the tree in sync
 * @param item
 */
void GraphicsEntry::setCurrentItem(QGraphicsItem * item) {
  QLOG_DEBUG() << Q_FUNC_INFO;
  m_view->setFocus();
  /// get the y position of the item
  /// add half the view height so that we are centering on the point that
  /// makes the top of the item, the top of the view
  QPointF pos = item->scenePos();
  pos += QPointF(0,m_view->height()/2);
  m_view->centerOn(pos);
  m_scene->setFocusItem(item);
  EntryItem * entry = dynamic_cast<EntryItem *>(item);
  if (entry) {
    m_place = entry->getPlace();
    updateCurrentPlace(m_place);
  }
}
/**
 * Local Search
 *
 * This uses different default search options from the other search routines
 * @return
 */
int GraphicsEntry::search() {
  QLOG_DEBUG() << Q_FUNC_INFO;

  QString target;
  int count = 0;

  SearchOptions options;
  QString v;

  if (m_lastSearchOptions.contains(SearchOptions::Local)) {
    SearchOptions * o = dynamic_cast<SearchOptions *>(m_lastSearchOptions.value(SearchOptions::Local));
    if (o) {
      options = SearchOptions(*o);
    }
  }
  /// if we haven't got a cache search options get it from settings
  if (! options.isValid()) {
    SETTINGS
      settings.beginGroup("LocalSearch");
    if (settings.value(SID_LOCALSEARCH_TYPE_REGEX,false).toBool()) {
      options.setSearchType(SearchOptions::Regex);
    }
    else {
      options.setSearchType(SearchOptions::Normal);
    }
    options.setSearchScope(SearchOptions::Local);
    options.setIgnoreDiacritics(settings.value(SID_LOCALSEARCH_DIACRITICS,true).toBool());
    options.setWholeWordMatch(settings.value(SID_LOCALSEARCH_WHOLE_WORD,true).toBool());
    options.setForceLTR(settings.value(SID_LOCALSEARCH_FORCE,false).toBool());
    options.setIgnoreCase(settings.value(SID_LOCALSEARCH_IGNORE_CASE,true).toBool());
    options.setShowAll(settings.value(SID_LOCALSEARCH_SHOW_ALL,false).toBool());
  }
  ArabicSearchDialog * d = new ArabicSearchDialog(SearchOptions::Local,this);
  d->setOptions(options);
  QString t;
  if (d->exec()) {
    t = d->getText();
    d->getOptions(options);
    delete d;
    if ( t.isEmpty()) {
      return -1;
    }
    m_currentSearchOptions = options;
    if (m_lastSearchOptions.contains(SearchOptions::Local)) {
      SearchOptions * o = dynamic_cast<SearchOptions *>(m_lastSearchOptions.take(SearchOptions::Local));
      if (o) {
        delete o;
      }
    }
    m_lastSearchOptions.insert(SearchOptions::Local,new SearchOptions(options));
  }
  else {
    delete d;
    return -1;
  }
  m_findCount = 0;
  m_highlightCount = 0;
  m_currentFind = 0;
  this->clearHighlights(false);
  m_searchItemPtr = 0;
  m_searchIndex = 0;
  /// TODO remove these

  m_searchItemIndexes.clear();
  QRegExp rx = SearchOptionsWidget::buildRx(t,m_pattern,options);
  QLOG_DEBUG() << "Search pattern" << rx.pattern();
  if (options.ignoreCase()) {
    rx.setCaseSensitivity(Qt::CaseInsensitive);
  }
  else {
    rx.setCaseSensitivity(Qt::CaseSensitive);
  }
  m_currentSearchRx = rx;
  m_currentSearchTarget = t;
  QGraphicsItem * focusItem = m_scene->focusItem();
  //  this->m_items[0]->ensureVisible();
  int pos = 0;
  bool found;
  for(int i=1;i < m_items.size();i++) {
    pos = 0;
    found = false;
    QPair<int,int> x;
    while(pos != -1) {
      pos += x.second;
      x = m_items[i]->find(rx,pos,options.showAll());
      if (x.first != -1) {
        found = true;
        count++;
        if (options.showAll()) {
          m_highlightCount++;
        }
      }
      pos = x.first;
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
    if (! options.showAll()) {
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
  QLOG_DEBUG() << Q_FUNC_INFO;
  int pos;
  if (m_currentSearchTarget.isEmpty()) {
    return;
  }
  if ((m_searchItemPtr >= 0) && (m_searchItemPtr < m_searchItemIndexes.size())) {
    pos = m_searchItemIndexes[m_searchItemPtr];
    int findCount = m_items[pos]->findCount();
    m_searchIndex++;
    if (m_searchIndex >= findCount) {
      m_searchItemPtr++;
      m_searchIndex = 0;
    }
  }
  if (m_searchItemPtr >= m_searchItemIndexes.size()) {
    QMessageBox msgBox;
    msgBox.setObjectName("wordnotfound");
    msgBox.setTextFormat(Qt::RichText);
    QString t;
    if (UcdScripts::isScript(m_currentSearchTarget,"Arabic")) {
      t = (qobject_cast<Lexicon *>(qApp))->spanArabic(m_currentSearchTarget,"wordnotfound");
    }
    else {
      t = m_currentSearchTarget;
    }
    msgBox.setText(QString(tr("No more occurrences of : %1")).arg(t));
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
  bool showAll = false;
  int pos = m_items[itemIndex]->showHighlight(ix);
  m_highlightCount++;
  m_currentFind++;
  if (m_lastSearchOptions.contains(SearchOptions::Local)) {
    showAll = m_lastSearchOptions.value(SearchOptions::Local)->showAll();
  }
  if (showAll) {
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
QString GraphicsEntry::getOutputFilename(const QString & pdfdir,bool autoname,const QString & method,const QString & node) {
  QString base;
  if (autoname == false) {
    return QFileDialog::getSaveFileName(this,tr("PDF name"),pdfdir,tr("PDF (*.pdf)"));
  }
  if (method == "node") {
    if (!node.isEmpty()) {
      base = node;
    }
    else {
      QString first;
      QString last;
      for(int i=0;(i < m_items.size()) && first.isEmpty();i++) {
        base = m_items[i]->getNode();
        if (! base.isEmpty()) {
          first = base;
        }
      }
      for(int i=m_items.size() - 1;(i >= 0) && last.isEmpty();i--) {
        base = m_items[i]->getNode();
        if (! base.isEmpty()) {
          last = base;
        }
      }
      base = QString("%1-%2").arg(first).arg(last);
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

QString GraphicsEntry::getPageInfo(bool sysinfo,bool nodesummary) {
  Place firstPlace;
  QString firstNode;
  QString html;
  if (sysinfo) {
  for(int i=0;(i < m_items.size()) && ! firstPlace.isValid();i++) {
    firstPlace = m_items[i]->getPlace();
  }
  for(int i=0;(i < m_items.size()) && firstNode.isEmpty();i++) {
    firstNode = m_items[i]->getPlace().getNode();
  }

  html += "<table class=\"pageinfo\" style=\"width:100%\">";
  html += ROW("<b>" + tr("Entry information") + "</b>","");
  html += ROW("","");

  html += ROW(tr("Root"),QString("<span class=\"arabic\">%1</span>").arg(firstPlace.getRoot()));
  QMultiMap<int,int> pages;
  int page,vol;
  for(int i=1; i < m_items.size();i++) {
    page = m_items[i]->getPlace().page();
    vol = m_items[i]->getPlace().volume();
    pages.insert(vol,page);
  }
  QList<int> vols = pages.uniqueKeys();
  for(int i=0;i < vols.size();i++) {
    QList<int> p = pages.values(vols[i]);
    QStringList str;
    for(int j=0;j < p.size();j++) {
      str << QString("%1").arg(p[j]);
    }
    str.removeDuplicates();
    html += ROW(QString(tr("Volume %1")).arg(vols[i]),QString("%1").arg(str.join(",")));
  }
  Place lastPlace;
  for(int i=m_items.size() - 1;(i >= 0) && ! lastPlace.isValid();i--) {
    lastPlace = m_items[i]->getPlace();
  }

  html += ROW(tr("Nodes"),QString("%1 - %2").arg(firstNode).arg(lastPlace.getNode()));
  html += ROW(tr("Date"),QDateTime::currentDateTime().toString(Qt::ISODate));
  QSqlQuery query;
  if (query.prepare(SQL_FIND_VERSION)) {
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
  html += "</table>";
  }
  html += "<br/>";
#define ROW4(a,b,c,d)    "<tr><td width=\"10%\">" + QString(a) + "</td><td width=\"10%\">" + QString(b) + "</td><td width=\"10%\">" + QString(c) + "</td> <td width=\"%70\">" + QString(d) + "</td></tr>"
  if (nodesummary ) {
    Place p;
    html += "<table class=\"pageinfo\" style=\"width:100%\">";
    html += "<p class=\"entrydetails\">Entry details</p>";
    html += ROW4(tr("Volume"),tr("Page"),tr("Node"),tr("Head word"));
    html += ROW4("","","","");
    for(int i=1; i < m_items.size();i++) {
      p = m_items[i]->getPlace();
      html += ROW4(QString("%1").arg(p.volume()),QString("%1").arg(p.page()),p.getNode(),QString("<span class=\"arabic\">%1</span>").arg(p.getWord()));
    }
    html += "</table>";
  }
  return html;
}
/**
 * If node is empty, print all the nodes
 *
 * @param printer
 * @param node
 */
void GraphicsEntry::print(QPrinter & printer,const QString & node) {
  SETTINGS
  settings.beginGroup("Printer");
  bool pdfoutput = settings.value(SID_PRINTER_OUTPUT_PDF).toBool();
  bool printerReUse = settings.value(SID_PRINTER_USE).toBool();

  if (printerReUse && pdfoutput) {
    QString pdfdir = settings.value(SID_PRINTER_PDF_DIRECTORY,QDir::tempPath()).toString();
    if (pdfdir.isEmpty()) {
      pdfdir = QDir::tempPath();
    }
    QString filename;
    filename = getOutputFilename(pdfdir,settings.value(SID_PRINTER_AUTONAME_PDF).toBool(),
                                 settings.value(SID_PRINTER_AUTONAME_METHOD).toString(),node);
    if (filename.isEmpty()) {
      return;
    }
    printer.setOutputFileName(filename);
  }
  settings.endGroup();
  QString html; // = "<html><body>";
  for(int i=0;i < m_items.size();i++) {
    if (node.isEmpty() || (node == m_items[i]->getPlace().getNode())) {
      /// without the trimmed() the printing of the root causes problems
      QString t = m_items[i]->getOutputHtml().trimmed();
      t.remove("<html>");
      t.remove("</html>");
      t.remove("<body>");
      t.remove("</body>");
      html += t;
    }
  }
  html.replace("<body","<div");
  html.replace("</body>","</div>");
  QRegularExpression rx("[\u2000-\u202e]");
  html.remove(rx);
  if (m_dumpOutputHtml) {
    QString n;
    if (node.isEmpty()) {
      n = m_items[1]->getPlace().getNode();
    }
    else {
      n = node;
    }
    QFileInfo fi(QDir::tempPath(),QString("print-%1.html").arg(n));
    QFile f(fi.filePath());
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QTextStream out(&f);
      out.setCodec("UTF-8");
      out << html;
    }
  }

  QTextDocument doc;
  doc.setDefaultStyleSheet(m_printCss);
  doc.setHtml(html); // + "</body></html>");
  doc.setDefaultTextOption(m_textOption);
  settings.beginGroup("Entry");

  int printNotes = settings.value(SID_ENTRY_PRINT_NOTES,-1).toInt();
  int printNodes = settings.value(SID_ENTRY_PRINT_NODES,-1).toInt();
  int printInfo = settings.value(SID_ENTRY_PRINT_INFO,-1).toInt();
  settings.endGroup();
  bool pNotes = false;
  bool pNodes = false;
  bool pInfo = false;
  if (printNotes == SID_ALWAYS) {
    pNotes = true;
  }
  if (printNodes == SID_ALWAYS) {
    pNodes = true;
  }
  if (printInfo == SID_ALWAYS) {
    pInfo = true;
  }
  if ((printInfo == SID_PROMPT) ||
      (printNotes == SID_PROMPT) ||
      (printNodes == SID_PROMPT)) {
    QScopedPointer<QDialog> d(new QDialog);
    d->setWindowTitle(tr("Print options"));
    QCheckBox * n = new QCheckBox(tr("Print notes"));
    QCheckBox * t = new QCheckBox(tr("Print node summary"));
    QCheckBox * i = new QCheckBox(tr("Print system information"));
    QVBoxLayout * layout = new QVBoxLayout;
    if (printInfo == SID_ALWAYS) {
      i->setChecked(true);
    }
    if (printNotes == SID_ALWAYS) {
      n->setChecked(true);
    }
    if (printNodes == SID_ALWAYS) {
      t->setChecked(true);
    }
    layout->addWidget(n);
    layout->addWidget(i);
    layout->addWidget(t);

    QDialogButtonBox * btns = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    QPushButton * btn = btns->button(QDialogButtonBox::Ok);
    btn->setText(tr("Print"));
    connect(btns,SIGNAL(accepted()),d.data(),SLOT(accept()));
    connect(btns,SIGNAL(rejected()),d.data(),SLOT(reject()));
    layout->addWidget(btns);
    d->setLayout(layout);
    if (d->exec() == QDialog::Accepted) {
      pInfo = i->isChecked();
      pNotes = n->isChecked();
      pNodes = t->isChecked();
    }
    else {
      return;
    }
  }
  if  (pNotes) {
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

  if (pInfo || pNodes) {
    QTextCursor cursor(&doc);
    cursor.movePosition(QTextCursor::End);
    QTextBlockFormat format;
    format.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysBefore);
    cursor.insertBlock(format);
    //    html += "<hr/>";
    cursor.insertHtml(getPageInfo(pInfo,pNodes));
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
/**
 * reread the CSS file from QSettings and therefore disk
 * and reread  the name of the XSLT file
 *
 * The readCssFromFile function use the Lexicon->getResource to look in the
 * the correct directory.
 */
void GraphicsEntry::onReload() {
  QLOG_DEBUG() << Q_FUNC_INFO;

  double scale = m_scale;
  QString title = m_userTitle;
  int textwidth = m_textWidth;
  readSettings();
  m_textWidth = textwidth;
  this->getXmlForRoot(m_place);
  this->setScale(scale,true);
  if (! title.isEmpty()) {
    QWidget * w = this->parentWidget();
    while(w) {
      QTabWidget * tab = qobject_cast<QTabWidget *>(w);
      if (tab)  {
        for(int i=0;i < tab->count();i++) {
          if (tab->widget(i) == this) {
            tab->setTabText(i,title);
          }
        }
        w = NULL;
      }
      else {
        w = w->parentWidget();
      }
    }
    this->setUserTitle(title);
  }
  /*
  QString html;
  for (int i=0;i < m_items.size();i++) {
    html = transform(ENTRY_XSLT_RECOMPILE,m_entryXslt,m_items[i]->getXml());
    m_items[i]->document()->clear();
    m_items[i]->document()->setDefaultStyleSheet(m_currentCss);
    m_items[i]->setHighlightColor(m_highlightColorName);
    m_items[i]->setTextWidth(m_textWidth);
    m_items[i]->setHtml(html);
    m_items[i]->setOutputHtml(html);
  }
  */
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
      html = transform(ENTRY_XSLT,m_entryXslt,m_items[i]->getXml());
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
  statusMessage(tr("In-memory page reload"));
}
/**
 * Params:
 * link info i.e either nolink=<orthid> or golink=<orthid>
 * link text - this is just the down arrow (or whatever the xslt puts in there)
 * target info in the form <node>:<word>
 * the node that needs fixing
 * reload page after fixup
 *
 * Update the link record, setting its target node. This will work for
 * all link records so can be used to redirect a 'working' link.
 *
 * If the fix is for a broken link (i.e. "nonlink=nn"), then also udpate
 * the XML for all entry records that use that link.
 *
 * void GraphicsEntry::fixLink(const QStringList &, bool) ("nolink=32090", "اثّغر‎", "n4460:اِتَّغَرَ", "n4459") true
 * @param params
 * @param reload
 */
void GraphicsEntry::fixLink(const QStringList & params,bool reload) {
  QLOG_DEBUG() << Q_FUNC_INFO << params << reload;
  EntryItem * item = qobject_cast<EntryItem *>(QObject::sender());

  if (! item ) {
    QLOG_DEBUG() << Q_FUNC_INFO << "EntryItem not defined";
    return;
  }
  QRegExp rx("(nolink|golink)=(.+)");
  if (rx.indexIn(params[0]) == -1) {
    QLOG_WARN() << QString(tr("Link fixup invalid parameter: %1")).arg(params[0]);
    return;
  }
  QString orthid = rx.cap(2);
  QString linkType = rx.cap(1);
  QString targetNode;
  QString targetWord;
  QStringList words = params[2].split(":");
  if (words.size() != 2) {
    QLOG_WARN() << QString(tr("Invalid parameter in link %1 update: %2")).arg(orthid).arg(params[2]);
    return;
  }
  targetNode = words[0];
  targetWord = words[1];


  /**
   * This SQL sets the matchtype to 100
   *
   */

  QSqlQuery updateLink;
  if (! updateLink.prepare(SQL_UPDATE_LINK_TO_NODE)) {
    QLOG_WARN() << updateLink.lastError().text();
    return;
  }
  QSqlQuery nodefind;
  if (! nodefind.prepare(SQL_ENTRY_FOR_NODE)) {
    QLOG_WARN() << nodefind.lastError().text();
    return;
  }
  QSqlQuery nodeupdate;
  if (! nodeupdate.prepare(SQL_UPDATE_XML_FOR_NODE)) {
    QLOG_WARN() << nodeupdate.lastError().text();
    return;
  }

 QSqlQuery findLink;
  if (! findLink.prepare(SQL_FIND_NODE_FOR_LINK)) {
    QLOG_WARN() << findLink.lastError().text();
    return;
  }
  findLink.bindValue(0,orthid);
  findLink.exec();

  if (! findLink.first()) {
    QLOG_INFO() << QString(tr("Cannot find link details for update: %1")).arg(orthid);
    return;
  }
  int c = 0;
  QLOG_DEBUG() << "link fixup" << findLink.value(1) << findLink.value(2) << findLink.value(3);
  QString node = item->getNode(); //findLink.value(2).toString();

  QString linkWord = findLink.value(3).toString();

  /// first time through optionally present the save link
  /// dialog
  if (m_showLinkWarning) {
    QString txt = QString(tr("Link id: %1\nFrom   :%2\nTo     :%3")).arg(orthid).arg(item->getNode()).arg(targetNode);
    if (! showLinkDialog(linkWord,targetWord,txt)) {
      return;
    }
  }
  updateLink.bindValue(0,targetNode);
  updateLink.bindValue(1,orthid);
  if (! updateLink.exec()) {
    QLOG_WARN() << QString(tr("Update links error: %1")).arg(updateLink.lastError().text());
    return;
  }
  QLOG_DEBUG() << "successfully updated link record";
  c++;
  /// Get the XML and update the <cref ...    select="target node>">
  ///
  nodefind.bindValue(0,node);
  if (nodefind.exec() && nodefind.first()) {
    QString xml = nodefind.value("xml").toString();
    QDomDocument doc;
    doc.setContent(xml);
    QDomNodeList refs = doc.elementsByTagName("ref");
    for(int i=0;i < refs.size();i++) {
      QDomElement e = refs.at(i).toElement();
      QString cref = e.attribute("cref");
      if (cref == orthid) {
        e.setAttribute("select",targetNode);
        QString nxml = doc.toString(-1);
        nodeupdate.bindValue(0,nxml);
        nodeupdate.bindValue(1,node);
        if (!nodeupdate.exec()) {
          QLOG_WARN() << QString(tr("Failed to update link details for node %1: %2")).arg(node).arg(nodeupdate.lastError().text());
          return;
        }
        item->setXml(this->wrapEntry(nodefind.record(),nxml));
        QSqlDatabase::database().commit();
        statusMessage(QString(tr("Updated link details")));
        QLOG_INFO() << QString(tr("Update link id: %1, from node : %2, to node : %3")).arg(orthid).arg(node).arg(targetNode);
        if (reload) {
          this->onReload();
        }
        return;
      }
    }
  }
  else {
    QLOG_WARN() << QString(tr("Failed to update xml for node %1: %2")).arg(node).arg(nodefind.lastError().text());
  }
}
bool GraphicsEntry::showLinkDialog(const QString & linkWord,const QString & target,const QString & text) {
  if (! m_showLinkWarning ) {
    return true;
  }
  QCheckBox * noshow = new QCheckBox(tr("Check this box to stop the dialog appearing again"));
  QMessageBox msgBox;
  msgBox.setTextFormat(Qt::RichText);
  msgBox.setCheckBox(noshow);
  msgBox.setWindowTitle(tr("Update link"));
  msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Save);
  QString html1 = qobject_cast<Lexicon *>(qApp)->spanArabic(linkWord);
  QString html2 = qobject_cast<Lexicon *>(qApp)->spanArabic(target);
  msgBox.setText(QString(tr("This will point %1 to %2.")).arg(html1).arg(html2));
  msgBox.setInformativeText(tr("Do you wish to update the link?"));
  QString txt(tr("You can change the link as many times as you want."));
  txt += QString(tr("\nThe current update is:\n%1")).arg(text);
  msgBox.setDetailedText(txt);
  msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
  int ret = msgBox.exec();
  if (ret == QMessageBox::Cancel) {
    return false;
  }
  if (noshow->isChecked()) {
    SETTINGS
    settings.beginGroup("Entry");
    settings.setValue(SID_ENTRY_SHOW_LINK_WARNING,false);
    m_showLinkWarning = false;
  }
  return true;
}
void GraphicsEntry::setUserTitle(const QString & text) {
  m_userTitle = text;
}
QString GraphicsEntry::userTitle() const {
  return m_userTitle;
}
void GraphicsEntry::checkLink(const QString link) {
  bool ok;

  qDebug() << Q_FUNC_INFO << link;
  QSqlQuery query;
  if (! query.prepare(SQL_LINK_TYPE)) {
    QLOG_WARN() << QString(tr("Prepare failed for SQL_LINK_TYPE query:%1")).arg(query.lastError().text());
    return;
  }
  query.bindValue(0,link);
  if (! query.exec()) {
    QLOG_WARN() << QString(tr("Exec failed for SQL_LINK_TYPE query:%1")).arg(query.lastError().text());
    return;
  }
  if (! query.first()) {
    QLOG_WARN() << QString(tr("Find failed for SQL_LINK_TYPE query:%1")).arg(query.lastError().text());
    return;
  }
  LinkCheckDialog * dlg = new LinkCheckDialog(query.record());
  if (dlg->exec() == QDialog::Accepted) {
    int status = dlg->getStatus();
    QString tonode = dlg->getTargetNode();
    QString note = dlg->getText();
    qDebug() << QString("Status:%1,note:%2").arg(status).arg(note);
    QSqlQuery u;
    if (u.prepare(SQL_LINK_UPDATE_STATUS)) {
      u.bindValue(0,status);
      u.bindValue(1,tonode);
      u.bindValue(2,note);
      u.bindValue(3,link);
      if (u.exec()) {
        QLOG_INFO() << QString("Link id %1, status set to: %2").arg(link).arg(status);
        QSqlDatabase::database().commit();
      }
      else {
        QLOG_WARN() << QString(tr("Exec failed for SQL_LINK_UPDATE_STATSU query:%1")).arg(u.lastError().text());
      }
    }
    else {
      QLOG_WARN() << QString(tr("Prepare failed for SQL_LINK_UPDATE_STATSU query:%1")).arg(u.lastError().text());
    }

  }
  delete dlg;
}
QString GraphicsEntry::wrapEntry(const QSqlRecord & record,const QString & xml) {
  QString t  = QString("<word buck=\"%1\" ar=\"%2\" page=\"%3\" itype=\"%4\" supp=\"%5\">")
    .arg(record.value("bword").toString())
    .arg(record.value("word").toString())
    .arg(record.value("page").toInt())
    .arg(record.value("itype").toString())
    .arg(record.value("supplement").toInt());
  t += xml;
  t += "</word>";

  return t;
}

QSqlRecord GraphicsEntry::findLinkRecord(const QString & linkid) const {
  QSqlRecord rec;
  QLOG_DEBUG() << Q_FUNC_INFO << linkid;

  QString node;
  QSqlQuery q;
  if (! q.prepare(SQL_LINKTO_NODE)) {
    QLOG_WARN() << QString(tr("Prepare failed for SQL_LINKTO_NODE :%1")).arg(q.lastError().text());
    return rec;
  }
  q.bindValue(0,linkid);
  q.exec();
  if (q.first()) {
    node = q.value(0).toString();
  }
  else {
    QLOG_WARN() << QString("Missing link record for link: %1").arg(linkid);
    return rec;
  }
  if (! q.prepare(SQL_FIND_ENTRY_BY_NODE)) {
      QLOG_WARN() << QString(tr("Prepare failed for SQL_FIND_ENTRY_BY_NODE for %1, error %2"))
      .arg(linkid)
      .arg(q.lastError().text());
    return rec;
  }

  q.bindValue(0,node);
  q.exec();
  if (q.first()) {
    return q.record();
  }
  return rec;
}
void GraphicsEntry::setXrefMode(int m) {
  m_linkCheckMode = m;
}
void GraphicsEntry::onFocusItemChanged(QGraphicsItem * newFocus, QGraphicsItem * oldFocus , Qt::FocusReason  reason ) {
  EntryItem * item1 = dynamic_cast<EntryItem *>(newFocus);
  EntryItem * item2 = dynamic_cast<EntryItem *>(oldFocus);

  if (item1) {
    m_focusPlace = item1->getPlace();
    m_focusNode = m_focusPlace.node();
  }
  ///
  /// this happens when the the graphicsentry loses focus because the user has clicked on
  /// e.g the toolbar or pressed one of the active keys
  ///
  /// We try to restore focus to the last know focus place
  ///
  if (!item1  && item2) { // && (reason != Qt::MouseFocusReason)) {
    this->hasPlace(m_focusPlace,GraphicsEntry::NodeSearch,true);
  }
  /// this updates the status bar
  emit(focusItemChanged(newFocus,oldFocus,reason));
}
