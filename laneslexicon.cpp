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
#include "notes.h"
#include "notebrowser.h"
#include "tabwidget.h"
#include "laneslexicon.h"
#include "searchoptionswidget.h"
#include "searchdialogs.h"
#include "fullsearch.h"
#include "headsearch.h"
#include "definedsettings.h"
#include "optionsdialog.h"
#include "logviewer.h"
#include "about.h"
#include "externs.h"
#include "historylist.h"
#include "fontchangedialog.h"
#include "zoomdialog.h"
#include "editview.h"
#include "themedialog.h"
#include "deletethemedialog.h"
#include "createthemedialog.h"
#include "definedsql.h"
#include "showmapwidget.h"
#include "exportsearchdialog.h"
#include "helpview.h"
#include "savepagesetdialog.h"
#include "editpagesetdialog.h"
#include "loadpagesetdialog.h"
#include "tablistdialog.h"
#include "textwidthdialog.h"
LanesLexicon::LanesLexicon(QWidget *parent) :
    QMainWindow(parent)

{
  QLOG_DEBUG() << Q_FUNC_INFO;
  setAttribute(Qt::WA_DeleteOnClose);
  setObjectName("lexicon");
  m_ok = false;
  m_history = 0;
  m_historyPos = -1;
  m_revertEnabled = false;
  m_editView = NULL;
  m_mapper = im_new();
  m_logview = NULL;
  m_shortcutMap = NULL;
  m_bookmarkMap = NULL;
  m_helpview = NULL;
  m_tabStyle = LanesLexicon::InsertTab;
  m_rootSearchDialog = NULL;
  m_wordSearchDialog = NULL;
  m_headSearchDialog = NULL;
  m_nodeSearchDialog = NULL;
  m_pageSearchDialog = NULL;
  m_allowNavMode = false;
  createActions();
  readSettings();

  if (! sanityCheck()) {
    return;
  }
  loadStyleSheet();

  m_tree = new ContentsWidget(this);
  m_tree->setObjectName("treeroots");
  m_tree->installEventFilter(this);
  m_tabs = new TabWidget(this);
  m_tabs->setTabsClosable(true);
  m_tabs->installEventFilter(this);
  m_tabs->tabBar()->installEventFilter(this);

  connect(m_tabs,SIGNAL(tabsChanged()),this,SLOT(tabsChanged()));
  connect(m_tabs,SIGNAL(closeOtherTab(int)),this,SLOT(onCloseOtherTabs(int)));
  connect(m_tabs,SIGNAL(closeThisTab(int)),this,SLOT(onCloseTab(int)));
  connect(m_tabs,SIGNAL(duplicateTab(int)),this,SLOT(onDuplicateTab(int)));
  connect(m_tabs,SIGNAL(savePageSet()),this,SLOT(onSavePageSet()));
  /// at the end of the history, but we should be able to restore from settings
  /// TODO would we want restore our current position in history?
  m_history = new HistoryMaster(m_historyDbName);
  if ( ! m_history->isOk()) {
    QLOG_WARN() << "History is not available";
    m_historyEnabled = false;
  }
  else {

  }
  m_history->setEnabled(m_historyEnabled);
  QLOG_DEBUG() << "history enabled" << m_historyEnabled;
  QSettings * settings  = (qobject_cast<Lexicon *>(qApp))->getSettings();
  settings->setIniCodec("UTF-8");
  m_notes = new NoteMaster(settings);

  if (m_docked) {
    m_treeDock = new QDockWidget("Contents",this);
    m_treeDock->setObjectName("contentsdock");
    m_treeDock->setAllowedAreas(Qt::LeftDockWidgetArea |
                                Qt::RightDockWidgetArea);
    m_treeDock->setWidget(m_tree);
    addDockWidget(Qt::LeftDockWidgetArea, m_treeDock);
    m_treeDock->installEventFilter(this);
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

  setIcons(m_currentTheme);
  createMenus();
  createToolBar();
  createStatusBar();

  if (m_allowNavMode) {
    if (m_navMode == LanesLexicon::ByRoot) {
      onNavigationMenuChanged(m_navModeRootAction);
      //    ->setChecked(true);
    }
    else {
      onNavigationMenuChanged(m_navModePageAction);//->setChecked(false);
    }

    if (m_navMode == LanesLexicon::ByPage) {
      m_navModePageAction->setChecked(true);
    }
    else {
      m_navModeRootAction->setChecked(true);
    }
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


  connect(m_tree,SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)),this,SLOT(treeItemDoubleClicked(QTreeWidgetItem *,int)));

  connect(m_tree,SIGNAL(itemActivated(QTreeWidgetItem *,int)),this,SLOT(entryActivated(QTreeWidgetItem *,int)));

  connect(m_tabs,SIGNAL(tabCloseRequested(int)),this,SLOT(onCloseTab(int)));
  connect(m_tabs,SIGNAL(currentChanged(int)),this,SLOT(currentTabChanged(int)));

  if (m_restoreTabs) {
    restoreTabs();
  }
  /**
   *  if we asked for a node or root at the command line, check if it showing in a the
   *  tabs and switch tabs if it is.
   *  Otherwise, create a new tab
   *
   *  When done, call currentTabChanged(), so the tree can be kept in sync
   */
  this->showStartupEntry();

  if (m_restoreBookmarks) {
    restoreBookmarks();
  }

  m_tree->resizeColumnToContents(0);

  setupHistory();

  // load the sitemap
  QSettings sitemap("sitemap.ini",QSettings::IniFormat);
  sitemap.setIniCodec("UTF-8");
  sitemap.beginGroup("Site Map");
  QStringList keys = sitemap.childKeys();
  for(int i=0;i < keys.size();i++) {
    if (! keys[i].isEmpty()) {
      m_siteMap.insert(keys[i].toLower(),sitemap.value(keys[i]).toString());
    }
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
  /*
  QApplication::setActiveWindow(m_tabs->currentWidget());

  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    entry->focusPlace();
    Place p = entry->getPlace();
    if (p.isValid()) {
      m_historyPos = m_history->hasPlace(p);
      setupHistory(m_historyPos);
    }
    QKeyEvent * event;
    int k = 0x20;
    event = new QKeyEvent(QEvent::KeyPress, k, Qt::NoModifier,QString(QChar(k)));
    QApplication::postEvent(entry,event);
  }
  */
  /*
  QList<QShortcut *> edits = this->findChildren<QShortcut *>();
  foreach(QShortcut *  widget,edits) {
    QLOG_DEBUG() << widget->key().toString();
  }
  edits.clear();
  */

  /**
   * this gets around the problem that treewidgets viewport has not been updated
   * At this point it reports its size 84x0
   * When syncFromEntry runs with the timer, view port is 160x809
   */


 QTimer::singleShot(200, this, SLOT(onReady()));

  QLOG_INFO() << "-----------------------";
  QLOG_INFO() << "Initialisation complete";
  QLOG_INFO() << "-----------------------";
  QMap<QString,QString> cmdOptions = getLexicon()->getOptions();
  if ( cmdOptions.contains("listicons")) {
    QList<QMenu *> menus = m_mainmenu->findChildren<QMenu *>(QString(),Qt::FindDirectChildrenOnly);
    for (int j=0;j < menus.size();j++) {
      QList<QAction *> actions = menus[j]->actions();//m_viewMenu->actions();
      if (! menus[j]->title().isEmpty()) {
        //        QLOG_INFO() << QString(">> %1 (%2)").arg(menus[j]->title()).arg(actions.size());
        for(int i=0;i < actions.size();i++) {
          QLOG_INFO() << menus[j]->title() << i << actions[i]->isCheckable() << actions[i]->text() << actions[i]->property("FILENAME").toString();
        }
      }
    }
  }
}

LanesLexicon::~LanesLexicon()
{
  //  cleanup();
}
void LanesLexicon::showStartupEntry() {
  Place p;
  int ix;
  if (! m_startupNode.isEmpty()) {
    p.setNode(m_startupNode);
    ix = this->hasPlace(p,GraphicsEntry::NodeSearch,true);
  }
  else if (! m_startupRoot.isEmpty()) {
    p.setRoot(convertString(m_startupRoot));
    ix = this->hasPlace(p,GraphicsEntry::RootSearch,true);
  }
  else if ((m_tabs->count() == 0) && ! m_firstRoot.isEmpty()) {
    p.setRoot(m_firstRoot);
    ix = this->hasPlace(p,GraphicsEntry::RootSearch,true);
  }
  if (! p.isValid())
    return;

  if (ix == -1) {  // not showing in another tab
    showPlace(p,false,true);
    m_tree->ensurePlaceVisible(p);
  }
  else {
    this->currentTabChanged(ix);
  }
}
/**

 */
void LanesLexicon::restoreSavedState() {
  SETTINGS

  settings.beginGroup("System");
  QMap<QString,QString> cmdOptions = getLexicon()->getOptions();
  if (! cmdOptions.contains("nostate"))
    this->restoreState(settings.value(SID_SYSTEM_STATE).toByteArray());


  resize(settings.value(SID_SYSTEM_SIZE,QSize(800,950)).toSize());
  move(settings.value(SID_SYSTEM_POS,QPoint(450,20)).toPoint());


}
void LanesLexicon::closeEvent(QCloseEvent * event) {
  cleanup();

  QMainWindow::closeEvent(event);
}
void LanesLexicon::cleanup() {
  QLOG_DEBUG() << Q_FUNC_INFO;

  if (m_ok) {
    writeSettings();
  }

  if (m_history) {
    delete m_history;
  }

  while(m_tabs->count() > 0) {
    this->onCloseTab(0);
  }
  if (m_editView != NULL){
    delete m_editView;
    m_editView = 0;
  }

  if (m_logview != NULL) {
    delete m_logview;
    m_logview = 0;
  }
  if (m_helpview != NULL) {
    delete m_helpview;
    m_helpview = 0;
  }
  if (m_rootSearchDialog != NULL) {
    delete m_rootSearchDialog;
    m_rootSearchDialog = 0;
  }
  if (m_wordSearchDialog != NULL) {
    delete m_wordSearchDialog;
    m_wordSearchDialog = 0;
  }

  if (m_headSearchDialog != NULL) {
    delete m_headSearchDialog;
    m_headSearchDialog = 0;
  }
  if (m_nodeSearchDialog != NULL) {
    delete m_nodeSearchDialog;
    m_nodeSearchDialog = 0;
  }
  if (m_pageSearchDialog != NULL) {
    delete m_pageSearchDialog;
    m_pageSearchDialog = 0;
  }
  QWidgetList widgets = QApplication::topLevelWidgets();
  for(int i=0;i < widgets.size();i++) {
    ShowMapWidget * w = qobject_cast<ShowMapWidget *>(widgets[i]);
    if (w) {
        /// has DeleteOnClose
      w->close();
    }
    OptionsDialog * d = qobject_cast<OptionsDialog *>(widgets[i]);
    if (d) {
        /// has deleteOnClose
      d->onClose();
    }
  }
  if (m_db.isOpen()) {
    m_db.close();
  }
  if (m_notesDb.isOpen())
    m_notesDb.close();
  QFontDatabase::removeAllApplicationFonts();
  freeXslt();
  im_free(m_mapper);
}
void LanesLexicon::onSetInterface(bool triggered) {
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
  if (m_shortcutMap != NULL) {
    QShortcut * sc = qobject_cast<QShortcut *>(m_shortcutMap->mapping(QString(SID_SHORTCUT_TOGGLE_INTERFACE)));
    if (sc) {
      QCheckBox * noshow = new QCheckBox(tr("Check this box to stop this dialog showing again"));
      QMessageBox msgBox;
      msgBox.setCheckBox(noshow);
      msgBox.setWindowTitle(QGuiApplication::applicationDisplayName());
      QString errorMessage(tr("Warning"));
      QString info(tr("The toolbars and menubars will not be visible"));
      QString next = QString(tr("To make them visible again, use the shortcut %1")).arg(sc->key().toString());
      msgBox.setText("<html><head/><body><h2>" + errorMessage + "</h2><p>"
                     + info + "</p><p>" + next + "</p></body></html>");
      msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
      msgBox.exec();
      m_interfaceWarning = ! noshow->isChecked();
    }
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
  connect(entry,SIGNAL(prevHead(const Place &)),this,SLOT(movePreviousHead(const Place &)));

  connect(entry,SIGNAL(historyPositionChanged(int)),this,SLOT(historyPositionChanged(int)));
  connect(entry,SIGNAL(historyAddition(const Place &)),this,SLOT(historyAddition(const Place &)));
  connect(entry,SIGNAL(bookmarkAdd(const QString &,const Place &)),this,SLOT(bookmarkAdd(const QString &,const Place &)));


  connect(entry,SIGNAL(placeChanged(const Place &)),this,SLOT(placeChanged (const Place &)));
  connect(entry,SIGNAL(clearPage()),this,SLOT(pageClear()));
  connect(entry,SIGNAL(searchPage()),this,SLOT(pageSearch()));

  connect(entry,SIGNAL(gotoNode(const Place &,bool,bool)),this,SLOT(showPlace(const Place &,bool,bool)));
  connect(entry,SIGNAL(printNode(const QString &)),this,SLOT(printNode(const QString &)));
  connect(entry,SIGNAL(showNode(const QString &,bool)),this,SLOT(showSearchNode(const QString &,bool)));
  connect(entry,SIGNAL(printPage()),this,SLOT(pagePrint()));
  connect(entry,SIGNAL(searchFinished()),this,SLOT(pageSearchComplete()));
  connect(entry,SIGNAL(searchStarted()),this,SLOT(pageSearchStart()));
  connect(entry,SIGNAL(searchFoundNext()),this,SLOT(pageSearchStart()));
}
void LanesLexicon::onCloseOtherTabs(int /* index */) {
  QLOG_DEBUG() << Q_FUNC_INFO;
  m_tabs->setUpdatesEnabled(false);
  QString label = m_tabs->tabText(m_tabs->currentIndex());
  QWidget * w = m_tabs->currentWidget();
  m_tabs->removeTab(m_tabs->currentIndex());
  this->closeAllTabs();
  m_tabs->addTab(w,label);
  m_tabs->setUpdatesEnabled(true);
}
/**
 *
 *
 * @param w
 *
 * @return
 */
bool LanesLexicon::deleteWidget(QWidget * w) {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(w);
  if (entry) {
    entry->close();
    delete entry;
    return true;
  }
  HeadSearchWidget * results = qobject_cast<HeadSearchWidget *>(w);
  if (results) {
      delete results;
      return true;
  }
  NoteBrowser * notes = qobject_cast<NoteBrowser *>(w);
  if (notes) {
      delete notes;
      return true;
  }
  FullSearchWidget * search = qobject_cast<FullSearchWidget *>(w);
  if (search) {
    delete search;
    return true;
  }
  return false;
}
void LanesLexicon::onCloseTab(int ix) {
  QLOG_DEBUG() << Q_FUNC_INFO << ix;
  QWidget * w = m_tabs->widget(ix);
  m_tabs->removeTab(ix);
  if (! deleteWidget(w)) {
    delete w;
  }
}
void LanesLexicon::closeAllTabs() {
  int m = m_tabs->count();

  for(int i = m -1;i > -1;i--) {
    QWidget * w = m_tabs->widget(i);
    m_tabs->removeTab(i);
    if (! deleteWidget(w)) {
      delete w;
    }
  }
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
  else if (key == SID_SHORTCUT_SEARCH_PAGE) {
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
    this->onFocusContent();
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
  else if (key == SID_SHORTCUT_BOOKMARK_INPUT) {
    this->bookmarkInput();
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
    if (m_historyPos == -1) {
      return;
    }
     int id = m_history->getNewestId();
     //     QLOG_DEBUG() << QString("history pos %1 (last = %2").arg(m_historyPos).arg(id);
     if (m_historyPos < id) {
       m_historyPos++;
       Place p = setupHistory(m_historyPos);
       if (p.isValid()) {
         this->showHistory(p);
       }
     }
     else {
       statusMessage(tr("At start of history"));
     }
  }
  else if (key == SID_SHORTCUT_HISTORY_BACK) {
    // if we don't have a history pos show the first item in the list
    int id;
     if (m_historyPos == -1) {
       m_historyPos = m_history->getNewestId();
       if (m_historyPos == -1) {
         setStatus(tr("No history available"));
         return;
       }
       /// increment it, because we are going to decrement
       m_historyPos++;
     }

     id = m_history->getOldestId();
     //     QLOG_DEBUG() << QString("history pos %1 (last = %2").arg(m_historyPos).arg(id);
     // if we are not at the beginning decrement it
     if (m_historyPos > id) {
       m_historyPos--;
     }
     else {
       statusMessage(tr("At end of history"));
       return;
     }
     Place p = setupHistory(m_historyPos);
     if (p.isValid()) {
       this->showHistory(p);
     }
  }
  else if (key == SID_SHORTCUT_HISTORY_ENABLE) {
    m_historyEnabled = true;
    setupHistory();
    setStatus(tr("History enabled"));
  }
  else if (key == SID_SHORTCUT_HISTORY_DISABLE) {
    m_historyEnabled = false;
    setupHistory();
    setStatus(tr("History disabled"));
  }
  else if (key == SID_SHORTCUT_HISTORY_LIST) {
    onShowHistory();
  }
  else if (key == SID_SHORTCUT_KEEP_SYNCED) {
    onLinkChanged();
  }
  else if (key == SID_SHORTCUT_SYNC_PAGE_WITH_CONTENTS) {
    syncFromContents();
  }
  else if (key == SID_SHORTCUT_SYNC_CONTENTS_WITH_PAGE) {
    syncFromEntry();
  }
  else if (key == SID_SHORTCUT_SHOW_NOTES) {
    showNoteBrowser();
  }
  else if (key == SID_SHORTCUT_DELETE_TAB) {
    this->onCloseTab(m_tabs->currentIndex());
  }
  else if (key == SID_SHORTCUT_DELETE_OTHER_TABS) {
    this->onCloseOtherTabs(-1);
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
  else if (key == SID_SHORTCUT_KEYMAPS_HELP) {
    this->onKeymapHelp();
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
  else if (key == SID_SHORTCUT_OPTIONS) {
    this->onOptions();
  }
  else if (key == SID_SHORTCUT_ARABIC_FONT) {
    this->onChangeArabicFont();
  }
  else if (key == SID_SHORTCUT_MENU_SHOW) {
    m_mainmenu->show();
    m_showMenuAction->setChecked(true);
  }
  else if (key == SID_SHORTCUT_XREF_MODE) {
    this->onXrefMode();
  }
  else if (key == SID_SHORTCUT_LIST_TABS) {
    this->onTabList();
  }
  else if (key == SID_SHORTCUT_ALLOW_DUPLICATES) {
    this->onAllowDuplicates();
  }
  else {
    QLOG_WARN() << "Unhandled shortcut" << key;
  }
  updateStatusBar();
  updateMenu();
}
/**
 * setup the shortcuts from the conf
 * the help shortcut is created in readSettings
 * TODO don't think we need the update stuff since the old shortcuts are
 * being deleted.
 */
void LanesLexicon::setupShortcuts() {
  bool update = false;
  if (m_shortcutMap == NULL) {
    m_shortcutMap = new QSignalMapper(this);
    connect(m_shortcutMap,SIGNAL(mapped(QString)),this,SLOT(shortcut(const QString &)));
  }
  else {
    update = true;
  }
  m_shortcutMap->blockSignals(true);
  SETTINGS
  settings.beginGroup("Shortcut");

  QStringList keys = settings.childKeys();
  for(int i=0;i < keys.size();i++) {
    if (keys[i] == "Go Tab") {
      /// TODO make this a user string of characters
      /// e.g 123456789abcdef
      for(int j=1;j < 10;j++) {
        QString ks = QString("%1,%2").arg(settings.value(keys[i]).toString()).arg(j);
        QString name = QString("%1-%2").arg(keys[i]).arg(j);
        addShortcut(name,ks,update);
      }
    }
    else {
      addShortcut(keys[i],settings.value(keys[i]).toString(),update);
    }
  }
  settings.endGroup();
  m_shortcutMap->blockSignals(false);

}
void LanesLexicon::addShortcut(const QString & name,const QString & k,bool update) {
  //  QLOG_DEBUG() << Q_FUNC_INFO << name << k << update;
  QShortcut * sc;
  if (update) {
    sc = qobject_cast<QShortcut *>(m_shortcutMap->mapping(name));
    if (sc) {
      m_shortcutMap->removeMappings(sc);
      delete sc;
      //      sc->setEnabled(false);
      //      sc->setKey(QKeySequence(k));
      //      sc->setEnabled(true);
      //      return;
    }
  }
  sc = new QShortcut(k,this);
  sc->setObjectName(name);
  connect(sc,SIGNAL(activated()),m_shortcutMap,SLOT(map()));
  m_shortcutMap->setMapping(sc,name);
}


/**
 * load the application level stylesheet and applies it
 *
 *
 */
void LanesLexicon::loadStyleSheet() {
  QLOG_DEBUG() << Q_FUNC_INFO << m_applicationCssFile;
  if (m_applicationCssFile.isEmpty()) {
    return;
  }
  QString filename = getLexicon()->getResourceFilePath(Lexicon::Stylesheet,m_applicationCssFile);
  if (filename.startsWith("Error:")) {
    QStringList errors = filename.split(":");
    QString msg;
    if (errors.size() >= 2) {

      msg = QString(tr("<p>Cannot find file: %1</p> \
                        <p>Directory is:%2</p> \
                        <p>Please review Preferences -> Layout</p>")).arg(errors[2]).arg(errors[1]);
    }
    else {
      msg = QString(tr("Cannot find file: %1")).arg(m_applicationCssFile);
    }
    QMessageBox::warning(this,tr("Missing Application Stylesheet"),msg,QMessageBox::Ok);
    QLOG_WARN() << QString(tr("Missing application CSS file: %1")).arg(m_applicationCssFile);
    return;
  }
  QFile f(filename);
  if ( ! f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QLOG_WARN() << QString(tr("Error opening stylesheet: %1")).arg(filename);
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

void LanesLexicon::createActions() {
  //  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());

  m_exitAction = new QAction(tr("E&xit"),this);
  //  m_exitAction->setProperty("ICONKEY",
  connect(m_exitAction,SIGNAL(triggered()),this,SLOT(onExit()));

  m_editViewAction = new QAction(tr("&Edit view"),this);
  connect(m_editViewAction,SIGNAL(triggered()),this,SLOT(onEditView()));

  m_minimalAction = new QAction(tr("Minimal &interface"),this);
  m_minimalAction->setCheckable(true);
  connect(m_minimalAction,SIGNAL(triggered(bool)),this,SLOT(onSetInterface(bool)));

  m_optionsAction = new QAction(tr("&Preferences"),this);
  connect(m_optionsAction,SIGNAL(triggered()),this,SLOT(onOptions()));

  m_logViewerAction = new QAction(tr("&View logs"),this);
  connect(m_logViewerAction,SIGNAL(triggered()),this,SLOT(onLogViewer()));

  m_linkAction = new QAction(tr("Lin&k contents"),this);
  m_linkAction->setCheckable(true);
  m_linkAction->setChecked(m_linkContents);

  m_syncFromEntryAction = new QAction(tr("Align co&ntents with entry"),this);
  m_syncFromContentsAction = new QAction(tr("Align &entry with contents"),this);

  m_defaultScaleAction = new QAction(tr("Set default &zoom"),this);
  m_defaultWidthAction = new QAction(tr("Set default text &width"),this);

  m_showContentsAction = new QAction(tr("Show &contents"),this);
  m_showContentsAction->setCheckable(true);

  m_showToolbarAction = new QAction(tr("Show &toolbar"),this);
  m_showToolbarAction->setCheckable(true);
  m_showMenuAction = new QAction(tr("Show &menu"),this);
  m_showMenuAction->setCheckable(true);

  m_showStatusBarAction = new QAction(tr("Show status &bar"),this);
  m_showStatusBarAction->setCheckable(true);

  m_tablistAction = new QAction(tr("&List tabs"),this);
  m_clearHistoryAction = new QAction(tr("&Clear"),this);
  connect(m_clearHistoryAction,SIGNAL(triggered()),this,SLOT(onClearHistory()));

  m_showHistoryAction = new QAction(tr("&List"),this);
  connect(m_showHistoryAction,SIGNAL(triggered()),this,SLOT(onShowHistory()));

  m_testAction = new QAction(tr("Test"),this);
  connect(m_testAction,SIGNAL(triggered()),this,SLOT(onTest()));
  /// probably need icons

  m_historyAction = new QAction(tr("History"),this);

  m_navNextAction = new QAction(tr("&Next"),this);
  m_navPrevAction = new QAction(tr("&Back"),this);
  m_navFirstAction = new QAction(tr("&First"),this);
  m_navLastAction = new QAction(tr("&Last"),this);

  connect(m_navNextAction,SIGNAL(triggered()),this,SLOT(onNavNext()));
  connect(m_navPrevAction,SIGNAL(triggered()),this,SLOT(onNavPrev()));
  connect(m_navFirstAction,SIGNAL(triggered()),this,SLOT(onNavFirst()));
  connect(m_navLastAction,SIGNAL(triggered()),this,SLOT(onNavLast()));


  m_docAction = new QAction(tr("&Documentation"),this);
  m_aboutAction = new QAction(tr("&About"),this);
  m_bookmarkAction = new QAction(tr("Bookmarks"),this);

  m_searchAction = new QAction(tr("Search"),this);

  m_moveGroup = new QActionGroup(this);
  m_navModeRootAction = new QAction(tr("By &root"),this);

  m_navModeRootAction->setData(LanesLexicon::ByRoot);
  m_navModeRootAction->setCheckable(true);
  if (m_navMode == LanesLexicon::ByRoot) {
    m_navModeRootAction->setChecked(true);
  }
  m_navModePageAction = new QAction(tr("By &page"),this);
  m_navModePageAction->setData(LanesLexicon::ByPage);
  m_navModePageAction->setCheckable(true);
  if (m_navMode == LanesLexicon::ByPage) {
    m_navModePageAction->setChecked(true);
  }
  m_moveGroup->addAction(m_navModeRootAction);
  m_moveGroup->addAction(m_navModePageAction);

  connect(m_moveGroup,SIGNAL(triggered(QAction *)),this,SLOT(onNavigationMenuChanged(QAction *)));

  connect(m_docAction,SIGNAL(triggered()),this,SLOT(onDocs()));
  connect(m_aboutAction,SIGNAL(triggered()),this,SLOT(onAbout()));

  m_searchWordAction = new QAction(tr("Arabic &word"),this);
  connect(m_searchWordAction,SIGNAL(triggered()),this,SLOT(searchForWord()));
  m_searchPageAction = new QAction(tr("&Page"),this);
  connect(m_searchPageAction,SIGNAL(triggered()),this,SLOT(searchForPage()));
  m_searchRootAction = new QAction(tr("&Root"),this);
  connect(m_searchRootAction,SIGNAL(triggered()),this,SLOT(searchForRoot()));
  m_searchNodeAction = new QAction(tr("&Node"),this);
  connect(m_searchNodeAction,SIGNAL(triggered()),this,SLOT(searchForNode()));
  m_searchEntryAction = new QAction(tr("&Head word"),this);
  connect(m_searchEntryAction,SIGNAL(triggered()),this,SLOT(searchForEntry()));

  m_zoomInAction = new QAction(tr("Zoom &in"),this);
  m_zoomOutAction =  new QAction(tr("Zoom &out"),this);
  m_widenAction = new QAction(tr("&Widen text"),this);
  m_narrowAction = new QAction(tr("&Narrow text"),this);
  m_printAction = new QAction(tr("&Print"),this);
  m_localSearchAction = new QAction(tr("&Search page"),this);
  m_localSearchNextAction = new QAction(tr("Search ne&xt"),this);
  m_localSearchNextAction->setEnabled(false);
  m_clearAction = new QAction(tr("&Clear"),this);
  m_convertToEntryAction = new QAction(tr("Convert"),this);
  m_clearAction->setEnabled(false);

  m_keymapsAction = new QAction(tr("Keymaps"),this);


  m_selectThemeAction = new QAction(tr("&Switch"),this);
  m_editThemeAction = new QAction(tr("&Edit"),this);
  m_deleteThemeAction = new QAction(tr("&Delete"),this);
  m_createThemeAction = new QAction(tr("&New/Copy"),this);

  m_changeArabicFontAction = new QAction(tr("Change &font"),this);


  m_exportLinksAction = new QAction(tr("Ex&port links"),this);
  m_importLinksAction = new QAction(tr("&Import links"),this);

  m_importXmlAction = new QAction(tr("Import &Xml"),this);

  m_loadPageSetAction = new QAction(tr("&Load"),this);
  m_savePageSetAction = new QAction(tr("&Save"),this);
  m_editPageSetAction = new QAction(tr("&Edit"),this);

  m_showNoteBrowserAction = new QAction(tr("View &notes"),this);



  connect(m_tablistAction,SIGNAL(triggered()),this,SLOT(onTabList()));
  connect(m_changeArabicFontAction,SIGNAL(triggered()),this,SLOT(onChangeArabicFont()));
  connect(m_deleteThemeAction,SIGNAL(triggered()),this,SLOT(onDeleteTheme()));
  connect(m_createThemeAction,SIGNAL(triggered()),this,SLOT(onCreateTheme()));

  connect(m_syncFromEntryAction,SIGNAL(triggered()),this,SLOT(syncFromEntry()));
  connect(m_syncFromContentsAction,SIGNAL(triggered()),this,SLOT(syncFromContents()));

  connect(m_zoomInAction,SIGNAL(triggered()),this,SLOT(pageZoomIn()));
  connect(m_zoomOutAction,SIGNAL(triggered()),this,SLOT(pageZoomOut()));
  connect(m_widenAction,SIGNAL(triggered()),this,SLOT(pageWiden()));
  connect(m_narrowAction,SIGNAL(triggered()),this,SLOT(pageNarrow()));
  connect(m_printAction,SIGNAL(triggered()),this,SLOT(pagePrint()));
  connect(m_localSearchAction,SIGNAL(triggered()),this,SLOT(pageSearch()));
  connect(m_localSearchNextAction,SIGNAL(triggered()),this,SLOT(localSearchNext()));
  connect(m_clearAction,SIGNAL(triggered()),this,SLOT(pageClear()));
  connect(m_convertToEntryAction,SIGNAL(triggered()),this,SLOT(convertToEntry()));
  connect(m_defaultScaleAction,SIGNAL(triggered()),this,SLOT(onDefaultScale()));
  connect(m_defaultWidthAction,SIGNAL(triggered()),this,SLOT(onDefaultWidth()));
  connect(m_selectThemeAction,SIGNAL(triggered()),this,SLOT(onSelectTheme()));
  connect(m_editThemeAction,SIGNAL(triggered()),this,SLOT(onEditTheme()));

  connect(m_showContentsAction,SIGNAL(triggered()),this,SLOT(onShowContents()));
  connect(m_showToolbarAction,SIGNAL(triggered()),this,SLOT(onShowToolbar()));
  connect(m_showMenuAction,SIGNAL(triggered()),this,SLOT(onShowMenubar()));
  connect(m_showStatusBarAction,SIGNAL(triggered()),this,SLOT(onShowStatusbar()));

  connect(m_importLinksAction,SIGNAL(triggered()),this,SLOT(onImportLinks()));
  connect(m_exportLinksAction,SIGNAL(triggered()),this,SLOT(onExportLinks()));

  connect(m_importXmlAction,SIGNAL(triggered()),this,SLOT(onImportXml()));
  connect(m_loadPageSetAction,SIGNAL(triggered()),this,SLOT(onLoadPageSet()));
  connect(m_savePageSetAction,SIGNAL(triggered()),this,SLOT(onSavePageSet()));
  connect(m_editPageSetAction,SIGNAL(triggered()),this,SLOT(onEditPageSet()));

  connect(m_showNoteBrowserAction,SIGNAL(triggered()),this,SLOT(showNoteBrowser()));
}
void LanesLexicon::createToolBar() {
  QMap<QString,QString> cmdOptions = getLexicon()->getOptions();
  m_mainbar = addToolBar("Main");
  m_mainbar->setObjectName("maintoolbar");
  m_mainbar->setIconSize(m_toolbarIconSize);
  m_exitButton = new QToolButton(m_mainbar);
  m_exitButton->setText(tr("Exit"));
  m_exitButton->setDefaultAction(m_exitAction);
  m_exitButton->setFocusPolicy(Qt::StrongFocus);
  m_mainbar->addWidget(m_exitButton);

  if (cmdOptions.contains("enabletest")) {
    m_mainbar->addAction(m_testAction);
  }

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
  //  addToolBarBreak();



  m_navigation = addToolBar(tr("Navigation"));
  m_navigation->setIconSize(m_toolbarIconSize);
  m_navigation->setObjectName("navigationtoolbar");

  m_navigation->setFloatable(true);
  if (m_allowNavMode) {
    m_navigationModeMenu = new QMenu(m_navigation);
    m_navigationModeMenu->addAction(m_navModeRootAction);
    m_navigationModeMenu->addAction(m_navModePageAction);
    m_navigationButton = new QToolButton(m_navigation);
    m_navigationButton->setObjectName("navigationby");
    m_navigationButton->setMenu(m_navigationModeMenu);
    m_navigationButton->setText(tr("Move by"));
    m_navigationButton->setFocusPolicy(Qt::StrongFocus);

    m_navigationButton->setPopupMode(QToolButton::InstantPopup);
    m_navigation->addWidget(m_navigationButton);

    m_navigationModeMenu->setVisible(false);
    m_navigationButton->setVisible(false);
  }
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


  m_entrybar = addToolBar(tr("Page"));
  m_entrybar->setIconSize(m_toolbarIconSize);

  m_entrybar->setObjectName("pagetoolbar");

  m_zoomInButton = new QToolButton(m_entrybar);
  m_zoomInButton->setDefaultAction(m_zoomInAction);
  m_zoomInButton->setText(tr("Zoom in"));
  m_zoomInButton->setFocusPolicy(Qt::StrongFocus);
  m_entrybar->addWidget(m_zoomInButton);


  m_zoomOutButton = new QToolButton(m_entrybar);
  m_zoomOutButton->setDefaultAction(m_zoomOutAction);
  m_zoomOutButton->setText(tr("Zoom out"));
  m_zoomOutButton->setFocusPolicy(Qt::StrongFocus);
  m_entrybar->addWidget(m_zoomOutButton);

  m_widenButton = new QToolButton(m_entrybar);
  m_widenButton->setDefaultAction(m_widenAction);
  m_widenButton->setText(tr("Widen"));
  m_widenButton->setFocusPolicy(Qt::StrongFocus);
  m_entrybar->addWidget(m_widenButton);

  m_narrowButton = new QToolButton(m_entrybar);
  m_narrowButton->setDefaultAction(m_narrowAction);
  m_narrowButton->setText(tr("Narrow"));
  m_narrowButton->setFocusPolicy(Qt::StrongFocus);
  m_entrybar->addWidget(m_narrowButton);

  m_printButton = new QToolButton(m_entrybar);
  m_printButton->setDefaultAction(m_printAction);
  m_printButton->setText(tr("Print"));
  m_printButton->setFocusPolicy(Qt::StrongFocus);
  m_entrybar->addWidget(m_printButton);

  m_localSearchButton = new QToolButton(m_entrybar);
  m_localSearchButton->setDefaultAction(m_localSearchAction);
  m_localSearchButton->setText(tr("Search page"));
  m_localSearchButton->setFocusPolicy(Qt::StrongFocus);
  m_entrybar->addWidget(m_localSearchButton);

  m_localSearchNextButton = new QToolButton(m_entrybar);
  m_localSearchNextButton->setDefaultAction(m_localSearchNextAction);
  m_localSearchNextButton->setText(tr("Search next page"));
  m_localSearchNextButton->setFocusPolicy(Qt::StrongFocus);
  m_entrybar->addWidget(m_localSearchNextButton);

  m_clearButton = new QToolButton(m_entrybar);
  m_clearButton->setDefaultAction(m_clearAction);
  m_clearButton->setText(tr("Clear search"));
  m_clearButton->setFocusPolicy(Qt::StrongFocus);
  m_entrybar->addWidget(m_clearButton);

  m_syncLeftButton = new QToolButton(m_entrybar);
  m_syncLeftButton->setDefaultAction(m_syncFromEntryAction);
  m_syncLeftButton->setText(tr("Align contents with page"));
  m_syncLeftButton->setFocusPolicy(Qt::StrongFocus);
  m_entrybar->addWidget(m_syncLeftButton);

  m_syncRightButton = new QToolButton(m_entrybar);
  m_syncRightButton->setDefaultAction(m_syncFromContentsAction);
  m_syncRightButton->setText(tr("Align contents with page"));
  m_syncRightButton->setFocusPolicy(Qt::StrongFocus);
  m_entrybar->addWidget(m_syncRightButton);

  m_entrybar->setFloatable(true);

  setTabOrder(m_exitButton,m_historyButton);
  setTabOrder(m_historyButton,m_bookmarkButton);
  setTabOrder(m_bookmarkButton,m_searchButton);
  setTabOrder(m_searchButton,m_docButton);
  setTabOrder(m_docButton,m_aboutButton);
  setTabOrder(m_aboutButton,m_optionsButton);
  setTabOrder(m_optionsButton,m_logButton);
  if (m_allowNavMode) {
    setTabOrder(m_logButton,m_navigationButton);
    setTabOrder(m_navigationButton,m_navFirstButton);
  }
  else {
    setTabOrder(m_logButton,m_navFirstButton);
  }
  setTabOrder(m_navFirstButton,m_navNextButton);
  setTabOrder(m_navNextButton,m_navPrevButton);
  setTabOrder(m_navPrevButton,m_navLastButton);
  setTabOrder(m_navLastButton,m_zoomInButton);
  setTabOrder(m_zoomInButton,m_zoomOutButton);
  setTabOrder(m_zoomOutButton,m_widenButton);
  setTabOrder(m_widenButton,m_narrowButton);
  setTabOrder(m_narrowButton,m_printButton);
  setTabOrder(m_printButton,m_localSearchButton);
  setTabOrder(m_localSearchButton,m_localSearchNextButton);
  setTabOrder(m_localSearchNextButton,m_clearButton);
  setTabOrder(m_clearButton,m_syncLeftButton);
  setTabOrder(m_syncLeftButton,m_syncRightButton);

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
    m_syncRightButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_syncLeftButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_aboutButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_localSearchNextButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  }

}
/**
 * when user has done something that adds to history
 * rebuild the dropdown, unchecking everything
 *
 */
void LanesLexicon::historyAddition(const Place & p) {
  QString t = p.getShortText();
  if (! t.isEmpty()) {
    statusBar()->showMessage(tr("History added:") + p.getShortText());
    setupHistory(-1);
  }
  else {
    QLOG_DEBUG() << Q_FUNC_INFO << "empty text" << p.toString();
  }
}
void LanesLexicon::historyPositionChanged(int pos) {
  setupHistory(pos);
}
/**
 * constructs the history menu list
 *
 * @param currPos  our current position in the history, -1 = at the end (default)
 */
Place LanesLexicon::setupHistory(int currPos) {
  Place currentPlace;
  currentPlace.setAction(Place::History);

  int lastId = m_history->getNewestId();
  int firstId = m_history->getOldestId();
  QLOG_DEBUG() << Q_FUNC_INFO << QString("First %1/Last %2/Current %3").arg(firstId).arg(lastId).arg(currPos);
  /// if we are out of range, set the history to the most recent entry
  //

  if (currPos == -1) {
    currPos = m_historyPos;
  }
  /*
  if (currPos > lastId) {
    currPos = lastId;
  }
  if (currPos < firstId) {
    currPos = firstId;
  }
  */
  m_historyPos = currPos;
  QList<HistoryEvent *> events = m_history->getHistory();//10,0,currPos);
  QLOG_DEBUG() << Q_FUNC_INFO << "event size" << events.size();
  if (events.size() == 0) {
    m_historyButton->setEnabled(false);
    m_clearHistoryAction->setEnabled(false);
    m_showHistoryAction->setEnabled(false);
  }
  else {
    // m_historyMenu->clear();
    m_historyButton->setEnabled(true);
    m_clearHistoryAction->setEnabled(true);
    m_showHistoryAction->setEnabled(true);
  }
  QList<QAction *> actions = m_historyMenu->actions();
  actions.removeOne(m_clearHistoryAction);
  actions.removeOne(m_showHistoryAction);
  for(int i=0;i < actions.size();i++) {
    m_historyMenu->removeAction(actions[i]);
    delete actions[i];
  }
  if (events.size() == 0) {
    return currentPlace;
  }

  //  QString fmt("%R ?H Head %H?H  ?N(Node:%N)?N");
  QActionGroup * group = new QActionGroup(this);
  int pos = 1;
  while(events.size() > 0) {
    HistoryEvent * event = events.takeFirst();
    Place p = event->getPlace();
    QString root = p.getRoot();
    QString word = p.getWord();
    int id = event->getId();

    QString txt = p.formatc(m_historyMenuFormat);
    //    txt = QString("%1 [id=%2] %3").arg(pos).arg(id).arg(txt);
    pos++;
    QAction * action = group->addAction(txt);
    action->setFont(m_historyMenuFont);
    action->setCheckable(true);
    if (id == currPos) {
      action->setChecked(true);
      QLOG_DEBUG() << "history set to" << p.toString();
      currentPlace = p;
    }
    else {
      action->setChecked(false);
    }
    //      m_historyPos = p.getId();
    action->setData(QVariant(p));//event->getId());
    action->setProperty("HISTORY_ID",event->getId());
    connect(action,SIGNAL(triggered()),this,SLOT(onHistorySelection()));
    delete event;
  }
  m_historyMenu->addActions(group->actions());
    //    m->addActions(group);
  m_historyButton->setEnabled(m_historyEnabled);
    //    m_historyButton->addActions(group->actions());//setMenu(m);
    //    m_historyButton->setMenu(m);
  m_history->setEnabled(m_historyEnabled);
  currentPlace.setAction(Place::History);
  return currentPlace;
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
  m_viewMenu->addAction(m_syncFromContentsAction);
  m_viewMenu->addAction(m_syncFromEntryAction);
  m_viewMenu->addAction(m_linkAction);
  m_viewMenu->addAction(m_defaultScaleAction);
  m_viewMenu->addAction(m_defaultWidthAction);
  m_viewMenu->addAction(m_showContentsAction);
  m_viewMenu->addAction(m_showToolbarAction);
  m_viewMenu->addAction(m_showStatusBarAction);
  m_viewMenu->addAction(m_showMenuAction);
  m_viewMenu->addAction(m_tablistAction);

  m_bookmarkMenu = m_mainmenu->addMenu(tr("&Bookmarks"));
  m_bookmarkMenu->setObjectName("bookmarkmenu");
  m_bookmarkMenu->setFocusPolicy(Qt::StrongFocus);

  m_bookmarkMenu->addAction(m_bookmarkListAction);
  m_bookmarkMenu->addAction(m_bookmarkClearAction);
  m_bookmarkMenu->addAction(m_bookmarkRevertAction);
  m_bookmarkMenu->addAction(m_bookmarkAddAction);

  connect(m_mainmenu,SIGNAL(rebuildBookmarks()),this,SLOT(bookmarkRebuildMenu()));

  m_pageSetMenu = m_mainmenu->addMenu(tr("T&ab set"));
  m_pageSetMenu->setObjectName("tabmenu");
  m_pageSetMenu->setFocusPolicy(Qt::StrongFocus);
  m_pageSetMenu->addAction(m_loadPageSetAction);
  m_pageSetMenu->addAction(m_savePageSetAction);
  m_pageSetMenu->addAction(m_editPageSetAction);

  m_historyMenu = m_mainmenu->addMenu(tr("&History"));
  m_historyMenu->setObjectName("historymenu");
  m_historyMenu->setFocusPolicy(Qt::StrongFocus);
  m_historyMenu->addAction(m_clearHistoryAction);
  m_historyMenu->addAction(m_showHistoryAction);

  m_moveMenu = m_mainmenu->addMenu(tr("&Roots"));
  m_moveMenu->setObjectName("navigationmenu");
  m_moveMenu->setFocusPolicy(Qt::StrongFocus);
  m_moveMenu->addAction(m_navFirstAction);
  m_moveMenu->addAction(m_navNextAction);
  m_moveMenu->addAction(m_navPrevAction);
  m_moveMenu->addAction(m_navLastAction);
  if (m_allowNavMode) {
    m_moveMenu->addAction(m_navModeRootAction);
    m_moveMenu->addAction(m_navModePageAction);
  }

  m_searchMenu = m_mainmenu->addMenu(tr("&Search"));
  m_searchMenu->setFocusPolicy(Qt::StrongFocus);
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
  m_pageMenu->addAction(m_localSearchNextAction);
  m_pageMenu->addAction(m_clearAction);

  m_toolMenu = m_mainmenu->addMenu(tr("&Tools"));
  m_toolMenu->setFocusPolicy(Qt::StrongFocus);
  m_toolMenu->setObjectName("toolmenu");
  m_toolMenu->addAction(m_changeArabicFontAction);
  m_toolMenu->addAction(m_logViewerAction);
  m_toolMenu->addAction(m_editViewAction);
  m_toolMenu->addAction(m_importLinksAction);
  m_toolMenu->addAction(m_exportLinksAction);
  m_toolMenu->addAction(m_importXmlAction);
  m_toolMenu->addAction(m_showNoteBrowserAction);

  m_themeMenu = m_mainmenu->addMenu(tr("The&mes"));
  m_themeMenu->setFocusPolicy(Qt::StrongFocus);
  m_themeMenu->setObjectName("thememenu");
  m_themeMenu->addAction(m_selectThemeAction);
  m_themeMenu->addAction(m_editThemeAction);
  m_themeMenu->addAction(m_deleteThemeAction);
  m_themeMenu->addAction(m_createThemeAction);

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
  QStringList maps =  m_maps.keys();
  QMenu * menu = new QMenu(this);
  for(int i=0;i < maps.size();i++) {
    QAction * action  = menu->addAction(maps[i]);
    action->setCheckable(true);
    action->setData(maps[i]);
    action->setEnabled(m_keymapsEnabled);
    if (m_currentMap == maps[i]) {
      action->setChecked(true);
    }
    connect(action,SIGNAL(triggered()),this,SLOT(onKeymapChanged()));

  }
  if (maps.size() > 0) {
    //    m_keymapsButton->setEnabled(false);
    QAction * action = menu->addAction(tr("Enabled"));
    action->setCheckable(true);
    action->setData("_ENABLE_");
    action->setChecked(m_keymapsEnabled);
    connect(action,SIGNAL(triggered()),this,SLOT(onKeymapChanged()));

  }
  m_keymapsButton->setDefaultAction(m_keymapsAction);
  m_keymapsButton->setMenu(menu);
  m_keymapsButton->setPopupMode(QToolButton::InstantPopup);


  m_linkButton = new QToolButton(this);
  m_linkButton->setDefaultAction(m_linkAction);
  m_linkButton->setToolTip(tr("Click to link/unlink contents with page"));
  //  m_linkButton->setEnabled(m_linkContents);
  if (m_linkContents) {
    m_linkButton->setToolTip(tr("Contents are linked. Click to unlink"));
    m_linkButton->setText(tr("Contents linked"));
  }
  else {
    m_linkButton->setToolTip(tr("Contents are not linked. Click to link"));
    m_linkButton->setText(tr("Contents not linked"));
  }


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


  if (action->data().toString() == "_ENABLE_") {
    action->blockSignals(true);
    this->enableKeymaps(action->isChecked());
    QList<QAction *> actions = m_keymapsButton->menu()->actions();
    for(int i=0;i < actions.size();i++) {
      if (actions[i] != action) {
        actions[i]->blockSignals(true);
        actions[i]->setEnabled(action->isChecked());
        actions[i]->blockSignals(false);
      }
    }
    action->blockSignals(false);
    return;
  }
  m_currentMap.clear();// = action->data().toString();

  bool enabled = false;
  QList<QAction *> actions = m_keymapsButton->menu()->actions();
  for(int i=0;i < actions.size();i++) {
    if (actions[i]->data().toString() != "_ENABLE_") {
      actions[i]->blockSignals(true);
      QLOG_DEBUG() << Q_FUNC_INFO << i << actions[i]->data().toString() << actions[i]->isChecked();
      if (action == actions[i]) {
        if (actions[i]->isChecked()) {
          enabled = true;
          m_currentMap = actions[i]->data().toString();
        }
      }
      else {
        actions[i]->setChecked(false);
      }
      actions[i]->blockSignals(false);
    }
  }
  this->enableKeymaps(enabled);
}
void LanesLexicon::onLinkChanged() {
  QString str;
  m_linkContents = ! m_linkContents;
  m_linkAction->setChecked(m_linkContents);
  if (m_linkContents) {
    m_linkButton->setToolTip(tr("Contents are linked. Click to unlink"));
    str = tr("Contents linked");
    m_linkButton->setText(str);
  }
  else {
    m_linkButton->setToolTip(tr("Contents are not linked. Click to link"));
    str = tr("Contents not linked");
    m_linkButton->setText(str);
  }
  setStatus(str);
}
QSize LanesLexicon::sizeHint() const {
  return QSize(800,950);
}
void LanesLexicon::showHistory(const Place & p) {
  QLOG_DEBUG() << Q_FUNC_INFO << p.toString();
  /*
  QList<QAction *> actions = m_historyMenu->actions();
  for(int i=0;i < actions.size();i++) {
      QVariant v = actions[i]->data();
      if (! v.isNull()) {
        Place hp = v.value<Place>();
        if (hp.isValid()) {
          if (p.isSamePlace(hp)) {
            actions[i]->setChecked(true);
          }
        }
      }
  }
  */
  showPlace(p,m_historyNewTab,m_historyGoTab);
}
void LanesLexicon::onHistorySelection() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  QAction * action = static_cast<QAction *>(QObject::sender());
  if (!action) {
    QLOG_DEBUG() << Q_FUNC_INFO << "No action" ;
    return;
  }

  m_historyPos = action->property("HISTORY_ID").toInt();
  QVariant v = action->data();

  Place p = v.value<Place>();
  p.setAction(Place::History);
  this->showHistory(p);
}
void LanesLexicon::onExit()
{
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
/**
 * double click on a tree item.
 * For root, add child entries if not already presetn
 * For headword, activate the item by giving it focus
 *
 *
 *
 * Currently if the tree keeps focus, the entry is made visible but
 * it is not obvious which one it is.
 *
 * @param item
 */
void LanesLexicon::treeItemDoubleClicked(QTreeWidgetItem * item,int /* column */) {
  QString root = item->text(0);
  QString supp = item->text(1);
  int p = 0;
  QLOG_DEBUG() << Q_FUNC_INFO << root << supp;
  /// check that the user has not clicked on a letter
  if (item->parent() == 0) {
    Place m = Place::fromRoot(item->text(0));
    showPlace(m,false,true);
    return;
  }
  /// TODO check this
  bool newTab = false;
  if (QApplication::keyboardModifiers() & Qt::ShiftModifier) {
    newTab = true;
  }
  /// check whether root or head word
  /// if root, add the child entries
  if  (item->parent()->parent() == 0) {
    m_tree->addEntries(root,item);
    if (supp == "*") {
      p = 1;
    }
    Place m(root,p);
    m.setAction(Place::User);
    showPlace(m,newTab,true);
  }
  else {
    /// double clicked on head word, highlight it
    entryActivated(item,0);
  }

  if ( ! item->isExpanded() ) {
    item->setExpanded(true);
  }
}
/**
 * the user has selected something in roots tree
 *
 * if the user clicks or hits space an entry (i.e. below a root) make
 * sure it is visible
 *
 * @param item
 * @param col
 */
void LanesLexicon::entryActivated(QTreeWidgetItem * item, int /* not used */) {
  /// ignore clicks on the root or the letter
  QLOG_DEBUG() << Q_FUNC_INFO;
  if ((item->parent() == 0) || (item->parent()->parent() == 0)) {
    return;
  }
  QString node = item->data(0,Qt::UserRole).toString();
  if ( node.isEmpty() ) {
    QLOG_DEBUG() << Q_FUNC_INFO << "no node";
    return;
  }
  ///  Tabs for the node
  int ix = this->searchTabs(node);
  if (ix != -1) {
      /// we found it in another tab, so switch to that one
      m_tabs->setCurrentIndex(ix);
      GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
      if (entry) {
        if (entry->hasNode(node)) {
          entry->focusNode(node);
          return;
        }
      }
  }
  Place p = Place::fromNode(node);
  bool newTab = false;
  if (QApplication::keyboardModifiers() & (Qt::ShiftModifier | Qt::ControlModifier)) {
    newTab = true;
  }
  GraphicsEntry * entry = showPlace(p,newTab,true);
  m_place = entry->getPlace();
  return;
}
GraphicsEntry * LanesLexicon::showPlace(const Place & p,bool createTab,bool activateTab) {
  Place np;

  QLOG_DEBUG() << Q_FUNC_INFO << p << createTab << activateTab;
  GraphicsEntry * entry = NULL;
  if (! p.isValid()) {
    QLOG_DEBUG() << Q_FUNC_INFO << "Invalid place";
    return NULL;
  }
  int ix = this->searchTabs(p);//.node());

  if ((ix != -1) && activateTab) {
    m_tabs->setCurrentIndex(ix);
    entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(ix));
    return entry;
  }
  /// if we don't have a tab or the current tab is not a graphicsentry
  /// force new tab creation
  int currentTab = m_tabs->currentIndex();
  if (currentTab == -1) {
    createTab = true;
  }
  if (!createTab)  {
    /// if current widget not graphicsentry, set createtab
    entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(currentTab));
    if (! entry ) {
      QWidget * w = m_tabs->currentWidget();
      m_tabs->removeTab(currentTab);
      delete w;
      createTab = true;
    }
  }
  if (createTab) {
    entry = new GraphicsEntry(this);
    setSignals(entry);
    entry->installEventFilter(this);
    entry->getView()->installEventFilter(this);
    np = entry->getXmlForRoot(p);
    if (np.isValid()) {
      int ix = this->addTab(true,entry,np.getShortText());
      if (activateTab) {
        m_tabs->setCurrentIndex(ix);
      }
      return entry;
    }
    else {
      QLOG_DEBUG() << Q_FUNC_INFO << "Entry get root returned invalid place";
      delete entry;
      return NULL;
    }
  }
  if (entry->hasPlace(p,GraphicsEntry::RootSearch,true) == -1) {
    /// TODO check this and onTabsChanged
    /// this sets the tab text to the headword
    /// tabs->tabContentsChanged() emits signa tabsChanged()
    /// which ends calling this->onTabsChanged()
    // and that inserts a number in tab title
    QLOG_DEBUG() << Q_FUNC_INFO << __LINE__;
    np = entry->getXmlForRoot(p);
    if (np.isValid()) {
      m_tabs->setTabText(currentTab,np.getShortText());
      entry->setFocus();
      m_tabs->tabContentsChanged();
    }
    else {
      return NULL;
    }
  }
 return entry;
}

/**
 * when user clicks on item reason Qt::MouseFocusReason
 * when setFocus called it is Qt::OtherFocusReason
 *
 * @param newFocus
 * @param oldFocus
 * @param reason
 **/
void LanesLexicon::focusItemChanged(QGraphicsItem * newFocus, QGraphicsItem * /* oldFocus */, Qt::FocusReason  /* reason */) {
  EntryItem * item1 = dynamic_cast<EntryItem *>(newFocus);
  //  EntryItem * item2 = dynamic_cast<EntryItem *>(oldFocus);

  if (item1) {
    updateStatusBar(item1->getPlace());
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
  if ((event->type() == QEvent::Close) && ((target == m_treeDock) || (target == m_tree))) {
    m_showContentsAction->setChecked(false);
  }
  /*
  if (target == m_tabs) {
    qDebug() << "tab has focus";
  }
  if (target == m_tabs->tabBar()) {
    qDebug() << "tab bar has focus";
  }
  */
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
    QLOG_DEBUG() << Q_FUNC_INFO << keyEvent->modifiers() << keyEvent->key() << keyEvent->text();
    switch(keyEvent->key()) {
    case Qt::Key_Tab: {
        if (target == m_tabs) {

        }
        else if (target == m_tabs->tabBar()) {
          m_exitButton->setFocus();
          return true;
        }
        else if (target == m_tree) {
        }
        else {
        }
        break;
    }
    case Qt::Key_Left: {
      //      qDebug() << "Left";
        break;
    }
    case Qt::Key_Right: {
      //      qDebug() << "Right";
        break;
    }
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
  if (m_editView == 0) {
    m_editView = new EditView;
    connect(m_editView,SIGNAL(reload(const QString &,const QString &)),this,SLOT(reloadEntry(const QString &,const QString &)));
    connect(m_editView,SIGNAL(showHelp(const QString &)),this,SLOT(showHelp(const QString &)));
  }
  m_editView->show();
}
void LanesLexicon::onLogViewer() {
  if (m_logview == NULL) {
    m_logview = new LogViewer();
  }
  m_logview->show();
}
void LanesLexicon::onTest() {
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
    connect(d,SIGNAL(showHelp(const QString &)),this,SLOT(showHelp(const QString &)));
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
    Place p = m_tree->getCurrentPlace();
    QLOG_DEBUG() << QString("Root %1, Word %2 , Node %3, Supplement %4")
      .arg(p.getRoot())
      .arg(p.getWord())
      .arg(p.getNode())
      .arg(p.getSupplement());
  }
  if (0) {
    FontChangeDialog * d = new FontChangeDialog(this);
    d->exec();
    delete d;
  }
  if (0) {
  if (m_editView == 0) {
    m_editView = new EditView;
    connect(m_editView,SIGNAL(reload(const QString &,const QString &)),this,SLOT(reloadEntry(const QString &,const QString &)));
  }
  if (0) {
    m_editView->show();
  }
  addShortcut("Search node","Ctrl+S,X",true);
  }
  if (0) {
    ExportSearchDialog * d = new ExportSearchDialog(QStringList() << "One" << "Two" << "Three" << "Four" << "Five" << "Six");
    if (d->exec() == QDialog::Accepted) {
      if (d->saveSettings()) {
        d->writeSettings();
      }
    }
    delete d;
  }
  qDebug() << ">>>>>" << getLexicon()->getSelectorCss("ImLineEdit");
}
/**
 * Read settings from INIFILE (by default : "default.ini");
 * Options can also come from the command line and they override settings
 * in the ini file
 *
 * All QActions have been created when we get here
 */
void LanesLexicon::readSettings() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  QString v;

  m_currentTheme = getLexicon()->currentTheme();
  Lexicon * app = qobject_cast<Lexicon *>(qApp);


  QMap<QString,QString> cmdOptions = app->getOptions();

  m_startupNode = cmdOptions.value("node");
  if (! m_startupNode.isEmpty() && ! m_startupNode.startsWith("n")) {
    m_startupNode = "n" + m_startupNode;
  }
  m_startupRoot = cmdOptions.value("root");
  SETTINGS
    //  QScopedPointer<QSettings> settings(app->getSettings());
  ///
  /// System group
  ///
  settings.beginGroup("System");

  m_dbName = settings.value(SID_SYSTEM_DATABASE,"lexicon.sqlite").toString();
  if (m_dbName.isEmpty()) {
    m_dbName = "lexicon.sqlite";
  }
  if (cmdOptions.contains("db")) {
    m_dbName = cmdOptions.value("db");
  }
  QDir dd = QDir::current();
  QFileInfo fi(dd,m_dbName);

  m_applicationCssFile = settings.value(SID_SYSTEM_STYLESHEET,"app.css").toString();
  if (m_applicationCssFile.isEmpty()) {
    m_applicationCssFile = "app.css";
  }

  m_toolbarText = settings.value(SID_SYSTEM_TOOLBAR_TEXT,false).toBool();

  m_interfaceWarning = settings.value(SID_SYSTEM_INTERFACE_WARNING,true).toBool();
  m_menuWarning = settings.value(SID_SYSTEM_MENU_WARNING,true).toBool();
  m_saveSettings = settings.value(SID_SYSTEM_SAVE_SETTINGS,true).toBool();
  if (cmdOptions.contains("nosave")) {
    m_saveSettings = false;
  }
  m_saveTabs = settings.value(SID_SYSTEM_SAVE_TABS,true).toBool();

  m_restoreTabs = settings.value(SID_SYSTEM_RESTORE_TABS,true).toBool();
  if (cmdOptions.contains("notabs")) {
    m_restoreTabs = false;
  }
  /// we let them toggle this but need to keep the original
  m_allowDuplicates = settings.value(SID_SYSTEM_ALLOW_DUPLICATES,false).toBool();
  m_allowDuplicatesPermanent = m_allowDuplicates;
  m_saveBookmarks = settings.value(SID_SYSTEM_SAVE_BOOKMARKS,true).toBool();
  m_restoreBookmarks = settings.value(SID_SYSTEM_RESTORE_BOOKMARKS,true).toBool();

  m_docked = settings.value(SID_SYSTEM_DOCKED,false).toBool();

  m_minimalAction->setChecked(settings.value(SID_SYSTEM_MINIMAL,false).toBool());

  if ( settings.value( SID_SYSTEM_BY_ROOT,true).toBool()) {
    m_navMode = LanesLexicon::ByRoot;
  }
  else {
    m_navMode = LanesLexicon::ByPage;
  }
  /// force by root
  m_navMode = LanesLexicon::ByRoot;

  v = settings.value(SID_SYSTEM_TITLE,tr("Lane's Arabic-English Lexicon")).toString();
  this->setWindowTitle(v);

  m_toolbarIconSize = settings.value(SID_SYSTEM_ICON_SIZE,QSize(16,16)).toSize();



  m_linkContents = settings.value(SID_SYSTEM_CONTENTS_LINKED,false).toBool();
  m_linkAction->setChecked(m_linkContents);

  m_messageInterval = settings.value(SID_SYSTEM_MESSAGE_TIMEOUT,5000).toInt();

  v = settings.value(SID_SYSTEM_HELP,"F1").toString();
  m_helpRequested = new QShortcut(QKeySequence(v),this);
  m_helpRequested->setContext(Qt::ApplicationShortcut);

  connect(m_helpRequested,SIGNAL(activated()),this,SLOT(onDocs()));

  if (settings.value(SID_SYSTEM_APPEND_NEW_TABS,true).toBool()) {
    m_tabStyle = LanesLexicon::AppendTab;
  }
  else {
    m_tabStyle = LanesLexicon::InsertTab;
  }

  settings.endGroup();

  settings.beginGroup("Maps");

  m_keymapsEnabled = settings.value(SID_MAPS_ENABLED,false).toBool();

  m_currentMap = settings.value(SID_MAPS_CURRENT_MAP,"Native").toString();

  settings.endGroup();

  ///
  /// Debug
  ///
  settings.beginGroup("Debug");
  m_valgrind = settings.value("Valgrind",false).toBool();
  settings.endGroup();
  ///
  /// Notes
  ///
  settings.beginGroup("Notes");
  m_notesDbName = settings.value(SID_NOTES_DATABASE,"notes.sqlite").toString();
  m_useNotes = settings.value(SID_NOTES_ENABLED,true).toBool();
  settings.endGroup();
  ///
  /// History
  ///
  settings.beginGroup("History");
  m_historyEnabled = settings.value(SID_HISTORY_ENABLED,true).toBool();
  m_historyDbName = settings.value(SID_HISTORY_DATABASE,"history.sqlite").toString();
  m_historyNewTab = settings.value(SID_HISTORY_NEW_TAB,false).toBool();
  m_historyGoTab = settings.value(SID_HISTORY_GO_TAB,false).toBool();
  m_historyMenuFormat = settings.value(SID_HISTORY_MENU_FORMAT,
                                       QString("%R ?H Head %H?H  ?N(Node:%N)?N")).toString();
  v = settings.value(SID_HISTORY_MENU_ARABIC_FONT,QString()).toString();
  if (! v.isEmpty()) {
    m_historyMenuFont.fromString(v);
  }
  settings.endGroup();
  /**
   * we are have a default map set that is used to convert input to Arabic
   *
   */
  ///
  /// Maps
  ///
  settings.beginGroup("Maps");
  QStringList groups = settings.childGroups();
  for(int i=0; i < groups.size();i++) {
    settings.beginGroup(groups[i]);
    v = settings.value("file",QString()).toString();
    if (!v.isEmpty()) {
      v = getLexicon()->getResourceFilePath(Lexicon::Map,v);
      QFile file(v);
      if ( file.exists() ) {
        if (im_load_map_from_json(m_mapper,v.toUtf8().constData(),groups[i].toUtf8().constData()) == -1) {
          QLOG_WARN() << QString(tr("Error loading map <%1> from file <%2>")).arg(groups[i]).arg(v);
        }
        else {
          m_maps.insert(groups[i],v);
        }
      }
      else {
        QLOG_WARN() << QString(tr("Could not load <%1> from file <%2> - file not found")).arg(groups[i]).arg(v);
      }
    }
    v = settings.value("help",QString()).toString();
    if (!v.isEmpty()) {
      v = getLexicon()->getResourceFilePath(Lexicon::Map,v);
      QFile file(v);
      if ( file.exists() ) {
           m_mapHelp.insert(groups[i],v);
      }
    }
    settings.endGroup();
  }
  settings.endGroup();
  if ((! m_currentMap.isEmpty()) && ! m_maps.contains(m_currentMap) ) {
    QLOG_WARN() << QString(tr("Default map <%1> not found in settings")).arg(m_currentMap);
  }

  settings.beginGroup("Bookmark");
  m_bookmarkMenuFormat = settings.value(SID_BOOKMARK_MENU_FORMAT,"Root:%R,Entry:%H, Vol %V/%P (%N)").toString();
  settings.endGroup();
}
void LanesLexicon::writeSettings() {

  QString v;
  QLOG_DEBUG() << Q_FUNC_INFO;
  if (! m_saveSettings )
    return;

  SETTINGS
  ///
  /// Tabs
  ///
  if (m_saveTabs) {
    settings.beginGroup("Tabs");
    settings.remove("");
    for(int i=0;i < m_tabs->count();i++) {
      GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(i));
      if (entry) {
        Place p = entry->getPlace();
        settings.beginGroup(QString("Tab-%1").arg(i));
        settings.setValue(SID_TABS_TYPE,"entry");
        settings.setValue(SID_TABS_PLACE,p.toString());
        settings.setValue(SID_TABS_ZOOM,entry->getScale());
        settings.setValue(SID_TABS_TEXTWIDTH,entry->getTextWidth());
        settings.setValue(SID_TABS_TITLE,entry->userTitle());
        v = entry->getHome();
        if (! v.isEmpty()) {
          settings.setValue(SID_TABS_HOME,v);
        }
        settings.endGroup();
      }
      else {
        NoteBrowser * browser = qobject_cast<NoteBrowser *>(m_tabs->widget(i));
        if (browser) {
          settings.beginGroup(QString("Tab-%1").arg(i));
          settings.setValue(SID_TABS_TYPE,"notes");
          settings.endGroup();
        }
        else {
          /// could be search results
        }
      }
    }
    settings.endGroup();
    ///
    ///  System
    ///
    settings.beginGroup("System");
    settings.setValue(SID_SYSTEM_MINIMAL,m_minimalAction->isChecked());
    settings.setValue(SID_SYSTEM_INTERFACE_WARNING,m_interfaceWarning);
    settings.setValue(SID_SYSTEM_MENU_WARNING,m_menuWarning);
    settings.setValue(SID_SYSTEM_CONTENTS_LINKED,m_linkContents);
    settings.setValue(SID_SYSTEM_STATE,this->saveState());
    settings.setValue(SID_SYSTEM_SIZE, size());
    settings.setValue(SID_SYSTEM_POS, pos());
    settings.setValue(SID_SYSTEM_CURRENT_TAB,m_tabs->currentIndex());
    settings.setValue(SID_SYSTEM_RUN_DATE,QDateTime::currentDateTime().toString(Qt::ISODate));
    settings.setValue(SID_SYSTEM_ALLOW_DUPLICATES,m_allowDuplicatesPermanent);
    if (m_navMode == LanesLexicon::ByRoot) {
      settings.setValue(SID_SYSTEM_BY_ROOT,true);
    }
    else {
      settings.setValue(SID_SYSTEM_BY_ROOT,false);
    }
    settings.endGroup();
    settings.beginGroup("Maps");
    settings.setValue(SID_MAPS_CURRENT_MAP,m_currentMap);
    settings.endGroup();
    }
  ///
  /// Bookmarks
  ///
  if (m_saveBookmarks) {
    settings.beginGroup("Bookmarks");
    settings.remove("");
    QStringList keys = m_bookmarks.keys();
    for(int i=0;i < keys.size();i++) {
      Place p = m_bookmarks.value(keys[i]);
      settings.setValue(keys[i],p.toString());
    }
    settings.endGroup();
  }
  ///
  /// HistorySID_HISTORY_MENU_FORMAT
  ///
  settings.beginGroup("History");
  settings.setValue("Enabled",m_historyEnabled);
  settings.endGroup();

}
void LanesLexicon::restoreTabs() {
  bool ok;
  QString v;
  QString home;
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QMap<QString,QString> cmdOptions = app->getOptions();

  SETTINGS
  settings.beginGroup("System");
  int focusTab =  settings.value(SID_SYSTEM_CURRENT_TAB,0).toInt();
  settings.endGroup();
  int textWidth;
  settings.beginGroup("Entry");
  textWidth = settings.value(SID_ENTRY_TEXT_WIDTH,400).toInt(&ok);
  if (!ok) {
    textWidth = 400;
  }
  if (cmdOptions.contains("textwidth")) {
    int w = cmdOptions.value("textwidth").toInt(&ok);
    if (ok) {
      textWidth = w;
    }
  }
  settings.endGroup();
  Place wp;
  settings.beginGroup("Tabs");
  QStringList tabs = settings.childGroups();
  // tab keys are in format 'Tab-nn', so they need to be sorted
  // to restore them in the correct sequence
  QMap<int,QString> tabmap;
  QRegularExpression rx("Tab-(\\d+)");
  for(int i=0;i < tabs.size();i++) {
    QRegularExpressionMatch m = rx.match(tabs[i]);
    if (m.hasMatch()) {
      bool ok;
      QString v = m.captured(1);
      tabmap.insert(v.toInt(&ok),tabs[i]);
    }
  }
  QList<int> tabkeys = tabmap.keys();
  qSort(tabkeys.begin(),tabkeys.end());
  int j = 0;
  for(int i=0;i < tabkeys.size();i++) {
    settings.beginGroup(tabmap.value(tabkeys[i]));
    if (settings.value(SID_TABS_TYPE,"entry").toString() == "notes") {
      NoteBrowser * notes = new NoteBrowser(this);
      connect(notes,SIGNAL(noteDeleted(const QStringList &)),this,SLOT(onNotesDeleted(const QStringList &)));
      m_tabs->addTab(notes,tr("Notes"));
      j++;
    }
    else {
      v = settings.value(SID_TABS_PLACE,QString()).toString();
      qreal scale = settings.value(SID_TABS_ZOOM,1.0).toReal(&ok);
      if ( !ok ) {
        scale = 1.0;
      }
      if (v.size() > 100) {
        QLOG_WARN() << QString(tr("Tab %1 is invalid, restore skipped [Size=%2]")).arg(i).arg(v.size());
        v = "";
      }
      Place p = Place::fromString(v);
      int tw;
      if (! cmdOptions.contains("textwidth")) {
        tw = settings.value(SID_TABS_TEXTWIDTH,textWidth).toInt(&ok);
        if (!ok) {
          tw = textWidth;
        }
      }
      else {
        tw = textWidth;
      }
      home = settings.value(SID_TABS_HOME,QString()).toString();
      if (p.isValid()) {
        if (focusTab == i) {
          focusTab = j;
          wp = p;
        }
        GraphicsEntry * entry = new GraphicsEntry(this);
        entry->setTextWidth(tw);
        setSignals(entry);
        p.setAction(Place::RestoreTab);
        entry->getXmlForRoot(p);
        entry->setScale(scale);
        entry->setHome(home);
        QString title = settings.value(SID_TABS_TITLE,QString()).toString();
        if (! title.isEmpty()) {
          entry->setUserTitle(title);
        }
        else {
          title = p.getShortText();
        }
        //      QLOG_DEBUG() << Q_FUNC_INFO << "adding tab" << p.getShortText();
        m_tabs->addTab(entry,title);
        m_tree->ensurePlaceVisible(p,true);
        j++;
      }
      else {
        QLOG_DEBUG() << Q_FUNC_INFO << "invalid place" << p;
      }
    }
    settings.endGroup();
  }
  settings.endGroup();
  wp.setAction(Place::RestoreTab);
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
/******************************************************************************
 * find Next/Prev Root are invoked when the user navigates forward or backward
 * from a graphicsentry page
 * @param root
 ******************************************************************************/
void LanesLexicon::findNextRoot(const QString & root) {
  GraphicsEntry * entry = dynamic_cast<GraphicsEntry *>(QObject::sender());
  QString nroot = m_tree->findNextRoot(root);
  if (entry && ! nroot.isEmpty()) {
    entry->setPagingForward();
    Place p;
    p.setRoot(nroot);
    p = entry->getXmlForRoot(p);
    if (m_linkContents) {
      m_tree->ensurePlaceVisible(p,true);
    }
  }
}
void LanesLexicon::findPrevRoot(const QString & root) {
  GraphicsEntry * entry = dynamic_cast<GraphicsEntry *>(QObject::sender());
  QString nroot = m_tree->findPrevRoot(root);
  if (entry && ! nroot.isEmpty()) {
    Place p = Place::fromRoot(nroot);
    entry->setPagingBackward();
    p = entry->getXmlForRoot(p);
    if (m_linkContents) {
      m_tree->ensurePlaceVisible(p,true);
    }
  }
}
/**********************************************************************************
 * The next four functions are invoked by clicking on the toolbar/menu navigation
 * icons.
 * If the current page is not a graphicsentry nothing happens.
 *
 * user clicks on next root button on the toolbar/menu
 *  - fetch the current root from GraphicsEntry page
 *  - query the ContentsWidget for the next root
 *  - if found
 *    tell the GraphicsEntry page to either show the root or fetch it and show it
 *
 **********************************************************************************/
void LanesLexicon::onNextRoot() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    Place p  =  entry->getPlace();
    Place np = m_tree->findNextPlace(p);
    if (! np.isEmpty()) {
      /// if it is, it move focus to it (if true is 2nd param)
      int ix = this->hasPlace(np,GraphicsEntry::RootSearch,true);
      if ( ix == -1) {
        entry->setPagingForward();
        entry->getXmlForRoot(np);
      }
      else {
        np.setAction(Place::SwitchTab);
        m_tabs->setCurrentIndex(ix);
      }
      if (m_linkContents) {
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
      int ix = this->hasPlace(np,GraphicsEntry::RootSearch,true);
      if (ix == -1) {
        entry->setPagingBackward();
        entry->getXmlForRoot(np);
      }
      else {
        np.setAction(Place::SwitchTab);
        m_tabs->setCurrentIndex(ix);
      }
      if (m_linkContents) {
        m_tree->ensurePlaceVisible(np,true);
      }
    }
    else {
      QLOG_DEBUG() << "No prev root for " << p.getRoot();
    }
  }
}
void LanesLexicon::onFirstRoot() {
  Place p;
  p.setRoot(m_firstRoot);
  showPlace(p,false,true);
  if (m_linkContents) {
    m_tree->ensurePlaceVisible(p);
  }
}
void LanesLexicon::onLastRoot() {
  Place p;
  p.setRoot(m_lastRoot);
  showPlace(p,false,true);
  if (m_linkContents) {
    m_tree->ensurePlaceVisible(p);
  }
}
/********************************************************************************
 *
 *
 ********************************************************************************/
void LanesLexicon::rootChanged(const QString & root,const QString & /* node */) {
  Place p;
  p.setRoot(root);
  if (m_linkContents) {
  m_tree->ensurePlaceVisible(p,true);
  }
}
/**
 * called when user moves around the current page, ensures that the tab text
 * and the contents are synced
 * @param p
 */
void LanesLexicon::placeChanged(const Place & p) {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(QObject::sender());
  if (entry) {
    int ix = m_tabs->indexOf(entry);
    if (ix != -1) {
      QString title = entry->userTitle();
      if (title.isEmpty()) {
        title = p.getShortText();
      }
      if (m_tabs->numberTab()) {
          title = QString("%1 %2").arg(ix+1).arg(title);
      }
      m_tabs->setTabText(ix,title);
    }
  }
  if (m_linkContents) {
    m_tree->ensurePlaceVisible(p,true);
  }
}
/***************************************************************************************************
 *
 * Sets up the first and last root and apgevariables, m_firstRoot,m_firstPage etc
 *
 ****************************************************************************************************/
void LanesLexicon::getFirstAndLast() {
  QSqlQuery query;
  bool ok = query.prepare(SQL_FIRST_ROOT);
  if (! ok ) {
    QLOG_DEBUG() << QString(tr("First root SQL prepare failed: %1")).arg(query.lastError().text());
  }
  query.exec();
  if (query.first()) {
    m_firstRoot = query.value(0).toString();
    m_firstPage = query.value(1).toInt();
  }
  ok = query.prepare(SQL_LAST_ROOT);
  if (! ok ) {
    QLOG_DEBUG() << QString(tr("Last root SQL prepare failed: %1")).arg(query.lastError().text());
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
/**********************************************************************
 *
 * the next four functions are invoked by clicking on the toolbar/menu
 * navigation icons when in page mode
 *
 **********************************************************************/
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
 * Converts supplied Buckwalter string param to Arabic using the default map
 * unrecognised characters are left in place
 * @param s
 *
 * @return the equivalent arabic string
 */
QString LanesLexicon::convertString(const QString & s) const {
  QLOG_DEBUG() << Q_FUNC_INFO << s << m_currentMap;
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
/**
 * Get the Place of the current widget. Returns invalid place if current widget
 * is not an GraphicsEntry
 *
 * @return
 */
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
void LanesLexicon::bookmarkShortcut(const QString & k) {
  QLOG_DEBUG() << Q_FUNC_INFO << k;
  QString key = k.toLower();
  if (key == "revert") {
    if (m_revertEnabled) {
      if (! m_bookmarks.contains("-here-")) {
        return;
      }
      Place p = m_bookmarks.value("-here-");
      /// since we are reverting to a previous place it must exist
      /// in one of the tabs
      showPlace(p,false,true);
      m_revertEnabled = false;
      return;
    }
    else {
      return;
    }
  }
  if (key == "list") {
    BookmarkWidget * dlg = new BookmarkWidget(m_bookmarks,this);
    if (dlg->exec() == QDialog::Accepted) {
      QString m = dlg->getSelected();
      bool newTab = dlg->getNewTab();
      bool switchTab = dlg->getSwitchTab();

      if ( ! m.isEmpty()) {
        bookmarkJump(m,newTab,switchTab);
      }
    }
    delete dlg;
    return;
  }
  if (key == "clear") {
    /// TODO warning dialog ?
    bookmarkClear();
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
    setStatus(QString(tr("Unknown bookmark shortcut activated:%1")).arg(key));
    return;
  }
  if (v == "jump") {
    bookmarkJump(id,false,false);
    return;
  }
  /// add a bookmark
  Place p = this->getCurrentPlace();
  if (! p.isValid()) {
    QLOG_WARN() << QString(tr("No place to bookmark"));
    return;
  }
  p.setAction(Place::Bookmark);
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
    QAction * action = m_bookmarkMenu->addAction(QString("%1 - %2")
                                                 .arg(id)
                                                 .arg(p.formatc(m_bookmarkMenuFormat)));
    action->setShortcut(sc->key());
    action->setShortcutContext(Qt::WidgetShortcut);
    connect(action,SIGNAL(triggered()),sc,SIGNAL(activated()));
  }
  else {
    QLOG_DEBUG() << Q_FUNC_INFO << "fetch mapping failed " << m_bookmarkMap->mapping(id) << id;
  }
}
void LanesLexicon::bookmarkJump(const QString & id,bool newTab,bool switchTab) {
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
  cp.setAction(Place::Bookmark);
  m_bookmarks.insert("-here-",cp);
  QLOG_DEBUG() << "-here- set to" << cp.toString();
  if (this->searchTabs(p,true) != -1) {
    return;
  }
  showPlace(p,newTab,switchTab);
  m_revertEnabled = true;
  updateMenu();
}
void LanesLexicon::restoreBookmarks() {
  SETTINGS
  settings.beginGroup("Bookmarks");
  QStringList keys = settings.childKeys();
  for(int i=0;i < keys.size();i++) {
    QString t = settings.value(keys[i]).toString();
    /// something was corrupting the bookmarks saved in the ini file
    /// generating values 210000 bytes long. This caused a segfault
    if (t.size() < 500) {
      Place p = Place::fromString(t);
      if (p.isValid()) {
        m_bookmarks.insert(keys[i],p);
        addBookmarkMenuItem(keys[i]);

      }
    }
  }

}
/**
 * setup all the bookmark shortcuts and actions
 * these are not in the [Shortcut] section because they need to be handled
 * differently
 *
 *   add,jump,save,revert,list
 *
 */
void LanesLexicon::setupBookmarkShortcuts() {
  SETTINGS
  settings.beginGroup("Bookmark");
  m_bookmarkMap = new QSignalMapper(this);
  QString key = settings.value(SID_BOOKMARK_ADD,"Ctrl+B").toString();
  QString ids = settings.value(SID_BOOKMARK_ID,"abcdefghijklmnopqrstuvwxyz").toString();
  for(int i=0;i < ids.size();i++) {
    QString ks = QString("%1,%2").arg(key).arg(ids.at(i).toLower());
    //    QString ks = QString("%1,%2").arg(key).arg(ids.at(i).toUpper());
    QShortcut * sc = new QShortcut(ks,this);
    sc->setContext(Qt::ApplicationShortcut);
    connect(sc,SIGNAL(activated()),m_bookmarkMap,SLOT(map()));
    connect(sc,SIGNAL(activatedAmbiguously()),m_bookmarkMap,SLOT(map()));
    m_bookmarkMap->setMapping(sc,QString("add-%1").arg(ids.at(i).toLower()));
  }
  for(int i=0;i < ids.size();i++) {
    QString ks = QString("%1,%2").arg(key).arg(ids.at(i).toUpper());
    QShortcut * sc = new QShortcut(ks,this);
    sc->setContext(Qt::ApplicationShortcut);
    connect(sc,SIGNAL(activated()),m_bookmarkMap,SLOT(map()));
    connect(sc,SIGNAL(activatedAmbiguously()),m_bookmarkMap,SLOT(map()));
    m_bookmarkMap->setMapping(sc,QString("add-%1").arg(ids.at(i).toUpper()));
  }
  m_bookmarkAddAction = new QAction(tr("&Add"),this);

  key = settings.value(SID_BOOKMARK_JUMP,"Ctrl+J").toString();
  for(int i=0;i < ids.size();i++) {
    QString ks = QString("%1,%2").arg(key).arg(ids.at(i).toLower());
    QShortcut * sc = new QShortcut(ks,this);
    connect(sc,SIGNAL(activated()),m_bookmarkMap,SLOT(map()));
    connect(sc,SIGNAL(activatedAmbiguously()),m_bookmarkMap,SLOT(map()));
    m_bookmarkMap->setMapping(sc,QString("jump-%1").arg(ids.at(i).toLower()));
  }
  for(int i=0;i < ids.size();i++) {
    QString ks = QString("%1,%2").arg(key).arg(ids.at(i).toUpper());
    QShortcut * sc = new QShortcut(ks,this);
    connect(sc,SIGNAL(activated()),m_bookmarkMap,SLOT(map()));
    connect(sc,SIGNAL(activatedAmbiguously()),m_bookmarkMap,SLOT(map()));
    m_bookmarkMap->setMapping(sc,QString("jump-%1").arg(ids.at(i).toUpper()));
  }
  m_bookmarkJumpAction = new QAction(tr("&Jump"),this);

  QShortcut * sc;
  key = settings.value(SID_BOOKMARK_LIST,"Ctrl+B,Ctrl+L").toString();
  sc = new QShortcut(key,this);
  connect(sc,SIGNAL(activated()),m_bookmarkMap,SLOT(map()));
  connect(sc,SIGNAL(activatedAmbiguously()),m_bookmarkMap,SLOT(map()));
  sc->setContext(Qt::ApplicationShortcut);
  m_bookmarkMap->setMapping(sc,QString("list"));
  m_bookmarkListAction = new QAction(tr("&List"),this);
  m_bookmarkListAction->setShortcut(sc->key());
  m_bookmarkListAction->setShortcutContext(Qt::WidgetShortcut);
  connect(m_bookmarkListAction,SIGNAL(triggered()),sc,SIGNAL(activated()));

  key = settings.value(SID_BOOKMARK_REVERT,"Ctrl+B,Ctrl+R").toString();
  sc = new QShortcut(key,this);
    sc->setContext(Qt::ApplicationShortcut);
  connect(sc,SIGNAL(activated()),m_bookmarkMap,SLOT(map()));
  m_bookmarkMap->setMapping(sc,QString("revert"));
  m_bookmarkRevertAction = new QAction(tr("&Revert"),this);
  m_bookmarkRevertAction->setShortcut(sc->key());
  m_bookmarkRevertAction->setShortcutContext(Qt::WidgetShortcut);
  connect(m_bookmarkRevertAction,SIGNAL(triggered()),sc,SIGNAL(activated()));

  key = settings.value(SID_BOOKMARK_CLEAR,"Ctrl+B,Ctrl+C").toString();
  sc = new QShortcut(key,this);
    sc->setContext(Qt::ApplicationShortcut);
  connect(sc,SIGNAL(activated()),m_bookmarkMap,SLOT(map()));
  m_bookmarkMap->setMapping(sc,QString("clear"));
  m_bookmarkClearAction = new QAction(tr("&Clear"),this);
  m_bookmarkClearAction->setShortcut(sc->key());
  m_bookmarkClearAction->setShortcutContext(Qt::WidgetShortcut);
  connect(m_bookmarkClearAction,SIGNAL(triggered()),sc,SIGNAL(activated()));
  connect(m_bookmarkMap,SIGNAL(mapped(QString)),this,SLOT(bookmarkShortcut(const QString &)));

  connect(m_bookmarkAddAction,SIGNAL(triggered()),this,SLOT(bookmarkInput()));
}

void LanesLexicon::bookmarkInput() {
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
  m_bookmarks.insert(id,p);
  addBookmarkMenuItem(id);
  setStatus(QString(tr("Added bookmark:%1")).arg(id));
}
/**
 * moveNext/movePrevious are connected to GraphicsEntry 'next/prev' signal
 *
 * @param p
 */
void LanesLexicon::moveNext(const Place & p) {
  if (m_navMode == LanesLexicon::ByRoot) {
    findNextRoot(p.getRoot());
  }
  else {
    onNextPage();
  }
}
void LanesLexicon::movePrevious(const Place & p) {
  if (m_navMode == LanesLexicon::ByRoot) {
    findPrevRoot(p.getRoot());
  }
  else {
    onPrevPage();
  }
}
/**
 * if scrolling up a page, then at the top of the page
 * find the last headword of previous root
 *
 * @param p
 */
void LanesLexicon::movePreviousHead(const Place & p) {
  if (m_navMode == LanesLexicon::ByRoot) {
    findPrevRoot(p.getRoot());
    GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
    if (entry) {
      entry->focusLast();
    }
  }
  else {
    onPrevPage();
  }
}
void LanesLexicon::setStatus(const QString & txt) {
  QLOG_INFO() << txt.toLocal8Bit().constData();
  statusBar()->showMessage(txt,m_messageInterval);
}
/**
 * Use the spanArabic function to set the font
 *
 */
void LanesLexicon::updateStatusBar(const Place & p) {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  QString headseparator;
  Place current;
  if (p.isValid()) {
    current = p;
  }
  else if (entry) {
    current = entry->getPlace();
  }
  if (! current.isValid()) {
    QLOG_DEBUG() << Q_FUNC_INFO << "Invalid place" << p.toString();
    m_placeIndicator->setText("");
    return;
  }
  QString root = tr("Root:") + qobject_cast<Lexicon *>(qApp)->spanArabic(current.getRoot());
  QString head = current.getWord();
  if (! head.isEmpty()) {
    head = tr("Entry:") + qobject_cast<Lexicon *>(qApp)->spanArabic(head);
    headseparator = ",";
  }
  qobject_cast<Lexicon *>(qApp)->spanArabic(current.getWord());
  QString page = QString(QObject::tr("Vol %1,Page %2")).arg(current.getVol()).arg(current.getPage());
  QString node = current.getNode();
  if ( ! node.isEmpty()) {
    node = "(" + node + ")";
  }
  QString html = QString("<body class=\"place\">%1,%2%3 %4 %5</body>").arg(root).arg(head).arg(headseparator).arg(page).arg(node);
  m_placeIndicator->setText(html);
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
  if (m_navMode == LanesLexicon::ByRoot) {
    onNextRoot();
  }
  else {
    onNextPage();
  }
}
void LanesLexicon::onNavPrev()  {
  if (m_navMode == LanesLexicon::ByRoot) {
    onPrevRoot();
  }
  else {
    onPrevPage();
  }
}
void LanesLexicon::onNavFirst()   {
  if (m_navMode == LanesLexicon::ByRoot) {
    onFirstRoot();
  }
  else {
    onFirstPage();
  }
}
void LanesLexicon::onNavLast()   {
  if (m_navMode == LanesLexicon::ByRoot) {
    onLastRoot();
  }
  else {
    onLastPage();
  }
}
void LanesLexicon::onNavigationMenuChanged(QAction * action) {
  if (m_allowNavMode) {
    m_navigationButton->setDefaultAction(action);
    m_navigationButton->menu()->hide();
  }
}
void LanesLexicon::onClearHistory() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  m_history->clear();
  setStatus(tr("History cleared"));
  setupHistory();
}
void LanesLexicon::onShowHistory() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  HistoryWidget * dlg = new HistoryWidget(m_history,this);
  if (dlg->exec() == QDialog::Accepted) {
    Place p = dlg->getSelected();
    bool newTab = dlg->getNewTab();
    bool switchTab = dlg->getSwitchTab();

    if ( p.isValid()) {
      p.setAction(Place::History);
      showPlace(p,newTab,switchTab);
    }
  }
  delete dlg;
  return;
}
void LanesLexicon::onDocs() {

  SETTINGS

  settings.beginGroup("System");
  if (settings.value("NoDocs",false).toBool()) {
    QMessageBox::information(NULL,tr("Information"),tr("Not yet implemented"));
    return;
  }
  /*
  QWidget * w;
  w = QApplication::activeWindow();
  if (w) {
    qDebug() << Q_FUNC_INFO  << "Active window" << w->objectName() << w->metaObject()->className();
  }
  w = QApplication::focusWidget();
  if (w) {
    qDebug() << Q_FUNC_INFO  << "Focus widget" << w->objectName() << w->metaObject()->className();
  }
  w = QApplication::activeModalWidget();
  if (w) {
    qDebug() << Q_FUNC_INFO  << "Active modal widget" << w->objectName() << w->metaObject()->className();
  }
  w = QApplication::activePopupWidget();
  if (w) {
    qDebug() << Q_FUNC_INFO  << "Active popup widget" << w->objectName() << w->metaObject()->className();
  }
  */
  if (m_helpview == NULL) {
    m_helpview = new HelpView();
    connect(m_helpview,SIGNAL(finished(bool)),this,SLOT(onHelpLoaded(bool)));
    m_helpview->loadHelpSystem();
  }
  else {
    if (m_helpview->isLoaded()) {
      if (m_helpview->isHidden()) {
        m_helpview->show();
      }
      else {
        m_helpview->hide();
      }
    }
  }
   return;
}
/**
 * HelpView emits signal when pageLoaded() is done
 *
 * @param ok
 */
void LanesLexicon::onHelpLoaded(bool ok) {
  if (! ok ) {
    /// check that the initial page has loaded
    /// if that has failed, then warn the user
    if (! m_helpview->isLoaded()) {
      QString docSource;
      if (m_helpview->isOffline()) {
        docSource = "local";
      }
      else {
        docSource = "online";
      }
      delete m_helpview;
      m_helpview = NULL;
      QMessageBox::warning(this, tr("Documentation Error"),
                           QString(tr("There was a problem loading the %1 documentation.\n"
                                      "Please check the settings and try again.")).arg(docSource),
                           QMessageBox::Ok);
      return;
    }
    else {
      /// this is more of a 404 error with an individual page
      QMessageBox::warning(this, tr("Documentation Error"),
                           QString(tr("There was a problem loading the page:\n"
                                      "%1")).arg(m_helpview->lastWishes().toString()),
                           QMessageBox::Ok);
      QLOG_WARN() << "Error loading page"  << m_helpview->lastWishes();
    }
  }
  m_helpview->activateWindow();
}
void LanesLexicon::onAbout() {
  AboutDialog d;
  d.exec();
}
void LanesLexicon::testSlot() {
  QLOG_DEBUG() << Q_FUNC_INFO;
}
/**
 *
 *
 */
void LanesLexicon::searchForPage() {
  int page = 0;
  SearchOptions options;
  if (m_pageSearchDialog == NULL) {
    m_pageSearchDialog  = new PageSearchDialog();
    connect(m_pageSearchDialog,SIGNAL(showHelp(const QString &)),this,SLOT(showHelp(const QString &)));
  }
  else {
    m_pageSearchDialog->setText("");
  }
  if (m_pageSearchDialog->exec()) {
      page = m_pageSearchDialog->getPage();
      m_pageSearchDialog->getOptions(options);
  }
  if (page == 0)
    return;

  QString root;
  QString node;
  QString sql;
  sql = QString(SQL_PAGE_FIND).arg(page);

  QSqlQuery q(m_db);
  if (! q.prepare(sql)) {
    QLOG_WARN() << QString(tr("Error preparing page search SQL :%1")).arg(sql);
    QLOG_WARN() << q.lastError().text();
    viewLogsMessage();
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
  if (m_navMode == LanesLexicon::ByRoot) {
    p.setRoot(root);
    p.setNode(node);
    /// if we have the node on another page,go there
    int ix = this->hasPlace(p,GraphicsEntry::NodeSearch,false);
    if (ix != -1) {
      p.setAction(Place::SwitchTab);
      m_tabs->setCurrentIndex(ix);
      return;
    }
    showPlace(p,options.newTab(),options.activateTab());
  }
  else {
    p.setPage(page);
    this->onGoToPage(p);
  }
  if (m_linkContents) {
    this->syncFromEntry();
  }
}
void LanesLexicon::searchForRoot() {
  QLOG_DEBUG() << Q_FUNC_INFO << m_allowDuplicates;
  int ix;
  if (m_rootSearchDialog == NULL) {
    m_rootSearchDialog = new ArabicSearchDialog(SearchOptions::Root);
    connect(m_rootSearchDialog,SIGNAL(showHelp(const QString &)),this,SLOT(showHelp(const QString &)));
  }
  else {
    //    m_rootSearchDialog->setText("");
  }
  if (m_rootSearchDialog->exec()) {
    QString t = m_rootSearchDialog->getText();
    if (! t.isEmpty()) {
      if (! UcdScripts::isScript(t,"Arabic")) {
        t = convertString(t);
      }
      SearchOptions opts;
      m_rootSearchDialog->getOptions(opts);
      Place p;
      p.setRoot(t);
      ix = this->hasPlace(p,GraphicsEntry::RootSearch,false);
      if (ix != -1) {
        p.setAction(Place::SwitchTab);
        m_tabs->setCurrentIndex(ix);
        return;
      }
      GraphicsEntry *  entry = showPlace(Place(t),opts.newTab(),opts.activateTab());
      if (! entry ) {
        QMessageBox msgBox;
        msgBox.setObjectName("rootnotfound");
        msgBox.setTextFormat(Qt::RichText);
        QString html = (qobject_cast<Lexicon *>(qApp))->spanArabic(t,"rootnotfound");
        msgBox.setText(QString(tr("Root not found:%1")).arg(html));
        msgBox.exec();
      }
      else {
        if (m_linkContents) {
          this->syncFromEntry();
        }
        else {
          m_tabs->currentWidget()->setFocus();
        }
      }
    }
  }
  m_rootSearchDialog->hideKeyboard();
}
int LanesLexicon::addTab(bool create,QWidget * w,const QString & title) {
  QLOG_DEBUG() << Q_FUNC_INFO << create << title;
  int ix = m_tabs->currentIndex();
  if (create) {
    if (m_tabStyle == LanesLexicon::AppendTab) {
      ix = m_tabs->addTab(w,title);
    }
    else {
      ix = m_tabs->insertTab(ix+1,w,title);
    }
  }
  else {
    this->onCloseTab(ix);
    ix = m_tabs->insertTab(ix,w,title);
  }
  this->onFocusContent();
  return ix;
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
  int ix = m_tabs->currentIndex();

  if (searchType == SearchOptions::Word) {
    FullSearchWidget * s = new FullSearchWidget;
    s->setSearch(t,options);
    s->setForceLTR(d->getForceLTR());
    s->findTarget(true);
    connect(s,SIGNAL(showNode(const QString &,bool)),this,SLOT(showSearchNode(const QString &,bool)));
    connect(s,SIGNAL(printNode(const QString &)),this,SLOT(printNode(const QString &)));
    /// this is a count of search tabs (not search results)
    int c = this->getSearchCount();
    ix = this->addTab(options.newTab(),s,QString(tr("Search %1")).arg(c+1));
    if (options.activateTab()) {
      m_tabs->setCurrentIndex(ix);
    }
    int n = s->findCount();
    statusMessage(QString(tr("Search returned %1 %2")).arg(n).arg(n == 1 ? "result" : "results"));
    ///
    /// this shifts focus from ContentsWidget (how it got focus is a mystery)
    ///
    if (options.newTab() && ! options.activateTab()) {
      s->setFocus(Qt::OtherFocusReason);
    }
    return;
  }
  /// head word search
  if (searchType == SearchOptions::Entry) {
      HeadSearchWidget * s = new HeadSearchWidget(this);
      connect(s,SIGNAL(searchResult(const QString &)),this,SLOT(setStatus(const QString &)));
      connect(s,SIGNAL(deleteSearch()),this,SLOT(deleteSearch()));
      connect(s,SIGNAL(showNode(const QString &,bool)),this,SLOT(showSearchNode(const QString &,bool)));

      s->search(target,options);
      if (s->count() == 0) {
        QMessageBox msgBox;
        msgBox.setObjectName("wordnotfound");
        msgBox.setTextFormat(Qt::RichText);
        msgBox.setText(QString(tr("Head word not found: %1")).arg(getLexicon()->spanArabic(target,"wordnotfound")));
        msgBox.exec();
        delete s;
        return;
      }
      ix = this->addTab(options.newTab(),s,QString(tr("Head word search for %1")).arg(target));
      if (options.activateTab()) {
        m_tabs->setCurrentIndex(ix);
      }
      if (options.newTab() && ! options.activateTab()) {
        s->setFocus(Qt::OtherFocusReason);
      }
  }
}
void LanesLexicon::searchForWord() {
  if (m_wordSearchDialog == NULL) {
    m_wordSearchDialog = new ArabicSearchDialog(SearchOptions::Word);
    connect(m_wordSearchDialog,SIGNAL(showHelp(const QString &)),this,SLOT(showHelp(const QString &)));
  }
  else {
    m_wordSearchDialog->setText("");
  }
  //  d.setOptions(m_searchOptions);
  if (m_wordSearchDialog->exec()) {
    QString t = m_wordSearchDialog->getText();
    if (! t.isEmpty()) {
      this->search(SearchOptions::Word,m_wordSearchDialog,t);
    }
  }
  m_searchButton->menu()->hide();
  m_wordSearchDialog->hideKeyboard();
}

/// TODO these needs to search the entry looking for bareword or word
void LanesLexicon::searchForEntry() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  if (m_headSearchDialog == NULL) {
    m_headSearchDialog  = new ArabicSearchDialog(SearchOptions::Entry);
    connect(m_headSearchDialog,SIGNAL(showHelp(const QString &)),this,SLOT(showHelp(const QString &)));

  }
  else {
    //    m_headSearchDialog->setText("");
  }

  if (m_headSearchDialog->exec()) {
    QString t = m_headSearchDialog->getText();
    if (! t.isEmpty()) {
      this->search(SearchOptions::Entry,m_headSearchDialog,t);
    }
  }
  m_headSearchDialog->hideKeyboard();
}
void LanesLexicon::searchForNode() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  if (m_nodeSearchDialog == NULL) {
    m_nodeSearchDialog = new NodeSearchDialog();
    connect(m_nodeSearchDialog,SIGNAL(showHelp(const QString &)),this,SLOT(showHelp(const QString &)));
  }
  else {
    m_nodeSearchDialog->setText("");
  }
  if (m_nodeSearchDialog->exec()) {
    QString t = m_nodeSearchDialog->getText();
    if (! t.isEmpty()) {
      SearchOptions options;
      m_nodeSearchDialog->getOptions(options);
      Place p;
      p.setNode(t);
      int i = this->hasPlace(p,GraphicsEntry::NodeSearch,false);
      if (i != -1) {
        p.setAction(Place::SwitchTab);
        m_tabs->setCurrentIndex(i);
        showPlace(p,false,true);
        return;
      }
      GraphicsEntry * entry = showPlace(p,options.newTab(),options.activateTab());
      if (! entry ) {
        QMessageBox msgBox;
        msgBox.setText(QString(tr("Node not found: %1")).arg(t));
        msgBox.exec();
      }
      else {
        entry->home();
      }
    }
  }
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

  SETTINGS
  settings.beginGroup("Printer");
  m_printerReUse = settings.value(SID_PRINTER_USE).toBool();

  if (! m_printer.isValid())
    m_printerReUse = false;


  if ( m_printerReUse ) {
    entry->print(m_printer,node);
    return;
  }
  QList<QPrinterInfo> printers = QPrinterInfo::availablePrinters();
  if (printers.size() == 0) {
    m_printer.setOutputFormat(QPrinter::PdfFormat);
    entry->print(m_printer,node);
    return;
  }
  QPrintDialog printDialog(&m_printer, this);
  if (printDialog.exec() == QDialog::Accepted) {
    if (m_printer.isValid()) {
      entry->print(m_printer,node);
    }
  }
}
void LanesLexicon::pageSearch() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if ( entry ) {
    entry->setFocus();
    entry->search();
  }
}
void LanesLexicon::pageClear() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if ( entry ) {
    entry->clearHighlights();
    m_clearAction->setEnabled(false);
    return;
  }
  /*
  HeadSearchWidget * results = qobject_cast<HeadSearchWidget *>(m_tabs->currentWidget());
  if (results) {
    results->getEntry()->clearHighlights();
    m_clearAction->setEnabled(false);
    return;
  }
  */


}
void LanesLexicon::currentTabChanged(int ix) {
  QLOG_DEBUG() << Q_FUNC_INFO << ix;

  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(ix));
  if ( entry ) {
    Place p = entry->getPlace();
    if (m_linkContents) {
      m_tree->ensurePlaceVisible(p,true);
    }
    this->enableForPage(true);
    return;
  }
  FullSearchWidget * full = qobject_cast<FullSearchWidget *>(m_tabs->currentWidget());
  if (full)  {
    full->selectFocus();
  }

  this->enableForPage(false);

}
/**
 * We can look for a root or a node. If we have a headword then we have a node.
 *
 * @param place
 * @param activate
 *
 * @return
 */
int LanesLexicon::searchTabs(const Place & p,bool activate) {
  if (m_allowDuplicates) {
    return -1;
  }
  if (! p.node().isEmpty()) {
    return this->searchTabs(p.node(),activate);
  }
  for(int i=0;i < m_tabs->count();i++) {
    GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(i));
    if (entry) {
      if (entry->getPlace().root() == p.root()) {
        if (activate) {
          m_tabs->setCurrentIndex(i);
          entry->focusNode("root");
          return i;
        }
        //        statusMessage(QString(tr("Requested entry already showing in tab %1")).arg(i + 1));
        return i;
      }
    }
  }
  return -1;
}
/**
 * Searchs the tabs for the requested node
 *
 * @param node
 *
 * @return tab index if found, otherwise -1
 */
int LanesLexicon::searchTabs(const QString & node,bool activate) {
  QLOG_DEBUG() << Q_FUNC_INFO << node << m_allowDuplicates;
  if (m_allowDuplicates) {
    return -1;
  }

  if (node.isEmpty()) {
    return -1;
  }
  QList<int> tabs;
  int ix = m_tabs->currentIndex();
  if (ix == -1) {
    return ix;
  }
  for(int i=0;i < m_tabs->count();i++) {
    if (i != ix) {
      tabs << i;
    }
  }
  tabs.insert(0,ix);
  qDebug() << Q_FUNC_INFO << tabs;
  for(int i=0;i < tabs.size();i++) {
    GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(tabs[i]));
    if (entry && entry->hasNode(node)) {
      if (tabs[i] == ix) {
        statusMessage(QString(tr("Requested entry is in current tab")));
      }
      else {
      statusMessage(QString(tr("Requested entry available in tab %1")).arg(tabs[i] + 1));
      }
      if (activate) {
        m_tabs->setCurrentIndex(tabs[i]);
      }
      return tabs[i];
    }
  }
  return -1;
}
void LanesLexicon::showNoteBrowser() {
  for(int i=0;i < m_tabs->count();i++) {
    NoteBrowser * notes = qobject_cast<NoteBrowser *>(m_tabs->widget(i));
    if (notes) {
      m_tabs->setCurrentIndex(i);
      return;
    }
  }
  NoteBrowser * notes = new NoteBrowser(this);
  connect(notes,SIGNAL(noteDeleted(const QStringList &)),this,SLOT(onNotesDeleted(const QStringList &)));

  int ix = m_tabs->addTab(notes,tr("Notes"));
  m_tabs->setCurrentIndex(ix);
}
int LanesLexicon::hasPlace(const Place & p,int searchtype,bool setFocus) {
  if (m_allowDuplicates) {
    return -1;
  }
  if (searchtype == GraphicsEntry::RootSearch) {
    if (p.root().isEmpty()) {
      return -1;
    }
  }
  if (searchtype == GraphicsEntry::NodeSearch) {
    if (p.node().isEmpty()) {
      return -1;
    }
  }
  if (searchtype == GraphicsEntry::WordSearch) {
    if (p.word().isEmpty()) {
      return -1;
    }
  }
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
 * Can be invoked by: FullSearchWidget,HeadSearchWidget or graphicsentry when view link details
 *
 * @param node
 */

/// TODO find out whether to do background loading
void LanesLexicon::showSearchNode(const QString & node,bool forceNewTab) {
  QLOG_DEBUG() << Q_FUNC_INFO << node << forceNewTab;
  GraphicsEntry * entry;
  /*
  FullSearchWidget * w = qobject_cast<FullSearchWidget *>(sender());
  if (w) {
    QLOG_DEBUG() << ">>>>>>>>>>>>>>>>> fullsearch" << node;
  }
  else {
    HeadSearchWidget * h = qobject_cast<HeadSearchWidget *>(sender());
    if (h) {
      QLOG_DEBUG() << ">>>>>>>>>>>>>>>>> headsearch" << node;
    }
    else {
      GraphicsEntry * e = qobject_cast<GraphicsEntry *>(sender());
      if (e) {
        QLOG_DEBUG() << ">>>>>>>>>>>>>>>>>>>> link view" << node;
      }
    }
  }
  */
  bool v = m_allowDuplicates;
  if (forceNewTab) {
    m_allowDuplicates = true;
  }
  Place p;
  p.setNode(node);
  int ix = this->searchTabs(node,false);
  if (ix != -1) {
    GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(ix));
    if (entry) {
      entry->focusNode(node);
    }
    m_allowDuplicates = v;
    return;
  }
  entry = showPlace(p,true,false);
  if (entry) {
    entry->focusNode(node);
    for(int i=0;i < m_tabs->count();i++) {
      GraphicsEntry * g = qobject_cast<GraphicsEntry *>(m_tabs->widget(i));
      if (g == entry) {
        statusMessage(QString(tr("Entry loaded into tab %1")).arg(i+1));
        m_allowDuplicates = v;
        return;
      }
    }
  }
  else {
    qDebug() << Q_FUNC_INFO << "not found";

  }
    m_allowDuplicates = v;

  /*
  //  int ix = this->searchTabs(node);

  if (ix != -1) {
    statusMessage(QString(tr("Entry loaded into tab %1")).arg(ix+1));
    GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(ix));
    if (entry) {
      entry->focusNode(node);
    }
    return;
  }
  */

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
    /*
    Place p = w->getEntry()->getPlace();
    if (p.isValid()) {
      int ix = m_tabs->currentIndex();
      this->onCloseTab(ix);
      GraphicsEntry * entry = new GraphicsEntry(this);
      setSignals(entry);
      entry->installEventFilter(this);
      entry->getView()->installEventFilter(this);

      entry->getXmlForRoot(p);
      ix = this->addTab(true,entry,p.getShortText());
      m_tabs->setCurrentIndex(ix);
      entry->home();
    }
    else {

    }
    */
  }
}
/**
 * Search for all controls that use use keymaps (ImLineEdit,ImEdit) and
 * enable/disable as appropriate
 *
 * Update checkboxes on the keymaps button on the statusbar
 *
 * @param v
 */
void LanesLexicon::enableKeymaps(bool v) {
  bool ok;
  m_keymapsEnabled = v;
  //  QLOG_DEBUG() << Q_FUNC_INFO << v << m_maps;
  QLOG_DEBUG() << Q_FUNC_INFO << "Current map" << m_currentMap;

  QString tip;
  if (m_keymapsEnabled)
    tip = QString(tr("Keymaps, enabled"));
  else
    tip = QString(tr("Keymaps, disabled"));

  m_keymapsButton->setToolTip(tip);
  foreach (QWidget *widget, QApplication::allWidgets()) {
    ImLineEdit * w = qobject_cast<ImLineEdit *>(widget);
    if (w) {
      ok = w->setCurrentMap(m_currentMap);
      if (! ok ) {
        QLOG_WARN() << QString(tr("Error loading map: %1")).arg(m_currentMap);
      }
      else {
        w->enableMapping(v);
      }
    }
    else {
      ImEdit * imedit = qobject_cast<ImEdit *>(widget);
      if (imedit) {
        ok = imedit->setCurrentMap(m_currentMap);
        if (! ok ) {
          QLOG_WARN() << QString(tr("Error loading map: %1")).arg(m_currentMap);
        }
        else {
          imedit->enableMapping(v);
        }
      }
      else {
        SearchOptionsWidget * search = qobject_cast<SearchOptionsWidget *>(widget);
        if (search) {
          search->setKeymapsEnabled(v);
        }
      }
    }
  }
  QList<QAction *> actions = m_keymapsButton->menu()->actions();
  for(int i=0;i < actions.size();i++) {
    if (actions[i]->data().toString() != "_ENABLE_") {
      actions[i]->blockSignals(true);
      if (!v) {
        actions[i]->setChecked(false);
      }
      else {
        if (actions[i]->data().toString() == m_currentMap) {
          actions[i]->setChecked(true);
        }
        else {
          actions[i]->setChecked(false);
        }
      }
      actions[i]->blockSignals(false);
    }
  }
  setStatus(tip);
  SETTINGS
  settings.beginGroup("Maps");
  settings.setValue(SID_MAPS_ENABLED,v);
}
QString LanesLexicon::getActiveKeymap() const {
  return m_currentMap;
}
QString LanesLexicon::getKeymapFileName(const QString & mapname) const {
  if (m_maps.contains(mapname)) {
    return m_maps.value(mapname);
  }
  return QString();
}
QMapIterator<QString,QString> LanesLexicon::getMapIterator() {
  QMapIterator<QString,QString> iter(m_maps);
  return iter;
}
void LanesLexicon::deleteSearch() {
  /*
  DELETE THIS
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
  */
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
    entry->clearHighlights();
  }
}
void LanesLexicon::localSearchShow() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    entry->showSelections();
  }
}
void LanesLexicon::tabsChanged() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  QRegExp rx("^\\d+");
  for(int i=0;i < m_tabs->count();i++) {
    QString t = m_tabs->tabText(i);
    t.remove(rx);
    QString title = QString("%1 %2").arg(i+1).arg(t);
    m_tabs->setTabText(i,title);
  }
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    this->enableForPage(true);
    if (m_linkContents) {
      this->syncFromEntry();
    }
  }
  else {
    this->enableForPage(false);
  }
}
void LanesLexicon::setIcon(QAction * action,const QString & imgdir,const QString & iconfile) {
  if (imgdir.isEmpty() || iconfile.isEmpty()) {
    return;
  }
  QDir d(imgdir);
  QFileInfo fi(d,iconfile);
  if (! fi.exists()) {
    QLOG_WARN() << QString(tr("Icon not found:%1")).arg(iconfile);
    return;
  }
  QIcon icon(fi.absoluteFilePath());
  if (! icon.isNull()) {
    action->setIcon(icon);
    action->setProperty("FILENAME",iconfile);
  }
  else {
    QLOG_WARN() << QString(tr("Error loading %1 : icon is invalid")).arg(iconfile);
  }
}
/**
 * sets icons for actions using the defined theme
 *
 * @param theme
 */
void LanesLexicon::setIcons(const QString & /* theme */) {

  QString iconfile;
  SETTINGS
  settings.beginGroup("Icons");
  QString imageDirectory = getLexicon()->getResourceFilePath(Lexicon::Image);
  QLOG_DEBUG() << Q_FUNC_INFO << imageDirectory;
  QDir imgd(imageDirectory);
  if (!imgd.exists()) {
    QLOG_WARN() << QString(tr("Theme image directory not found : %1")).arg(imgd.absolutePath());
    return;
  }

  QString imgdir = imgd.absolutePath();
  iconfile = settings.value(SID_ICON_EXIT,QString()).toString();
  setIcon(m_exitAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_HISTORY,QString()).toString();
  setIcon(m_historyAction,imgdir,iconfile);


  iconfile = settings.value(SID_ICON_NEXT,QString()).toString();
  setIcon(m_navNextAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_BACK,QString()).toString();
  setIcon(m_navPrevAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_FIRST,QString()).toString();
  setIcon(m_navFirstAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_LAST,QString()).toString();
  setIcon(m_navLastAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_DOCS,QString()).toString();
  setIcon(m_docAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_ABOUT,QString()).toString();
  setIcon(m_aboutAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_BOOKMARKS,QString()).toString();
  setIcon(m_bookmarkAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_SEARCH,QString()).toString();
  setIcon(m_searchAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_ZOOM,QString()).toString();
  setIcon(m_defaultScaleAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_ZOOM_IN,QString()).toString();
  setIcon(m_zoomInAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_ZOOM_OUT,QString()).toString();
  setIcon(m_zoomOutAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_WIDEN,QString()).toString();
  setIcon(m_widenAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_NARROW,QString()).toString();
  setIcon(m_narrowAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_PRINT,QString()).toString();
  setIcon(m_printAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_LOCAL_SEARCH,QString()).toString();
  setIcon(m_localSearchAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_LOCAL_SEARCH_NEXT,QString()).toString();
  setIcon(m_localSearchNextAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_CLEAR,QString()).toString();
  setIcon(m_clearAction,imgdir,iconfile);
  m_clearAction->setEnabled(false);

  iconfile = settings.value(SID_ICON_KEYMAPS,QString()).toString();
  setIcon(m_keymapsAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_KEYMAPS_DISABLED,QString()).toString();
  setIcon(m_keymapsAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_PREFERENCES,QString()).toString();
  setIcon(m_optionsAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_LOGS,QString()).toString();
  setIcon(m_logViewerAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_SYNC_RIGHT,QString()).toString();
  setIcon(m_syncFromContentsAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_SYNC_LEFT,QString()).toString();
  setIcon(m_syncFromEntryAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_NAVIGATION,QString()).toString();
  setIcon(m_navModeRootAction,imgdir,iconfile);

  iconfile = settings.value(SID_ICON_PAGE_NAVIGATION,QString()).toString();
  setIcon(m_navModePageAction,imgdir,iconfile);
    //  m_navigationModeMenu->addAction(m_navModePageAction);

  QIcon icon;
  iconfile = settings.value(SID_ICON_LINK,QString()).toString();
  if (!iconfile.isEmpty() && imgd.exists(iconfile)) {
    icon.addPixmap(imgd.absoluteFilePath(iconfile),QIcon::Normal,QIcon::On);
  }

  iconfile = settings.value(SID_ICON_UNLINK,QString()).toString();
  if (!iconfile.isEmpty() && imgd.exists(iconfile)) {
    icon.addPixmap(imgd.absoluteFilePath(iconfile),QIcon::Normal,QIcon::Off);
  }
  m_linkAction->setIcon(icon);
}
/**
 * show current contents item as page
 *
 */
void LanesLexicon::syncFromContents() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  //  QWidget * w = QApplication::focusWidget();
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (!entry) {
    QLOG_WARN() << "Cannot sync to current tab";
    return;
  }
  Place p = m_tree->getCurrentPlace();
  if (p.isValid()) {
       this->showPlace(p,false,true);
  }
}
/**
 * make current page item, current item in contents
 *
 */
void LanesLexicon::syncFromEntry() {
  QLOG_DEBUG() << Q_FUNC_INFO;

  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if ( ! entry )
    return;

  Place p = entry->getPlace();
  if (p.isValid()) {
    m_tree->ensurePlaceVisible(p);
    entry->focusPlace();
  }
}
/**
 * Redundant replace by syncFromContents and syncFromEntry
 *
 */
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
      this->showPlace(p,false,true);
    }
  }
  else {
    Place p = entry->getPlace();
    if (p.isValid()) {
      m_tree->ensurePlaceVisible(p);
    }
  }
}
/**
 * printNode signal can be from graphicsentry (via context menu)
 * or from fullsearchwidget -> nodeview -> print
 *
 * From nodeview, there is no guarantee that the node is currently
 * visible, so we have extra work to do.
 *
 * @param node
 */
void LanesLexicon::printNode(const QString & node) {
  QLOG_DEBUG() << Q_FUNC_INFO << node;
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(sender());
  if (entry) {
    printCurrentPage(node);
    return;
  }
  FullSearchWidget * search = qobject_cast<FullSearchWidget *>(sender());
  if (! search) {
    return;
  }
  Place p;
  p.setNode(node);
  int i = this->hasPlace(p,GraphicsEntry::NodeSearch,false);
  if (i != -1) {
    entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(1));
    if (entry) {
      printNodeView(entry,node);
      return;
    }
  }
  /// create a graphicsentry so we can print the node
  bool h = history()->enabled();
  history()->setEnabled(false);
  entry = new GraphicsEntry();
  p = entry->getXmlForRoot(p);
  if (! p.isValid()) {
    QMessageBox msgBox;
    msgBox.setText(QString(tr("Node not found: %1")).arg(node));
    msgBox.exec();
  }
  else {
    printNodeView(entry,node);

  }
  delete entry;
  history()->setEnabled(h);
}
void LanesLexicon::printNodeView(GraphicsEntry * entry,const QString & node) {

  SETTINGS
  settings.beginGroup("Printer");
  m_printerReUse = settings.value(SID_PRINTER_USE).toBool();

  if (! m_printer.isValid())
    m_printerReUse = false;


  if ( m_printerReUse ) {
    entry->print(m_printer,node);
    return;
  }
  QList<QPrinterInfo> printers = QPrinterInfo::availablePrinters();
  if (printers.size() == 0) {
    m_printer.setOutputFormat(QPrinter::PdfFormat);
    entry->print(m_printer,node);
    return;
  }

  QPrintDialog printDialog(&m_printer, this);
  if (printDialog.exec() == QDialog::Accepted) {
    if (m_printer.isValid()) {
      entry->print(m_printer,node);
    }
  }
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
/**
 * Create an OptionsDialog if one is not already running
 *
 */
void LanesLexicon::onOptions() {
  QWidgetList widgets = QApplication::topLevelWidgets();
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    OptionsDialog * d = qobject_cast<OptionsDialog *>(widget); // ->metaObject()->className();
    if (d) {
      if (! d->isVisible()) {
        d->show();
      }
      d->raise();
      d->activateWindow();
      d->setFocus();
      return;
    }
  }
  OptionsDialog * d  = new OptionsDialog(m_currentTheme);//,this);
  connect(d,SIGNAL(showHelp(const QString &)),this,SLOT(showHelp(const QString &)));
  connect(d,SIGNAL(hasChanges(bool)),this,SLOT(onOptionsChanged(bool)));
  d->show();
}
void LanesLexicon::onOptionsChanged(bool v) {
  QLOG_DEBUG() << Q_FUNC_INFO << v;
  if (v) {
    activateTheme(m_currentTheme);
  }
}
void LanesLexicon::pageSearchComplete() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(QObject::sender());
  if (!entry) {
    return;
  }
  m_localSearchNextAction->setEnabled(entry->hasMoreFinds());
  m_clearAction->setEnabled(entry->hasHighlights());

}
void LanesLexicon::pageSearchStart() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(QObject::sender());
  if (!entry) {
    return;
  }
  m_localSearchNextAction->setEnabled(entry->hasMoreFinds());
  m_clearAction->setEnabled(entry->hasHighlights());
}
/**
 * enabled/disable toolbar amd menubar items that only work when showing
 * a page
 *
 * @param v
 */
void LanesLexicon::enableForPage(bool v) {
  m_entrybar->setEnabled(v);
  m_navigation->setEnabled(v);
  if (v) {
    GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
    if (entry) {
      m_localSearchNextAction->setEnabled(entry->hasMoreFinds());
      m_clearAction->setEnabled(entry->hasHighlights());
    }

  }
  m_pageMenu->setEnabled(v);
  m_moveMenu->setEnabled(v);
}
/**
 * set the default scaling factor and apply it to the current tab widget
 * if appropriate
 */
void LanesLexicon::onDefaultScale() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  SETTINGS

  settings.beginGroup("Entry");
  qreal scale  = settings.value(SID_ENTRY_SCALE,1.0).toDouble();
  ZoomDialog * d = new ZoomDialog(scale);

  if (d->exec()) {
    settings.setValue(SID_ENTRY_SCALE,d->value());
    if (d->applyAll()) {
      GraphicsEntry * entry;
      for(int i=0;i < m_tabs->count();i++) {
        entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(i));
        if (entry) {
          entry->setScale(d->value());
        }
      }
    }
  }
  delete d;
}
void LanesLexicon::onDefaultWidth() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  SETTINGS

  settings.beginGroup("Entry");
  int w  = settings.value(SID_ENTRY_TEXT_WIDTH,200).toInt();

  TextwidthDialog * d = new TextwidthDialog(w);

  if (d->exec()) {
    settings.setValue(SID_ENTRY_TEXT_WIDTH,d->value());
    if (d->applyAll()) {
      GraphicsEntry * entry;
      for(int i=0;i < m_tabs->count();i++) {
        entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(i));
        if (entry) {
          entry->setTextWidth(d->value(),true);
        }
      }
    }
  }
  delete d;
}
void LanesLexicon::onSelectTheme() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  QStringList themes = getLexicon()->getThemes() ;
  QString currentTheme = m_currentTheme;
  if (themes.size() == 1) {
    QMessageBox msgBox;
    msgBox.setText(tr("Only one theme has been setup."));
    msgBox.exec();
    return;
  }
  QDialog * d = new QDialog(this);
  d->setWindowTitle(tr("Change theme"));
  QVBoxLayout * layout = new QVBoxLayout;
  QComboBox * b = new QComboBox();
  QLabel * label = new QLabel(QString(tr("Select a theme from the list.\nThe current theme is: %1.")).arg(m_currentTheme));
  b->addItems(themes);
  b->setCurrentText(m_currentTheme);
  layout->addWidget(label);
  layout->addWidget(b);
  QDialogButtonBox * btns =  new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  layout->addWidget(btns);
  d->setLayout(layout);
  b->setFocus();
  connect(btns, SIGNAL(accepted()), d, SLOT(accept()));
  connect(btns, SIGNAL(rejected()), d, SLOT(reject()));
  if ((d->exec() == QDialog::Rejected) ||
      (b->currentText() == m_currentTheme)) {
    delete d;
    return;
  }
  QString theme = b->currentText();
  delete d;

  activateTheme(theme);
}
void LanesLexicon::onEditTheme() {
  ThemeDialog d;
  if (d.exec() == QDialog::Rejected) {
    return;
  }
  OptionsDialog opts(d.theme());
  opts.exec();
}
/**
 *
 *
 * @param type 0 - on startup, 1 - on theme change
 *
 * @return
 */
bool LanesLexicon::sanityCheck(int type) {
  if (! openDatabase(m_dbName)) {
    QMessageBox msgBox;
    msgBox.setWindowTitle(QGuiApplication::applicationDisplayName());
    QString errorMessage(tr("Database not found"));
    QDir d;
    QString currentDir = d.absoluteFilePath(m_dbName);
    QString info = QString("Missing file %1").arg(QDir::cleanPath(currentDir));
    QString next;
    if (type == 0) {
      next = tr("This application will terminate");
    }
    else {
      next = tr("Theme change will be ignored");
    }
    msgBox.setText("<html><head/><body><h2>" + errorMessage + "</h2><p>"
                   + info + "</p><p>" + next + "</p></body></html>");
    msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
    msgBox.exec();
    return false;
  }
  QString xslt =  getLexicon()->getXsltFileName();
  if (xslt.isEmpty() || ! QFileInfo::exists(xslt)) {
    QMessageBox msgBox;
    msgBox.setWindowTitle(QGuiApplication::applicationDisplayName());
    QString errorMessage(tr("Critical file not found"));
    QDir d;
    QString info = QString("Missing file %1").arg(QDir::current().relativeFilePath(xslt));
    QString next;
    if (type == 0) {
      next = tr("This application will terminate");
    }
    else {
      next = tr("Theme change will be ignored");
    }
    msgBox.setText("<html><head/><body><h2>" + errorMessage + "</h2><p>"
                   + info + "</p><p>" + next + "</p></body></html>");
    msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
    msgBox.exec();
    return false;

  }
  return true;
}
/**
 * For the optionsdialogs, section is metaObect()->className()
 *
 * @param section - used as a key for the sitemap from which we get the URL
 */
void LanesLexicon::showHelp(const QString & section,const QString & para ) {
  QLOG_DEBUG() << Q_FUNC_INFO << section << para;
  if (m_helpview == NULL) {
    m_helpview = new HelpView();
    connect(m_helpview,SIGNAL(finished(bool)),this,SLOT(onHelpLoaded(bool)));
    //    connect(m_helpview,SIGNAL(helpSystemLoaded(bool)),this,SLOT(onHelpSystemLoaded(bool)));
    m_helpview->loadHelpSystem();
  }

  if (m_siteMap.contains(section.toLower())) {
    QString url = m_siteMap.value(section.toLower());
    if (! para.isEmpty()) {
      url = QString("%1#%2").arg(url).arg(para);
    }
    m_helpview->showSection(url);
  }
  m_helpview->setFocus();
}
void LanesLexicon::onDeleteTheme() {
  DeleteThemeDialog d;
  if (d.exec() == QDialog::Rejected) {
    return;
  }
  QPair<QString,QString> m = d.getThemes();
  QString srcFilePath = getLexicon()->getResourceFilePath(Lexicon::ThemeRoot,m.first);
  activateTheme(m.second);
}

void LanesLexicon::activateTheme(const QString & theme) {
  QLOG_DEBUG() << Q_FUNC_INFO << theme;
  int ret = getLexicon()->setTheme(theme);
  if (ret != Lexicon::Ok) {
    /// TODO say something
  }
  readSettings();
  if (! sanityCheck(1)) {
    getLexicon()->setTheme(m_currentTheme);
    readSettings();
    return;
  }
  setIcons(theme);
  loadStyleSheet();
  setupShortcuts();
  // reload pages
  for(int i=0;i < m_tabs->count();i++) {
    GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(i));
    if (entry) {
      entry->onReload();
    }
  }
  statusMessage(QString(tr("Theme set to: %1")).arg(theme));
  m_currentTheme = theme;
  /// may need to something about printer setup
}
bool LanesLexicon::removeDirectory(const QString & dirName){
    bool result = true;


    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDirectory(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }

    return result;
}
void LanesLexicon::onCreateTheme() {
  CreateThemeDialog d;
  if (d.exec() == QDialog::Rejected) {
    return;
  }
  QPair<QString,QString> m = d.getThemes();
  QString themeRoot = getLexicon()->getResourceFilePath(Lexicon::ThemeRoot);
  QString srcFilePath = themeRoot + QDir::separator() + m.first;
  QString tgtFilePath = themeRoot + QDir::separator() + m.second;

  int copyCount = 0;
  copyRecursively(srcFilePath,tgtFilePath,&copyCount);
  if (d.activate()) {
    this->activateTheme(m.second);
  }
}
bool LanesLexicon::copyRecursively(const QString & srcPath,const QString & targetPath,int * copyCount) {
  QFileInfo srcFileInfo(srcPath);
  if (srcFileInfo.isDir()) {
    QDir targetDir(targetPath);
    targetDir.cdUp();
    if (!targetDir.mkdir(QFileInfo(targetPath).fileName()))
      return false;
    QDir sourceDir(srcPath);
    QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
    foreach (const QString &fileName, fileNames) {
      const QString newSrcFilePath = srcPath + QDir::separator() + fileName;
      const QString newTgtFilePath = targetPath + QDir::separator() + fileName;
      if (!copyRecursively(newSrcFilePath, newTgtFilePath,copyCount))
        return false;
    }
  }
  else {
    if (!QFile::copy(srcPath, targetPath)) {
      return false;
    }
    else {
      (*copyCount)++;
    }
  }
  return true;
}
void LanesLexicon::onChangeArabicFont() {
  FontChangeDialog * d = new FontChangeDialog;
  connect(d,SIGNAL(showHelp(const QString &)),this,SLOT(showHelp(const QString &)));
  d->exec();
  if (d->isModified()) {
    for(int i=0;i < m_tabs->count();i++) {
      GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(i));
      if (entry) {
        entry->onReload();
      }
    }
    m_tree->reloadFont();
  }
  delete d;
}
void LanesLexicon::onKeymapHelp(const QString & mapname) {
  QLOG_DEBUG() << Q_FUNC_INFO << mapname;
  QString map = mapname;
  if (mapname.isEmpty()) {
    map = m_currentMap;
  }
  QWidgetList widgets = QApplication::topLevelWidgets();

  for(int i=0;i < widgets.size();i++) {
    ShowMapWidget * w = qobject_cast<ShowMapWidget *>(widgets[i]);
    if (w && (w->map() == map))  {
      w->show();
      w->raise();
      return;
    }
  }


  if (m_mapHelp.contains(map)) {
    QString fileName = m_mapHelp.value(map);
    ShowMapWidget * w = new ShowMapWidget();
    w->setMapName(map);
    w->loadHtml(fileName);
    w->show();
  }
}
void LanesLexicon::onShowContents() {
  QWidget * w;
  if (m_docked) {
    w = m_treeDock;
  }
  else {
    w = m_tree;
  }
  if (! m_showContentsAction->isChecked()) {
    w->hide();
  }
  else {
    w->show();
  }
}
void LanesLexicon::onShowToolbar() {
  if (! m_showToolbarAction->isChecked()) {
    m_mainbar->hide();
    m_navigation->hide();
    m_entrybar->hide();
  }
  else {
    m_mainbar->show();
    m_navigation->show();
    m_entrybar->show();
  }
}
void LanesLexicon::onShowStatusbar() {
  this->statusBar()->setVisible(m_showStatusBarAction->isChecked());
}
void LanesLexicon::onShowMenubar() {
  if ( m_showMenuAction->isChecked()) {
    m_mainmenu->show();
    return;
  }
  if (m_menuWarning) {
    QShortcut * sc = qobject_cast<QShortcut *>(m_shortcutMap->mapping(QString(SID_SHORTCUT_MENU_SHOW)));
    if (sc) {
      QCheckBox * noshow = new QCheckBox(tr("Check this box to stop this dialog showing again"));
      QMessageBox msgBox;
      msgBox.setCheckBox(noshow);
      msgBox.setWindowTitle(QGuiApplication::applicationDisplayName());
      QString errorMessage(tr("Warning"));
      QString info(tr("The menubar will not be visible"));
      QString next = QString(tr("To make it visible again, press %1")).arg(sc->key().toString());
      msgBox.setText("<html><head/><body><h2>" + errorMessage + "</h2><p>"
                     + info + "</p><p>" + next + "</p></body></html>");
      msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
      msgBox.exec();
      m_menuWarning = ! noshow->isChecked();
    }
  }
  m_mainmenu->hide();
}
void LanesLexicon::onReady() {
  QWidget * w;
  if (m_docked) {
    w = m_treeDock;
  }
  else {
    w = m_tree;
  }
  if (w) {
    m_showContentsAction->setChecked(w->isVisible());
  }
  m_showToolbarAction->setChecked(m_mainbar->isVisible());
  m_showMenuAction->setChecked(m_mainmenu->isVisible());
  m_showStatusBarAction->setChecked(this->statusBar()->isVisible());
  syncFromEntry();
}
void LanesLexicon::onImportLinks() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Import Fixed Links"),
                                                  ".",
                                                  tr("CSV (*.csv *.txt)"));
  if (fileName.isEmpty()) {
    return;
  }
  QFile f(fileName);
  if ( ! f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString str =  QString(tr("Import links error: cannot  open import file: %1 - %2")).arg(fileName).arg(f.errorString());
    QLOG_WARN() << str;
    statusMessage(str);
    return;
  }
  QTextStream in(&f);
  in.setCodec("UTF-8");

  QString dbVersion;
  QSqlQuery query(SQL_GET_INFO);
  if (query.exec() && query.first()) {
    dbVersion = query.value("dbid").toString();
  }
  if (!query.prepare(SQL_UPDATE_LINK_TO_NODE)) {
    QString str = QString(tr("SQL error in import links: %1")).arg(query.lastError().text());
    QLOG_WARN() << str;
    statusMessage(str);
    return;
  }
  QSqlQuery nodefind;
  if (! nodefind.prepare(SQL_ENTRY_FOR_NODE)) {
    QString str = nodefind.lastError().text();
    QLOG_WARN() << str;
    statusMessage(str);
    return;
  }
  QSqlQuery nodeupdate;
  if (! nodeupdate.prepare(SQL_UPDATE_XML_FOR_NODE)) {
    QString str = nodeupdate.lastError().text();
    QLOG_WARN() << str;
    statusMessage(str);
    return;
  }

  int count=0;
  int skipCount = 0;
  int updateCount = 0;
  bool ignoreDb = false;
  QString str;

  while(! in.atEnd()) {
    count++;
    str = in.readLine();
    QStringList p = str.split(",",QString::KeepEmptyParts);
    if (p.size() != 4) {
      QLOG_WARN() << QString(tr("Skipping record %1, incorrect format:[%2]")).arg(count).arg(str);
      skipCount++;
    }
    else {
      QString orthid = p[1];
      QString fromnode = p[2];
      QString targetnode = p[3];
      /// check db version are the same
      if (!ignoreDb && (p[0] != dbVersion)) {
        QString warn = QString(tr("The link update is for a different database version.\nThe links may not match.\n\nDo you wish to continue?"));
        int ret = QMessageBox::warning(this, tr("Link import"),warn,
                                       QMessageBox::Yes | QMessageBox::Cancel);
        if (ret == QMessageBox::Yes) {
          ignoreDb = true;
        }
        else {
          statusMessage(tr("Import cancelled, database version mismatch"));
          return;
        }
      }
      nodefind.bindValue(0,fromnode);
      if (! nodefind.exec()) {
        QString str = QString(tr("Import links error: SQL error : %1")).arg(nodefind.lastError().text());
        QLOG_WARN() << str;
        statusMessage(str);
        return;
      }
      if (! nodefind.first()) {
        QString str = QString(tr("Import links error: cannot find entry: %1")).arg(fromnode);
        QLOG_WARN() << str;
        statusMessage(str);
        return;
      }
      ///
      /// update the entry xml, sett <cref select="tonode">
      //
      QString xml = nodefind.value("xml").toString();
      QDomDocument doc;
      doc.setContent(xml);
      QDomNodeList refs = doc.elementsByTagName("ref");
      for(int i=0;i < refs.size();i++) {
        QDomElement e = refs.at(i).toElement();
        QString cref = e.attribute("cref");
        if (cref == orthid) {
          e.setAttribute("select",targetnode);
          QString nxml = doc.toString(-1);
          nodeupdate.bindValue(0,nxml);
          nodeupdate.bindValue(1,fromnode);
          if (!nodeupdate.exec()) {
            QString str = QString(tr("Import links error: failed to update link details for node %1: %2")).arg(fromnode).arg(nodeupdate.lastError().text());
            QLOG_WARN() << str;
            statusMessage(str);
            return;
          }
          QLOG_DEBUG() << "Updated entry record for node" << fromnode;
        }
      }
      query.bindValue(0,targetnode);
      query.bindValue(1,orthid);
      if (! query.exec()) {
        QLOG_WARN() << QString(tr("Link id: %1, From : %2 , To : 3")).arg(orthid).arg(fromnode).arg(targetnode);
        QLOG_WARN() << QString(tr("SQL error in update links: %1")).arg(query.lastError().text());
      }
      else {
        updateCount++;
      }
    }
  }

  f.close();
  str.clear();
  if (updateCount == 0) {
    str = QString(tr("No links were updated."));
  }
  else {
    str = QString(tr("Imported %1 %2 from %3."))
      .arg(updateCount)
      .arg(updateCount > 1 ? "links" : "link")
      .arg(QDir::current().relativeFilePath(fileName));
  }
  if (skipCount > 0) {
    str += QString(tr(" (Skipped %1 %2)"))
      .arg(skipCount)
      .arg((skipCount > 1 ? tr("records") : tr("record")));
  }
  statusMessage(str);
  return;
}
/**
 * Export links where matchtype = 100 i.e. manually fixed links
 *
 * Fields are: dbversion,orthid,fromnode,tonode
 *
 */

void LanesLexicon::onExportLinks() {
  QString fileName = QFileDialog::getSaveFileName(this, tr("Export Fixed Links"),
                           ".",
                           tr("CSV (*.csv *.txt)"));
  if (fileName.isEmpty()) {
    return;
  }
  QFile f(fileName);
  if ( ! f.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QLOG_WARN() << QString(tr("Error opening export file: %1 - %2")).arg(fileName).arg(f.errorString());
    return;
  }
  QTextStream out(&f);
  out.setCodec("UTF-8");

  QString dbVersion;
  QSqlQuery query(SQL_GET_INFO);
  if (query.exec() && query.first()) {
    dbVersion = query.value("dbid").toString();
  }
  if (!query.prepare(SQL_FIXED_LINKS)) {
    QLOG_WARN() << QString(tr("SQL error in export links: %1")).arg(query.lastError().text());
    return;
  }
  int count=0;
  query.exec();
  while(query.next()) {
    count++;
    out << dbVersion << "," << query.value("orthid").toString() << "," << query.value("fromnode").toString() << "," << query.value("tonode").toString() << endl;
  }
  if (count > 0) {
    f.close();
    statusMessage(QString(tr("Exported %1 %2 to %3"))
                  .arg(count).
                  arg(count > 1 ? "links" : "link").
                  arg(QDir::current().relativeFilePath(fileName)));
  }
  else {
    f.remove();
    statusMessage(tr("No fixed links were found to export"));
  }
  return;
}
void LanesLexicon::onImportXml() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Import XML"),
                           ".",
                           tr("XML (*.xml)"));
  if (fileName.isEmpty()) {
    return;
  }
  importXml(fileName);
}
void LanesLexicon::importXml(const QString & filename) {
  QDomDocument doc;
  int errorLine;
  int errorColumn;
  QString errorStr;
  QFile file(filename);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    QLOG_WARN() << QString("Cannot open XML file:%1").arg(filename);
    return;
  }
  if (!doc.setContent(&file,false,&errorStr,&errorLine,&errorColumn)) {
    QLOG_WARN() << QString(tr("XML parse error: at line %1: %2")).arg(errorLine).arg(errorStr);
    return;
  }
  QSqlQuery q;
  if (!q.prepare(SQL_UPDATE_ENTRY_XML)) {
    QLOG_WARN() << QString(tr("SQL prepare failed for update entry xml:%1")).arg(q.lastError().text());
    return;
  }
  int writeCount = 0;
  QDomElement root = doc.documentElement();
  QDomNodeList nodes = doc.elementsByTagName("entryFree");
  for(int i=0;i < nodes.size();i++) {
    QDomElement e = nodes.at(i).toElement();
    if (e.hasAttribute("id")) {
      QString node = e.attribute("id");
      if (node.startsWith("n")) {
        QString xml;
        QTextStream str(&xml);
        e.save(str,-1);
        q.bindValue(0,xml);
        q.bindValue(1,node);
        if (! q.exec()) {
          QLOG_WARN() << QString(tr("Exec failed for SQL_LINK_UPDATE_STATSU query:%1")).arg(q.lastError().text());
        }
        else {
          writeCount++;
        }
      }
    }
  }
  if (writeCount > 0) {
    QSqlDatabase::database().commit();
  }
  QString str = tr("entries");
  if (writeCount == 1) {
    str = tr("entry");
  }
  statusMessage(QString(tr("Imported %1 %2")).arg(writeCount).arg(str));
}
void LanesLexicon::onDuplicateTab(int index) {
  qDebug() << Q_FUNC_INFO << index;
  GraphicsEntry * entry;
  Place p;
  entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(index));

  if (entry) {
    p = entry->getPlace();
  }
  else {
    return;
  }
  bool v = m_allowDuplicates;

  if (! p.isValid()) {
    QLOG_DEBUG() << Q_FUNC_INFO << "Invalid place" << p.toString();
    return;
  }
  m_allowDuplicates = true;
  entry = this->showPlace(p,true,false);
  if (entry) {
    statusMessage(QString(tr("Duplicated tab %1")).arg(index + 1));
  }
  m_allowDuplicates = v;
}
/**
 * show dialog, accept settings
 * write to .ini
 * update an GraphicsEntry tabs
 */
void LanesLexicon::onXrefMode() {
  QLOG_DEBUG() << Q_FUNC_INFO;

  SETTINGS

  settings.beginGroup("System");

  int m = settings.value(SID_SYSTEM_LINK_CHECK_MODE,0).toInt();
  QDialog * d = new QDialog();
  d->setWindowTitle(tr("Set cross-reference mode"));
  QVBoxLayout * layout = new QVBoxLayout;
  QVBoxLayout * btnlayout = new QVBoxLayout;
  QGroupBox * group = new QGroupBox(tr("Set cross-reference mode"));
  QRadioButton * btn1 = new QRadioButton(tr("Follow cross-reference"));
  QRadioButton * btn2 = new QRadioButton(tr("Show dialog and do not follow cross-reference"));
  QRadioButton * btn3 = new QRadioButton(tr("Show dialog and follow cross-reference"));

  switch(m) {
  case 0 : { btn1->setChecked(true);break; }
  case 1 : { btn2->setChecked(true);break; }
  case 2 : { btn3->setChecked(true);break; }
  default : btn1->setChecked(true);
  }

  btnlayout->addWidget(btn1);
  btnlayout->addWidget(btn2);
  btnlayout->addWidget(btn3);
  btnlayout->addStretch();
  group->setLayout(btnlayout);

  QDialogButtonBox * btns = new QDialogButtonBox(QDialogButtonBox::Ok| QDialogButtonBox::Cancel);
  layout->addWidget(group);
  layout->addWidget(btns);
  layout->addStretch();
  d->setLayout(layout);
  connect(btns,SIGNAL(accepted()),d,SLOT(accept()));
  connect(btns,SIGNAL(rejected()),d,SLOT(reject()));
  if (d->exec() != QDialog::Accepted) {
    delete d;
    return;
  }
  int v;
  if (btn1->isChecked()) {
    v = 0;
  }
  if (btn2->isChecked()) {
    v = 1;
  }
  if (btn3->isChecked()) {
    v = 2;
  }
  if (v != m) {
    settings.setValue(SID_SYSTEM_LINK_CHECK_MODE,v);
    for(int i=0;i < m_tabs->count();i++) {
      GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(i));
      if (entry) {
        entry->setXrefMode(v);
      }
    }
  }
  delete d;
}

void LanesLexicon::onSavePageSet() {
  qDebug() << Q_FUNC_INFO;

  SavePageSetDialog *  d = new SavePageSetDialog(m_tabs);
  if (d->exec() != QDialog::Accepted) {
    delete d;
    return;
  }
  QString title = d->pageSetTitle();
  QList<int> tabs = d->requestedTabs();
  QLOG_DEBUG() << Q_FUNC_INFO << tabs;
  int oid = d->overwriteId();

  delete d;

  if (oid != -1) {
    EditPageSetDialog::deletePageSet(oid);
  }
  QSqlQuery q(QSqlDatabase::database("notesdb"));
  if (! q.prepare(SQL_PAGESET_ADD_HEADER)) {
    QLOG_WARN() << QString(tr("Prepare failed for SQL_PAGESET_ADD_HEADER query:%1")).arg(q.lastError().text());
    return;
  }
  QSqlQuery p(QSqlDatabase::database("notesdb"));
  if (! p.prepare(SQL_PAGESET_ADD_PAGE)) {
    QLOG_WARN() << QString(tr("Prepare failed for SQL_PAGESET_ADD_PAGE query:%1")).arg(p.lastError().text());
    return;
  }



  q.bindValue(0,title);
  q.bindValue(1,QDateTime::currentDateTime().toString());
  if (! q.exec()) {
    QLOG_WARN() << QString(tr("Exec failed for SQL_PAGESET_ADD_HEADER query:%1")).arg(q.lastError().text());
    return;
  }
  QVariant v = q.lastInsertId().toInt();
  if (! v.isValid()) {
    QLOG_WARN() << QString(tr("No valid id return from pageset insert:%1")).arg(q.lastError().text());
    return;
  }
  int id = v.toInt();
  int ix = 0;

  for(int i=0;i < tabs.size();i++) {
    GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(tabs[i]));
    QString data;
    QString place;
    if (entry) {
      ix++;
      place = entry->getPlace().toString();
      data += QString("?type=%1").arg("entry");
      data += QString("?zoom=%1").arg(entry->getScale());
      data += QString("?textwidth=%1").arg(entry->getTextWidth());
      data += QString("?usertitle=%1").arg(entry->userTitle());
      QString h = entry->getHome();
      if (! h.isEmpty()) {
        data += QString("?home=%1").arg(h);
      }

      p.bindValue(0,id);
      p.bindValue(1,place);
      p.bindValue(2,data);
      p.bindValue(3,ix);
      if (!p.exec()) {
        QLOG_WARN() << QString(tr("Exec failed for SQL_PAGESET_ADD_PAGE query:%1")).arg(p.lastError().text());
      }
    }
  }
  QSqlDatabase::database("notesdb").commit();
  QString plural;
  switch(ix) {
  case 0 : { plural = tr("tabs");break;}
  case 1 : { plural = tr("tab");break;}
  default : { plural = tr("tabs");break;}
  }
  plural = (ix > 1 ? "tabs" : "tab");
  statusMessage(QString("Saved tab set \"%1\", %2 %3").arg(title).arg(ix).arg(plural));
}
void LanesLexicon::onLoadPageSet() {
  QLOG_DEBUG() << Q_FUNC_INFO;

  LoadPageSetDialog * d = new LoadPageSetDialog;
  QList<int> pages;
  bool clearTabs;
  if (d->exec() == QDialog::Accepted) {
    pages = d->pages();
    clearTabs = d->closeExisting();
  }
  delete d;
  if (pages.size() == 0) {
    return;
  }
  QSqlQuery q(QSqlDatabase::database("notesdb"));
  if (! q.prepare(SQL_PAGESET_DETAIL_BY_ID)) {
    QLOG_WARN() << QString(tr("Prepare failed for SQL_PAGESET_DETAIL query:%1")).arg(q.lastError().text());
    return;
  }
  if (clearTabs) {
    this->closeAllTabs();
  }
  int ix=0;
  QRegularExpression rx("(\\w+)=(.+)");
  bool ok;
  for(int i=0;i < pages.size();i++) {
    q.bindValue(0,pages[i]);
    q.exec();
    if (q.first()) {
      QString str = q.record().value("place").toString();
      Place p = Place::fromString(str);
      if (p.isValid()) {
	GraphicsEntry * entry = this->showPlace(p,true,false);
	QString d = q.record().value("userdata").toString();
	QStringList v = d.split("?");
	for(int j=0;j < v.size();j++) {
	  QRegularExpressionMatch m = rx.match(v[j]);
          if (m.hasMatch()) {
            QString k = m.captured(1);
            QString v = m.captured(2);
            if (k == "usertitle") {
              entry->setUserTitle(v);
            }
            else if (k == "zoom") {
              double d = v.toDouble(&ok);
              if (ok) {
                entry->setScale(d);
              }
            }
            else if (k == "textwidth") {
              int w = v.toInt(&ok);
              if (ok) {
                entry->setTextWidth(w);
              }
            }
            else if (k == "type") {
              /// TODO ??
            }
            else if (k == "home") {
              entry->setHome(v);
            }
          }
	}
	ix++;
      }
      else {
	QLOG_WARN() << QString(tr("Invalid place not loaded:%1")).arg(str);
      }
    }
    else {
    }
  }
  QString plural;
  switch(ix) {
  case 0 : { plural = tr("tabs");break;}
  case 1 : { plural = tr("tab");break;}
  default : { plural = tr("tabs");break;}
  }
  plural = (ix > 1 ? "tabs" : "tab");
  statusMessage(QString("Restored %1 %2").arg(ix).arg(plural));
}
void LanesLexicon::onEditPageSet() {
  QLOG_DEBUG() << Q_FUNC_INFO;

  EditPageSetDialog  d;
  d.exec();
}
/**
 * when notes are deleted we have to check if any are showing and reload the tab
 *
 * so get a list of tabs which need reloading
 *
 * @param nodes
 */
void LanesLexicon::onNotesDeleted(const QStringList & nodes) {
  QLOG_DEBUG() << Q_FUNC_INFO << nodes;
  QList<int> t;
  for(int i=0;i < nodes.size();i++) {
    for(int j=0;j < m_tabs->count();j++) {
      GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(j));
      if (entry && entry->hasNode(nodes[i])) {
        if (! t.contains(j)) {
          t << j;
        }
      }
    }
  }
  for(int i=0;i < t.size();i++) {
    GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(t[i]));
    entry->onReload();
  }
}
QTabWidget * LanesLexicon::tabwidget() const {
  return m_tabs;
}
void LanesLexicon::onTabList() {
  TabListDialog * d = new TabListDialog;

  d->setAttribute(Qt::WA_DeleteOnClose);
  d->show();
}
QWidget * LanesLexicon::currentTab() {
  return m_tabs->currentWidget();
}
void LanesLexicon::onFocusContent() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    entry->focusPlace();
  }
  else {
    m_tabs->currentWidget()->setFocus();
  }
}
void LanesLexicon::onAllowDuplicates() {
  m_allowDuplicates = ! m_allowDuplicates;
  SETTINGS

 settings.beginGroup("System");
 settings.setValue(SID_SYSTEM_ALLOW_DUPLICATES,m_allowDuplicates);
 if (m_allowDuplicates) {
   statusMessage(tr("Duplicates allowed"));
 }
 else {
   statusMessage(tr("Duplicates not allowed"));
 }
}
