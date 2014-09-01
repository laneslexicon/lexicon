#include "application.h"
#include "inputmapper.h"
#include "contentswidget.h"
#include "graphicsentry.h"
#include "history.h"
#include "QsLog.h"
#include "place.h"
#include "bookmarkwidget.h"
#include "scripts.h"
#include "xsltsupport.h"
#include "menubar.h"
#include "helpviewer.h"
#include "notes.h"
#include "notebrowser.h"
#include "tabwidget.h"
#include "namespace.h"
#include "laneslexicon.h"
#include "searchoptions.h"
#include "searchdialogs.h"
#include "fullsearch.h"
#include "headsearch.h"
#include "definedsettings.h"
//extern cmdOptions progOptions;
extern QSettings * getSettings();
extern void testfocus();
LanesLexicon::LanesLexicon(QWidget *parent) :
    QMainWindow(parent)

{
  setAttribute(Qt::WA_DeleteOnClose);
  setObjectName("lexicon");
  m_ok = false;
  m_history = 0;
  m_revertEnabled = false;
  m_mapper = im_new();
  readSettings();

  if (! openDatabase(m_dbName)) {
    QMessageBox msgBox;
    msgBox.setWindowTitle(QGuiApplication::applicationDisplayName());
    QString errorMessage(tr("Database not found"));
    QDir d;
    QString currentDir = d.absoluteFilePath(m_dbName);
    QString info = QString("Missing file %1").arg(QDir::cleanPath(currentDir));
    QString next(tr("This application will terminate"));
    msgBox.setText("<html><head/><body><h2>" + errorMessage + "</h2><p>"
                   + info + "</p><p>" + next + "</p></body></html>");
    msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
    msgBox.exec();
    return;
  }
  loadStyleSheet();

  m_tree = new ContentsWidget(this);
  m_tree->setObjectName("treeRoots");
  m_tree->installEventFilter(this);
  m_tabs = new TabWidget(this);
  m_tabs->setTabsClosable(true);
  connect(m_tabs,SIGNAL(tabsChanged()),this,SLOT(tabsChanged()));
  /// at the end of the history, but we should be able to restore from settings
  /// TODO would we want restore our current position in history?
  m_history = new HistoryMaster(m_historyDbName);
  if ( ! m_history->isOk()) {
    QLOG_WARN() << "History is not available";
    m_historyEnabled = false;
  }
  m_history->setEnabled(m_historyEnabled);

  QSettings * settings  = ((qobject_cast<Lexicon *>(qApp))->getSettings());
  m_notes = new NoteMaster(settings);
  delete settings;

  if (m_docked) {
    m_treeDock = new QDockWidget("Contents",this);
    m_treeDock->setObjectName("contentsdock");
    m_treeDock->setAllowedAreas(Qt::LeftDockWidgetArea |
                                Qt::RightDockWidgetArea);
    m_treeDock->setWidget(m_tree);
    addDockWidget(Qt::LeftDockWidgetArea, m_treeDock);
    setCentralWidget(m_tabs);
  }
  else {
    QSplitter * splitter = new QSplitter(this);
    splitter->addWidget(m_tree);
    splitter->addWidget(m_tabs);
    splitter->setStretchFactor(0,0);
    splitter->setStretchFactor(1,4);
    //  m_tabs->addTab(m_notes,"Notes");
    setCentralWidget(splitter);
  }
  setupShortcuts();
  setupBookmarkShortcuts();

  createActions();
  setIcons(m_iconTheme);
  createMenus();
  createToolBar();
  createStatusBar();

  if (m_db.isOpen()) {
    statusBar()->showMessage(tr("Ready"));
    if (! m_valgrind ) {
      m_tree->loadContents();
      getFirstAndLast();
      m_ok = true;
    }
  }
  else {
    statusBar()->showMessage(tr("Failed to open database"));
  }


  connect(m_tree,SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)),this,SLOT(rootClicked(QTreeWidgetItem *,int)));

  connect(m_tree,SIGNAL(itemActivated(QTreeWidgetItem *,int)),this,SLOT(entryActivated(QTreeWidgetItem *,int)));

  connect(m_tabs,SIGNAL(tabCloseRequested(int)),this,SLOT(onCloseTab(int)));
  connect(m_tabs,SIGNAL(currentChanged(int)),this,SLOT(currentTabChanged(int)));


  if ( m_useNotes ) {

  }
  if (m_restoreTabs) {
    restoreTabs();
  }
  /// TODO if no tabs, show first root. Change this to somethign else ?

  Place p;
  int ix;
  if (! m_startupNode.isEmpty()) {
    p.setNode(m_startupNode);
    ix = this->hasPlace(p,GraphicsEntry::NodeSearch,true);
  }
  else if (! m_startupRoot.isEmpty()) {
    p.setRoot(m_startupRoot);
    ix = this->hasPlace(p,GraphicsEntry::RootSearch,true);
  }
  else if ((m_tabs->count() == 0) && ! m_firstRoot.isEmpty()) {
    p.setRoot(m_firstRoot);
    ix = this->hasPlace(p,GraphicsEntry::RootSearch,true);
  }
  if (p.isValid()) {
    if (ix == -1) {  // not showing in another tab
      int options = 0;
      options |= Lane::Create_Tab;
      options |= Lane::Switch_Tab;
      showPlace(p,options);
      m_tree->ensurePlaceVisible(p);
    }
  }

  if (m_restoreBookmarks) {
    restoreBookmarks();
  }

  m_tree->resizeColumnToContents(0);

  if (m_historyEnabled) {
    setupHistory();
  }


  setupInterface();

  restoreSavedState();
  /// without this, the QSplashScreen is the active window

  //  m_tabs->currentWidget()->setFocus();

  setTabOrder(m_tree,m_tabs);
  setTabOrder(m_tabs->tabBar(),m_tree);
  /**
   * if the current widget is a graphics entry , sent it a <space> to simulate
   * the user activating the page (<return> would do). This selects the current item
   * and gives focus etc. It's a kludge, but the whole focus/selection stuff is a
   * bit confusing.
   */
  QApplication::setActiveWindow(m_tabs->currentWidget());
  qDebug() << "Activating current entry";
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    entry->focusPlace();
    QKeyEvent * event;
    int k = 0x20;
    event = new QKeyEvent(QEvent::KeyPress, k, Qt::NoModifier,QString(QChar(k)));
    QApplication::postEvent(entry,event);
  }
  /*
  QList<QShortcut *> edits = this->findChildren<QShortcut *>();
  foreach(QShortcut *  widget,edits) {
    //    qDebug() << widget->key().toString();
  }
  edits.clear()
  */

  QLOG_DEBUG() << "-----------------------";
  QLOG_DEBUG() << "Initialisation complete";
  QLOG_DEBUG() << "-----------------------";


}

LanesLexicon::~LanesLexicon()
{
  /// TODO make this check something has changed or give
  /// option to save settings ?
  //  cleanup();
}
void LanesLexicon::restoreSavedState() {
  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());
  settings->setIniCodec("UTF-8");
  settings->beginGroup("Main");
  this->restoreGeometry(settings->value("Geometry").toByteArray());
  this->restoreState(settings->value("State").toByteArray());
 }
void LanesLexicon::closeEvent(QCloseEvent * event) {
  cleanup();
  QMainWindow::closeEvent(event);
}
void LanesLexicon::cleanup() {
  if (m_ok) {
    writeSettings();
  }
  if (m_history) {
    delete m_history;
  }
  while(m_tabs->count() > 0) {
    this->onCloseTab(0);
  }

  if (m_db.isOpen()) {
    m_db.close();
  }
  if (m_notesDb.isOpen())
    m_notesDb.close();

  delete m_tree;
  /// TODO close notes db
  freeXslt();
  im_free(m_mapper);
  QLOG_DEBUG() << Q_FUNC_INFO << "exit";
}
void LanesLexicon::setupInterface() {
  if (m_interface == "minimal") {
    QList<QToolBar *> toolbars = this->findChildren<QToolBar *>();
    for(int i=0;i < toolbars.size();i++) {
      toolbars[i]->hide();
    }
    statusBar()->hide();
    menuBar()->hide();
  }
  else  {
    QList<QToolBar *> toolbars = this->findChildren<QToolBar *>();
    for(int i=0;i < toolbars.size();i++) {
      toolbars[i]->show();
    }
    statusBar()->show();
    menuBar()->show();
  }
  updateMenu();
}
/**
 * convenience
 *
 * @param entry
 */
void LanesLexicon::setSignals(GraphicsEntry * entry) {
  connect(entry,SIGNAL(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)),
          this,SLOT(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)));
  connect(entry,SIGNAL(nextRoot(const QString &)),this,SLOT(findNextRoot(const QString &)));
  connect(entry,SIGNAL(prevRoot(const QString &)),this,SLOT(findPrevRoot(const QString &)));
  connect(entry,SIGNAL(next(const Place &)),this,SLOT(moveNext(const Place &)));
  connect(entry,SIGNAL(prev(const Place &)),this,SLOT(movePrevious(const Place &)));
  connect(entry,SIGNAL(historyPositionChanged(int)),this,SLOT(historyPositionChanged(int)));
  connect(entry,SIGNAL(historyAddition(const Place &)),this,SLOT(historyAddition(const Place &)));
  connect(entry,SIGNAL(bookmarkAdd(const QString &,const Place &)),this,SLOT(bookmarkAdd(const QString &,const Place &)));

  //  connect(entry,SIGNAL(rootChanged(const QString & ,const QString & )),this,SLOT(rootChanged(const QString &, const QString &)));

  connect(entry,SIGNAL(placeChanged(const Place &)),this,SLOT(placeChanged (const Place &)));
  connect(entry,SIGNAL(clearPage()),this,SLOT(pageClear()));
  connect(entry,SIGNAL(searchPage()),this,SLOT(pageSearch()));

  connect(entry,SIGNAL(gotoNode(const Place &,int)),this,SLOT(gotoPlace(const Place &,int)));
  //  connect(entry,SIGNAL(saveNote(Note *)),this,SLOT(saveNote(Note *)));

}
void LanesLexicon::onCloseTab(int ix) {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(ix));
  if (entry) {
    m_tabs->removeTab(ix);
    entry->close();
    delete entry;
    return;
  }
  HeadSearchWidget * results = qobject_cast<HeadSearchWidget *>(m_tabs->widget(ix));
  if (results) {
      m_tabs->removeTab(ix);
      delete results;
      return;
  }
  NoteBrowser * notes = qobject_cast<NoteBrowser *>(m_tabs->widget(ix));
  if (notes) {
      m_tabs->removeTab(ix);
      delete notes;
      return;
  }
  FullSearchWidget * search = qobject_cast<FullSearchWidget *>(m_tabs->widget(ix));
  if (search) {
    delete search;
    return;
  }
  m_tabs->removeTab(ix);
}
void LanesLexicon::shortcut(const QString & key) {
  QLOG_DEBUG() << Q_FUNC_INFO << key;
  //  QString key = k.toCaseFolded();
  if (key == SID_SHORTCUT_SEARCH_ROOT) {
    searchForRoot();
  }
  else if (key == SID_SHORTCUT_SEARCH_NODE) {
    searchForNode();
  }
  else if (key == SID_SHORTCUT_SEARCH_WORD) {
    searchForWord();
  }
  else if (key == SID_SHORTCUT_SEARCH_HEAD) {
    searchForEntry();
  }
  else if (key == SID_SHORTCUT_PAGE_SEARCH) {
    searchForPage();
  }
  else if (key == SID_SHORTCUT_CONTENTS_COLLAPSE_ALL) {
    m_tree->collapseAll();
  }
  else if (key == SID_SHORTCUT_CONTENTS_COLLAPSE_LETTER) {
    QTreeWidgetItem * item = m_tree->currentItem();
    if (item) {
      /// if item is a root, get the parent (letter)
      if (item->childCount() == 0) {
        item = item->parent();
        if (! item ) {
          return;
        }
      }
      if (item->isExpanded()) {
        m_tree->collapseItem(item);
      }
      else {
        m_tree->expandItem(item);
      }
    }
    else {
      QLOG_DEBUG() << Q_FUNC_INFO << "No current item";
    }
  }
  else if (key == SID_SHORTCUT_QUIT) {
    onExit();
  }
  else if (key == SID_SHORTCUT_TOGGLE_INTERFACE) {
    if (m_interface == "minimal") {
      m_interface = "default";
    }
    else {
      m_interface = "minimal";
    }
    setupInterface();
  }
  else if (key == SID_SHORTCUT_CONTENTS_SHOW) {
    if (m_treeDock->isVisible()) {
      m_treeDock->hide();
    }
    else {
      m_treeDock->show();
    }
  }
  else if (key == SID_SHORTCUT_NAV_NEXT) {
    this->onNavNext();
  }
  else if (key == SID_SHORTCUT_NAV_PREV) {
    this->onNavPrev();
  }
  else if (key == SID_SHORTCUT_NAV_FIRST) {
    this->onNavFirst();
  }
  else if (key == SID_SHORTCUT_NAV_LAST) {
    this->onNavLast();
  }
  else if (key == SID_SHORTCUT_FOCUS_CONTENT) {
    /// if an item has focus, this loses it
    GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
    if (entry) {
      entry->focusPlace();
    }
    else {
      m_tabs->currentWidget()->setFocus();
    }
  }
  else if (key == SID_SHORTCUT_FOCUS_TREE) {
    m_tree->setFocus();
  }
  else if (key.startsWith(SID_SHORTCUT_GO_TAB)) {
    QString nstr = key.right(1);
    bool ok;
    int ix = nstr.toInt(&ok);
    if (ok) {
      ix--;
      if ((ix >= 0) && (ix < m_tabs->count())) {
        m_tabs->setCurrentIndex(ix);
      }
    }
  }
  else if (key == SID_SHORTCUT_BOOKMARK_ADD) {
    this->bookmarkAdd();
  }
  else if (key.startsWith("bookmark")) {
    bookmarkShortcut(key);
  }
  else if (key == SID_SHORTCUT_SELECT_ENTRY) {

    GraphicsEntry * page = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
    if (page) {
      page->selectEntry();
    }
  }
  else if (key == SID_SHORTCUT_SELECT_ALL) {
    GraphicsEntry * page = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
    if (page) {
      page->selectAll();
    }
  }
  else if (key == SID_SHORTCUT_NAV_ROOT_MODE) {
    m_navMode = Lane::By_Root;
  }
  else if (key == SID_SHORTCUT_NAV_PAGE_MODE) {
    m_navMode = Lane::By_Page;
  }
  else if (key == SID_SHORTCUT_HISTORY_NEXT) {
    /// increment history pos
  }
  else if (key == SID_SHORTCUT_HISTORY_BACK) {
    if (m_historyPos > 0) {
      m_historyPos--;
      setupHistory(m_historyPos);
    }
  }
  else if (key == SID_SHORTCUT_SYNC) {
    syncContents();
  }
  else if (key == SID_SHORTCUT_SHOW_NOTES) {
    showNoteBrowser();
  }
  else if (key == SID_SHORTCUT_DELETE_TAB) {
    this->onCloseTab(m_tabs->currentIndex());
  }
  else if (key == SID_SHORTCUT_CONVERT_TO_ENTRY) {
    this->convertToEntry();
  }
  else if (key == SID_SHORTCUT_KEYMAPS_ENABLE) {
    this->enableKeymaps(true);
  }
  else if (key == SID_SHORTCUT_KEYMAPS_DISABLE) {
    this->enableKeymaps(false);
  }
  else if (key == SID_SHORTCUT_SEARCH_DELETE) {
    this->deleteSearch();
  }
  else if (key == SID_SHORTCUT_LOCAL_SEARCH_FIND) {
    this->localSearch();
  }
  else if (key == SID_SHORTCUT_LOCAL_SEARCH) {
    this->localSearch();
  }
  else if (key == SID_SHORTCUT_LOCAL_SEARCH_NEXT) {
    this->localSearchNext();
  }
  else if (key == SID_SHORTCUT_LOCAL_SEARCH_CLEAR) {
    this->localSearchClear();
  }
  else if (key == SID_SHORTCUT_LOCAL_SEARCH_SHOW) {
    this->localSearchShow();
  }
  else if (key == "sync") {
    this->sync();
  }
  else {
    QLOG_WARN() << "Unhandled shortcut" << key;
  }
  updateStatusBar();
  updateMenu();
  //QLOG_DEBUG() << qobject_cast<QShortcut *>(m_signalMapper->mapping(k));
}
/**
 * setup the shortcuts from the conf
 *
 */
void LanesLexicon::setupShortcuts() {
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  settings->beginGroup("Shortcut");
  m_signalMapper = new QSignalMapper(this);
  QStringList keys = settings->childKeys();
  for(int i=0;i < keys.size();i++) {
    if (keys[i] == "Go Tab") {
      /// TODO make this a user string of characters
      /// e.g 123456789abcdef
      for(int j=1;j < 10;j++) {
        QString ks = QString("%1,%2").arg(settings->value(keys[i]).toString()).arg(j);
        QShortcut * sc = new QShortcut(ks,this);
        connect(sc,SIGNAL(activated()),m_signalMapper,SLOT(map()));
        m_signalMapper->setMapping(sc,QString("%1-%2").arg(keys[i]).arg(j));
      }
    }
    else {
      QString k = settings->value(keys[i]).toString();
      QShortcut * sc = new QShortcut(k,this);
      connect(sc,SIGNAL(activated()),m_signalMapper,SLOT(map()));
      /// use the setting name as the shortcut 'name'
      m_signalMapper->setMapping(sc,keys[i]);
    }
  }
  connect(m_signalMapper,SIGNAL(mapped(QString)),this,SLOT(shortcut(const QString &)));
  settings->endGroup();
  delete settings;
}



/**
 * load the application level stylesheet, stripping out lines
 * beginning with #
 *
 */
void LanesLexicon::loadStyleSheet() {
  /// TODO from variable setting by QSettings
  QFile f("app.css");
  if ( ! f.open(QIODevice::ReadOnly)) {
    QLOG_WARN() << "Unable to open stylesheet";
    return;
  }
  QString css;
  QTextStream in(&f);
  in.setCodec("UTF-8");
  QString t;
  while(! in.atEnd()) {
    t = in.readLine();
    if (! t.startsWith("#")) {
      css += t;
    }
  }
  if (! css.isEmpty()) {
    qApp->setStyleSheet(css);
  }
}
/*
QAction * LanesLexicon::createIconAction(const QString imgdir,const QString & iconfile,const QString & text) {
  QAction * action;
  QFileInfo fi;
  fi.setFile(imgdir,iconfile);
  if ( ! iconfile.isEmpty() && fi.exists() ) {
    action = new QAction(QIcon(fi.absoluteFilePath()),text,this);
  }
  else {
    action = new QAction(text,this);

  }
  return action;
}
*/
void LanesLexicon::createActions() {
  /// TODO get this from QSettings
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  settings->setIniCodec("UTF-8");
  settings->beginGroup("Icons");

  m_exitAction = new QAction(tr("Exit"),this);
  connect(m_exitAction,SIGNAL(triggered()),this,SLOT(onExit()));

  m_clearHistoryAction = new QAction(tr("Clear"),this);
  connect(m_clearHistoryAction,SIGNAL(triggered()),this,SLOT(onClearHistory()));

  m_testAction = new QAction(tr("Test"),this);
  connect(m_testAction,SIGNAL(triggered()),this,SLOT(onTest()));
  /// probably need icons

  m_historyAction = new QAction(tr("History"),this);

  m_navNextAction = new QAction(tr("Next"),this);
  m_navPrevAction = new QAction(tr("Back"),this);
  m_navFirstAction = new QAction(tr("First"),this);
  m_navLastAction = new QAction(tr("Last"),this);

  connect(m_navNextAction,SIGNAL(triggered()),this,SLOT(onNavNext()));
  connect(m_navPrevAction,SIGNAL(triggered()),this,SLOT(onNavPrev()));
  connect(m_navFirstAction,SIGNAL(triggered()),this,SLOT(onNavFirst()));
  connect(m_navLastAction,SIGNAL(triggered()),this,SLOT(onNavLast()));


  m_docAction = new QAction(tr("Docs"),this);

  m_bookmarkAction = new QAction(tr("Bookmarks"),this);

  m_navigationAction = new QAction(tr("Move"),this);

  m_searchAction = new QAction(tr("Search"),this);



  m_navModeRootAction = new QAction(tr("By root"),this);
  m_navModeRootAction->setData(Lane::By_Root);
  m_navModeRootAction->setCheckable(true);
  if (m_navMode == Lane::By_Root) {
    m_navModeRootAction->setChecked(true);
  }
  m_navModePageAction = new QAction(tr("By page"),this);
  m_navModePageAction->setData(Lane::By_Page);
  m_navModePageAction->setCheckable(true);
  if (m_navMode == Lane::By_Page) {
    m_navModePageAction->setChecked(true);
  }
  connect(m_navModeRootAction,SIGNAL(triggered()),this,SLOT(onNavModeChanged()));
  connect(m_navModePageAction,SIGNAL(triggered()),this,SLOT(onNavModeChanged()));

  connect(m_docAction,SIGNAL(triggered()),this,SLOT(onDocs()));

  m_searchWordAction = new QAction(tr("For Arabic &word"),this);
  connect(m_searchWordAction,SIGNAL(triggered()),this,SLOT(searchForWord()));
  m_searchPageAction = new QAction(tr("For &page"),this);
  connect(m_searchPageAction,SIGNAL(triggered()),this,SLOT(searchForPage()));
  m_searchRootAction = new QAction(tr("For &root"),this);
  connect(m_searchRootAction,SIGNAL(triggered()),this,SLOT(searchForRoot()));
  m_searchNodeAction = new QAction(tr("For &node"),this);
  connect(m_searchNodeAction,SIGNAL(triggered()),this,SLOT(searchForNode()));

  m_zoomInAction = new QAction(tr("Zoom in"),this);
  m_zoomOutAction =  new QAction(tr("Zoom out"),this);
  m_widenAction = new QAction(tr("Widen text"),this);
  m_narrowAction = new QAction(tr("Narrow text"),this);
  m_printAction = new QAction(tr("Print"),this);
  m_localSearchAction = new QAction(tr("Search page"),this);
  m_clearAction = new QAction(tr("Clear"),this);
  m_convertToEntryAction = new QAction(tr("Convert"),this);
  m_clearAction->setEnabled(false);

  m_keymapsAction = new QAction(tr("Keymaps"),this);

  m_linkAction = new QAction(tr("Link contents"),this);
  m_linkAction->setCheckable(true);

  connect(m_zoomInAction,SIGNAL(triggered()),this,SLOT(pageZoomIn()));
  connect(m_zoomOutAction,SIGNAL(triggered()),this,SLOT(pageZoomOut()));
  connect(m_widenAction,SIGNAL(triggered()),this,SLOT(pageWiden()));
  connect(m_narrowAction,SIGNAL(triggered()),this,SLOT(pageNarrow()));
  connect(m_printAction,SIGNAL(triggered()),this,SLOT(pagePrint()));
  connect(m_localSearchAction,SIGNAL(triggered()),this,SLOT(pageSearch()));
  connect(m_clearAction,SIGNAL(triggered()),this,SLOT(pageClear()));
  connect(m_convertToEntryAction,SIGNAL(triggered()),this,SLOT(convertToEntry()));

  delete settings;
}
void LanesLexicon::createToolBar() {
  QToolBar * mainbar = addToolBar("Main");
  mainbar->setObjectName("maintoolbar");

  mainbar->addAction(m_exitAction);
  mainbar->addAction(m_testAction);

  //  QToolBar * history = addToolBar(tr("History"));
  //  history->setObjectName("historytoolbar");
  m_hBackwardBtn = new QToolButton(mainbar);
  m_hBackwardBtn->setText(tr("History"));
  m_hBackwardBtn->setDefaultAction(m_historyAction);
  m_hBackwardBtn->setPopupMode(QToolButton::MenuButtonPopup);
  m_hBackwardBtn->setEnabled(false);
  m_hBackwardBtn->setMenu(m_historyMenu);
  m_hBackwardBtn->setFocusPolicy(Qt::ClickFocus);
  mainbar->addWidget(m_hBackwardBtn);
  //  history->addSeparator();

  //  QToolBar * bookmarks = addToolBar(tr("Bookmarks"));
  //  bookmarks->setObjectName("bookmarkstoolbar");
  m_bookmarkBtn = new QToolButton(mainbar);
  m_bookmarkBtn->setDefaultAction(m_bookmarkAction);
  m_bookmarkBtn->setText(tr("Bookmarks"));
  m_bookmarkBtn->setPopupMode(QToolButton::MenuButtonPopup);
  m_bookmarkBtn->setEnabled(true);
  m_bookmarkBtn->setMenu(m_bookmarkMenu);
  mainbar->addWidget(m_bookmarkBtn);

  //  QToolBar * search = addToolBar(tr("Search"));
  //  search->setObjectName("searchtoolbar");
  m_searchButton = new QToolButton(mainbar);
  m_searchButton->setDefaultAction(m_searchAction);
  m_searchButton->setText(tr("Search"));
  m_searchButton->setPopupMode(QToolButton::MenuButtonPopup);
  m_searchButton->setEnabled(true);
  m_searchButton->setMenu(m_searchMenu);
  mainbar->addWidget(m_searchButton);


  //  QToolBar * docs = addToolBar("&Docs");
  //  docs->setObjectName("docstoolbar");
  mainbar->addAction(m_docAction);
  mainbar->setFloatable(true);
  mainbar->setIconSize(m_toolbarIconSize);
  //  addToolBarBreak();
  QToolBar * navigation = addToolBar(tr("Navigation"));
  navigation->setObjectName("navigationtoolbar");
  navigation->setIconSize(m_toolbarIconSize);
  m_navText = new QLabel("");
  if (m_navMode == Lane::By_Root) {
    m_navText->setText(tr("Root"));
  }
  else {
    m_navText->setText(tr("Page"));
  }

  QToolButton * m_navBtn = new QToolButton(navigation);
  m_navBtn->setDefaultAction(m_navigationAction);
  m_navBtn->setText(tr("Move"));


  /// TODO should we reuse the m_navMode{Root,Page}Action

  QActionGroup * group = new QActionGroup(this);

  QAction * action = group->addAction(tr("By root"));
  action->setCheckable(true);
  if (m_navMode == Lane::By_Root) {
    action->setChecked(true);
  }
  else {
    action->setChecked(false);
  }
  action->setData(QVariant(0));//event->getId());
  connect(action,SIGNAL(triggered()),this,SLOT(onNavModeChanged()));

  action = group->addAction(tr("By page"));
  action->setCheckable(true);

  if (m_navMode == Lane::By_Page) {
    action->setChecked(true);
  }
  else {
    action->setChecked(false);
  }
  action->setData(QVariant(1));//event->getId());
  connect(action,SIGNAL(triggered()),this,SLOT(onNavModeChanged()));
  QMenu * m = new QMenu;
  m->addActions(group->actions());
  m_navBtn->setEnabled(true);
  m_navBtn->setMenu(m);
  m_navBtn->setPopupMode(QToolButton::MenuButtonPopup);
  navigation->addWidget(m_navBtn);
  navigation->addAction(m_navFirstAction);
  navigation->addAction(m_navNextAction);
  navigation->addAction(m_navPrevAction);
  navigation->addAction(m_navLastAction);
  navigation->addSeparator();
  navigation->setFloatable(true);

  //  addToolBarBreak();

  QToolBar * page = addToolBar(tr("&Page"));
  page->setObjectName("pagetoolbar");
  page->setIconSize(m_toolbarIconSize);
  page->addAction(m_zoomInAction);
  page->addAction(m_zoomOutAction);
  page->addAction(m_widenAction);
  page->addAction(m_narrowAction);
  page->addAction(m_printAction);
  page->addAction(m_localSearchAction);
  page->addAction(m_clearAction);
  page->addAction(m_convertToEntryAction);
  page->setFloatable(true);

}
/**
 * when user has done something that adds to history
 * rebuild the dropdown, unchecking everything
 *
 */
void LanesLexicon::historyAddition(const Place & p) {
  statusBar()->showMessage(tr("History added:") + p.getShortText());
  setupHistory(-1);
}
void LanesLexicon::historyPositionChanged(int pos) {
  setupHistory(pos);
}
/**
 *
 *
 * @param currPos  our current position in the history, -1 = at the end (default)
 */
void LanesLexicon::setupHistory(int currPos) {
  // get backward history
  /// TODO set 10 to something from the QSettings
  QLOG_DEBUG() << Q_FUNC_INFO << currPos;
  m_historyPos = currPos;
  QList<HistoryEvent *> events = m_history->getHistory();//10,0,currPos);
  qDebug() << Q_FUNC_INFO << "event size" << events.size();
  if (events.size() == 0) {
    m_hBackwardBtn->setEnabled(false);
    m_clearHistoryAction->setEnabled(false);
  }
  else {
    // m_historyMenu->clear();
    m_hBackwardBtn->setEnabled(true);
    m_clearHistoryAction->setEnabled(true);
  }
  QList<QAction *> actions = m_historyMenu->actions();
  qDebug() << Q_FUNC_INFO << "history actions size" << actions.size();
  actions.removeOne(m_clearHistoryAction);
  for(int i=0;i < actions.size();i++) {
    m_historyMenu->removeAction(actions[i]);
    delete actions[i];
  }
  if (events.size() == 0) {
    return;
  }
  QActionGroup * group = new QActionGroup(this);
  while(events.size() > 0) {
    HistoryEvent * event = events.takeFirst();
    Place p = event->getPlace();
    QString root = p.getRoot();
    QString word = p.getWord();
    int id = event->getId();

    QString txt;
    if (! word.isEmpty()) {
      txt = QString("%1 %2").arg(id).arg(word);
    }
    else {
      txt = QString("%1 %2").arg(id).arg(root);
    }
    QAction * action = group->addAction(txt);
    action->setCheckable(true);
    if (id == currPos) {
      action->setChecked(true);
    }
    else {
      action->setChecked(false);
    }
    //      m_historyPos = p.getId();
    action->setData(QVariant(p));//event->getId());
    connect(action,SIGNAL(triggered()),this,SLOT(onHistorySelection()));
    delete event;
  }
  m_historyMenu->addActions(group->actions());
    //    m->addActions(group);
  m_hBackwardBtn->setEnabled(true);
    //    m_hBackwardBtn->addActions(group->actions());//setMenu(m);
    //    m_hBackwardBtn->setMenu(m);
}
void LanesLexicon::createMenus() {
  m_mainmenu = new AppMenu(this);
  setMenuBar(m_mainmenu);
  m_fileMenu = menuBar()->addMenu(tr("&File"));
  m_fileMenu->addAction(m_exitAction);
  m_fileMenu->setObjectName("filemenu");

  m_bookmarkMenu = m_mainmenu->addMenu(tr("&Bookmark"));
  m_bookmarkMenu->setObjectName("bookmarkmenu");

  m_bookmarkMenu->addAction(m_bookmarkListAction);
  m_bookmarkMenu->addAction(m_bookmarkClearAction);
  m_bookmarkMenu->addAction(m_bookmarkRevertAction);
  m_bookmarkMenu->addAction(m_bookmarkAddAction);
  //  m_bookmarkMenu->addAction(m_bookmarkJumpAction);

  connect(m_mainmenu,SIGNAL(rebuildBookmarks()),this,SLOT(bookmarkRebuildMenu()));

  m_historyMenu = m_mainmenu->addMenu(tr("&History"));
  m_historyMenu->setObjectName("historymenu");
  m_historyMenu->addAction(m_clearHistoryAction);
  m_navMenu = m_mainmenu->addMenu(tr("&Navigation"));
  m_navMenu->setObjectName("navigationmenu");
  m_navMenu->addAction(m_navFirstAction);
  m_navMenu->addAction(m_navNextAction);
  m_navMenu->addAction(m_navPrevAction);
  m_navMenu->addAction(m_navLastAction);
  m_navMenu->addAction(m_navModeRootAction);
  m_navMenu->addAction(m_navModePageAction);

  m_searchMenu = m_mainmenu->addMenu(tr("&Search"));
  m_searchMenu->addAction(m_searchRootAction);
  m_searchMenu->addAction(m_searchWordAction);
  m_searchMenu->addAction(m_searchPageAction);
  m_searchMenu->addAction(m_searchNodeAction);
}

void LanesLexicon::createStatusBar() {
  m_navModeIndicator = new QLabel("");
  m_placeIndicator = new QLabel("");


  m_keymapsButton = new QToolButton(this);
  QStringList maps =  m_mapper->getMaps();
  maps << tr("None");
  QMenu * menu = new QMenu;
  for(int i=0;i < maps.size();i++) {
    QAction * action  = menu->addAction(maps[i]);
    action->setCheckable(true);
    action->setData(maps[i]);
    if (m_activeMap == maps[i]) {
      action->setChecked(true);
    }
    connect(action,SIGNAL(triggered()),this,SLOT(onKeymapChanged()));

  }
  if (maps.size() == 1) {
    m_keymapsButton->setEnabled(false);
  }
  m_keymapsButton->setDefaultAction(m_keymapsAction);
  m_keymapsButton->setMenu(menu);
  m_keymapsButton->setPopupMode(QToolButton::InstantPopup);


  m_linkButton = new QToolButton(this);
  m_linkButton->setDefaultAction(m_linkAction);
  m_linkButton->setEnabled(m_linkContents);



  connect(m_linkAction,SIGNAL(triggered()),this,SLOT(onLinkChanged()));

  statusBar()->addPermanentWidget(m_placeIndicator);
  statusBar()->addPermanentWidget(m_keymapsButton);
  statusBar()->addPermanentWidget(m_linkButton);
  statusBar()->addPermanentWidget(m_navModeIndicator);

  QString tip;
  if (m_keymapsEnabled) {
    tip = QString(tr("Keymaps, enabled"));
  }
  else {
    tip = QString(tr("Keymaps, disabled"));
  }

  m_keymapsButton->setToolTip(tip);


  updateStatusBar();
}
void LanesLexicon::onKeymapChanged() {
  QAction * action = qobject_cast<QAction *>(sender());
  if (! action)
    return;
  m_activeMap = action->data().toString();
  QList<QAction *> actions = m_keymapsButton->menu()->actions();
  for(int i=0;i < actions.size();i++) {
    if (actions[i]->data().toString() == m_activeMap) {
      actions[i]->setChecked(true);
    }
    else {
      actions[i]->setChecked(false);
    }
  }
  foreach (QWidget *widget, QApplication::allWidgets()) {
    ImLineEdit * w = qobject_cast<ImLineEdit *>(widget);
    if (w) {
      w->activateMap(m_activeMap,true);
    }
    else {
      ImEdit * imedit = qobject_cast<ImEdit *>(widget);
      if (imedit) {
        imedit->activateMap(m_activeMap,true);
      }
    }
  }
}
void LanesLexicon::onLinkChanged() {
  m_linkContents = ! m_linkContents;
  m_linkAction->setChecked(m_linkContents);
  if (m_linkContents) {
    m_linkAction->setText(tr("Contents linked"));
  }
  else {
    m_linkAction->setText(tr("Contents not linked"));
  }
}
QSize LanesLexicon::sizeHint() const {
  return QSize(800,600);
}
void LanesLexicon::onHistorySelection() {
  QAction * action = static_cast<QAction *>(QObject::sender());
  QVariant v = action->data();
  Place p = v.value<Place>();
  p.setType(Place::History);
  showPlace(p,false);

}
void LanesLexicon::onExit()
{
  if (m_db.isOpen()) {
    m_db.close();
  }
  close();
}
bool LanesLexicon::openDatabase(const QString & dbname) {
  QFile dbfile(dbname);
  if (! dbfile.exists()) {
    QLOG_WARN() << QString(tr("Cannot find database : %1")).arg(dbname);
    return false;
  }
  if (m_db.isOpen()) {
    m_db.close();
  }
  m_db = QSqlDatabase::addDatabase("QSQLITE");
  m_db.setDatabaseName(dbname);
  return m_db.open();
}
void LanesLexicon::rootClicked(QTreeWidgetItem * item,int /* column */) {
  int options = 0;
  QString root = item->text(0);
  QString supp = item->text(1);
  int p = 0;
  QLOG_DEBUG() << Q_FUNC_INFO << root << supp;
  /// check that the user has not clicked on a letter
  if (item->parent() == 0) {
    return;
  }
  /// TODO check this
  if (QApplication::keyboardModifiers() & Qt::ShiftModifier) {
      options |= Lane::Create_Tab;
  }
  /// check whether root or head word
  if  (item->parent()->parent() == 0) {
    m_tree->addEntries(root,item);
    if (supp == "*") {
      p = 1;
    }
    Place m(root,p);
    m.setType(Place::User);
    showPlace(m,options);
  }
  else {
    /// double clicked on head word, highlight it
    entryActivated(item,0);
  }
  /// TODO are these still needed ?
  if (m_treeKeepsFocus) {
    QLOG_DEBUG() << "tree focus";
    m_tree->setFocus();
  }

  if ( ! item->isExpanded() ) {
    item->setExpanded(true);
  }
}
/**
 * if the user clicks or hits space an entry (i.e. below a root) make
 * sure it is visible
 *
 * @param item
 * @param col
 */
void LanesLexicon::entryActivated(QTreeWidgetItem * item, int /* not used */) {
  int options = 0;
  /// ignore clicks on the root or the letter
  QLOG_DEBUG() << Q_FUNC_INFO;
  if ((item->parent() == 0) || (item->parent()->parent() == 0)) {
    return;
  }
 QString node = item->data(0,Qt::UserRole).toString();
 QLOG_DEBUG() << "node" << node;
 if ( node.isEmpty() )
   return;
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    if (entry->hasNode(node)) {
      entry->focusNode(node);
    }
    else {
      /// node not in the current tab
      int ix = this->searchTabs(node);
      if (ix != -1) {
        /// we found it in another tab, so switch to that one
        m_tabs->setCurrentIndex(ix);
        GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
        if (entry) {
          if (entry->hasNode(node)) {
            entry->focusNode(node);
          }
        }
      }
      else {
        /// fetch to node and show it
        Place p;
        p.setNode(node);
        ///
        /// TODO needs to check whether to open in new tab
        ///
        options |= Lane::Create_Tab;
        p = showPlace(p,options);
      }
    }
  }
  else {
    /// TODO fix this
    QLOG_DEBUG() << Q_FUNC_INFO << "No GraphicsEntry page available";
  }
}

void LanesLexicon::gotoPlace(const Place & p,int options) {
  showPlace(p,options);
}
Place LanesLexicon::showPlace(const Place & p,int options) {
  Place np;
  GraphicsEntry * entry;
  if (! p.isValid()) {
    return p;
  }
  int currentTab = m_tabs->currentIndex();
  if (currentTab == -1) {
    options |= Lane::Create_Tab;
  }
  else {
    /// if current widget not graphicsentry, set createtab
    entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(currentTab));
    if (! entry ) {
      options |= Lane::Create_Tab;
    }
  }
  if (options & Lane::Create_Tab) {
    /// turn history on as the user has clicked on something
    /// and the root is not already shown
    entry = new GraphicsEntry(this);
    setSignals(entry);
    entry->installEventFilter(this);
    np = entry->getXmlForRoot(p);

    int ix = m_tabs->insertTab(m_tabs->currentIndex()+1,entry,np.getShortText());
    if (options & Lane::Switch_Tab) {
      m_tabs->setCurrentIndex(ix);
    }
  }
  else {
    if (entry->hasPlace(p,GraphicsEntry::RootSearch,true) == -1) {
      np = entry->getXmlForRoot(p);
      m_tabs->setTabText(currentTab,np.getShortText());
      entry->setFocus();
      m_tabs->tabContentsChanged();
    }
    else {
      return p;
    }
  }
 return np;
}
/**
 * when user clicks on item reason Qt::MouseFocusReason
 * when setFocus called it is Qt::OtherFocusReason
 *
 * @param newFocus
 * @param oldFocus
 * @param reason
 **/
void LanesLexicon::focusItemChanged(QGraphicsItem * newFocus, QGraphicsItem * /* oldFocus */, Qt::FocusReason /* reason */) {
  EntryItem * item = dynamic_cast<EntryItem *>(newFocus);
  if (item) {
    Place p = item->getPlace();
    m_placeIndicator->setText(p.getText());
  }

}
/**
 * Ctrl-T moves focus to the tree
 * Ctrl-E moves focus to the current display widget
 * TODO move these to shortcuts
 * @param target
 * @param event
 *
 * @return
 */
bool LanesLexicon::eventFilter(QObject * target,QEvent * event) {
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
    switch(keyEvent->key()) {
      case Qt::Key_T: {
        if (keyEvent->modifiers() && Qt::ControlModifier) {
          m_tree->setFocus();
          return true;
        }
        break;
      }
      case Qt::Key_E: {
        if (keyEvent->modifiers() && Qt::ControlModifier) {
          if (target == m_tree)
          m_tabs->currentWidget()->setFocus();
          return true;
        }
        break;
      }
    default:
      break;
    }
  }
  return QMainWindow::eventFilter(target,event);
}
void LanesLexicon::onTest() {
  //  QKeySequenceEdit * w = new QKeySequenceEdit;
  //  w->show();
  if (0) {
    SearchOptions * s = new SearchOptions(Lane::Word);
    s->addKeymaps("map1",QStringList() << "map0" << "map1" << "map2");
    m_tabs->addTab(s,"Test");
    s->setOptions(Lane::Create_Tab | Lane::Regex_Search | Lane::Arabic);
    //    s->setOptions(0);
    ArabicSearchDialog * d = new ArabicSearchDialog(Lane::Word);
    d->exec();
  }
  if (0) {
    FullSearchWidget * w = new FullSearchWidget(this);
    connect(w,SIGNAL(showNode(const QString &)),this,SLOT(showSearchNode(const QString &)));
    int c = this->getSearchCount();
    m_tabs->addTab(w,QString(tr("Search %1")).arg(c+1));;
  }
  foreach (QWidget *widget, QApplication::allWidgets()) {
    ImLineEdit * w = qobject_cast<ImLineEdit *>(widget);
    if (w) {
      qDebug() << "Found linedit";
    }
  }
  if (0) {
    HeadSearchWidget * w = qobject_cast<HeadSearchWidget *>(m_tabs->currentWidget());
    if (w) {
      Place p = w->getEntry()->getPlace();
      if (p.isValid()) {
        int ix = m_tabs->currentIndex();
        this->onCloseTab(ix);
        GraphicsEntry * entry = new GraphicsEntry(this);
        setSignals(entry);
        entry->installEventFilter(this);
        entry->getXmlForRoot(p);
        m_tabs->insertTab(ix,entry,p.getShortText());
        m_tabs->setCurrentIndex(ix);
      }
      else {
        qDebug() << "could not clone graphcicsentry";
      }
    }
  }
}
/**
 * Read settings from INIFILE (by default : "default.ini");
 * Options can also come from the command line and they override settings
 * in the ini file
 */
void LanesLexicon::readSettings() {
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QMap<QString,QString> cmdOptions = app->getOptions();

  m_startupNode = cmdOptions.value("node");
  if (! m_startupNode.isEmpty() && ! m_startupNode.startsWith("n")) {
    m_startupNode = "n" + m_startupNode;
  }
  m_startupRoot = cmdOptions.value("root");
  QScopedPointer<QSettings> settings(app->getSettings());
  //  QSettings * settings = app->getSettings();
  settings->setIniCodec("UTF-8");
  settings->beginGroup("System");
  m_dbName = settings->value("Database","lexicon.sqlite").toString();
  if (cmdOptions.contains("db")) {
    m_dbName = cmdOptions.value("db");
  }
  QString ar = settings->value("Arabic font").toString();
  if (! ar.isEmpty()) {
    arFont.fromString(ar);
  }
  m_iconTheme = settings->value("Theme",QString()).toString();

  m_saveSettings = settings->value("Save settings",true).toBool();
  if (cmdOptions.contains("nosave")) {
    m_saveSettings = false;
  }
  m_saveTabs = settings->value("Save tabs",true).toBool();

  m_restoreTabs = settings->value("Restore tabs",true).toBool();
  if (cmdOptions.contains("notabs")) {
    m_restoreTabs = false;
  }

  m_saveBookmarks = settings->value("Save bookmarks",true).toBool();
  m_restoreBookmarks = settings->value("Restore bookmarks",true).toBool();

  m_docked = settings->value("Docked",false).toBool();
  m_interface = settings->value("Interface","default").toString();
  m_navigationMode = settings->value("Navigation","root").toString();

  m_activeMap = settings->value("Default map","Buckwalter").toString();
  if (m_navigationMode.toLower() == "root") {
    m_navMode = Lane::By_Root;
  }
  else {
    m_navMode = Lane::By_Page;
  }
  QString title = settings->value("Title",tr("Lane's Arabic-English Lexicon")).toString();
  this->setWindowTitle(title);

  m_toolbarIconSize = settings->value("Icon size",QSize(16,16)).toSize();

  m_keymapsEnabled = settings->value("Keymaps",false).toBool();

  m_linkContents = settings->value("Contents linked",false).toBool();

  settings->endGroup();

  settings->beginGroup("Search");
  m_searchNewTab = settings->value("New tab",true).toBool();
  m_searchSwitchTab = settings->value("Switch tab",true).toBool();
  m_defaultSearchOptions = 0;
  QString v;
  v  = settings->value("Type",QString("normal")).toString();
  if (v == "normal")
    m_defaultSearchOptions |= Lane::Normal_Search;
  else
    m_defaultSearchOptions |= Lane::Regex_Search;

  v = settings->value("Ignore diacritics",QString("yes")).toString();
  if (v == "yes")
    m_defaultSearchOptions |= Lane::Ignore_Diacritics;

  v = settings->value("Whole word",QString("no")).toString();
  if (v == "yes")
    m_defaultSearchOptions |= Lane::Whole_Word;

  v = settings->value("Target",QString("Arabic")).toString();

  if (v == "Arabic")
    m_defaultSearchOptions |= Lane::Arabic;
  else
    m_defaultSearchOptions |= Lane::Buckwalter;

  if (m_keymapsEnabled)
    m_defaultSearchOptions |= Lane::Keymaps_Enabled;

  settings->endGroup();

  settings->beginGroup("FullSearch");
  if (settings->value("Include heads",false).toBool()) {
    m_defaultSearchOptions |= Lane::Include_Heads;

  }
  settings->endGroup();

  settings->beginGroup("Debug");
  m_valgrind = settings->value("Valgrind",false).toBool();
  settings->endGroup();

  settings->beginGroup("Notes");
  m_notesDbName = settings->value("Database","notes.sqlite").toString();
  m_useNotes = settings->value("Enabled",true).toBool();
  settings->endGroup();
  settings->beginGroup("History");
  m_historyEnabled = settings->value("Enabled",true).toBool();
  m_historyDbName = settings->value("Database","history.sqlite").toString();
  settings->endGroup();

  settings->beginGroup("Roots");
  m_treeKeepsFocus = settings->value("Keep focus",true).toBool();
  settings->endGroup();

  /**
   * we are have a default map set that is used to convert input to Arabic
   *
   */

  settings->beginGroup("Maps");
  QStringList groups = settings->childGroups();
  if ( ! groups.contains(m_activeMap) ) {
    QLOG_WARN() << QString(tr("Default map <%1> not found in settings")).arg(m_activeMap);
    return;
  }
  settings->beginGroup(m_activeMap);
  QString filename = settings->value("file",QString()).toString();
  QFile file(filename);
  if ( file.exists() ) {
    if (! im_load_map_from_json(m_mapper,filename.toUtf8().constData(),m_activeMap.toUtf8().constData())) {
      QLOG_WARN() << QString(tr("Could not load <%1> from file <%2>")).arg(m_activeMap).arg(filename);
    }
  }
  else {
    QLOG_WARN() << QString(tr("Could not load <%1> from file <%2> - file not found")).arg(m_activeMap).arg(filename);
  }
  settings->endGroup();
  settings->endGroup();

}
void LanesLexicon::writeSettings() {
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QScopedPointer<QSettings> settings(app->getSettings());
  settings->setIniCodec("UTF-8");

  if (! m_saveSettings )
    return;


  if (m_saveTabs) {
    settings->beginGroup("Tabs");
    settings->remove("");
    for(int i=0;i < m_tabs->count();i++) {
      GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(i));
      if (entry) {
        Place p = entry->getPlace();
        settings->beginGroup(QString("Tab-%1").arg(i));
        settings->setValue("type","entry");
        settings->setValue("place",p.toString());
        settings->setValue("zoom",entry->getScale());
        settings->setValue("textwidth",entry->getTextWidth());
        settings->endGroup();
      }
      else {
        NoteBrowser * browser = qobject_cast<NoteBrowser *>(m_tabs->widget(i));
        if (browser) {
          settings->beginGroup(QString("Tab-%1").arg(i));
          settings->setValue("type","notes");
          settings->endGroup();
        }
        else {
          /// could be search results
        }
      }
    }
    settings->endGroup();
    settings->beginGroup("System");
    settings->setValue("Focus tab",m_tabs->currentIndex());
    settings->endGroup();
  }

  if (m_saveBookmarks) {
    settings->beginGroup("Bookmarks");
    settings->remove("");
    QStringList keys = m_bookmarks.keys();
    for(int i=0;i < keys.size();i++) {
      Place p = m_bookmarks.value(keys[i]);
      settings->setValue(keys[i],p.toString());
    }
    settings->endGroup();
  }
  settings->beginGroup("Main");
  settings->setValue("State",this->saveState());
  settings->setValue("Geometry", saveGeometry());
  settings->endGroup();
}
void LanesLexicon::restoreTabs() {
  bool ok;
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QMap<QString,QString> cmdOptions = app->getOptions();

  QSettings * settings = app->getSettings();

  settings->setIniCodec("UTF-8");
  settings->beginGroup("System");
  int focusTab =  settings->value("Focus tab",0).toInt();
  settings->endGroup();
  int textWidth;
  settings->beginGroup("Entry");
  textWidth = settings->value("Text width",400).toInt(&ok);
  if (!ok) {
    textWidth = 400;
  }
  if (cmdOptions.contains("textwidth")) {
    int w = cmdOptions.value("textwidth").toInt(&ok);
    if (ok) {
      textWidth = w;
    }
  }
  settings->endGroup();
  Place wp;
  settings->beginGroup("Tabs");
  QStringList tabs = settings->childGroups();
  /// restore tab may fail, so we need to keep count of the actual tabs
  int j = 0;
  for(int i=0;i < tabs.size();i++) {
    settings->beginGroup(tabs[i]);
    if (settings->value("type","entry").toString() == "notes") {
      NoteBrowser * notes = new NoteBrowser(this);
      m_tabs->addTab(notes,tr("Notes"));
      j++;
    }
    else {
      QString v = settings->value("place",QString()).toString();
      qreal scale = settings->value("zoom",1.0).toReal(&ok);
      if ( !ok ) {
        scale = 1.0;
      }
      Place p = Place::fromString(v);
      qDebug() << "restoring" << v << p.isValid();
      int tw;
      if (! cmdOptions.contains("textwidth")) {
        tw = settings->value("textwidth",textWidth).toInt(&ok);
        if (!ok) {
          tw = textWidth;
        }
      }
      else {
        tw = textWidth;
      }
      if (p.isValid()) {
        if (focusTab == i) {
          focusTab = j;
          wp = p;
        }
        GraphicsEntry * entry = new GraphicsEntry(this);
        entry->setTextWidth(tw);
        setSignals(entry);
        p.setType(Place::RestoreTab);
        entry->getXmlForRoot(p);
        entry->setScale(scale);
        //      QLOG_DEBUG() << Q_FUNC_INFO << "adding tab" << p.getShortText();
        m_tabs->addTab(entry,p.getShortText());
        m_tree->ensurePlaceVisible(p,true);
        j++;
      }
      else {
        QLOG_DEBUG() << Q_FUNC_INFO << "invalid place" << p;
      }
    }
    settings->endGroup();
  }
  settings->endGroup();
  wp.setType(Place::RestoreTab);
  if (focusTab < m_tabs->count()) {
    m_tabs->setCurrentIndex(focusTab);
    GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
    if (entry)
      entry->focusPlace();
    /// sync the current tab with the tree
    //QLOG_DEBUG() << "Synching tab" << focusTab;
    currentTabChanged(focusTab);
  }
  delete settings;
}

HistoryMaster * LanesLexicon::history() {
  return m_history;
}
NoteMaster * LanesLexicon::notes() {
  return m_notes;
}
/**
 * this needs to be fixed to return supplement as well
 *
 * @param root
 */
void LanesLexicon::findNextRoot(const QString & root) {
  GraphicsEntry * entry = dynamic_cast<GraphicsEntry *>(QObject::sender());
  QString nroot = m_tree->findNextRoot(root);
  if (entry && ! nroot.isEmpty()) {
    entry->setPagingForward();
    Place p;
    p.setRoot(nroot);
    entry->getXmlForRoot(p);
  }
}
void LanesLexicon::findPrevRoot(const QString & root) {
  GraphicsEntry * entry = dynamic_cast<GraphicsEntry *>(QObject::sender());
  QString nroot = m_tree->findPrevRoot(root);
  if (entry && ! nroot.isEmpty()) {
    entry->setPagingBackward();
    entry->getXmlForRoot(Place(nroot));
  }
}
/**
 * user clicks on next root button
 *  - fetch the current root from GraphicsEntry page
 *  - query the ContentsWidget for the next root
 *  - if found
 *    tell the GraphicsEntry page to either show the root or fetch it and show it
 *
 */
void LanesLexicon::onNextRoot() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    Place p  =  entry->getPlace();
    Place np = m_tree->findNextPlace(p);
    if (! np.isEmpty()) {
      /// if it is, it move focus to it (if true is 2nd param)
      if (entry->hasPlace(np,GraphicsEntry::RootSearch,true) == -1) {
        entry->setPagingForward();
        entry->getXmlForRoot(np);
        m_tree->ensurePlaceVisible(np,true);
      }
    }
    else {
      QLOG_DEBUG() << "No next root for " << p.getRoot();
    }
  }
}
void LanesLexicon::onPrevRoot() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    Place p  =  entry->getPlace();
    Place np = m_tree->findPrevPlace(p);
    if (! np.isEmpty()) {
      /// if it is, it move focus to it (if true is 2nd param)
      if (entry->hasPlace(np,GraphicsEntry::RootSearch,true) == -1) {
        entry->setPagingBackward();
        entry->getXmlForRoot(np);
        m_tree->ensurePlaceVisible(np,true);
      }
    }
    else {
      QLOG_DEBUG() << "No prev root for " << p.getRoot();
    }
  }
}
void LanesLexicon::onFirstRoot() {
  int options = 0;
  Place p;
  p.setRoot(m_firstRoot);
  showPlace(p,options);
  m_tree->ensurePlaceVisible(p);
}
void LanesLexicon::onLastRoot() {
  int options = 0;
  Place p;
  p.setRoot(m_lastRoot);
  showPlace(p,options);
  m_tree->ensurePlaceVisible(p);
}
void LanesLexicon::rootChanged(const QString & root,const QString & /* node */) {
  Place p;
  p.setRoot(root);
  m_tree->ensurePlaceVisible(p,true);
}
void LanesLexicon::placeChanged(const Place & p) {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(QObject::sender());
  if (entry) {
    int ix = m_tabs->indexOf(entry);
    if (ix != -1) {
      m_tabs->setTabText(ix,p.getShortText());
    }
  }

  m_tree->ensurePlaceVisible(p,true);
}
void LanesLexicon::getFirstAndLast() {
  QSqlQuery query;
  bool ok = query.prepare("select root,page from entry where datasource = 1 order by nodenum limit 5;");
  if (! ok ) {
    QLOG_DEBUG() << "first root SQL prepare failed";
  }
  query.exec();
  if (query.first()) {
    m_firstRoot = query.value(0).toString();
    m_firstPage = query.value(1).toInt();
  }
  ok = query.prepare("select root,page from entry where datasource = 1 order by nodenum desc limit 5;");
  if (! ok ) {
    QLOG_DEBUG() << "last root SQL prepare failed";
  }
  query.exec();
  if (query.first()) {
    m_lastRoot = query.value(0).toString();
    m_lastPage = query.value(1).toInt();
  }
}
void LanesLexicon::onGoToPage(const Place & p) {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    entry->setPagingForward();

    entry->getPage(p);
    //        m_tree->ensurePlaceVisible(np,true);
  }
}
void LanesLexicon::onNextPage() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    Place p = entry->getPlace();
    int page = p.getPage();
    if (page == -1) {
      page = 0;
    }
    p.setPage(page + 1);
    entry->setPagingForward();
    entry->getPage(p);
    //        m_tree->ensurePlaceVisible(np,true);
  }
}
void LanesLexicon::onPrevPage() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    Place p = entry->getPlace();
    int page = p.getPage();
    if (page < 0) {
      page = 1;
    }
    if (page == 1) {
      return;
    }
    p.setPage(page - 1);
    entry->setPagingForward();
    entry->getPage(p);
    //        m_tree->ensurePlaceVisible(np,true);
  }
}
void LanesLexicon::onFirstPage() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    entry->setPagingForward();
    Place p;
    p.setPage(m_firstPage);
    entry->getPage(p);
    //        m_tree->ensurePlaceVisible(np,true);
  }
}
void LanesLexicon::onLastPage() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    entry->setPagingForward();
    Place p;
    p.setPage(m_lastPage);
    entry->getPage(p);
    //        m_tree->ensurePlaceVisible(np,true);
  }
}
/**
 * Converts supplied param to Arabic using the default map
 * unrecognised characters are left in place
 * @param s
 *
 * @return the equivalent arabic string
 */
QString LanesLexicon::convertString(const QString & s) const {
  if ( m_activeMap.isEmpty()) {
    return s;
  }
  if (UcdScripts::isScript(s,"Arabic")) {
    return s;
  }
  bool ok;
  QString t = im_convert_string(m_mapper,m_activeMap,s,&ok);
  if ( ! ok ) {
    QLOG_INFO() << QString(tr("Error converting <%1> with map <%2>")).arg(s).arg(m_activeMap);
  }
  return t;
}
Place LanesLexicon::getCurrentPlace() {
  Place p;
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    p = entry->getPlace();
  }
  return p;
}
/************************************************************************************
*
*
*    BOOKMARKS
*
*
************************************************************************************/

/**
 * Possible values: revert,list,jump,add,clear
 *
 * @param key
 */
void LanesLexicon::bookmarkShortcut(const QString & key) {
  int options = 0;
  if (m_revertEnabled && (key == "revert")) {
    if (! m_bookmarks.contains("-here-")) {
      return;
    }
    Place p = m_bookmarks.value("-here-");
    showPlace(p,options);
    m_revertEnabled = false;
    return;
  }
  if (key == "list") {
    BookmarkWidget * dlg = new BookmarkWidget(m_bookmarks,this);
    if (dlg->exec() == QDialog::Accepted) {
      QString m = dlg->getSelected();
      delete dlg;
      if ( ! m.isEmpty()) {
        bookmarkJump(m);
      }
    }
    return;
  }
  if (key == "clear") {
    /// TODO warning dialog ?
    bookmarkClear();
    /// TODO clear menu
    return;
  }
  QRegExp rx("([^-]+)-(.+)");
  QString v;
  QString id;
  if (rx.indexIn(key, 0) != -1) {
    v = rx.cap(1);
    id = rx.cap(2);
  }
  else {
    QLOG_WARN() << tr("Unknown bookmark shortcut activated") << key;
    return;
  }
  if (v == "jump") {
    bookmarkJump(id);
    return;
  }
  /// add a bookmark
  Place p = this->getCurrentPlace();
  if (! p.isValid()) {
    QLOG_WARN() << QString(tr("No place to bookmark"));
    return;
  }
  p.setType(Place::Bookmark);
  m_bookmarks.insert(id,p);
  addBookmarkMenuItem(id);
  setStatus(QString(tr("added bookmark (%1) at %2")).arg(id).arg(p.getShortText()));
}
/**
 * we've added a bookmark so we need to add the jump-<id> shortcut to the
 * menu
 *
 * @param action
 * @param id
 */
void LanesLexicon::addBookmarkMenuItem(const QString & id) {
  if (id == "-here-") {
    return;
  }
  QString ks = QString("jump-%1").arg(id);
  QShortcut * sc = qobject_cast<QShortcut *>(m_bookmarkMap->mapping(ks));
  if (sc) {
    QList<QAction *> actions  = m_bookmarkMenu->actions();
    int ix = -1;
    for(int i=0;i < actions.size();i++) {
      if (actions[i]->shortcut() == sc->key()) {
        ix = i;
        i = actions.size();
      }
    }
    if (ix != -1) {
      m_bookmarkMenu->removeAction(actions[ix]);
    }
    Place p = m_bookmarks.value(id);
    QAction * action = m_bookmarkMenu->addAction(p.getText());
    action->setShortcut(sc->key());
    action->setShortcutContext(Qt::WidgetShortcut);
    connect(action,SIGNAL(triggered()),sc,SIGNAL(activated()));
  }
  else {
    QLOG_DEBUG() << Q_FUNC_INFO << "fetch mapping failed " << m_bookmarkMap->mapping(id) << id;
  }
  //  connect(m_bookmarkMap,SIGNAL(mapped(QString())triggered()),m_bookmarkMap,SLOT(map()));
  //  m_bookmarkMap->setMapping(sc,QString("jump-%1").arg(ids.at(i)));

}
void LanesLexicon::bookmarkJump(const QString & id) {
  int options = 0;
  if ( ! m_bookmarks.contains(id) ) {
    QLOG_WARN() << QString(tr("Unknown bookmark jump activated: %1")).arg(id);
    return;
  }
  Place p = m_bookmarks.value(id);
  if (! p.isValid()) {
    QLOG_WARN() << QString(tr("Jumping to invalid place:")) << p;
    return;
  }
  /// get the current place and save it so we can jump back
  Place cp = this->getCurrentPlace();
  cp.setType(Place::Bookmark);
  m_bookmarks.insert("-here-",cp);
  showPlace(p,options);
  m_revertEnabled = true;
  updateMenu();
}
void LanesLexicon::restoreBookmarks() {
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  settings->setIniCodec("UTF-8");
  settings->beginGroup("Bookmarks");
  QStringList keys = settings->childKeys();
  for(int i=0;i < keys.size();i++) {
    QString t = settings->value(keys[i]).toString();
    Place p = Place::fromString(t);
    if (p.isValid()) {
      m_bookmarks.insert(keys[i],p);
      addBookmarkMenuItem(keys[i]);
    }
  }
  delete settings;
}
void LanesLexicon::setupBookmarkShortcuts() {
  /**
   * setup all the bookmark shortcuts and actions
   *   add,jump,save,revert,list
   *
   *  TODO bookmark Icons ?
   */
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();

  settings->setIniCodec("UTF-8");
  settings->beginGroup("Bookmark");
  m_bookmarkMap = new QSignalMapper(this);
  QString key = settings->value("Add","Ctrl+B").toString();
  QString ids = settings->value("Id","abcdefghijklmnopqrstuvwxyz").toString();
  for(int i=0;i < ids.size();i++) {
    QString ks = QString("%1,%2").arg(key).arg(ids.at(i).toLower());
    //    QString ks = QString("%1,%2").arg(key).arg(ids.at(i).toUpper());
    QShortcut * sc = new QShortcut(ks,this);
    sc->setContext(Qt::ApplicationShortcut);
    connect(sc,SIGNAL(activated()),m_bookmarkMap,SLOT(map()));
    m_bookmarkMap->setMapping(sc,QString("add-%1").arg(ids.at(i).toLower()));
  }
  for(int i=0;i < ids.size();i++) {
    QString ks = QString("%1,%2").arg(key).arg(ids.at(i).toUpper());
    QShortcut * sc = new QShortcut(ks,this);
    sc->setContext(Qt::ApplicationShortcut);
    connect(sc,SIGNAL(activated()),m_bookmarkMap,SLOT(map()));
    m_bookmarkMap->setMapping(sc,QString("add-%1").arg(ids.at(i).toUpper()));
  }
  m_bookmarkAddAction = new QAction(tr("Add"),this);

  key = settings->value("Jump","Ctrl+J").toString();
  for(int i=0;i < ids.size();i++) {
    QString ks = QString("%1,%2").arg(key).arg(ids.at(i).toLower());
    QShortcut * sc = new QShortcut(ks,this);
    connect(sc,SIGNAL(activated()),m_bookmarkMap,SLOT(map()));
    m_bookmarkMap->setMapping(sc,QString("jump-%1").arg(ids.at(i).toLower()));
  }
  for(int i=0;i < ids.size();i++) {
    QString ks = QString("%1,%2").arg(key).arg(ids.at(i).toUpper());
    QShortcut * sc = new QShortcut(ks,this);
    connect(sc,SIGNAL(activated()),m_bookmarkMap,SLOT(map()));
    m_bookmarkMap->setMapping(sc,QString("jump-%1").arg(ids.at(i).toUpper()));
  }
  m_bookmarkJumpAction = new QAction(tr("Jump"),this);

  QShortcut * sc;
  key = settings->value("List","Ctrl+B,Ctrl+L").toString();
  sc = new QShortcut(key,this);
  connect(sc,SIGNAL(activated()),m_bookmarkMap,SLOT(map()));
  connect(sc,SIGNAL(activatedAmbiguously()),m_bookmarkMap,SLOT(map()));
  sc->setContext(Qt::ApplicationShortcut);
  m_bookmarkMap->setMapping(sc,QString("list"));
  m_bookmarkListAction = new QAction(tr("List"),this);
  m_bookmarkListAction->setShortcut(sc->key());
  m_bookmarkListAction->setShortcutContext(Qt::WidgetShortcut);
  connect(m_bookmarkListAction,SIGNAL(triggered()),sc,SIGNAL(activated()));

  key = settings->value("Revert","Ctrl+B,Ctrl+R").toString();
  sc = new QShortcut(key,this);
    sc->setContext(Qt::ApplicationShortcut);
  connect(sc,SIGNAL(activated()),m_bookmarkMap,SLOT(map()));
  m_bookmarkMap->setMapping(sc,QString("revert"));
  m_bookmarkRevertAction = new QAction(tr("Revert"),this);
  m_bookmarkRevertAction->setShortcut(sc->key());
  m_bookmarkRevertAction->setShortcutContext(Qt::WidgetShortcut);
  connect(m_bookmarkRevertAction,SIGNAL(triggered()),sc,SIGNAL(activated()));

  key = settings->value("Clear","Ctrl+B,Ctrl+C").toString();
  sc = new QShortcut(key,this);
    sc->setContext(Qt::ApplicationShortcut);
  connect(sc,SIGNAL(activated()),m_bookmarkMap,SLOT(map()));
  m_bookmarkMap->setMapping(sc,QString("clear"));
  m_bookmarkClearAction = new QAction(tr("Clear"),this);
  m_bookmarkClearAction->setShortcut(sc->key());
  m_bookmarkClearAction->setShortcutContext(Qt::WidgetShortcut);
  connect(m_bookmarkClearAction,SIGNAL(triggered()),sc,SIGNAL(activated()));
  connect(m_bookmarkMap,SIGNAL(mapped(QString)),this,SLOT(bookmarkShortcut(const QString &)));

  connect(m_bookmarkAddAction,SIGNAL(triggered()),this,SLOT(bookmarkAdd()));
  delete settings;
}

void LanesLexicon::bookmarkAdd() {
  bool ok;
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (! entry) {
    return;
  }
  Place p = entry->getPlace();
  QString title = tr("Add bookmark to ") + p.getShortText();

  QString text = QInputDialog::getText(this, title,
                                         tr("Bookmark ID:"), QLineEdit::Normal,
                                         QString(), &ok);
  if (ok && !text.isEmpty())
    bookmarkShortcut("add-" + text);

}
/*
void LanesLexicon::bookmarkJump() {
  bool ok;
  QString text = QInputDialog::getText(this, tr("Bookmark Jump"),
                                         tr("Bookmark ID:"), QLineEdit::Normal,
                                         QString(), &ok);
  if (ok && !text.isEmpty())
    bookmarkShortcut("jump-" + text);
}
*/
/**
 * remove the menu items and then clear the map
 *
 */
void LanesLexicon::bookmarkClear() {
  QStringList keys = m_bookmarks.keys();
  for(int i=0;i < keys.size();i++) {
    QString ks = QString("jump-%1").arg(keys[i]);
    QShortcut * sc = qobject_cast<QShortcut *>(m_bookmarkMap->mapping(ks));
    if (sc) {
      QList<QAction *> actions  = m_bookmarkMenu->actions();
      int ix = -1;
      for(int j=0;j < actions.size();j++) {
        if (actions[j]->shortcut() == sc->key()) {
          ix = j;
          j = actions.size();
        }
      }
      if (ix != -1) {
        m_bookmarkMenu->removeAction(actions[ix]);
      }
    }
  }
  m_bookmarks.clear();
  setStatus(tr("Bookmarks cleared"));
}
void LanesLexicon::bookmarkRebuildMenu() {
  QLOG_DEBUG() << Q_FUNC_INFO;
}
void LanesLexicon::bookmarkAdd(const QString & id,const Place & p) {
  //  p.setType(Place::Bookmark);
  m_bookmarks.insert(id,p);
  addBookmarkMenuItem(id);

}

void LanesLexicon::moveNext(const Place & p) {
  if (m_navMode == Lane::By_Root) {
    findNextRoot(p.getRoot());
  }
  else {
    onNextPage();
  }
}
void LanesLexicon::movePrevious(const Place & p) {
  if (m_navMode == Lane::By_Root) {
    findPrevRoot(p.getRoot());
  }
  else {
    onPrevPage();
  }
}
void LanesLexicon::setStatus(const QString & txt) {
  statusBar()->showMessage(txt);
}
void LanesLexicon::updateStatusBar() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    m_placeIndicator->setText(entry->getPlace().getText());
  }
  else {
    m_placeIndicator->setText("");

  }
  if (m_navMode == Lane::By_Root) {
    m_navModeIndicator->setText(tr("Nav mode: by root"));
    m_navText->setText(tr("Root"));
  }
  else {
    m_navModeIndicator->setText(tr("Nav mode: by page"));
    m_navText->setText(tr("Page"));
  }
  m_keymapsButton->setEnabled(m_keymapsEnabled);
}
void LanesLexicon::updateMenu() {
  if (m_navMode == Lane::By_Root) {
    m_navModeRootAction->setChecked(true);
  }
  else {
    m_navModeRootAction->setChecked(false);
  }
  if (m_navMode == Lane::By_Page) {
    m_navModePageAction->setChecked(true);
  }
  else {
    m_navModePageAction->setChecked(false);
  }
  if (m_revertEnabled) {
    m_bookmarkRevertAction->setEnabled(true);
  }
  else {
    m_bookmarkRevertAction->setEnabled(false);
  }
}
void LanesLexicon::onNavNext() {
  if (m_navMode == Lane::By_Root) {
    onNextRoot();
  }
  else {
    onNextPage();
  }
}
void LanesLexicon::onNavPrev()  {
  if (m_navMode == Lane::By_Root) {
    onPrevRoot();
  }
  else {
    onPrevPage();
  }
}
void LanesLexicon::onNavFirst()   {
  if (m_navMode == Lane::By_Root) {
    onFirstRoot();
  }
  else {
    onFirstPage();
  }
}
void LanesLexicon::onNavLast()   {
  if (m_navMode == Lane::By_Root) {
    onLastRoot();
  }
  else {
    onLastPage();
  }
}
void LanesLexicon::onNavModeChanged() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  QAction * action = qobject_cast<QAction *>(QObject::sender());
  if (action) {
    bool ok;
    int m = action->data().toInt(&ok);
    if (ok) {
      m_navMode = m;
      updateStatusBar();
      updateMenu();
    }
  }
}
void LanesLexicon::onClearHistory() {
  QLOG_DEBUG() << Q_FUNC_INFO <<  m_history->clear();
  setStatus(tr("History cleared"));
  setupHistory();
}
void LanesLexicon::onDocs() {
  HelpWidget * w = new HelpWidget(this);
   m_tabs->setCurrentIndex(m_tabs->addTab(w,"Docs"));
   return;
}
void LanesLexicon::testSlot() {
  QLOG_DEBUG() << Q_FUNC_INFO;
}
void LanesLexicon::searchForPage() {
  int page = 0;
  int options;
  PageSearchDialog * d = new PageSearchDialog(this);
  d->setOptions(m_defaultSearchOptions);
  if (d->exec()) {
      page = d->getPage();
      options = d->getOptions();
  }
  delete d;
  if (page == 0)
    return;

  QString root;
  QString node;
  QString sql = QString("select root,nodeid from entry where page = %1").arg(page);

  QSqlQuery q(m_db);
  if (! q.prepare(sql)) {
    QLOG_WARN() << "Error preparing SQL" << sql;
    return;
  }
  if (q.exec() && q.first()) {
    root = q.value(0).toString();
    node = q.value(1).toString();
  }
  else {
    QMessageBox msg;
    msg.setText(QString(tr("Page %1 not found")).arg(page));
    msg.exec();
    return;
  }
  Place p;
  if (m_navMode == Lane::By_Root) {
    p.setRoot(root);
    p.setNode(node);
    this->gotoPlace(p,options);
  }
  else {
    p.setPage(page);
    this->onGoToPage(p);
  }
  this->syncContents();
}
void LanesLexicon::searchForRoot() {
  /// TODO this will show 'ignore diacritics' and 'whole word'
  /// which doesn't make much sense ?
  ArabicSearchDialog * d = new ArabicSearchDialog(Lane::Root,this);
  //    d->setWindowTitle(tr("Search for Root"));
  //    d->setPrompt(tr("Find root"));
  //    d->setNewTab(m_searchNewTab);
    if (d->exec()) {
      QString t = d->getText();
      if (! t.isEmpty()) {
        /// TODO maybe change this to if == Latin
        if (! UcdScripts::isScript(t,"Arabic")) {
          t = convertString(t);
        }
        int opts = d->getOptions();
        Place p = showPlace(Place(t),(opts & Lane::Create_Tab));
        if (! p.isValid()) {
          QMessageBox msgBox;
          msgBox.setObjectName("rootnotfound");
          msgBox.setTextFormat(Qt::RichText);
          //          msgBox.setStyleSheet(".arabic { font-family : Amiri;font-size : 18px}");
          /*this works, but affects the english and the arabic:
          msgBox.setStyleSheet("font-family :  Amiri; font-size : 18px");
          */
          /// this also works
          /// msgBox.setText(QString(tr("Root not found: <span style=\"font-family : Amiri;font-size : 18pt\">%1</span>")).arg(t));
          ///
          /// TODO get this from INI file
          msgBox.setText(QString(tr("Root not found: <span style=\"font-family : Amiri;font-size : 18pt\">%1</span>")).arg(t));
          msgBox.exec();
        }
        else {
          this->syncContents();
        }
      }
    }
    delete d;
}
/**
 *
 *
 */
void LanesLexicon::search(int searchType,ArabicSearchDialog * d,const QString & t) {
  qDebug() << Q_FUNC_INFO << searchType;
  QString target = t;

  int options = d->getOptions();
  options |= searchType;
  if (searchType & Lane::Word) {
      FullSearchWidget * s = new FullSearchWidget;
      s->setSearch(t,options);
      s->setForceLTR(d->getForceLTR());
      s->findTarget(true);
      connect(s,SIGNAL(showNode(const QString &)),this,SLOT(showSearchNode(const QString &)));
      int c = this->getSearchCount();
      int i = m_tabs->insertTab(m_tabs->currentIndex()+1,s,QString(tr("Search %1")).arg(c+1));
      m_tabs->setCurrentIndex(i);
      s->focusTable();
      return;
  }
  if (searchType & Lane::Entry) {
    //      if (! UcdScripts::isScript(target,"Arabic")) {
    //        target = convertString(target);
    //      }
      HeadSearchWidget * s = new HeadSearchWidget(this);
      connect(s,SIGNAL(searchResult(const QString &)),this,SLOT(setStatus(const QString &)));
      connect(s,SIGNAL(deleteSearch()),this,SLOT(deleteSearch()));
      s->search(target,options);
      if (s->count() == 0) {
        QMessageBox msgBox;
        msgBox.setObjectName("wordnotfound");
        msgBox.setTextFormat(Qt::RichText);
        msgBox.setText(QString(tr("Entry not found: <span style=\"font-family : Amiri;font-size : 18pt\">%1</span>")).arg(target));
        msgBox.exec();
        delete s;
      }
      else {
        int i = m_tabs->insertTab(m_tabs->currentIndex()+1,s,QString(tr("Entry search for %1")).arg(target));
        m_tabs->setCurrentIndex(i);
        setSignals(s->getEntry());
        s->showFirst();
      }
  }
}
void LanesLexicon::searchForWord() {
  ArabicSearchDialog * d = new ArabicSearchDialog(Lane::Word,this);
  d->setOptions(m_defaultSearchOptions);
  if (d->exec()) {
    QString t = d->getText();
    if (! t.isEmpty()) {
      this->search(Lane::Word,d,t);
    }
  }
  delete d;
}

/// TODO these needs to search the entry looking for bareword or word
void LanesLexicon::searchForEntry() {
  ArabicSearchDialog * d = new ArabicSearchDialog(Lane::Entry,this);
  d->setOptions(m_defaultSearchOptions);
  if (d->exec()) {
    QString t = d->getText();
    if (! t.isEmpty()) {
      this->search(Lane::Entry,d,t);
    }
  }
  delete d;
}
void LanesLexicon::searchForNode() {
  NodeSearchDialog * d = new NodeSearchDialog(this);
  d->setOptions(m_defaultSearchOptions);
  if (d->exec()) {
    QString t = d->getText();
    if (! t.isEmpty()) {
      /// TODO make this an option to always show root
      /// show only node
      //        Place p = showNode(t,true);
      Place p;
      p.setNode(t);
      p = showPlace(p,d->getOptions());
      if (! p.isValid()) {
        QMessageBox msgBox;
        msgBox.setText(QString(tr("Node not found: %1")).arg(t));
        msgBox.exec();
      }
    }
  }
  delete d;
}
void LanesLexicon::pageZoomIn() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    entry->onZoomIn();
  }
}
void LanesLexicon::pageZoomOut() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    entry->onZoomOut();
  }
}
void LanesLexicon::pageWiden() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    entry->onWiden();
  }
}
void LanesLexicon::pageNarrow() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    entry->onNarrow();
  }
}
void LanesLexicon::pagePrint() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (!entry)
    return;

  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());
  settings->beginGroup("Printer");
  m_printerReUse = settings->value(SID_PRINTER_USE).toBool();

  if (! m_printer.isValid())
    m_printerReUse = false;


  if ( m_printerReUse ) {
    entry->print(m_printer);
    return;
  }

  QPrintDialog printDialog(&m_printer, this);
  if (printDialog.exec() == QDialog::Accepted) {
    if (m_printer.isValid()) {
    entry->print(m_printer);
    }
  }
    /*
      QMessageBox msgBox;
      msgBox.setWindowTitle(QGuiApplication::applicationDisplayName());
      QString errorMessage(tr("Do you want to use these settings next time?"));

      QString pdf(tr("If you have selected PDF print, all subsequent prints will be sent to the same directory and named automatically."));
      QString info(tr("If you select 'Yes', the print options dialog will not appear again and all prints will be sent to the destination you selected with the same options."));
      QString clear(tr("To clear the saved printer information and cause the print dialog to re-appear, click on the 'Clear printer' button."));
        msgBox.setText("<html><head/><body><h2>" + errorMessage + "</h2>"
                     + "<p>" + info + "</p>"
                     + "<p>" + clear + "</p>"
                     + "<p>" + pdf + "</p>"
                     + "</body></html>");
      msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
      msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
      int ret = msgBox.exec();
      if (ret == QMessageBox::Yes) {
        m_printerReUse = true;
        if (m_printer.outputFormat() == QPrinter::PdfFormat) {
          QFileInfo fi(m_printer.outputFileName());
          m_printPdfLocation  = fi.absoluteDir().absolutePath();
          m_printToPdf = true;
        }
      }
    }
    else {
      qDebug() << "printer is not valid";
    }
  */


}
void LanesLexicon::pageSearch() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  /// TODO this
  if ( entry ) {
    if (entry->search() > 0) {
      m_clearAction->setEnabled(true);
    }
  }
}
void LanesLexicon::pageClear() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if ( entry ) {
    entry->clearHighlights();
    m_clearAction->setEnabled(false);
    return;
  }
  HeadSearchWidget * results = qobject_cast<HeadSearchWidget *>(m_tabs->currentWidget());
  if (results) {
    results->getEntry()->clearHighlights();
    m_clearAction->setEnabled(false);
    return;
  }


}
void LanesLexicon::currentTabChanged(int) {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if ( entry ) {
    Place p = entry->getPlace();
    m_tree->ensurePlaceVisible(p,true);
    m_convertToEntryAction->setEnabled(false);
    return;
  }
  HeadSearchWidget * results = qobject_cast<HeadSearchWidget *>(m_tabs->currentWidget());
  if (results) {
    m_convertToEntryAction->setEnabled(true);
  }
  else {
    m_convertToEntryAction->setEnabled(false);
  }
}
int LanesLexicon::searchTabs(const QString & node) {
  for(int i=0;i < m_tabs->count();i++) {
    GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(i));
    if (entry) {
      if (entry->hasNode(node)) {
        return i;
      }
    }
  }
  return -1;
}
void LanesLexicon::syncContents() {
  qDebug() << Q_FUNC_INFO;
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if ( ! entry )
    return;

  Place p = entry->getPlace();
  if (p.isValid())
    m_tree->ensurePlaceVisible(p);
}
void LanesLexicon::showNoteBrowser() {
  for(int i=0;i < m_tabs->count();i++) {
    NoteBrowser * notes = qobject_cast<NoteBrowser *>(m_tabs->widget(i));
    if (notes) {
      m_tabs->setCurrentIndex(i);
      return;
    }
  }
  m_tabs->addTab(new NoteBrowser(this),"Notes");
}
int LanesLexicon::hasPlace(const Place & p,int searchtype,bool setFocus) {
  for(int i=0;i < m_tabs->count();i++) {
    GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(i));
    if (entry) {
      if (entry->hasPlace(p,searchtype,setFocus) != -1) {
        if (setFocus) {
          m_tabs->setCurrentIndex(i);
        }
        return i;
      }
    }
  }
  return -1;

}
/**
 * Can be invoked by: NodeView,FullSearchWidget
 *
 * @param node
 */
void LanesLexicon::showSearchNode(const QString & node) {
  Place p;
  p.setNode(node);
  this->gotoPlace(p,Lane::Create_Tab);
}
int LanesLexicon::getSearchCount() {
  int c = 0;
  for(int i=0;i < m_tabs->count();i++) {
    FullSearchWidget * w = qobject_cast<FullSearchWidget *>(m_tabs->widget(i));
    if (w) {
      c++;
    }
  }
  return c;
}
void LanesLexicon::convertToEntry() {
  HeadSearchWidget * w = qobject_cast<HeadSearchWidget *>(m_tabs->currentWidget());
  if (w) {
    Place p = w->getEntry()->getPlace();
    if (p.isValid()) {
      int ix = m_tabs->currentIndex();
      this->onCloseTab(ix);
      GraphicsEntry * entry = new GraphicsEntry(this);
      setSignals(entry);
      entry->installEventFilter(this);
      entry->getXmlForRoot(p);
      m_tabs->insertTab(ix,entry,p.getShortText());
      m_tabs->setCurrentIndex(ix);
      entry->home();
    }
    else {

    }
  }
}
void LanesLexicon::enableKeymaps(bool v) {
  m_keymapsEnabled = v;
  qDebug() << Q_FUNC_INFO << v;
  m_keymapsButton->setEnabled(v);
  QString tip;
  if (m_keymapsEnabled)
    tip = QString(tr("Keymaps, enabled"));
  else
    tip = QString(tr("Keymaps, disabled"));

  m_keymapsButton->setToolTip(tip);
  foreach (QWidget *widget, QApplication::allWidgets()) {
    ImLineEdit * w = qobject_cast<ImLineEdit *>(widget);
    if (w) {
      w->setEnabled(v);
    }
    else {
      ImEdit * imedit = qobject_cast<ImEdit *>(widget);
      if (imedit) {
        imedit->setEnabled(v);
      }
      else {
        SearchOptions * search = qobject_cast<SearchOptions *>(widget);
        if (search) {
          search->setKeymapsEnabled(v);
        }
      }
    }
  }
  m_defaultSearchOptions -= Lane::Keymaps_Enabled;
  if (v) {
    m_defaultSearchOptions |= Lane::Keymaps_Enabled;
  }
  QSettings * settings;
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  settings = app->getSettings();
  settings->setIniCodec("UTF-8");
  settings->beginGroup("System");
  settings->setValue("Keymaps",v);
  delete settings;
}
QString LanesLexicon::getActiveKeymap() const {
  return m_activeMap;
}
void LanesLexicon::deleteSearch() {
  //  if (! p.isValid()) {
  //    return;
  //  }
  Place p;
  int currentTab = m_tabs->currentIndex();
  HeadSearchWidget * searchwidget = qobject_cast<HeadSearchWidget *>(m_tabs->widget(currentTab));
  if (searchwidget) {
    p = searchwidget->getPlace();
    delete searchwidget;
  }
  else {
    return;
  }
  GraphicsEntry *  entry = new GraphicsEntry(this);
  setSignals(entry);
  entry->installEventFilter(this);
  entry->getXmlForRoot(p);
  m_tabs->insertTab(currentTab,entry,p.getShortText());
  m_tabs->setCurrentIndex(currentTab);
  entry->home();
  return;

}
void LanesLexicon::localSearch() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    entry->search();
  }
}
void LanesLexicon::localSearchNext() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    entry->searchNext();
  }
}
void LanesLexicon::localSearchClear() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    entry->clearSelections();
  }
}
void LanesLexicon::localSearchShow() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    entry->showSelections();
  }
}
void LanesLexicon::tabsChanged() {
  QRegExp rx("^\\d+");
  for(int i=0;i < m_tabs->count();i++) {
    QString t = m_tabs->tabText(i);
    t.remove(rx);
    QString title = QString("%1  %2").arg(i+1).arg(t);
    m_tabs->setTabText(i,title);
  }
}
void LanesLexicon::setIcon(QAction * action,const QString & imgdir,const QString & iconfile) {
  QDir d(imgdir);
  QFileInfo fi(d,iconfile);
  if (! fi.exists()) {
    QLOG_WARN() << "Icon not found" << fi.absolutePath();
    return;
  }
  QIcon icon(fi.absoluteFilePath());
  if (! icon.isNull()) {
    action->setIcon(icon);
  }
}
void LanesLexicon::setIcons(const QString & theme) {
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  settings->setIniCodec("UTF-8");
  QStringList groups = settings->childGroups();
  QString icongroup;
  if (theme.isEmpty()) {
    icongroup = "Icons-Default";
  }
  else {
    icongroup = QString("Icons-%1").arg(theme);
  }
  if (! groups.contains(icongroup)) {
    QLOG_WARN() << "theme not found" << theme;
    delete settings;
    return;
  }
  settings->beginGroup(icongroup);

  QString imgdir = settings->value("Directory","images").toString();
  QFileInfo fi(imgdir);
  if (! fi.exists()) {
    QLOG_WARN() << "theme directory not found" << imgdir;
    delete settings;
    return;
  }
  if (! fi.isDir()) {
    QLOG_WARN() << "theme directory is not a directory" << imgdir;
    delete settings;
    return;
  }

  //  fi.setFile(imgdir,iconfile);
  //  if ( ! iconfile.isEmpty() && fi.exists() ) {
  //    action = new QAction(QIcon(fi.absoluteFilePath()),text,this);
  //  }

  QString iconfile;

  iconfile = settings->value("Exit",QString()).toString();
  setIcon(m_exitAction,imgdir,iconfile);

  //  m_clearHistoryAction;

  //m_testAction

  iconfile = settings->value("History",QString()).toString();
  setIcon(m_historyAction,imgdir,iconfile);


  iconfile = settings->value("Next",QString()).toString();
  setIcon(m_navNextAction,imgdir,iconfile);

  iconfile = settings->value("Back",QString()).toString();
  setIcon(m_navPrevAction,imgdir,iconfile);

  iconfile = settings->value("First",QString()).toString();
  setIcon(m_navFirstAction,imgdir,iconfile);

  iconfile = settings->value("Last",QString()).toString();
  setIcon(m_navLastAction,imgdir,iconfile);

  iconfile = settings->value("Docs",QString()).toString();
  setIcon(m_docAction,imgdir,iconfile);

  iconfile = settings->value("Bookmarks",QString()).toString();
  setIcon(m_bookmarkAction,imgdir,iconfile);

  iconfile = settings->value("Move",QString()).toString();
  setIcon(m_navigationAction,imgdir,iconfile);

  iconfile = settings->value("Search",QString()).toString();
  setIcon(m_searchAction,imgdir,iconfile);

  //  m_navModeRootAction
  // m_navModePageAction

  // m_searchWordAction = new QAction(tr("For Arabic &word"),this);
  //m_searchPageAction = new QAction(tr("For &page"),this);
  //  m_searchRootAction = new QAction(tr("For &root"),this);
  //m_searchNodeAction = new QAction(tr("For &node"),this);


  iconfile = settings->value("ZoomIn",QString()).toString();
  setIcon(m_zoomInAction,imgdir,iconfile);

  iconfile = settings->value("ZoomOut",QString()).toString();
  setIcon(m_zoomOutAction,imgdir,iconfile);

  iconfile = settings->value("Widen",QString()).toString();
  setIcon(m_widenAction,imgdir,iconfile);

  iconfile = settings->value("Narrow",QString()).toString();
  setIcon(m_narrowAction,imgdir,iconfile);

  iconfile = settings->value("Print",QString()).toString();
  setIcon(m_printAction,imgdir,iconfile);

  iconfile = settings->value("LocalSearch",QString()).toString();
  setIcon(m_localSearchAction,imgdir,iconfile);

  //  m_clearAction = createIconAction(imgdir,settings->value("Clear",QString()).toString(),tr("Clear highlights"));
  //  m_convertToEntryAction = createIconAction(imgdir,settings->value("SearchToEntry",QString()).toString(),tr("Convert to Entry"));
  //  m_clearAction->setEnabled(false);

  iconfile = settings->value("Keymaps",QString()).toString();
  setIcon(m_keymapsAction,imgdir,iconfile);

  iconfile = settings->value("Keymaps-disabled",QString()).toString();
  setIcon(m_keymapsAction,imgdir,iconfile);

  iconfile = settings->value("Link",QString()).toString();

  //setIcon(m_linkAction,imgdir,iconfile);

  QIcon icon;

  fi.setFile(imgdir,iconfile);
  icon.addPixmap(fi.absoluteFilePath(),QIcon::Normal,QIcon::On);

  fi.setFile(imgdir,settings->value("Unlink",QString()).toString());
  icon.addPixmap(fi.absoluteFilePath(),QIcon::Normal,QIcon::Off);

  m_linkAction->setIcon(icon);
  m_linkAction->setChecked(m_linkContents);

  delete settings;
}
void LanesLexicon::sync() {
  qDebug() << Q_FUNC_INFO;
  QWidget * w = QApplication::focusWidget();
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (!entry) {
    QLOG_WARN() << "Cannot sync to current tab";
    return;
  }
  if (w == m_tree) {
    Place p = m_tree->getCurrentPlace();
    if (p.isValid()) {
      this->showPlace(p,0);
    }
  }
  else {
    Place p = entry->getPlace();
    if (p.isValid()) {
      m_tree->ensurePlaceVisible(p);
    }
  }
}
