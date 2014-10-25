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
#include "searchoptionswidget.h"
#include "searchdialogs.h"
#include "fullsearch.h"
#include "headsearch.h"
#include "definedsettings.h"
#include "entrylayoutwidget.h"
#include "optionsdialog.h"
#include "logviewer.h"
#include "about.h"
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
  m_entryLayout = NULL;
  m_mapper = im_new();
  m_logview = NULL;
  createActions();
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
  m_tabs->installEventFilter(this);
  m_tabs->tabBar()->installEventFilter(this);
  connect(m_tabs,SIGNAL(tabsChanged()),this,SLOT(tabsChanged()));
  /// at the end of the history, but we should be able to restore from settings
  /// TODO would we want restore our current position in history?
  m_history = new HistoryMaster(m_historyDbName);
  if ( ! m_history->isOk()) {
    QLOG_WARN() << "History is not available";
    m_historyEnabled = false;
  }
  m_history->setEnabled(m_historyEnabled);

  QSettings * settings  = (qobject_cast<Lexicon *>(qApp))->getSettings();
  m_notes = new NoteMaster(settings);

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


  setIcons(m_iconTheme);
  createMenus();
  createToolBar();
  createStatusBar();


  if (m_navMode == Lane::By_Root) {
    onNavigationMenuChanged(m_navModeRootAction);
    //    ->setChecked(true);
  }
  else {
    onNavigationMenuChanged(m_navModePageAction);//->setChecked(false);
  }

  if (m_navMode == Lane::By_Page) {
    m_navModePageAction->setChecked(true);
  }
  else {
    m_navModeRootAction->setChecked(true);
  }

  if (m_db.isOpen()) {
    setStatus(tr("Ready"));
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


  onSetInterface();

  restoreSavedState();
  /// without this, the QSplashScreen is the active window

  //  m_tabs->currentWidget()->setFocus();

  //  setTabOrder(m_tree,m_tabs);
  //  setTabOrder(m_tabs->tabBar(),m_tree);
  /**
   * if the current widget is a graphics entry , sent it a <space> to simulate
   * the user activating the page (<return> would do). This selects the current item
   * and gives focus etc. It's a kludge, but the whole focus/selection stuff is a
   * bit confusing.
   */
  QApplication::setActiveWindow(m_tabs->currentWidget());

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
    //    QLOG_DEBUG() << widget->key().toString();
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

  if (m_entryLayout != NULL) {
    delete m_entryLayout;
    m_entryLayout = 0;
  }

  if (m_logview != NULL) {
    delete m_logview;
    m_logview = 0;
  }
  /// TODO close notes db
  freeXslt();
  im_free(m_mapper);
  QLOG_DEBUG() << Q_FUNC_INFO << "exit";
}
void LanesLexicon::onSetInterface(bool triggered) {
  qStrip << Q_FUNC_INFO << "got action for set interface" << triggered;
  bool v = m_minimalAction->isChecked();
  QList<QToolBar *> toolbars = this->findChildren<QToolBar *>();
  for(int i=0;i < toolbars.size();i++) {
    toolbars[i]->setVisible(!v);
  }
  statusBar()->setVisible(!v);
  menuBar()->setVisible(!v);
  updateMenu();
  if (! triggered || ! v || ! m_interfaceWarning) {
    return;
  }
  QShortcut * sc = qobject_cast<QShortcut *>(m_signalMapper->mapping(QString("Toggle interface")));
  if (sc) {
    QCheckBox * noshow = new QCheckBox(tr("Check this box to stop this dialog showing again"));
    QMessageBox msgBox;
    msgBox.setCheckBox(noshow);
    msgBox.setWindowTitle(QGuiApplication::applicationDisplayName());
    QString errorMessage(tr("Warning"));
    QString info(tr("The toolbars and menubars will not be visible"));
    QString next = QString(tr("To make them visible again, press %1")).arg(sc->key().toString());
    msgBox.setText("<html><head/><body><h2>" + errorMessage + "</h2><p>"
                   + info + "</p><p>" + next + "</p></body></html>");
    msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
    msgBox.exec();
    m_interfaceWarning = ! noshow->isChecked();
  }
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


  connect(entry,SIGNAL(placeChanged(const Place &)),this,SLOT(placeChanged (const Place &)));
  connect(entry,SIGNAL(clearPage()),this,SLOT(pageClear()));
  connect(entry,SIGNAL(searchPage()),this,SLOT(pageSearch()));

  connect(entry,SIGNAL(gotoNode(const Place &,int)),this,SLOT(gotoPlace(const Place &,int)));
  connect(entry,SIGNAL(printNode(const QString &)),this,SLOT(printNode(const QString &)));
  connect(entry,SIGNAL(printPage()),this,SLOT(pagePrint()));

}
void LanesLexicon::onCloseOtherTabs() {
  m_tabs->setUpdatesEnabled(false);
  QString label = m_tabs->tabText(m_tabs->currentIndex());
  QWidget * w = m_tabs->currentWidget();
  m_tabs->removeTab(m_tabs->currentIndex());
  while(m_tabs->count() > 0) {
    this->onCloseTab(0);
  }
  m_tabs->addTab(w,label);
  m_tabs->setUpdatesEnabled(true);
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
    m_minimalAction->setChecked(! m_minimalAction->isChecked());
    onSetInterface(true);
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
    onNavigationMenuChanged(m_navModeRootAction);
  }
  else if (key == SID_SHORTCUT_NAV_PAGE_MODE) {
    onNavigationMenuChanged(m_navModePageAction);
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
  else if (key == SID_SHORTCUT_DELETE_OTHER_TABS) {
    this->onCloseOtherTabs();
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
  else if (key == SID_SHORTCUT_SHOW_LOGS) {
    this->onLogViewer();
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
  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());
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
      sc->setObjectName(keys[i]);
      connect(sc,SIGNAL(activated()),m_signalMapper,SLOT(map()));
      /// use the setting name as the shortcut 'name'
      m_signalMapper->setMapping(sc,keys[i]);
    }
  }
  connect(m_signalMapper,SIGNAL(mapped(QString)),this,SLOT(shortcut(const QString &)));
  settings->endGroup();
}



/**
 * load the application level stylesheet, stripping out lines
 * beginning with #
 *
 */
void LanesLexicon::loadStyleSheet() {
  QFile f(m_applicationCssFile);
  if ( ! f.open(QIODevice::ReadOnly)) {
    QLOG_WARN() << "Unable to open stylesheet" << m_applicationCssFile;
    return;
  }
  QTextStream in(&f);
  in.setCodec("UTF-8");
  QString css = in.readAll();
  if (! css.isEmpty()) {
    qApp->setStyleSheet(css);
  }
  f.close();
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
  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());

  settings->beginGroup("Icons");

  m_exitAction = new QAction(tr("Exit"),this);
  connect(m_exitAction,SIGNAL(triggered()),this,SLOT(onExit()));

  m_editViewAction = new QAction(tr("Edit view"),this);
  connect(m_editViewAction,SIGNAL(triggered()),this,SLOT(onEditView()));

  m_minimalAction = new QAction(tr("Minimal interface"),this);
  m_minimalAction->setCheckable(true);
  connect(m_minimalAction,SIGNAL(triggered(bool)),this,SLOT(onSetInterface(bool)));

  m_optionsAction = new QAction(tr("&Preferences"),this);
  connect(m_optionsAction,SIGNAL(triggered()),this,SLOT(onOptions()));

  m_logViewerAction = new QAction(tr("View logs"),this);
  connect(m_logViewerAction,SIGNAL(triggered()),this,SLOT(onLogViewer()));

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


  m_docAction = new QAction(tr("Documentation"),this);
  m_aboutAction = new QAction(tr("About"),this);
  m_bookmarkAction = new QAction(tr("Bookmarks"),this);

  //  m_navigationAction = new QAction(tr("Move"),this);

  m_searchAction = new QAction(tr("Search"),this);


  m_moveGroup = new QActionGroup(this);
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
  m_moveGroup->addAction(m_navModeRootAction);
  m_moveGroup->addAction(m_navModePageAction);

  connect(m_moveGroup,SIGNAL(triggered(QAction *)),this,SLOT(onNavigationMenuChanged(QAction *)));

  connect(m_docAction,SIGNAL(triggered()),this,SLOT(onDocs()));
  connect(m_aboutAction,SIGNAL(triggered()),this,SLOT(onAbout()));

  m_searchWordAction = new QAction(tr("For Arabic &word"),this);
  connect(m_searchWordAction,SIGNAL(triggered()),this,SLOT(searchForWord()));
  m_searchPageAction = new QAction(tr("For &page"),this);
  connect(m_searchPageAction,SIGNAL(triggered()),this,SLOT(searchForPage()));
  m_searchRootAction = new QAction(tr("For &root"),this);
  connect(m_searchRootAction,SIGNAL(triggered()),this,SLOT(searchForRoot()));
  m_searchNodeAction = new QAction(tr("For &node"),this);
  connect(m_searchNodeAction,SIGNAL(triggered()),this,SLOT(searchForNode()));
  m_searchEntryAction = new QAction(tr("For &entry"),this);
  connect(m_searchEntryAction,SIGNAL(triggered()),this,SLOT(searchForEntry()));

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


}
void LanesLexicon::createToolBar() {
  QToolBar * m_mainbar = addToolBar("Main");
  m_mainbar->setObjectName("maintoolbar");

  m_exitButton = new QToolButton(m_mainbar);
  m_exitButton->setText(tr("Exit"));
  m_exitButton->setDefaultAction(m_exitAction);
  m_exitButton->setFocusPolicy(Qt::StrongFocus);
  m_mainbar->addWidget(m_exitButton);

  m_mainbar->addAction(m_testAction);

  m_historyButton = new QToolButton(m_mainbar);
  m_historyButton->setText(tr("History"));
  m_historyButton->setDefaultAction(m_historyAction);
  m_historyButton->setEnabled(false);
  m_historyButton->setMenu(m_historyMenu);
  m_historyButton->setFocusPolicy(Qt::StrongFocus);
  m_historyButton->setPopupMode(QToolButton::InstantPopup);
  m_mainbar->addWidget(m_historyButton);

  m_bookmarkButton = new QToolButton(m_mainbar);
  m_bookmarkButton->setDefaultAction(m_bookmarkAction);
  m_bookmarkButton->setText(tr("Bookmarks"));
  m_bookmarkButton->setFocusPolicy(Qt::StrongFocus);
  m_bookmarkButton->setEnabled(true);
  m_bookmarkButton->setMenu(m_bookmarkMenu);
  m_bookmarkButton->setPopupMode(QToolButton::InstantPopup);
  m_mainbar->addWidget(m_bookmarkButton);

  m_searchButton = new QToolButton(m_mainbar);
  m_searchButton->setDefaultAction(m_searchAction);
  m_searchButton->setText(tr("Search"));
  m_searchButton->setPopupMode(QToolButton::InstantPopup);
  m_searchButton->setFocusPolicy(Qt::StrongFocus);
  m_searchButton->setEnabled(true);
  m_searchButton->setMenu(m_searchMenu);
  m_mainbar->addWidget(m_searchButton);


  m_docButton = new QToolButton(m_mainbar);
  m_docButton->setDefaultAction(m_docAction);
  m_docButton->setText(tr("Help"));
  m_docButton->setFocusPolicy(Qt::StrongFocus);
  m_mainbar->addWidget(m_docButton);

  m_aboutButton = new QToolButton(m_mainbar);
  m_aboutButton->setDefaultAction(m_aboutAction);
  m_aboutButton->setText(tr("About"));
  m_aboutButton->setFocusPolicy(Qt::StrongFocus);
  m_mainbar->addWidget(m_aboutButton);

  m_optionsButton = new QToolButton(m_mainbar);
  m_optionsButton->setDefaultAction(m_optionsAction);
  m_optionsButton->setText(tr("Preferences"));
  m_optionsButton->setFocusPolicy(Qt::StrongFocus);
  m_mainbar->addWidget(m_optionsButton);

  m_logButton = new QToolButton(m_mainbar);
  m_logButton->setDefaultAction(m_logViewerAction);
  m_logButton->setText(tr("View logs"));
  m_logButton->setFocusPolicy(Qt::StrongFocus);
  m_mainbar->addWidget(m_logButton);

  m_mainbar->setFloatable(true);
  m_mainbar->setIconSize(m_toolbarIconSize);
  //  addToolBarBreak();

  m_navigation = addToolBar(tr("Navigation"));
  m_navigation->setObjectName("navigationtoolbar");
  m_navigation->setIconSize(m_toolbarIconSize);
  m_navigation->setFloatable(true);

  m_navigationModeMenu = new QMenu(m_navigation);
  m_navigationModeMenu->addAction(m_navModeRootAction);
  m_navigationModeMenu->addAction(m_navModePageAction);
  m_navigationButton = new QToolButton(m_navigation);
  m_navigationButton->setMenu(m_navigationModeMenu);
  m_navigationButton->setText(tr("Move by"));
  m_navigationButton->setFocusPolicy(Qt::StrongFocus);
  if (m_navMode == Lane::By_Root) {
    m_navigationButton->setDefaultAction(m_navModeRootAction);
  }
  else {
    m_navigationButton->setDefaultAction(m_navModePageAction);
  }
  m_navigationButton->setPopupMode(QToolButton::InstantPopup);
  m_navigation->addWidget(m_navigationButton);

  m_navFirstButton = new QToolButton(m_navigation);
  m_navFirstButton->setDefaultAction(m_navFirstAction);
  m_navFirstButton->setText(tr("First"));
  m_navFirstButton->setFocusPolicy(Qt::StrongFocus);
  m_navigation->addWidget(m_navFirstButton);

  m_navNextButton = new QToolButton(m_navigation);
  m_navNextButton->setDefaultAction(m_navNextAction);
  m_navNextButton->setText(tr("Next"));
  m_navNextButton->setFocusPolicy(Qt::StrongFocus);
  m_navigation->addWidget(m_navNextButton);

  m_navPrevButton = new QToolButton(m_navigation);
  m_navPrevButton->setDefaultAction(m_navPrevAction);
  m_navPrevButton->setText(tr("Prev"));
  m_navPrevButton->setFocusPolicy(Qt::StrongFocus);
  m_navigation->addWidget(m_navPrevButton);

  m_navLastButton = new QToolButton(m_navigation);
  m_navLastButton->setDefaultAction(m_navLastAction);
  m_navLastButton->setText(tr("Last"));
  m_navLastButton->setFocusPolicy(Qt::StrongFocus);
  m_navigation->addWidget(m_navLastButton);

  m_navigation->addSeparator();


  //  addToolBarBreak();

  m_entrybar = addToolBar(tr("Page"));
  m_entrybar->setObjectName("pagetoolbar");
  m_entrybar->setIconSize(m_toolbarIconSize);

  m_zoomInButton = new QToolButton(m_entrybar);
  m_zoomInButton->setDefaultAction(m_zoomInAction);
  m_zoomInButton->setText(tr("Zoom in"));
  m_zoomInButton->setFocusPolicy(Qt::StrongFocus);
  m_entrybar->addWidget(m_zoomInButton);


  //  m_entrybar->addAction(m_zoomInAction);
  m_zoomOutButton = new QToolButton(m_entrybar);
  m_zoomOutButton->setDefaultAction(m_zoomOutAction);
  m_zoomOutButton->setText(tr("Zoom out"));
  m_zoomOutButton->setFocusPolicy(Qt::StrongFocus);
  m_entrybar->addWidget(m_zoomOutButton);

  //  m_entrybar->addAction(m_zoomOutAc!tion);
  m_widenButton = new QToolButton(m_entrybar);
  m_widenButton->setDefaultAction(m_widenAction);
  m_widenButton->setText(tr("Widen"));
  m_widenButton->setFocusPolicy(Qt::StrongFocus);
  m_entrybar->addWidget(m_widenButton);

  //  m_entrybar->addAction(m_widenAction);
  m_narrowButton = new QToolButton(m_entrybar);
  m_narrowButton->setDefaultAction(m_narrowAction);
  m_narrowButton->setText(tr("Narrow"));
  m_narrowButton->setFocusPolicy(Qt::StrongFocus);
  m_entrybar->addWidget(m_narrowButton);

  //  m_entrybar->addAction(m_narrowAction);
  m_printButton = new QToolButton(m_entrybar);
  m_printButton->setDefaultAction(m_printAction);
  m_printButton->setText(tr("Print"));
  m_printButton->setFocusPolicy(Qt::StrongFocus);
  m_entrybar->addWidget(m_printButton);

  //  m_entrybar->addAction(m_printAction);
  m_localSearchButton = new QToolButton(m_entrybar);
  m_localSearchButton->setDefaultAction(m_localSearchAction);
  m_localSearchButton->setText(tr("Search page"));
  m_localSearchButton->setFocusPolicy(Qt::StrongFocus);
  m_entrybar->addWidget(m_localSearchButton);

  //  m_entrybar->addAction(m_localSearchAction);
  m_clearButton = new QToolButton(m_entrybar);
  m_clearButton->setDefaultAction(m_clearAction);
  m_clearButton->setText(tr("Clear search"));
  m_clearButton->setFocusPolicy(Qt::StrongFocus);
  m_entrybar->addWidget(m_clearButton);

  //  m_entrybar->addAction(m_clearAction);

  //  m_entrybar->addAction(m_convertToEntryAction);
  m_entrybar->setFloatable(true);

  setTabOrder(m_exitButton,m_historyButton);
  setTabOrder(m_historyButton,m_bookmarkButton);
  setTabOrder(m_bookmarkButton,m_searchButton);
  setTabOrder(m_searchButton,m_docButton);
  setTabOrder(m_docButton,m_aboutButton);
  setTabOrder(m_aboutButton,m_optionsButton);
  setTabOrder(m_optionsButton,m_logButton);
  setTabOrder(m_logButton,m_navigationButton);
  setTabOrder(m_navigationButton,m_navFirstButton);
  setTabOrder(m_navFirstButton,m_navNextButton);
  setTabOrder(m_navNextButton,m_navPrevButton);
  setTabOrder(m_navPrevButton,m_navLastButton);
  setTabOrder(m_navLastButton,m_zoomInButton);
  setTabOrder(m_zoomInButton,m_zoomOutButton);
  setTabOrder(m_zoomOutButton,m_widenButton);
  setTabOrder(m_widenButton,m_narrowButton);
  setTabOrder(m_narrowButton,m_printButton);
  setTabOrder(m_printButton,m_localSearchButton);

  if (m_toolbarText) {
    m_exitButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_historyButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_bookmarkButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_searchButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_docButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_optionsButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_logButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_navFirstButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_navNextButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_navPrevButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_navLastButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_zoomInButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_zoomOutButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_widenButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_narrowButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_printButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_localSearchButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_clearButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  }

  //  setTabOrder(m_mainbar,m_navigation);
  //  setTabOrder(m_navigation,m_entrybar);


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
  QLOG_DEBUG() << Q_FUNC_INFO << "event size" << events.size();
  if (events.size() == 0) {
    m_historyButton->setEnabled(false);
    m_clearHistoryAction->setEnabled(false);
  }
  else {
    // m_historyMenu->clear();
    m_historyButton->setEnabled(true);
    m_clearHistoryAction->setEnabled(true);
  }
  QList<QAction *> actions = m_historyMenu->actions();
  QLOG_DEBUG() << Q_FUNC_INFO << "history actions size" << actions.size();
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
  m_historyButton->setEnabled(true);
    //    m_historyButton->addActions(group->actions());//setMenu(m);
    //    m_historyButton->setMenu(m);
}
void LanesLexicon::createMenus() {
  m_mainmenu = new AppMenu(this);
  m_mainmenu->setFocusPolicy(Qt::StrongFocus);

  setMenuBar(m_mainmenu);
  m_fileMenu = menuBar()->addMenu(tr("&File"));
  m_fileMenu->setObjectName("filemenu");
  m_fileMenu->setFocusPolicy(Qt::StrongFocus);

  m_fileMenu->addAction(m_exitAction);

  m_viewMenu = menuBar()->addMenu(tr("&View"));
  m_viewMenu->setObjectName("viewmenu");
  m_viewMenu->setFocusPolicy(Qt::StrongFocus);
  m_viewMenu->addAction(m_minimalAction);
  m_viewMenu->addAction(m_optionsAction);


  m_bookmarkMenu = m_mainmenu->addMenu(tr("&Bookmarks"));
  m_bookmarkMenu->setObjectName("bookmarkmenu");
  m_bookmarkMenu->setFocusPolicy(Qt::StrongFocus);

  m_bookmarkMenu->addAction(m_bookmarkListAction);
  m_bookmarkMenu->addAction(m_bookmarkClearAction);
  m_bookmarkMenu->addAction(m_bookmarkRevertAction);
  m_bookmarkMenu->addAction(m_bookmarkAddAction);
  //  m_bookmarkMenu->addAction(m_bookmarkJumpAction);

  connect(m_mainmenu,SIGNAL(rebuildBookmarks()),this,SLOT(bookmarkRebuildMenu()));

  m_historyMenu = m_mainmenu->addMenu(tr("&History"));
  m_historyMenu->setObjectName("historymenu");
  m_historyMenu->setFocusPolicy(Qt::StrongFocus);
  m_historyMenu->addAction(m_clearHistoryAction);

  m_moveMenu = m_mainmenu->addMenu(tr("&Navigation"));
  m_moveMenu->setObjectName("navigationmenu");
  m_moveMenu->setFocusPolicy(Qt::StrongFocus);
  m_moveMenu->addAction(m_navFirstAction);
  m_moveMenu->addAction(m_navNextAction);
  m_moveMenu->addAction(m_navPrevAction);
  m_moveMenu->addAction(m_navLastAction);
  m_moveMenu->addAction(m_navModeRootAction);
  m_moveMenu->addAction(m_navModePageAction);

  m_searchMenu = m_mainmenu->addMenu(tr("&Search"));
  m_searchMenu->setFocusPolicy(Qt::StrongFocus);
  m_searchMenu->setTitle(tr("Search"));
  m_searchMenu->setObjectName("searchmenu");
  m_searchMenu->addAction(m_searchRootAction);
  m_searchMenu->addAction(m_searchEntryAction);
  m_searchMenu->addAction(m_searchWordAction);
  m_searchMenu->addAction(m_searchPageAction);
  m_searchMenu->addAction(m_searchNodeAction);

  m_pageMenu  = m_mainmenu->addMenu(tr("&Entry"));
  m_pageMenu->setObjectName("pagemenu");
  m_pageMenu->setFocusPolicy(Qt::StrongFocus);
  m_pageMenu->addAction(m_zoomInAction);
  m_pageMenu->addAction(m_zoomOutAction);
  m_pageMenu->addAction(m_widenAction);
  m_pageMenu->addAction(m_narrowAction);
  m_pageMenu->addAction(m_printAction);
  m_pageMenu->addAction(m_localSearchAction);
  m_pageMenu->addAction(m_clearAction);

  m_toolMenu = m_mainmenu->addMenu(tr("&Tools"));
  m_toolMenu->setFocusPolicy(Qt::StrongFocus);
  m_toolMenu->setObjectName("toolmenu");
  m_toolMenu->addAction(m_logViewerAction);
  m_toolMenu->addAction(m_editViewAction);

  m_helpMenu = m_mainmenu->addMenu(tr("&Help"));
  m_helpMenu->setFocusPolicy(Qt::StrongFocus);
  m_helpMenu->setObjectName("helpmenu");
  m_helpMenu->addAction(m_aboutAction);
  m_helpMenu->addAction(m_docAction);

}

void LanesLexicon::createStatusBar() {
  m_placeIndicator = new QLabel("",this);
  m_placeIndicator->setObjectName("placeindicator");

  m_keymapsButton = new QToolButton(this);
  QStringList maps =  m_definedMaps;//m_mapper->getMaps();
  maps << m_nullMap;
  QMenu * menu = new QMenu(this);
  for(int i=0;i < maps.size();i++) {
    QAction * action  = menu->addAction(maps[i]);
    action->setCheckable(true);
    action->setData(maps[i]);
    if (m_currentMap == maps[i]) {
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
  m_currentMap = action->data().toString();
  QLOG_DEBUG() << Q_FUNC_INFO << "map set to" << m_currentMap;
  QList<QAction *> actions = m_keymapsButton->menu()->actions();
  for(int i=0;i < actions.size();i++) {
    if (actions[i]->data().toString() == m_currentMap) {
      actions[i]->setChecked(true);
    }
    else {
      actions[i]->setChecked(false);
    }
  }
  foreach (QWidget *widget, QApplication::allWidgets()) {
    ImLineEdit * w = qobject_cast<ImLineEdit *>(widget);
    if (w) {
      w->activateMap(m_currentMap,true);
    }
    else {
      ImEdit * imedit = qobject_cast<ImEdit *>(widget);
      if (imedit) {
        imedit->activateMap(m_currentMap,true);
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

  if (! dbfile.exists() ) {
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

  QLOG_DEBUG() << Q_FUNC_INFO << p;
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
    entry->getView()->installEventFilter(this);
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
    updateStatusBar();
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
    case Qt::Key_Tab: {
      qStrip << Q_FUNC_INFO << "got tab" << target->objectName() << target->metaObject()->className();
        if (target == m_tabs) {

        }
        else if (target == m_tabs->tabBar()) {
          qStrip << "Focus given to menu";
          m_exitButton->setFocus();
          return true;
        }
        else if (target == m_tree) {
        }
        else {
        }
        break;
    }
      case Qt::Key_Left:
        qStrip << Q_FUNC_INFO << "got left" << target->objectName() << target->metaObject()->className();
        break;
        /*
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
        */
    default:
      break;
    }
  }

  return QMainWindow::eventFilter(target,event);
}
void LanesLexicon::onEditView() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  if (m_entryLayout != NULL) {
    m_entryLayout->show();
    return;
  }
  m_entryLayout  = new EntryLayoutWidget();
  connect(m_entryLayout,SIGNAL(reload(const QString &,const QString &)),this,SLOT(reloadEntry(const QString &,const QString &)));
  connect(m_entryLayout,SIGNAL(revert()),this,SLOT(revertEntry()));
  m_entryLayout->show();
}
void LanesLexicon::onLogViewer() {
  if (m_logview == NULL) {
    m_logview = new LogViewer();
  }
  m_logview->show();
}
void LanesLexicon::onTest() {
  m_logview = new LogViewer();
  m_logview->show();
  if (0) {
    SearchOptionsWidget * s = new SearchOptionsWidget(SearchOptions::Word);
    s->addKeymaps("map1",QStringList() << "map0" << "map1" << "map2");
    m_tabs->addTab(s,"Test");
    SearchOptions options;
    options.setNewTab(true);
    options.setSearchType(SearchOptions::Regex);
    s->setOptions(options);
    //    s->setOptions(0);
    ArabicSearchDialog * d = new ArabicSearchDialog(SearchOptions::Word);
    d->exec();
  }
  if (0) {
  foreach (QWidget *widget, QApplication::allWidgets()) {
    ImLineEdit * w = qobject_cast<ImLineEdit *>(widget);
    if (w) {
      QLOG_DEBUG() << "Found linedit";
    }
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
        entry->getView()->installEventFilter(this);

        entry->getXmlForRoot(p);
        m_tabs->insertTab(ix,entry,p.getShortText());
        m_tabs->setCurrentIndex(ix);
      }
      else {
        QLOG_DEBUG() << "could not clone graphcicsentry";
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
  QString v;
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QMap<QString,QString> cmdOptions = app->getOptions();

  m_startupNode = cmdOptions.value("node");
  if (! m_startupNode.isEmpty() && ! m_startupNode.startsWith("n")) {
    m_startupNode = "n" + m_startupNode;
  }
  m_startupRoot = cmdOptions.value("root");
  QScopedPointer<QSettings> settings(app->getSettings());
  settings->beginGroup("System");
  m_dbName = settings->value("Database","lexicon.sqlite").toString();
  if (cmdOptions.contains("db")) {
    m_dbName = cmdOptions.value("db");
  }
  m_applicationCssFile = settings->value("Stylesheet","app.css").toString();
  QString ar = settings->value("Arabic font").toString();
  if (! ar.isEmpty()) {
    arFont.fromString(ar);
  }
  m_iconTheme = settings->value("Theme",QString()).toString();
  m_toolbarText = settings->value("Toolbar text",false).toBool();
  m_nullMap = settings->value("Null map","Native").toString();
  m_currentMap = settings->value("Current map","Native").toString();

  m_interfaceWarning = settings->value("Show interface warning",true).toBool();
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
  m_minimalAction->setChecked(settings->value("Minimal interface",false).toBool());

  v  = settings->value("Navigation","root").toString();

  if (v.toLower() == "root") {
    m_navMode = Lane::By_Root;
  }
  else {
    m_navMode = Lane::By_Page;
  }
  v = settings->value("Title",tr("Lane's Arabic-English Lexicon")).toString();
  this->setWindowTitle(v);

  m_toolbarIconSize = settings->value("Icon size",QSize(16,16)).toSize();

  m_keymapsEnabled = settings->value("Keymaps",false).toBool();

  m_linkContents = settings->value("Contents linked",false).toBool();

  settings->endGroup();

  settings->beginGroup("Search");

  m_searchOptions.setNewTab(settings->value("New tab",true).toBool());
  m_searchOptions.setActivateTab(settings->value("Switch tab",true).toBool());

  v  = settings->value("Type",QString("normal")).toString();
  if (v == "normal")
    m_searchOptions.setSearchType(SearchOptions::Normal);
  else
    m_searchOptions.setSearchType(SearchOptions::Regex);

  m_searchOptions.setIgnoreDiacritics(settings->value("Ignore diacritics",true).toBool());
  m_searchOptions.setWholeWordMatch(settings->value("Whole word",false).toBool());


  // v = settings->value("Target",QString("Arabic")).toString();
  // if (v == "Arabic")
  //   m_searchOptions |= Lane::Arabic;
  // else
  //   m_searchOptions |= Lane::Buckwalter;

  m_searchOptions.setKeymaps(m_keymapsEnabled);

  settings->endGroup();

  settings->beginGroup("FullSearch");
  m_searchOptions.setIncludeHeads(settings->value("Include heads",false).toBool());
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
  for(int i=0; i < groups.size();i++) {
    settings->beginGroup(groups[i]);
    v = settings->value("file",QString()).toString();
    QFile file(v);
    if ( file.exists() ) {
      if (! im_load_map_from_json(m_mapper,v.toUtf8().constData(),groups[i].toUtf8().constData())) {
        QLOG_WARN() << QString(tr("Could not load <%1> from file <%2>")).arg(groups[i]).arg(v);
      }
      else {
        m_definedMaps << groups[i];
          }
    }
    else {
      QLOG_WARN() << QString(tr("Could not load <%1> from file <%2> - file not found")).arg(groups[i]).arg(v);
    }
    settings->endGroup();
  }
  settings->endGroup();
  if ((m_currentMap != m_nullMap) && ! m_definedMaps.contains(m_currentMap) ) {
    QLOG_WARN() << QString(tr("Default map <%1> not found in settings")).arg(m_currentMap);
  }
}
void LanesLexicon::writeSettings() {
  QString v;
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QScopedPointer<QSettings> settings(app->getSettings());

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
        v = entry->getHome();
        if (! v.isEmpty()) {
          settings->setValue("home",v);
        }
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
    settings->setValue("Minimal interface",m_minimalAction->isChecked());
    settings->setValue("Show interface warning",m_interfaceWarning);
    settings->setValue("Current map",m_currentMap);
    if (m_navMode == Lane::By_Root) {
      settings->setValue("Navigation","root");
    }
    else {
      settings->setValue("Navigation","page");
    }
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
  QString v;
  QString home;
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QMap<QString,QString> cmdOptions = app->getOptions();


  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());
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
      v = settings->value("place",QString()).toString();
      qreal scale = settings->value("zoom",1.0).toReal(&ok);
      if ( !ok ) {
        scale = 1.0;
      }
      Place p = Place::fromString(v);
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
      home = settings->value("home",QString()).toString();
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
        entry->setHome(home);

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
    int page = entry->getPageNumber();
    if (page == m_lastPage) {
      setStatus(tr("This is the last page"));
      //      QLOG_INFO() << tr("At last page");
      return;
    }
    if (page == -1) {
      QLOG_WARN() << "Cannot find current page" << page;
    }
    else {
      Place p = entry->getPlace(1);
      p.setPage(page + 1);
      entry->setPagingForward();
      entry->getPage(p);
    }
  }
}
void LanesLexicon::onPrevPage() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    int page = entry->getPageNumber(1); // get the lowest page number
    if (page == m_firstPage) {
      //      QLOG_INFO() << tr("At first page");
      setStatus(tr("This is the first page"));
      return;
    }
    if (page == -1) {
      QLOG_WARN() << "Cannot find current page" << page;
    }
    else {
      Place p = entry->getPlace();
      p.setPage(page - 1);
      entry->setPagingForward();
      entry->getPage(p);
    }
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
  if ( m_currentMap.isEmpty()) {
    return s;
  }
  if (UcdScripts::isScript(s,"Arabic")) {
    return s;
  }
  bool ok;
  QString t = im_convert_string(m_mapper,m_currentMap,s,&ok);
  if ( ! ok ) {
    QLOG_INFO() << QString(tr("Error converting <%1> with map <%2>")).arg(s).arg(m_currentMap);
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
  if (id.length() > 1) {
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
}
void LanesLexicon::bookmarkJump(const QString & id) {
  int options = 0;
  QLOG_DEBUG() << Q_FUNC_INFO << id;
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
  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());
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

}
void LanesLexicon::setupBookmarkShortcuts() {
  /**
   * setup all the bookmark shortcuts and actions
   *   add,jump,save,revert,list
   *
   *  TODO bookmark Icons ?
   */
  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());
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
  setStatus(QString(tr("Added bookmark:%1")).arg(id));
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
  QLOG_INFO() << txt;
  statusBar()->showMessage(txt);
}
void LanesLexicon::updateStatusBar() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  QString headseparator;
  if (entry) {
    Place p = entry->getPlace();
    QString root = tr("Root:") + qobject_cast<Lexicon *>(qApp)->spanArabic(p.getRoot());
    QString head = p.getWord();
    if (! head.isEmpty()) {
      head = tr("Entry:") + qobject_cast<Lexicon *>(qApp)->spanArabic(head);
      headseparator = ",";
    }
    qobject_cast<Lexicon *>(qApp)->spanArabic(p.getWord());
    QString page = QString(QObject::tr("Vol %1,Page %2")).arg(p.getVol()).arg(p.getPage());
    QString node = p.getNode();
    if ( ! node.isEmpty()) {
      node = "(" + node + ")";
    }
    QString html = QString("<body class=\"place\">%1,%2%3 %4 %5</body>").arg(root).arg(head).arg(headseparator).arg(page).arg(node);
    m_placeIndicator->setText(html);
  }
  else {
    m_placeIndicator->setText("");

  }
  m_keymapsButton->setEnabled(m_keymapsEnabled);
}
/**
 * navigation mode can be changed:
 *  (1) from the toolbar by changing the dropdown
 *  (2) from the menubar by selecting the mode
 *  (3) by shortcut
 */
void LanesLexicon::updateMenu() {

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
void LanesLexicon::onNavigationMenuChanged(QAction * action) {
  m_navigationButton->setDefaultAction(action);
  m_navigationButton->menu()->hide();
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
void LanesLexicon::onAbout() {
  AboutDialog d;
  d.exec();
}
void LanesLexicon::testSlot() {
  QLOG_DEBUG() << Q_FUNC_INFO;
}
void LanesLexicon::searchForPage() {
  int page = 0;
  SearchOptions options;
  PageSearchDialog * d = new PageSearchDialog(this);
  d->setOptions(m_searchOptions);
  if (d->exec()) {
      page = d->getPage();
      d->getOptions(options);
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
    int taboptions = 0;
    if (options.newTab()) {
      taboptions |= Lane::Create_Tab;
    }
    if (options.activateTab()) {
      taboptions |= Lane::Switch_Tab;
    }
    this->gotoPlace(p,taboptions);
  }
  else {
    p.setPage(page);
    this->onGoToPage(p);
  }
  this->syncContents();
}
void LanesLexicon::searchForRoot() {
  ArabicSearchDialog * d = new ArabicSearchDialog(SearchOptions::Root,this);
  d->setOptions(m_searchOptions);
  if (d->exec()) {
    QString t = d->getText();
    if (! t.isEmpty()) {
      /// TODO maybe change this to if == Latin
      if (! UcdScripts::isScript(t,"Arabic")) {
        t = convertString(t);
      }
      SearchOptions opts;
      d->getOptions(opts);
      Place p = showPlace(Place(t),opts.newTab());
      if (! p.isValid()) {
        QMessageBox msgBox;
        msgBox.setObjectName("rootnotfound");
        msgBox.setTextFormat(Qt::RichText);
        QString html = (qobject_cast<Lexicon *>(qApp))->spanArabic(t,"rootnotfound");
        msgBox.setText(QString(tr("Root not found:%1")).arg(html));
        msgBox.exec();
      }
      else {
        /// TODO make dependent
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
  QLOG_DEBUG() << Q_FUNC_INFO << searchType;
  QString target = t;
  SearchOptions options;
  d->getOptions(options);
  if (searchType == SearchOptions::Word) {
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
  if (searchType == SearchOptions::Entry) {
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
  ArabicSearchDialog * d = new ArabicSearchDialog(SearchOptions::Word,this);
  d->setOptions(m_searchOptions);
  if (d->exec()) {
    QString t = d->getText();
    if (! t.isEmpty()) {
      this->search(SearchOptions::Word,d,t);
    }
  }
  delete d;
  m_searchButton->menu()->hide();
}

/// TODO these needs to search the entry looking for bareword or word
void LanesLexicon::searchForEntry() {
  ArabicSearchDialog * d = new ArabicSearchDialog(SearchOptions::Entry,this);
  d->setOptions(m_searchOptions);
  if (d->exec()) {
    QString t = d->getText();
    if (! t.isEmpty()) {
      this->search(SearchOptions::Entry,d,t);
    }
  }
  delete d;
}
void LanesLexicon::searchForNode() {
  NodeSearchDialog * d = new NodeSearchDialog(this);
  d->setOptions(m_searchOptions);
  if (d->exec()) {
    QString t = d->getText();
    if (! t.isEmpty()) {
      /// TODO make this an option to always show root
      /// show only node
      //        Place p = showNode(t,true);
      Place p;
      p.setNode(t);
      SearchOptions options;
      int opts = 0;

      if (options.newTab()) {
        opts |= Lane::Create_Tab;
      }
      if (options.activateTab()) {
        opts |= Lane::Switch_Tab;
      }
      p = showPlace(p,opts);
      if (! p.isValid()) {
        QMessageBox msgBox;
        msgBox.setText(QString(tr("Node not found: %1")).arg(t));
        msgBox.exec();
      }
    }
  }
  delete d;
  m_searchButton->menu()->hide();
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
  printCurrentPage();
}
void LanesLexicon::printCurrentPage(const QString & node) {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (!entry)
    return;

  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());
  settings->beginGroup("Printer");
  m_printerReUse = settings->value(SID_PRINTER_USE).toBool();

  if (! m_printer.isValid())
    m_printerReUse = false;


  if ( m_printerReUse ) {
    entry->print(m_printer,node);
    return;
  }

  QPrintDialog printDialog(&m_printer, this);
  if (printDialog.exec() == QDialog::Accepted) {
    if (m_printer.isValid()) {
      entry->print(m_printer,node);
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
      QLOG_DEBUG() << "printer is not valid";
    }
  */


}
void LanesLexicon::pageSearch() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if ( entry ) {
    entry->setFocus();
    if (entry->search()) {
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
  QLOG_DEBUG() << Q_FUNC_INFO;
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
      entry->getView()->installEventFilter(this);

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
  QLOG_DEBUG() << Q_FUNC_INFO << v;
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
        SearchOptionsWidget * search = qobject_cast<SearchOptionsWidget *>(widget);
        if (search) {
          search->setKeymapsEnabled(v);
        }
      }
    }
  }
  m_searchOptions.setKeymaps(v);

  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());
  settings->beginGroup("System");
  settings->setValue("Keymaps",v);
}
QString LanesLexicon::getActiveKeymap() const {
  return m_currentMap;
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
  entry->getView()->installEventFilter(this);

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
  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());
  QStringList groups = settings->childGroups();
  QString icongroup;
  if (theme.isEmpty()) {
    icongroup = "Icons-Default";
  }
  else {
    icongroup = QString("Icons-%1").arg(theme);
  }
  if (! groups.contains(icongroup)) {
    QLOG_WARN() << QString(tr("Theme not found:%1")).arg(theme);
    return;
  }
  settings->beginGroup(icongroup);

  QString imgdir = settings->value("Directory","images").toString();
  QFileInfo fi(imgdir);
  if (! fi.exists()) {
    QLOG_WARN() << "theme directory not found" << imgdir;
    return;
  }
  if (! fi.isDir()) {
    QLOG_WARN() << "theme directory is not a directory" << imgdir;
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

  iconfile = settings->value("About",QString()).toString();
  setIcon(m_aboutAction,imgdir,iconfile);

  iconfile = settings->value("Bookmarks",QString()).toString();
  setIcon(m_bookmarkAction,imgdir,iconfile);

  //  iconfile = settings->value("Move",QString()).toString();
  //  setIcon(m_navigationAction,imgdir,iconfile);

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

  iconfile = settings->value("Preferences",QString()).toString();
  setIcon(m_optionsAction,imgdir,iconfile);

  iconfile = settings->value("Logs",QString()).toString();
  setIcon(m_logViewerAction,imgdir,iconfile);

  QIcon icon;
  iconfile = settings->value("Link",QString()).toString();


  fi.setFile(imgdir,iconfile);
  icon.addPixmap(fi.absoluteFilePath(),QIcon::Normal,QIcon::On);

  fi.setFile(imgdir,settings->value("Unlink",QString()).toString());
  icon.addPixmap(fi.absoluteFilePath(),QIcon::Normal,QIcon::Off);

  m_linkAction->setIcon(icon);
  m_linkAction->setChecked(m_linkContents);

}
void LanesLexicon::sync() {
  QLOG_DEBUG() << Q_FUNC_INFO;
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
void LanesLexicon::printNode(const QString & node) {
  QLOG_DEBUG() << Q_FUNC_INFO << node;
  printCurrentPage(node);
}
void LanesLexicon::reloadEntry(const QString & css,const QString & xslt) {
  QLOG_DEBUG() << Q_FUNC_INFO;
  for(int i=0;i < m_tabs->count();i++) {
    GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
    if (entry) {
      entry->onReload(css,xslt);
    }
  }
}
void LanesLexicon::revertEntry() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  for(int i=0;i < m_tabs->count();i++) {
    GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
    if (entry) {
      entry->onReload();
    }
  }
}
void LanesLexicon::onOptions() {
  OptionsDialog * d = new OptionsDialog(this);
  d->exec();
  delete d;
}
