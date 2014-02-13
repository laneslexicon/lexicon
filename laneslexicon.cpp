#include "laneslexicon.h"
extern cmdOptions progOptions;
LanesLexicon::LanesLexicon(QWidget *parent) :
    QMainWindow(parent)

{
  m_ok = false;
  m_history = 0;
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
  m_history = new HistoryMaster(m_notesDbName);
  m_history->setEnabled(m_historyEnabled);
  m_pwidget = new PlaceWidget(this);
  //  m_notes->setObjectName("notes");
  if (m_docked) {
    m_treeDock = new QDockWidget("Contents",this);
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
    splitter->setStretchFactor(1,1);
    //  m_tabs->addTab(m_notes,"Notes");
    setCentralWidget(splitter);
  }
  createActions();
  createToolBar();
  createMenus();
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

  setupShortcuts();
  connect(m_tree,SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)),this,SLOT(rootClicked(QTreeWidgetItem *,int)));

  connect(m_tree,SIGNAL(itemActivated(QTreeWidgetItem *,int)),this,SLOT(rootClicked(QTreeWidgetItem *,int)));
  connect(m_tabs,SIGNAL(tabCloseRequested(int)),this,SLOT(onCloseTab(int)));


  if ( m_useNotes ) {
  connect(this,SIGNAL(nodeActivated(const QString & ,const QString & )),
          m_notes,SLOT(setActiveNode(const QString & ,const QString & )));
  connect(m_notesBtn,SIGNAL(clicked()),this,SLOT(onNotesClicked()));
  }


  /// TOTDO replace this last visited item
  if (m_restoreTabs) {
    restoreTabs();
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
  m_tree->resizeColumnToContents(0);

  if (m_historyEnabled) {
    setupHistory();
  }


  setupInterface();
}

LanesLexicon::~LanesLexicon()
{
  QLOG_DEBUG() << "main window destructor";
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
  freeXslt();
  QLOG_DEBUG() << "finished main window destructor";
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
  connect(entry,SIGNAL(historyPositionChanged(int)),this,SLOT(historyPositionChanged(int)));
  connect(entry,SIGNAL(historyAddition()),this,SLOT(historyAddition()));

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
  qDebug() << Q_FUNC_INFO << k;
  if (k == "Root Search") {
    RootSearchDialog * d = new RootSearchDialog(this);
    if (d->exec()) {
      QString t = d->getText();
      if (! t.isEmpty()) {
        Place p = showPlace(Place(t),false);
        if (! p.isValid()) {
          QMessageBox msgBox;
          msgBox.setText(QString(tr("%1 not found")).arg(t));
          msgBox.exec();
        }
      }
    }
    delete d;
  }
  else if (k == "Node Search") {
    NodeSearchDialog * d = new NodeSearchDialog(this);
    if (d->exec()) {
      QString t = d->getText();
      if (! t.isEmpty()) {
        /// TODO make this an option to always show root
        /// show only node
        Place p = showNode(t,true);
        if (! p.isValid()) {
          QMessageBox msgBox;
          msgBox.setText(QString(tr("%1 not found")).arg(t));
          msgBox.exec();
        }
      }
    }
    delete d;
  }
  else if (k == "Word Search") {
    WordSearchDialog * d = new WordSearchDialog(this);
    if (d->exec()) {
      QString t = d->getText();
      if (! t.isEmpty()) {
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
  else if (k == "Page Search") {
    bool ok;
    /// not sure about putting max/mins
    int page = QInputDialog::getInt(this, tr("Page Search"),
                                    tr("Page:"), -1,1,3016, 1,&ok);
    if (ok && (page != -1)) {
      this->onGoToPage(page);
    }
  }
  else if (k == "Contents Collapse All") {
    m_tree->collapseAll();
  }
  else if (k == "Contents Collapse Letter") {
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
  else if (k == "Quit") {
    on_actionExit();
  }
  else if (k == "Toggle Interface") {
    if (m_interface == "minimal") {
      m_interface = "default";
    }
    else {
      m_interface = "minimal";
    }
    setupInterface();
  }
  else if (k == "Contents Show") {
    if (m_treeDock->isVisible()) {
      m_treeDock->hide();
    }
    else {
      m_treeDock->show();
    }
  }
  else if (k == "Root Next") {
    this->on_actionNextRoot();
  }
  else if (k == "Root Prev") {
    this->on_actionPrevRoot();
  }
  else if (k == "Root First") {
    this->on_actionFirstRoot();
  }
  else if (k == "Root Last") {
    this->on_actionLastRoot();
  }
  else if (k == "Page Next") {
    this->on_actionNextPage();
  }
  else if (k == "Page Prev") {
    this->on_actionPrevPage();
  }
  else if (k == "Page First") {
    this->on_actionFirstPage();
  }
  else if (k == "Page Last") {
    this->on_actionLastPage();
  }
  else if (k == "Focus Content") {
    m_tabs->currentWidget()->setFocus();
  }
  else if (k == "Focus Tree") {
    m_tree->setFocus();
  }
  else if (k.startsWith("Go Tab")) {
    QString nstr = k.right(1);
    bool ok;
    int ix = nstr.toInt(&ok);
    if (ok) {
      ix--;
      if ((ix >= 0) && (ix < m_tabs->count())) {
        m_tabs->setCurrentIndex(ix);
      }
    }
  }
  else {
    QLOG_WARN() << "Unhandled shortcut" << k;
  }

  //qDebug() << qobject_cast<QShortcut *>(m_signalMapper->mapping(k));
}
/**
 * setup the shortcuts from the conf
 *
 */
void LanesLexicon::setupShortcuts() {
  QSettings settings;
  settings.beginGroup("Shortcut");
  m_signalMapper = new QSignalMapper(this);
  QStringList keys = settings.childKeys();
  for(int i=0;i < keys.size();i++) {
    if (keys[i] == "Go Tab") {
      for(int j=1;j < 10;j++) {
        QString ks = QString("%1,%2").arg(settings.value(keys[i]).toString()).arg(j);
        QShortcut * sc = new QShortcut(ks,this);
        connect(sc,SIGNAL(activated()),m_signalMapper,SLOT(map()));
        /// doesn't seem to be emitted
        connect(sc,SIGNAL(activatedAmbiguously()),this,SLOT(ambiguousShortcut()));
        m_signalMapper->setMapping(sc,QString("%1-%2").arg(keys[i]).arg(j));
      }
    }
    else {
      QString k = settings.value(keys[i]).toString();
      QShortcut * sc = new QShortcut(k,this);
      connect(sc,SIGNAL(activated()),m_signalMapper,SLOT(map()));
      /// doesn't seem to be emitted
      connect(sc,SIGNAL(activatedAmbiguously()),this,SLOT(ambiguousShortcut()));
      /// use the setting name as the shortcut 'name'
      m_signalMapper->setMapping(sc,keys[i]);
    }
  }
  connect(m_signalMapper,SIGNAL(mapped(QString)),this,SLOT(shortcut(const QString &)));
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
void LanesLexicon::createActions() {
  /// TODO get this from QSettings
  QString imgdir = QString("./images/32");

  m_exitAction = new QAction(tr("Exit"),this);
  connect(m_exitAction,SIGNAL(triggered()),this,SLOT(on_actionExit()));

  m_testAction = new QAction(tr("Test"),this);
  connect(m_testAction,SIGNAL(triggered()),this,SLOT(on_actionTest()));
  /// probably need icons

  m_hBackward = new QAction(QIcon(imgdir + "/go-previous.png"),tr("Back"),this);


  connect(m_hBackward,SIGNAL(triggered()),this,SLOT(onHistoryBackward()));
  //    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
  m_rootForwardAction = new QAction(QIcon(imgdir + "/go-next.png"),tr("Next Root"),this);
  m_rootBackwardAction = new QAction(QIcon(imgdir + "/go-previous.png"),tr("Prev Root"),this);
  m_rootFirstAction = new QAction(QIcon(imgdir + "/go-first.png"),tr("First Root"),this);
  m_rootLastAction = new QAction(QIcon(imgdir + "/go-last.png"),tr("Last Root"),this);

  connect(m_rootForwardAction,SIGNAL(triggered()),this,SLOT(on_actionNextRoot()));
  connect(m_rootBackwardAction,SIGNAL(triggered()),this,SLOT(on_actionPrevRoot()));
  connect(m_rootFirstAction,SIGNAL(triggered()),this,SLOT(on_actionFirstRoot()));
  connect(m_rootLastAction,SIGNAL(triggered()),this,SLOT(on_actionLastRoot()));

  m_pageForwardAction = new QAction(QIcon(imgdir + "/go-next.png"),tr("Next Page"),this);
  m_pageBackwardAction = new QAction(QIcon(imgdir + "/go-previous.png"),tr("Prev Page"),this);
  m_pageFirstAction = new QAction(QIcon(imgdir + "/go-first.png"),tr("First Page"),this);
  m_pageLastAction = new QAction(QIcon(imgdir + "/go-last.png"),tr("Last Page"),this);

  connect(m_pageForwardAction,SIGNAL(triggered()),this,SLOT(on_actionNextPage()));
  connect(m_pageBackwardAction,SIGNAL(triggered()),this,SLOT(on_actionPrevPage()));
  connect(m_pageFirstAction,SIGNAL(triggered()),this,SLOT(on_actionFirstPage()));
  connect(m_pageLastAction,SIGNAL(triggered()),this,SLOT(on_actionLastPage()));
}
void LanesLexicon::createToolBar() {
  m_fileToolBar = addToolBar(tr("&File"));
  m_fileToolBar->addAction(m_exitAction);
  m_fileToolBar->addAction(m_testAction);

  QToolBar * history = addToolBar(tr("History"));




  m_hBackwardBtn = new QToolButton(history);
  m_hBackwardBtn->setText("Back");
  m_hBackwardBtn->setDefaultAction(m_hBackward);
  m_hBackwardBtn->setPopupMode(QToolButton::MenuButtonPopup);
  m_hBackwardBtn->setEnabled(false);


  history->addWidget(m_hBackwardBtn);

  history->addSeparator();
  QToolBar * navigation = addToolBar(tr("Root Navigation"));
  navigation->addWidget(new QLabel("Navigate<br/>by root"));
  navigation->addAction(m_rootFirstAction);
  navigation->addAction(m_rootForwardAction);
  navigation->addAction(m_rootBackwardAction);
  navigation->addAction(m_rootLastAction);
  navigation->addSeparator();
  navigation->setFloatable(true);
  navigation = addToolBar(tr("Page Navigation"));
  navigation->addWidget(new QLabel("Navigate<br/>by page"));
  navigation->addAction(m_pageFirstAction);
  navigation->addAction(m_pageForwardAction);
  navigation->addAction(m_pageBackwardAction);
  navigation->addAction(m_pageLastAction);
  navigation->addSeparator();
  navigation->setFloatable(true);

  QToolBar * place = addToolBar("Place");
  place->addWidget(m_pwidget);
}
/**
 * when user has done something that adds to history
 * rebuild the dropdown, unchecking everything
 *
 */
void LanesLexicon::historyAddition() {
  setupHistory(-1);
}
void LanesLexicon::historyPositionChanged(int pos) {
  qDebug() << Q_FUNC_INFO << pos;
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
    m_hBackwardBtn->setEnabled(true);
  }
  else {
    QMenu * m = new QMenu;
    m->setObjectName("history");
    QActionGroup * group = new QActionGroup(this);
    while(events.size() > 0) {
      HistoryEvent * event = events.takeFirst();
      QString root = event->getRoot();
      QString word = event->getWord();
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
      Place p = event->getPlace();
      //      m_historyPos = p.getId();
      action->setData(QVariant(p));//event->getId());
      connect(action,SIGNAL(triggered()),this,SLOT(onHistorySelection()));
      delete event;
    }
    m->addActions(group->actions());
    //    m->addActions(group);
    m_hBackwardBtn->setEnabled(true);
    //    m_hBackwardBtn->addActions(group->actions());//setMenu(m);
    m_hBackwardBtn->setMenu(m);
  }
  if (currPos  == -1) {
    return;
  }
  return;
}
void LanesLexicon::createMenus() {
  m_fileMenu = menuBar()->addMenu(tr("&File"));
  m_fileMenu->addAction(m_exitAction);
}
void LanesLexicon::createStatusBar() {
  QWidget * w = new QWidget;
  QHBoxLayout * layout = new QHBoxLayout;
  layout->addWidget(new QLabel(tr("Current root:")));
  m_currentRoot = new QLabel("None");
  m_currentRoot->setObjectName("currentRoot");
  layout->addWidget(m_currentRoot);
  w->setLayout(layout);
  statusBar()->insertPermanentWidget(0,w,0);
  if (m_useNotes) {
    m_notesBtn = new QPushButton(tr("Notes"));
    m_notesBtn->setEnabled(true);
    statusBar()->insertPermanentWidget(0,m_notesBtn,0);
  }
  statusBar()->showMessage(tr("Ready"));
}
QSize LanesLexicon::sizeHint() const {
  return QSize(800,600);
}
void LanesLexicon::onHistorySelection() {
  QAction * action = static_cast<QAction *>(QObject::sender());
  QVariant v = action->data();
  Place p = v.value<Place>();
  p.setType(Place::History);
  qDebug() << "history change place" << p;
  showPlace(p,false);
  /*
  m_historyPos = action->data().toInt();
  HistoryEvent * event = m_history->getEvent(m_historyPos);
  setupHistory();
  Place p;
  p.setNode(event->getNode());
  p.setRoot(event->getRoot());
  GraphicsEntry * w = dynamic_cast<GraphicsEntry *>(m_tabs->widget(0));
  /// TODO fix for supplement
  w->getXmlForRoot(p);
  m_tabs->setTabText(0,p.getRoot());
  w->setFocus();
  */
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
  showPlace(m,newTab);
}
/// TODO is this redundant ?
Place LanesLexicon::showNode(const QString & node,bool nodeOnly,bool createTab) {
  Place p;
  p.setNode(node);
  /// TODO make this come from QSettings
  p.setNodeOnly(false);
  int currentTab = m_tabs->currentIndex();
  if (currentTab == -1) {
    createTab = true;
  }
 if (createTab) {
    /// turn history on as the user has clicked on something
    /// and the root is not already shown
    GraphicsEntry * w = new GraphicsEntry(this);
    setSignals(w);
    w->installEventFilter(this);
    if (w->hasPlace(p,GraphicsEntry::NodeSearch,true) == -1) {
      m_history->on();
      m_tabs->insertTab(m_tabs->currentIndex()+1,w,node);
      //      p = w->getXmlForNode(node,nodeOnly);
      p = w->getXmlForRoot(p);
      if (p.isValid()) {
        /// set focus
        p.setNode(node);
        w->hasPlace(p,GraphicsEntry::NodeSearch,true);
      }
      //      connect(w,SIGNAL(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)),
      //              this,SLOT(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)));

    }
  }
  else {
    GraphicsEntry * w = dynamic_cast<GraphicsEntry *>(m_tabs->widget(currentTab));
    if (w->hasPlace(p,GraphicsEntry::NodeSearch,true) == -1) {
      m_history->on();
      //      p = w->getXmlForNode(node,nodeOnly);
      p = w->getXmlForRoot(p);
      m_tabs->setTabText(0,p.getNode());
      if (p.isValid()) {
        /// set focus
        p.setNode(node);
        w->hasPlace(p,GraphicsEntry::NodeSearch,true);
      }
      // this works but sets it for all tabs
      //m_tabs->setStyleSheet("QTabBar {font-family : Amiri;font-size : 16px}");
      // this sets it for all the items in graphicsentry
      // but not the tab title
      //    w->setStyleSheet("font-family : Amiri;font-size : 16px");
      w->setFocus();
    }
  }
 if (p.isValid()) {
   if (m_place.getRoot() != p.getRoot()) {
     placeChanged(p);
   }
   m_place = p;
 }
 return p;
}
Place LanesLexicon::showPlace(const Place & p,bool createTab) {
  Place np;
  QString root = p.getRoot();

  if (! p.isValid()) {
    return np;
  }
  int currentTab = m_tabs->currentIndex();
  if (currentTab == -1) {
    createTab = true;
  }
  qDebug() << Q_FUNC_INFO << currentTab << createTab;
 if (createTab) {
    /// turn history on as the user has clicked on something
    /// and the root is not already shown
    GraphicsEntry * w = new GraphicsEntry(this);
    setSignals(w);
    w->installEventFilter(this);
    if (w->hasPlace(p,GraphicsEntry::RootSearch,true) == -1) {
      m_tabs->insertTab(m_tabs->currentIndex()+1,w,root);
      np = w->getXmlForRoot(p);
      /// TODO decide whether to make new tab the current tab
      //      connect(w,SIGNAL(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)),
      //              this,SLOT(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)));

    }
  }
  else {
    GraphicsEntry * w = dynamic_cast<GraphicsEntry *>(m_tabs->widget(currentTab));
    if (w->hasPlace(p,GraphicsEntry::RootSearch,true) == -1) {
      np = w->getXmlForRoot(p);
      m_tabs->setTabText(currentTab,root);
      // this works but sets it for all tabs
      //m_tabs->setStyleSheet("QTabBar {font-family : Amiri;font-size : 16px}");
      // this sets it for all the items in graphicsentry
      // but not the tab title
      //    w->setStyleSheet("font-family : Amiri;font-size : 16px");
      w->setFocus();
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
 */
void LanesLexicon::focusItemChanged(QGraphicsItem * newFocus, QGraphicsItem * oldFocus, Qt::FocusReason reason) {

  EntryItem * item = dynamic_cast<EntryItem *>(newFocus);
  if (item) {
    m_pwidget->setPlace(item->getPlace());
    QString node = item->getNode();
    QString word = item->getWord();
    QString root = item->getRoot();
    QPointF p = item->pos();
    if (! node.isEmpty()) {
      QString txt = QString("%1  %2  At:%3,%4").arg(node).arg(word).arg(p.x()).arg(p.y());
      statusBar()->showMessage(txt);
      if (m_useNotes) {
        m_notesBtn->setEnabled(true);
        emit(nodeActivated(item->getNode(),item->getWord()));
      }
    }
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
void LanesLexicon::readSettings() {
  QSettings settings;
  settings.setIniCodec("UTF-8");
  settings.beginGroup("System");
  m_dbName = settings.value("Database","lexicon.sqlite").toString();
  QString ar = settings.value("Arabic font").toString();
  if (! ar.isEmpty()) {
    arFont.fromString(ar);
  }

  m_saveTabs = settings.value("Save Tabs",true).toBool();
  m_restoreTabs = settings.value("Restore Tabs",true).toBool();
  m_docked = settings.value("Docked",false).toBool();
  m_interface = settings.value("Interface","default").toString();
  m_navigationMode = settings.value("Navigation","root").toString();
  m_useNotes = settings.value("Use Notes",false).toBool();
  settings.endGroup();

  settings.beginGroup("Debug");
  m_valgrind = settings.value("Valgrind",false).toBool();
  settings.endGroup();

  settings.beginGroup("Notes");
  m_notesDbName = settings.value("Database","notes.sqlite").toString();
  settings.endGroup();
  settings.beginGroup("History");
  m_historyEnabled = settings.value("Enabled",true).toBool();
  settings.endGroup();
}
void LanesLexicon::writeSettings() {
  QSettings settings;
  settings.setIniCodec("UTF-8");
  /*
  settings.beginGroup("System");
  QDateTime now = QDateTime::currentDateTime();
  settings.setValue("Run date",now);
  settings.setValue("Save Tabs",m_saveTabs);
  settings.setValue("Database",m_dbName);
  settings.setValue("History",m_historyEnabled);
  settings.setValue("Arabic font",arFont.toString());
  settings.setValue("Restore Tabs",m_restoreTabs);
  settings.setValue("Interface",m_interface);
  settings.setValue("Use Notes",m_useNotes);
  settings.setValue("Navigation",m_navigationMode);
  settings.endGroup();
  */
  if (m_saveTabs) {
    settings.beginGroup("Tabs");
    settings.remove("");
    for(int i=0;i < m_tabs->count();i++) {
      GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(i));
      if (entry) {
        Place p = entry->getPlace();
        settings.beginGroup(QString("Tab-%1").arg(i));
        settings.setValue("root",p.getRoot());
        settings.setValue("word",p.getWord());
        settings.setValue("node",p.getNode());
        settings.setValue("supplement",p.getSupplement());
        //        settings.setValue("nodeOnly",p.getNodeOnly());
        settings.setValue("page",p.getPage());
        settings.endGroup();
      }
    }
    settings.endGroup();
    settings.beginGroup("System");
    settings.setValue("Focus tab",m_tabs->currentIndex());
    settings.endGroup();
  }
}
void LanesLexicon::restoreTabs() {
  QSettings settings;
  settings.setIniCodec("UTF-8");
  settings.beginGroup("System");
  int tab =  settings.value("Focus tab",0).toInt();
  settings.endGroup();
  Place wp;
  settings.beginGroup("Tabs");
  QStringList tabs = settings.childGroups();
  for(int i=0;i < tabs.size();i++) {
    settings.beginGroup(tabs[i]);
    Place p;
    p.setNode(settings.value("node",QString()).toString());
    p.setRoot(settings.value("root",QString()).toString());
    p.setSupplement(settings.value("supplement").toInt());
    p.setWord(settings.value("word",QString()).toString());
    //    p.setNodeOnly(settings.value("nodeOnly",false).toBool());
    if (p.getNode().isEmpty()) {
      //      p.setNodeOnly(false);
    }
    if (tab == i) {
      wp = p;
    }
    GraphicsEntry * entry = new GraphicsEntry(this);
    setSignals(entry);
    p.setType(Place::RestoreTab);
    p = entry->getXmlForRoot(p);
    m_tabs->addTab(entry,p.getRoot());
    m_tree->ensurePlaceVisible(p,true);
    settings.endGroup();
  }
  settings.endGroup();
  wp.setType(Place::RestoreTab);
  if ((tab >=0) && (tab << m_tabs->count())) {
    m_tabs->setCurrentIndex(tab);
    m_pwidget->setPlace(wp);
  }

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
  qDebug() << Q_FUNC_INFO  << root;
  QString nroot = m_tree->findNextRoot(root);
  if (entry && ! nroot.isEmpty()) {
    entry->setPagingForward();
    entry->getXmlForRoot(Place(nroot));
  }
}
void LanesLexicon::findPrevRoot(const QString & root) {
  GraphicsEntry * entry = dynamic_cast<GraphicsEntry *>(QObject::sender());
  qDebug() << Q_FUNC_INFO  << root;
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
  qDebug() << Q_FUNC_INFO;
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    //    QString root =  entry->currentRoot();
    //    QString nroot = m_tree->findNextRoot(root);
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
    //    QString root =  entry->currentRoot();
    //    QString nroot = m_tree->findPrevRoot(root);
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
  m_currentRoot->setText(root);
  m_tree->ensureVisible(root,true);
}
void LanesLexicon::placeChanged(const Place & p) {
  qDebug() << Q_FUNC_INFO << p.getRoot();
  m_currentRoot->setText(p.getRoot());
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
  qDebug() << "First" << m_firstRoot << "last" << m_lastRoot;
  qDebug() << "First page" << m_firstPage << "last page" << m_lastPage;
}
void LanesLexicon::shortcutActivated() {
  QShortcut * sc = qobject_cast<QShortcut *>(QObject::sender());
  qDebug() << "got the fucking shortcut" << sc->key().toString();
}
void LanesLexicon::ambiguousShortcut() {
  QShortcut * sc = qobject_cast<QShortcut *>(QObject::sender());
  qDebug() << "got the ambiguous shortcut" << sc->key().toString();
}
void LanesLexicon::onGoToPage(int page) {
  qDebug() << Q_FUNC_INFO;
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    entry->setPagingForward();
    entry->getPage(page);
    //        m_tree->ensurePlaceVisible(np,true);
  }
}
void LanesLexicon::on_actionNextPage() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    int page = entry->getPlace().getPage();
    if (page == -1) {
      page = 0;
    }
    page++;
    entry->setPagingForward();
    entry->getPage(page);
    //        m_tree->ensurePlaceVisible(np,true);
  }
}
void LanesLexicon::on_actionPrevPage() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    int page = entry->getPlace().getPage();
    if (page < 0) {
      page = 1;
    }
    if (page == 1) {
      QLOG_INFO() << "At first page";
      return;
    }
    page--;
    entry->setPagingForward();
    entry->getPage(page);
    //        m_tree->ensurePlaceVisible(np,true);
  }
}
void LanesLexicon::on_actionFirstPage() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    entry->setPagingForward();
    entry->getPage(m_firstPage);
    //        m_tree->ensurePlaceVisible(np,true);
  }
}
void LanesLexicon::on_actionLastPage() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->currentWidget());
  if (entry) {
    entry->setPagingForward();
    entry->getPage(m_lastPage);
    //        m_tree->ensurePlaceVisible(np,true);
  }
}
bool LanesLexicon::sanityCheck() {
  /// TODO check database exists and entry stylesheet
}
