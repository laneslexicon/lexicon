#include "laneslexicon.h"
LanesLexicon::LanesLexicon(QWidget *parent) :
    QMainWindow(parent)

{
  readSettings();

  openDatabase("lexicon.sqlite");
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
    }
    /// at the end of the history, but we should be able to restore from settings
    m_historyPos = 9;
    m_history = new HistoryMaster(m_notesDbName);
    m_history->setEnabled(m_historyEnabled);
    setupHistory();
  }
    else {
      statusBar()->showMessage(tr("Failed to open database"));
    }

  setupShortcuts();
  connect(m_tree,SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)),this,SLOT(rootClicked(QTreeWidgetItem *,int)));

  connect(m_tree,SIGNAL(itemActivated(QTreeWidgetItem *,int)),this,SLOT(rootClicked(QTreeWidgetItem *,int)));


  if ( m_useNotes ) {
  connect(this,SIGNAL(nodeActivated(const QString & ,const QString & )),
          m_notes,SLOT(setActiveNode(const QString & ,const QString & )));
  connect(m_notesBtn,SIGNAL(clicked()),this,SLOT(onNotesClicked()));
  }
  connect(m_tabs,SIGNAL(tabCloseRequested(int)),this,SLOT(onCloseTab(int)));
  /// TOTDO replace this last visited item
  if (m_restoreTabs) {
    restoreTabs();
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
  setupInterface();
}

LanesLexicon::~LanesLexicon()
{
  QLOG_DEBUG() << "main window destructor";
  /// TODO make this check something has changed or give
  /// option to save settings ?
  writeSettings();
  if (m_history) {
    delete m_history;
  }
  if (m_db.isOpen()) {
    m_db.close();
  }
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
    QString k = settings.value(keys[i]).toString();
    QShortcut * sc = new QShortcut(k,this);
    connect(sc,SIGNAL(activated()),m_signalMapper,SLOT(map()));
    /// doesn't seem to be emitted
    connect(sc,SIGNAL(activatedAmbiguously()),this,SLOT(ambiguousShortcut()));
    m_signalMapper->setMapping(sc,keys[i]);
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
  m_exitAction = new QAction(tr("Exit"),this);
  connect(m_exitAction,SIGNAL(triggered()),this,SLOT(on_actionExit()));

  m_testAction = new QAction(tr("Test"),this);
  connect(m_testAction,SIGNAL(triggered()),this,SLOT(on_actionTest()));
  /// probably need icons
  m_hForward = new QAction(tr("Forward"),this);
  m_hBackward = new QAction(tr("Backard"),this);

  connect(m_hForward,SIGNAL(triggered()),this,SLOT(onHistoryForward()));
  connect(m_hBackward,SIGNAL(triggered()),this,SLOT(onHistoryBackward()));
  /// TODO get this from QSettings
  QString imgdir = QString("./images/32");
  //    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
  m_rootForwardAction = new QAction(QIcon(imgdir + "/go-next.png"),tr("Next Root"),this);
  m_rootBackwardAction = new QAction(QIcon(imgdir + "/go-previous.png"),tr("Prev Root"),this);
  m_rootFirstAction = new QAction(QIcon(imgdir + "/go-first.png"),tr("First Root"),this);
  m_rootLastAction = new QAction(QIcon(imgdir + "/go-last.png"),tr("Last Root"),this);

  connect(m_rootForwardAction,SIGNAL(triggered()),this,SLOT(on_actionNextRoot()));
  connect(m_rootBackwardAction,SIGNAL(triggered()),this,SLOT(on_actionPrevRoot()));
  connect(m_rootFirstAction,SIGNAL(triggered()),this,SLOT(on_actionFirstRoot()));
  connect(m_rootLastAction,SIGNAL(triggered()),this,SLOT(on_actionLastRoot()));
}
void LanesLexicon::createToolBar() {
  m_fileToolBar = addToolBar(tr("&File"));
  m_fileToolBar->addAction(m_exitAction);
  m_fileToolBar->addAction(m_testAction);

  QToolBar * history = addToolBar(tr("History"));

  m_hForwardBtn = new QToolButton(history);
  m_hForwardBtn->setText("Forward");
  m_hForwardBtn->setDefaultAction(m_hForward);
  m_hForwardBtn->setPopupMode(QToolButton::MenuButtonPopup);
  m_hForwardBtn->setEnabled(false);
  m_hBackwardBtn = new QToolButton(history);
  m_hBackwardBtn->setText("Back");
  m_hBackwardBtn->setDefaultAction(m_hBackward);
  m_hBackwardBtn->setPopupMode(QToolButton::MenuButtonPopup);
  m_hBackwardBtn->setEnabled(false);


  history->addWidget(m_hBackwardBtn);
  history->addWidget(m_hForwardBtn);
  history->addSeparator();
  QToolBar * navigation = addToolBar(tr("Navigation"));
  navigation->addAction(m_rootFirstAction);
  navigation->addAction(m_rootForwardAction);
  navigation->addAction(m_rootBackwardAction);
  navigation->addAction(m_rootLastAction);
  navigation->addSeparator();
}
void LanesLexicon::setupHistory() {
  // get backward history
  QList<HistoryEvent *> events = m_history->getHistory(10,0,m_historyPos);
  if (events.size() == 0) {
    m_hBackwardBtn->setEnabled(true);
  }
  else {
    QMenu * m = new QMenu;
    while(events.size() > 0) {
      HistoryEvent * event = events.takeFirst();
      QString root = event->getRoot();
      QString word = event->getWord();
      QAction * action;
      if (! word.isEmpty()) {
        action = m->addAction(word);
      }
      else {
        action = m->addAction(root);
      }
      action->setData(event->getId());
      connect(action,SIGNAL(triggered()),this,SLOT(onHistoryBackward()));
    }
    m_hBackwardBtn->setEnabled(true);
    m_hBackwardBtn->setMenu(m);
  }
  events = m_history->getHistory(10,1,m_historyPos);
  if (events.size() == 0) {
    m_hForwardBtn->setEnabled(true);
  }
  else {
    QMenu * m = new QMenu;
    while(events.size() > 0) {
      HistoryEvent * event = events.takeFirst();
      QString root = event->getRoot();
      QString word = event->getWord();
      QAction * action;
      if (! word.isEmpty()) {
        action = m->addAction(word);
      }
      else {
        action = m->addAction(root);
      }
      action->setData(event->getId());
      connect(action,SIGNAL(triggered()),this,SLOT(onHistoryForward()));
    }
    m_hForwardBtn->setEnabled(true);
    m_hForwardBtn->setMenu(m);
  }
  if (! m_historyEnabled) {
    m_hForwardBtn->setEnabled(false);
    m_hBackwardBtn->setEnabled(false);
  }
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
void LanesLexicon::onHistoryForward() {
  QAction * action = static_cast<QAction *>(QObject::sender());
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
}
void LanesLexicon::onHistoryBackward() {
  QAction * action = static_cast<QAction *>(QObject::sender());
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
Place LanesLexicon::showNode(const QString & node,bool nodeOnly,bool createTab) {
  Place p;
  p.setNode(node);
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
    if (w->hasPlace(p,true) == -1) {
      m_history->on();
      m_tabs->insertTab(m_tabs->currentIndex()+1,w,node);
      p = w->getXmlForNode(node,nodeOnly);

      //      connect(w,SIGNAL(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)),
      //              this,SLOT(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)));

    }
  }
  else {
    GraphicsEntry * w = dynamic_cast<GraphicsEntry *>(m_tabs->widget(currentTab));
    if (w->hasPlace(p,true) == -1) {
      m_history->on();
      p = w->getXmlForNode(node,nodeOnly);
      m_tabs->setTabText(0,p.getNode());
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
    if (w->hasPlace(p,true) == -1) {
      m_tabs->insertTab(m_tabs->currentIndex()+1,w,root);
      np = w->getXmlForPlace(p);

      //      connect(w,SIGNAL(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)),
      //              this,SLOT(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)));

    }
  }
  else {
    GraphicsEntry * w = dynamic_cast<GraphicsEntry *>(m_tabs->widget(currentTab));
    if (w->hasPlace(p,true) == -1) {
      np = w->getXmlForPlace(p);
      m_tabs->setTabText(0,root);
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
Place LanesLexicon::showRoot(const QString & root,int supplement,bool createTab) {
  Place p;
  QLOG_DEBUG() << Q_FUNC_INFO << "REDUNDANT CODE";
  /*
  if (createTab) {
    /// turn history on as the user has clicked on something
    /// and the root is not already shown
    GraphicsEntry * w = new GraphicsEntry(this);
    setSignals(w);
    w->installEventFilter(this);
    if (w->hasRoot(root,true) == -1) {
      m_tabs->insertTab(m_tabs->currentIndex()+1,w,root);
      p = w->getXmlForRoot(root,supplement);
    }
  }
  else {
    GraphicsEntry * w = dynamic_cast<GraphicsEntry *>(m_tabs->widget(0));
    if (w->hasRoot(root,true) == -1) {
      p = w->getXmlForRoot(root,supplement);
      m_tabs->setTabText(0,root);
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
  */
  return p;
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
    QString node = item->getNode();
    QString word = item->getWord();
    QString root = item->getRoot();
    qDebug() << Q_FUNC_INFO << root << word << node << reason;
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
  //  qDebug() << "keys" << settings.allKeys();
  settings.beginGroup("General");
  m_dbName = settings.value("Database","lexicon.sqlite").toString();
  QString ar = settings.value("Arabic font").toString();
  if (! ar.isEmpty()) {
    arFont.fromString(ar);
  }
  m_historyEnabled = settings.value("History",true).toBool();
  m_saveTabs = settings.value("Save Tabs",true).toBool();
  m_restoreTabs = settings.value("Restore Tabs",true).toBool();
  m_docked = settings.value("Docked",false).toBool();
  m_interface = settings.value("Interface","default").toString();
  m_rootMode = settings.value("Root Mode",true).toBool();
  m_useNotes = settings.value("Use Notes",false).toBool();
  settings.endGroup();

  settings.beginGroup("Debug");
  m_valgrind = settings.value("Valgrind",false).toBool();
  settings.endGroup();

  settings.beginGroup("Notes");
  m_notesDbName = settings.value("Database","notes.sqlite").toString();
  settings.endGroup();

}
void LanesLexicon::writeSettings() {
  QSettings settings;
  settings.setIniCodec("UTF-8");
  settings.beginGroup("General");
  QDateTime now = QDateTime::currentDateTime();
  settings.setValue("Run date",now);
  settings.setValue("Save Tabs",m_saveTabs);
  settings.setValue("Database",m_dbName);
  settings.setValue("History",m_historyEnabled);
  settings.setValue("Aarabic font",arFont.toString());
  settings.setValue("Restore Tabs",m_restoreTabs);
  settings.endGroup();
  if (m_saveTabs) {
    settings.beginGroup("Tabs");
    for(int i=0;i < m_tabs->count();i++) {
      GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(m_tabs->widget(i));
      if (entry) {
        Place p = entry->getPlace();
        settings.beginGroup(QString("Tab-%1").arg(i));
        settings.setValue("root",p.getRoot());
        settings.setValue("word",p.getWord());
        settings.setValue("node",p.getNode());
        settings.setValue("supplement",p.getSupplement());
        settings.setValue("nodeOnly",p.getNodeOnly());
        settings.endGroup();
      }
    }
  }
}
void LanesLexicon::restoreTabs() {
  QSettings settings;
  settings.setIniCodec("UTF-8");
  settings.beginGroup("Tabs");
  QStringList tabs = settings.childGroups();
  for(int i=0;i < tabs.size();i++) {
    settings.beginGroup(tabs[i]);
    Place p;
    p.setNode(settings.value("node",QString()).toString());
    p.setRoot(settings.value("root",QString()).toString());
    p.setSupplement(settings.value("supplement").toInt());
    p.setWord(settings.value("word",QString()).toString());
    p.setNodeOnly(settings.value("nodeOnly",false).toBool());
    if (p.getNode().isEmpty()) {
      p.setNodeOnly(false);
    }
    QLOG_DEBUG() << "restore tab" << p.getNodeOnly() << p.getRoot() << p.getNode();
    GraphicsEntry * entry = new GraphicsEntry(this);
    setSignals(entry);
    entry->getXmlForPlace(p);
    m_tabs->addTab(entry,p.getRoot());
    settings.endGroup();
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
      /// hasRoot will checks if root already shown
      /// if it is, it move focus to it (if true is 2nd param)
      if (entry->hasPlace(np,true) == -1) {
        entry->setPagingForward();
        entry->getXmlForPlace(np);
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
      /// hasRoot will checks if root already shown
      /// if it is, it move focus to it (if true is 2nd param)
      if (entry->hasPlace(np,true) == -1) {
        entry->setPagingBackward();
        entry->getXmlForPlace(np);
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
  bool ok = query.prepare("select root from entry order by nodenum limit 5;");
  if (! ok ) {
    QLOG_DEBUG() << "first root SQL prepare failed";
  }
  query.exec();
  if (query.first()) {
    m_firstRoot = query.value(0).toString();
  }
  ok = query.prepare("select root from entry order by nodenum desc limit 5;");
  if (! ok ) {
    QLOG_DEBUG() << "last root SQL prepare failed";
  }
  query.exec();
  if (query.first()) {
    m_lastRoot = query.value(0).toString();
  }
  qDebug() << "First" << m_firstRoot << "last" << m_lastRoot;
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
