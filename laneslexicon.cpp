#include "laneslexicon.h"
extern cmdOptions progOptions;
LanesLexicon::LanesLexicon(QWidget *parent) :
    QMainWindow(parent)

{
  m_ok = false;
  m_history = 0;
  m_revertEnabled = false;
  m_mapper = im_new();
  readSettings();

  if (! openDatabase(m_dbName)) {
    QMessageBox msgBox;
    msgBox.setText(QString(tr("No database, nothing to show")));
    QDir d;
    QString currentDir = d.absoluteFilePath(m_dbName);
    msgBox.setInformativeText(QString("Missing file %1").arg(QDir::cleanPath(currentDir)));
    msgBox.exec();
    return;
  }
  loadStyleSheet();

  m_tree = new ContentsWidget(this);
  m_tree->setObjectName("treeRoots");
  m_tree->installEventFilter(this);
  m_tabs = new QTabWidget(this);
  m_tabs->setTabsClosable(true);

  if ( m_useNotes) {
    m_notes = new NotesWidget(this);
    m_notes->hide();
  }

  /// at the end of the history, but we should be able to restore from settings
  /// TODO would we want restore our current position in history?
  m_history = new HistoryMaster(m_historyDbName);
  if ( ! m_history->isOk()) {
    QLOG_WARN() << "History is not available";
    m_historyEnabled = false;
  }
  m_history->setEnabled(m_historyEnabled);

  //  m_notes->setObjectName("notes");
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
    QSplitter * splitter = new QSplitter;
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

  //  connect(m_tree,SIGNAL(itemActivated(QTreeWidgetItem *,int)),this,SLOT(rootClicked(QTreeWidgetItem *,int)));
  connect(m_tabs,SIGNAL(tabCloseRequested(int)),this,SLOT(onCloseTab(int)));


  if ( m_useNotes ) {
  connect(this,SIGNAL(nodeActivated(const QString & ,const QString & )),
          m_notes,SLOT(setActiveNode(const QString & ,const QString & )));
  connect(m_notesBtn,SIGNAL(clicked()),this,SLOT(onNotesClicked()));
  }


  /// TOTDO replace this last visited item
  if (m_restoreTabs) {
    restoreTabs();
  }
  if (m_tabs->count() > 0) {
      m_tabs->currentWidget()->setFocus();
   }
  else {
    GraphicsEntry * entry = new GraphicsEntry(this);
    entry->installEventFilter(this);
    setSignals(entry);
    if (! m_firstRoot.isEmpty()) {
      m_tabs->addTab(entry,tr(""));
      Place p;
      p.setRoot(m_firstRoot);
      showPlace(p,false);
      m_tree->ensureVisible(m_firstRoot);
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

  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  settings->setIniCodec("UTF-8");
  settings->beginGroup("Main");
  this->restoreGeometry(settings->value("Geometry").toByteArray());
  this->restoreState(settings->value("State").toByteArray());
  delete settings;
}

LanesLexicon::~LanesLexicon()
{
  /// TODO make this check something has changed or give
  /// option to save settings ?
  if (m_ok) {
    writeSettings();
  }
  if (m_history) {
    delete m_history;
  }
  if (m_db.isOpen()) {
    m_db.close();
  }
  /// TODO close notes db
  freeXslt();
  im_free(m_mapper);
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
  connect(entry,SIGNAL(historyAddition()),this,SLOT(historyAddition()));
  connect(entry,SIGNAL(bookmarkAdd(const QString &,const Place &)),this,SLOT(bookmarkAdd(const QString &,const Place &)));
  //  connect(entry,SIGNAL(rootChanged(const QString & ,const QString & )),this,SLOT(rootChanged(const QString &, const QString &)));

  connect(entry,SIGNAL(placeChanged(const Place &)),this,SLOT(placeChanged (const Place &)));
}
void LanesLexicon::onCloseTab(int ix) {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(ix));
  if (entry) {
    m_tabs->removeTab(ix);
    delete entry;
    return;
  }
  SearchResultsWidget * results = qobject_cast<SearchResultsWidget *>(m_tabs->widget(ix));
  if (results) {
      m_tabs->removeTab(ix);
      delete results;
      return;
  }
  m_tabs->removeTab(ix);
}
void LanesLexicon::shortcut(const QString & k) {
  QString key = k.toCaseFolded();
  qDebug() << Q_FUNC_INFO << k;
  if (key == QString("search root")) {
    searchForRoot();
  }
  else if (key == QString("search node")) {
    searchForNode();
  }
  else if (key == QString("search word")) {
    searchForWord();
  }
  else if (key == QString("Page search").toCaseFolded()) {
    searchForPage();
  }
  else if (key == QString("Contents collapse all").toCaseFolded()) {
    m_tree->collapseAll();
  }
  else if (key == QString("Contents collapse letter").toCaseFolded()) {
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
      qDebug() << Q_FUNC_INFO << "No current item";
    }
  }
  else if (key == QString("Quit").toCaseFolded()) {
    on_actionExit();
  }
  else if (key == QString("Toggle Interface").toCaseFolded()) {
    if (m_interface == "minimal") {
      m_interface = "default";
    }
    else {
      m_interface = "minimal";
    }
    setupInterface();
  }
  else if (key == QString("Contents Show").toCaseFolded()) {
    if (m_treeDock->isVisible()) {
      m_treeDock->hide();
    }
    else {
      m_treeDock->show();
    }
  }
  else if (key == QString("Root Next").toCaseFolded()) {
    this->on_actionNextRoot();
  }
  else if (key == QString("Root Prev").toCaseFolded()) {
    this->on_actionPrevRoot();
  }
  else if (key == QString("Root First").toCaseFolded()) {
    this->on_actionFirstRoot();
  }
  else if (key == QString("Root Last").toCaseFolded()) {
    this->on_actionLastRoot();
  }
  else if (key == QString("Page Next").toCaseFolded()) {
    this->on_actionNextPage();
  }
  else if (key == QString("Page Prev").toCaseFolded()) {
    this->on_actionPrevPage();
  }
  else if (key == QString("Page First").toCaseFolded()) {
    this->on_actionFirstPage();
  }
  else if (key == QString("Page Last").toCaseFolded()) {
    this->on_actionLastPage();
  }
  else if (key == QString("Focus Content").toCaseFolded()) {
    m_tabs->currentWidget()->setFocus();
  }
  else if (key == QString("Focus Tree").toCaseFolded()) {
    m_tree->setFocus();
  }
  else if (key.startsWith(QString("Go Tab").toCaseFolded())) {
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
  else if (key.startsWith("bookmark")) {
    bookmarkShortcut(key);
  }
  else if (key == "select entry") {

    GraphicsEntry * page = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
    if (page) {
      page->selectEntry();
    }
  }
  else if (key == "select all") {
    GraphicsEntry * page = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
    if (page) {
      page->selectAll();
    }
  }
  else if (key == "nav root mode") {
    m_navMode = 0;
  }
  else if (key == "nav page mode") {
    m_navMode = 1;
  }
  else if (key == "history next") {
    /// increment history pos
  }
  else if (key == "history back") {
    if (m_historyPos > 0) {
      m_historyPos--;
      setupHistory(m_historyPos);
    }
  }
  else {
    QLOG_WARN() << "Unhandled shortcut" << key;
  }
  updateStatusBar();
  updateMenu();
  //qDebug() << qobject_cast<QShortcut *>(m_signalMapper->mapping(k));
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
void LanesLexicon::createActions() {
  /// TODO get this from QSettings
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  settings->setIniCodec("UTF-8");
  settings->beginGroup("Icons");

  QString imgdir = settings->value("Directory","images").toString();

  m_exitAction = createIconAction(imgdir,settings->value("Exit",QString()).toString(),tr("Exit"));
  connect(m_exitAction,SIGNAL(triggered()),this,SLOT(on_actionExit()));

  m_clearHistoryAction = new QAction(tr("Clear"),this);
  connect(m_clearHistoryAction,SIGNAL(triggered()),this,SLOT(on_actionClearHistory()));

  m_testAction = new QAction(tr("Test"),this);
  connect(m_testAction,SIGNAL(triggered()),this,SLOT(on_actionTest()));
  /// probably need icons

  m_historyAction = createIconAction(imgdir,settings->value("History",QString()).toString(),tr("History"));
    //ew QAction(QIcon(imgdir + "/go-previous.png"),tr("Back"),this);

  m_navNextAction = createIconAction(imgdir,settings->value("Next",QString()).toString(),tr("Next"));
  m_navPrevAction = createIconAction(imgdir,settings->value("Back",QString()).toString(),tr("Back"));
  m_navFirstAction = createIconAction(imgdir,settings->value("First",QString()).toString(),tr("First"));
  m_navLastAction = createIconAction(imgdir,settings->value("Last",QString()).toString(),tr("Last"));

  connect(m_navNextAction,SIGNAL(triggered()),this,SLOT(on_actionNavNext()));
  connect(m_navPrevAction,SIGNAL(triggered()),this,SLOT(on_actionNavPrev()));
  connect(m_navFirstAction,SIGNAL(triggered()),this,SLOT(on_actionNavFirst()));
  connect(m_navLastAction,SIGNAL(triggered()),this,SLOT(on_actionNavLast()));


  m_rootForwardAction = new QAction(QIcon(imgdir + "/go-next.png"),tr("Next Root"),this);
  m_rootBackwardAction = new QAction(QIcon(imgdir + "/go-previous.png"),tr("Prev Root"),this);
  m_rootFirstAction = new QAction(QIcon(imgdir + "/go-first.png"),tr("First Root"),this);
  m_rootLastAction = new QAction(QIcon(imgdir + "/go-last.png"),tr("Last Root"),this);

  //  connect(m_rootForwardAction,SIGNAL(triggered()),this,SLOT(on_actionNextRoot()));
  //  connect(m_rootBackwardAction,SIGNAL(triggered()),this,SLOT(on_actionPrevRoot()));
  //  connect(m_rootFirstAction,SIGNAL(triggered()),this,SLOT(on_actionFirstRoot()));
  //  connect(m_rootLastAction,SIGNAL(triggered()),this,SLOT(on_actionLastRoot()));

  m_pageForwardAction = new QAction(QIcon(imgdir + "/go-next.png"),tr("Next Page"),this);
  m_pageBackwardAction = new QAction(QIcon(imgdir + "/go-previous.png"),tr("Prev Page"),this);
  m_pageFirstAction = new QAction(QIcon(imgdir + "/go-first.png"),tr("First Page"),this);
  m_pageLastAction = new QAction(QIcon(imgdir + "/go-last.png"),tr("Last Page"),this);

  m_docAction = createIconAction(imgdir,settings->value("Docs",QString()).toString(),tr("Docs"));

  m_bookmarkAction = createIconAction(imgdir,settings->value("Bookmarks",QString()).toString(),tr("Bookmarks"));

  m_navigationAction = createIconAction(imgdir,settings->value("Move",QString()).toString(),tr("Move"));

  m_searchAction = createIconAction(imgdir,settings->value("Search",QString()).toString(),tr("Search"));

  //  connect(m_pageForwardAction,SIGNAL(triggered()),this,SLOT(on_actionNextPage()));
  //  connect(m_pageBackwardAction,SIGNAL(triggered()),this,SLOT(on_actionPrevPage()));
  //  connect(m_pageFirstAction,SIGNAL(triggered()),this,SLOT(on_actionFirstPage()));
  //  connect(m_pageLastAction,SIGNAL(triggered()),this,SLOT(on_actionLastPage()));


  m_navModeRootAction = new QAction(tr("By root"),this);
  m_navModeRootAction->setData(LanesLexicon::ByRoot);
  m_navModeRootAction->setCheckable(true);
  if (m_navMode == LanesLexicon::ByRoot) {
    m_navModeRootAction->setChecked(true);
  }
  m_navModePageAction = new QAction(tr("By page"),this);
  m_navModePageAction->setData(LanesLexicon::ByPage);
  m_navModePageAction->setCheckable(true);
  if (m_navMode == LanesLexicon::ByPage) {
    m_navModePageAction->setChecked(true);
  }
  connect(m_navModeRootAction,SIGNAL(triggered()),this,SLOT(onNavModeChanged()));
  connect(m_navModePageAction,SIGNAL(triggered()),this,SLOT(onNavModeChanged()));

  connect(m_docAction,SIGNAL(triggered()),this,SLOT(on_actionDocs()));

  m_searchWordAction = new QAction(tr("For &word"),this);
  connect(m_searchWordAction,SIGNAL(triggered()),this,SLOT(searchForWord()));
  m_searchPageAction = new QAction(tr("For &page"),this);
  connect(m_searchPageAction,SIGNAL(triggered()),this,SLOT(searchForPage()));
  m_searchRootAction = new QAction(tr("For &root"),this);
  connect(m_searchRootAction,SIGNAL(triggered()),this,SLOT(searchForRoot()));
  m_searchNodeAction = new QAction(tr("For &node"),this);
  connect(m_searchNodeAction,SIGNAL(triggered()),this,SLOT(searchForNode()));

  m_zoomInAction = createIconAction(imgdir,settings->value("ZoomIn",QString()).toString(),tr("Zoom in"));
  m_zoomOutAction = createIconAction(imgdir,settings->value("ZoomOut",QString()).toString(),tr("Zoom in"));
  m_widenAction = createIconAction(imgdir,settings->value("Widen",QString()).toString(),tr("Widen text"));
  m_narrowAction = createIconAction(imgdir,settings->value("Narrow",QString()).toString(),tr("Narrow text"));
  m_printAction = createIconAction(imgdir,settings->value("Print",QString()).toString(),tr("Print"));
  m_localSearchAction = createIconAction(imgdir,settings->value("LocalSearch",QString()).toString(),tr("Search page"));

  delete settings;
}
void LanesLexicon::createToolBar() {
  m_fileToolBar = addToolBar(tr("&File"));
  m_fileToolBar->setObjectName("filetoolbar");

  m_fileToolBar->addAction(m_exitAction);
  m_fileToolBar->addAction(m_testAction);

  QToolBar * history = addToolBar(tr("History"));
  history->setObjectName("historytoolbar");
  m_hBackwardBtn = new QToolButton(history);
  m_hBackwardBtn->setText("History");
  m_hBackwardBtn->setDefaultAction(m_historyAction);
  m_hBackwardBtn->setPopupMode(QToolButton::MenuButtonPopup);
  m_hBackwardBtn->setEnabled(false);
  m_hBackwardBtn->setMenu(m_historyMenu);
  history->addWidget(m_hBackwardBtn);
  history->addSeparator();


  QToolBar * navigation = addToolBar(tr("Navigation"));
  navigation->setObjectName("navigationtoolbar");
  m_navText = new QLabel("");
  if (m_navMode == 0) {
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
  if (m_navMode == 0) {
    action->setChecked(true);
  }
  else {
    action->setChecked(false);
  }
  action->setData(QVariant(0));//event->getId());
  connect(action,SIGNAL(triggered()),this,SLOT(onNavModeChanged()));

  action = group->addAction(tr("By page"));
  action->setCheckable(true);

  if (m_navMode == 1) {
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

  QToolBar * bookmarks = addToolBar(tr("Bookmarks"));
  bookmarks->setObjectName("bookmarkstoolbar");
  m_bookmarkBtn = new QToolButton(bookmarks);
  m_bookmarkBtn->setDefaultAction(m_bookmarkAction);
  m_bookmarkBtn->setText(tr("Bookmarks"));
  m_bookmarkBtn->setPopupMode(QToolButton::MenuButtonPopup);
  m_bookmarkBtn->setEnabled(true);
  m_bookmarkBtn->setMenu(m_bookmarkMenu);
  bookmarks->addWidget(m_bookmarkBtn);
  bookmarks->addSeparator();

  QToolBar * search = addToolBar(tr("Search"));
  search->setObjectName("searchtoolbar");
  m_searchButton = new QToolButton(search);
  m_searchButton->setDefaultAction(m_searchAction);
  m_searchButton->setText(tr("Search"));
  m_searchButton->setPopupMode(QToolButton::MenuButtonPopup);
  m_searchButton->setEnabled(true);
  m_searchButton->setMenu(m_searchMenu);
  search->addWidget(m_searchButton);
  search->addSeparator();


  QToolBar * docs = addToolBar("&Docs");
  docs->setObjectName("docstoolbar");
  docs->addAction(m_docAction);

  QToolBar * page = addToolBar("&Page");
  page->setObjectName("pagetoolbar");
  page->addAction(m_zoomInAction);
  page->addAction(m_zoomOutAction);
  page->addAction(m_widenAction);
  page->addAction(m_narrowAction);
  page->addAction(m_printAction);
  page->addAction(m_localSearchAction);
  page->addSeparator();
}
/**
 * when user has done something that adds to history
 * rebuild the dropdown, unchecking everything
 *
 */
void LanesLexicon::historyAddition() {
  statusBar()->showMessage(tr("History added"));
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
  qDebug() << Q_FUNC_INFO << currPos;
  m_historyPos = currPos;
  QList<HistoryEvent *> events = m_history->getHistory();//10,0,currPos);
  if (events.size() == 0) {
    m_hBackwardBtn->setEnabled(false);
    m_clearHistoryAction->setEnabled(false);
  }
  else {
    // m_historyMenu->clear();
    m_hBackwardBtn->setEnabled(true);
    m_clearHistoryAction->setEnabled(true);
    QList<QAction *> actions = m_historyMenu->actions();
    actions.removeOne(m_clearHistoryAction);
    for(int i=0;i < actions.size();i++) {
        m_historyMenu->removeAction(actions[i]);
        delete actions[i];
    }
    QActionGroup * group = new QActionGroup(this);
    while(events.size() > 0) {
      HistoryEvent * event = events.takeFirst();
      Place p = event->getPlace();
      QString root = p.getRoot();
      QString word = p.getWord();
      int id = event->getId();

      QString txt;
      if (p.getPageMode()) {
        txt = QString(tr("%1 Page %2")).arg(id).arg(p.getPage());
      }
      else if (! word.isEmpty()) {
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
  if (currPos  == -1) {
    return;
  }
  return;
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
  statusBar()->addPermanentWidget(m_placeIndicator);
  statusBar()->addPermanentWidget(m_navModeIndicator);

  updateStatusBar();
}
QSize LanesLexicon::sizeHint() const {
  return QSize(800,600);
}
void LanesLexicon::onHistorySelection() {
  QAction * action = static_cast<QAction *>(QObject::sender());
  QVariant v = action->data();
  Place p = v.value<Place>();
  p.setType(Place::History);
  if (p.getPageMode()) {
    onGoToPage(p);
  }
  else {
    showPlace(p,false);
  }
}
void LanesLexicon::on_actionExit()
{
  if (m_db.isOpen()) {
    m_db.close();
  }
  if (m_useNotes) {
    m_notes->close();
    delete m_notes;
  }
  close();
}
void LanesLexicon::onNotesClicked() {
  //  m_tabs->setCurrentWidget(m_notes);
  m_notes->show();
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
  bool ok;
  m_db = QSqlDatabase::addDatabase("QSQLITE");
  m_db.setDatabaseName(dbname);
  ok = m_db.open();

  QLOG_INFO() << "Opened database" << dbname;
  return ok;
}
void LanesLexicon::rootClicked(QTreeWidgetItem * item,int /* column */) {
  bool newTab = false;
  QString root = item->text(0);
  QString supp = item->text(1);
  int p = 0;
  if (supp == "*") {
    p = 1;
  }
  if (QApplication::keyboardModifiers() & Qt::ShiftModifier) {
    newTab = true;
  }
  Place m(root,p);
  m.setType(Place::User);
  showPlace(m,newTab);
}
Place LanesLexicon::showPlace(const Place & p,bool createTab) {
  Place np;
  GraphicsEntry * entry;
  if (! p.isValid()) {
    return p;
  }
  int currentTab = m_tabs->currentIndex();
  if (currentTab == -1) {
    createTab = true;
  }
  else {
    /// it could be either a graphicsentry or a search results window
    entry = dynamic_cast<GraphicsEntry *>(m_tabs->widget(currentTab));
    if (! entry ) {
      SearchResultsWidget * w = dynamic_cast<SearchResultsWidget *>(m_tabs->widget(currentTab));
      if (w) {
        m_tabs->removeTab(currentTab);
        delete w;
        createTab = true;
      }
    }
  }
  if (createTab) {
    /// turn history on as the user has clicked on something
    /// and the root is not already shown
    entry = new GraphicsEntry(this);
    setSignals(entry);
    entry->installEventFilter(this);
    np = entry->getXmlForRoot(p);
    m_tabs->insertTab(m_tabs->currentIndex()+1,entry,np.getShortText());
    /// TODO decide whether to make new tab the current tab
  }
  else {
    if (entry->hasPlace(p,GraphicsEntry::RootSearch,true) == -1) {
      np = entry->getXmlForRoot(p);
      m_tabs->setTabText(currentTab,np.getShortText());
      // this works but sets it for all tabs
      //m_tabs->setStyleSheet("QTabBar {font-family : Amiri;font-size : 16px}");
      // this sets it for all the items in graphicsentry
      // but not the tab title
      //    w->setStyleSheet("font-family : Amiri;font-size : 16px");
      entry->setFocus();
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
void LanesLexicon::on_actionTest() {
  //  QKeySequenceEdit * w = new QKeySequenceEdit;
  //  w->show();
}
/**
 * TODO tidy up navMode
 *
 */
void LanesLexicon::readSettings() {
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  settings->setIniCodec("UTF-8");
  settings->beginGroup("System");
  m_dbName = settings->value("Database","lexicon.sqlite").toString();
  QString ar = settings->value("Arabic font").toString();
  if (! ar.isEmpty()) {
    arFont.fromString(ar);
  }

  m_saveTabs = settings->value("Save tabs",true).toBool();
  m_restoreTabs = settings->value("Restore tabs",true).toBool();

  m_saveBookmarks = settings->value("Save bookmarks",true).toBool();
  m_restoreBookmarks = settings->value("Restore bookmarks",true).toBool();

  m_docked = settings->value("Docked",false).toBool();
  m_interface = settings->value("Interface","default").toString();
  m_navigationMode = settings->value("Navigation","root").toString();
  m_useNotes = settings->value("Use notes",false).toBool();
  m_activeMap = settings->value("Default map","Buckwalter").toString();
  if (m_navigationMode.toLower() == "root") {
    m_navMode = LanesLexicon::ByRoot;
  }
  else {
    m_navMode = LanesLexicon::ByPage;
  }

  settings->endGroup();

  settings->beginGroup("Search");
  m_searchNewTab = settings->value("New tab",true).toBool();
  m_searchSwitchTab = settings->value("Switch tab",true).toBool();
  settings->endGroup();

  settings->beginGroup("Debug");
  m_valgrind = settings->value("Valgrind",false).toBool();
  settings->endGroup();

  settings->beginGroup("Notes");
  m_notesDbName = settings->value("Database","notes.sqlite").toString();
  settings->endGroup();
  settings->beginGroup("History");
  m_historyEnabled = settings->value("Enabled",true).toBool();
  m_historyDbName = settings->value("Database","history.sqlite").toString();
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
  delete settings;
}
void LanesLexicon::writeSettings() {
  QSettings * settings;
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  settings = app->getSettings();
  settings->setIniCodec("UTF-8");
  /*
  settings->beginGroup("System");
  QDateTime now = QDateTime::currentDateTime();
  settings->setValue("Run date",now);
  settings->setValue("Save Tabs",m_saveTabs);
  settings->setValue("Database",m_dbName);
  settings->setValue("History",m_historyEnabled);
  settings->setValue("Arabic font",arFont.toString());
  settings->setValue("Restore Tabs",m_restoreTabs);
  settings->setValue("Interface",m_interface);
  settings->setValue("Use Notes",m_useNotes);
  settings->setValue("Navigation",m_navigationMode);
  settings->endGroup();
  */
  if (m_saveTabs) {
    settings->beginGroup("Tabs");
    settings->remove("");
    for(int i=0;i < m_tabs->count();i++) {
      GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(i));
      if (entry) {
        Place p = entry->getPlace();
        settings->beginGroup(QString("Tab-%1").arg(i));
        settings->setValue("place",p.toString());
        settings->setValue("zoom",entry->getScale());
        settings->setValue("textwidth",entry->getTextWidth());
        //           QVariant v;
        //   v.setValue(p);

        settings->endGroup();
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
  delete settings;
}
void LanesLexicon::restoreTabs() {
  bool ok;
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();

  settings->setIniCodec("UTF-8");
  settings->beginGroup("System");
  int tab =  settings->value("Focus tab",0).toInt();
  settings->endGroup();
  int textWidth;
  settings->beginGroup("Entry");
  textWidth = settings->value("Text width",400).toInt(&ok);
  if (!ok) {
    textWidth = 400;
  }
  settings->endGroup();
  Place wp;
  settings->beginGroup("Tabs");
  QStringList tabs = settings->childGroups();
  /// restore tab may fail, so we need to keep count of the actual tabs
  int j = 0;
  for(int i=0;i < tabs.size();i++) {
    settings->beginGroup(tabs[i]);
    Place p;
    QString v = settings->value("place",QString()).toString();

    qreal scale = settings->value("zoom",1.0).toReal(&ok);

    if ( !ok ) {
      scale = 1.0;
    }
    if (! v.isNull()) {
      p = Place::fromString(v);
    }
    int tw = settings->value("textwidth",textWidth).toInt(&ok);
    if (!ok) {
      tw = textWidth;
    }
    if (p.isValid()) {
      if (tab == i) {
        tab = j;
        wp = p;
      }
      GraphicsEntry * entry = new GraphicsEntry(this);
      entry->setTextWidth(tw);
      setSignals(entry);
      p.setType(Place::RestoreTab);

      if (p.getPageMode()) {
        entry->setPagingForward();
        entry->getPage(p);
      }
      else {
        entry->getXmlForRoot(p);
      }
      entry->setScale(scale);
      //      qDebug() << Q_FUNC_INFO << "adding tab" << p.getShortText();
      m_tabs->addTab(entry,p.getShortText());
      m_tree->ensurePlaceVisible(p,true);
      j++;
    }
    settings->endGroup();
  }
  settings->endGroup();
  wp.setType(Place::RestoreTab);
  if ((tab >=0) && (tab << m_tabs->count())) {
    m_tabs->setCurrentIndex(tab);
  }
  delete settings;
}

HistoryMaster * LanesLexicon::history() {
  return m_history;
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
    entry->getXmlForRoot(Place(nroot));
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
void LanesLexicon::on_actionNextRoot() {
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
void LanesLexicon::on_actionPrevRoot() {
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
void LanesLexicon::on_actionFirstRoot() {
  Place p;
  p.setRoot(m_firstRoot);
  showPlace(p,false);
  m_tree->ensurePlaceVisible(p);
}
void LanesLexicon::on_actionLastRoot() {
  Place p;
  p.setRoot(m_lastRoot);
  showPlace(p,false);
  m_tree->ensureVisible(m_lastRoot);
}
void LanesLexicon::rootChanged(const QString & root,const QString & node) {
  m_tree->ensureVisible(root,true);
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
  bool ok = query.prepare("select root,page from entry order by nodenum limit 5;");
  if (! ok ) {
    QLOG_DEBUG() << "first root SQL prepare failed";
  }
  query.exec();
  if (query.first()) {
    m_firstRoot = query.value(0).toString();
    m_firstPage = query.value(1).toInt();
  }
  ok = query.prepare("select root,page from entry order by nodenum desc limit 5;");
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
void LanesLexicon::on_actionNextPage() {
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
void LanesLexicon::on_actionPrevPage() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    Place p = entry->getPlace();
    int page = p.getPage();
    if (page < 0) {
      page = 1;
    }
    if (page == 1) {
      QLOG_INFO() << "At first page";
      return;
    }
    p.setPage(page - 1);
    entry->setPagingForward();
    entry->getPage(p);
    //        m_tree->ensurePlaceVisible(np,true);
  }
}
void LanesLexicon::on_actionFirstPage() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    entry->setPagingForward();
    Place p;
    p.setPage(m_firstPage);
    entry->getPage(p);
    //        m_tree->ensurePlaceVisible(np,true);
  }
}
void LanesLexicon::on_actionLastPage() {
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
 *
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
  if (m_revertEnabled && (key == "revert")) {
    if (! m_bookmarks.contains("-here-")) {
      return;
    }
    Place p = m_bookmarks.value("-here-");
    showPlace(p,false);
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
  showPlace(p,false);
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
    QString ks = QString("%1,%2").arg(key).arg(ids.at(i));
    QShortcut * sc = new QShortcut(ks,this);
    sc->setContext(Qt::ApplicationShortcut);
    connect(sc,SIGNAL(activated()),m_bookmarkMap,SLOT(map()));
    m_bookmarkMap->setMapping(sc,QString("add-%1").arg(ids.at(i)));
  }
  m_bookmarkAddAction = new QAction(tr("Add"),this);

  key = settings->value("Jump","Ctrl+J").toString();
  for(int i=0;i < ids.size();i++) {
    QString ks = QString("%1,%2").arg(key).arg(ids.at(i));
    QShortcut * sc = new QShortcut(ks,this);
    connect(sc,SIGNAL(activated()),m_bookmarkMap,SLOT(map()));
    m_bookmarkMap->setMapping(sc,QString("jump-%1").arg(ids.at(i)));
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
  QString text = QInputDialog::getText(this, tr("Bookmark Add)"),
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
}
void LanesLexicon::bookmarkRebuildMenu() {
  qDebug() << Q_FUNC_INFO;
}
void LanesLexicon::bookmarkAdd(const QString & id,const Place & p) {
  //  p.setType(Place::Bookmark);
  m_bookmarks.insert(id,p);
  addBookmarkMenuItem(id);

}

void LanesLexicon::moveNext(const Place & p) {
  if (m_navMode == 0) {
    findNextRoot(p.getRoot());
  }
  else {
    on_actionNextPage();
  }
}
void LanesLexicon::movePrevious(const Place & p) {
  if (m_navMode == 0) {
    findPrevRoot(p.getRoot());
  }
  else {
    on_actionPrevPage();
  }
}
void LanesLexicon::updateStatusBar() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    m_placeIndicator->setText(entry->getPlace().getText());
  }
  else {
    m_placeIndicator->setText("");

  }
  if (m_navMode == 0) {
    m_navModeIndicator->setText(tr("Nav mode: by root"));
    m_navText->setText(tr("Root"));
  }
  else {
    m_navModeIndicator->setText(tr("Nav mode: by page"));
    m_navText->setText(tr("Page"));
  }

}
void LanesLexicon::updateMenu() {
  if (m_navMode == LanesLexicon::ByRoot) {
    m_navModeRootAction->setChecked(true);
  }
  else {
    m_navModeRootAction->setChecked(false);
  }
  if (m_navMode == LanesLexicon::ByPage) {
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
void LanesLexicon::on_actionNavNext() {
  if (m_navMode == 0) {
    on_actionNextRoot();
  }
  else {
    on_actionNextPage();
  }
}
void LanesLexicon::on_actionNavPrev()  {
  if (m_navMode == 0) {
    on_actionPrevRoot();
  }
  else {
    on_actionPrevPage();
  }
}
void LanesLexicon::on_actionNavFirst()   {
  if (m_navMode == 0) {
    on_actionFirstRoot();
  }
  else {
    on_actionFirstPage();
  }
}
void LanesLexicon::on_actionNavLast()   {
  if (m_navMode == 0) {
    on_actionLastRoot();
  }
  else {
    on_actionLastPage();
  }
}
void LanesLexicon::onNavModeChanged() {
  qDebug() << Q_FUNC_INFO;
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
void LanesLexicon::on_actionClearHistory() {
  qDebug() << Q_FUNC_INFO <<  m_history->clear();
  setupHistory();
}
void LanesLexicon::on_actionDocs() {
  HelpWidget * w = new HelpWidget(this);
   m_tabs->setCurrentIndex(m_tabs->addTab(w,"Docs"));
   return;
}
void LanesLexicon::testSlot() {
  qDebug() << Q_FUNC_INFO;
}
void LanesLexicon::searchForPage() {
    bool ok;
    /// not sure about putting max/mins
    int page = QInputDialog::getInt(this, tr("Page Search"),
                                    tr("Page:"), -1,1,3016, 1,&ok);
    if (ok && (page != -1)) {
      Place p;
      p.setPage(page);
      this->onGoToPage(p);
    }
}
void LanesLexicon::searchForRoot() {
    SearchDialog * d = new SearchDialog(this);
    d->setWindowTitle(tr("Search for Root"));
    d->setPrompt(tr("Find root"));
    d->setNewTab(m_searchNewTab);
    if (d->exec()) {
      QString t = d->getText();
      if (! t.isEmpty()) {
        if (! UcdScripts::isScript(t,"Arabic")) {
          t = convertString(t);
        }
        Place p = showPlace(Place(t),d->getNewTab());
        if (! p.isValid()) {
          QMessageBox msgBox;
          msgBox.setText(QString(tr("%1 not found")).arg(t));
          msgBox.exec();
        }
      }
    }
    delete d;
}
void LanesLexicon::searchForWord() {
    SearchDialog * d = new SearchDialog(this);
    d->setWindowTitle(tr("Search for Word"));
    d->setPrompt(tr("Find word"));
    if (d->exec()) {
      QString t = d->getText();
      if (! t.isEmpty()) {
        if (! UcdScripts::isScript(t,"Arabic")) {
          t = convertString(t);
        }
        SearchResultsWidget * s = new SearchResultsWidget(t,this);
        if (s->count() == 0) {
          QMessageBox msgBox;
          msgBox.setText(QString(tr("%1 not found")).arg(t));
          msgBox.exec();
          delete s;
        }
        else {
          int i = m_tabs->insertTab(m_tabs->currentIndex()+1,s,t);
          m_tabs->setCurrentIndex(i);
          setSignals(s->getEntry());
        }
      }
    }
    delete d;
}
void LanesLexicon::searchForNode() {
    NodeSearchDialog * d = new NodeSearchDialog(this);
    d->setNewTab(m_searchNewTab);
    if (d->exec()) {
      QString t = d->getText();
      if (! t.isEmpty()) {
        /// TODO make this an option to always show root
        /// show only node
        //        Place p = showNode(t,true);
        Place p;
        p.setNode(t);
        p = showPlace(p,d->getNewTab());
        if (! p.isValid()) {
          QMessageBox msgBox;
          msgBox.setText(QString(tr("%1 not found")).arg(t));
          msgBox.exec();
        }
      }
    }
    delete d;
}
