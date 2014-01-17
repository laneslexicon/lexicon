#include "laneslexicon.h"
LanesLexicon::LanesLexicon(QWidget *parent) :
    QMainWindow(parent)

{
  readSettings();
  openDatabase("lexicon.sqlite");
  loadStyleSheet();
  QSplitter * splitter = new QSplitter;
  m_tree = new ContentsWidget(this);
  m_tree->setObjectName("treeRoots");
  m_tree->installEventFilter(this);
  m_tabs = new QTabWidget(this);
  GraphicsEntry * entry = new GraphicsEntry(this);
  entry->installEventFilter(this);

  m_notes = new NotesWidget();
  //  m_notes->setObjectName("notes");
  splitter->addWidget(m_tree);
  splitter->addWidget(m_tabs);
  splitter->setStretchFactor(0,0);
  splitter->setStretchFactor(1,1);


  m_tabs->addTab(entry,tr(""));
  //  m_tabs->addTab(m_notes,"Notes");
  setCentralWidget(splitter);
  createActions();
  createToolBar();
  createMenus();
  createStatusBar();

  if (m_db.isOpen()) {
    statusBar()->showMessage(tr("Ready"));
    m_tree->loadContents();
    getFirstAndLast();
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

  setSignals(entry);

  connect(this,SIGNAL(nodeActivated(const QString & ,const QString & )),
          m_notes,SLOT(setActiveNode(const QString & ,const QString & )));

  connect(m_notesBtn,SIGNAL(clicked()),this,SLOT(onNotesClicked()));
  // the following line shows an entry but nothing in the tree is highlighted;
  //m_place = Place("يى",1);

  // this one works
  //  m_place = Place("يمر",1);
  showPlace(Place(m_firstRoot,0),false);
  m_tree->ensureVisible(m_firstRoot);
  m_tree->resizeColumnToContents(0);
}

LanesLexicon::~LanesLexicon()
{
  QLOG_DEBUG() << "main window destructor";
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
void LanesLexicon::shortcut(const QString & k) {
  if (k == "Root Search") {
    RootSearchDialog * d = new RootSearchDialog(this);
    if (d->exec()) {
      QString t = d->getText();
      if (! t.isEmpty()) {
        Place p = showRoot(t,0,false);
        if (! p.isValid()) {
          QMessageBox msgBox;
          msgBox.setText(QString("%1 not found").arg(t));
          msgBox.exec();
        }
      }
    }
  }
  if (k == "Node Search") {
    NodeSearchDialog * d = new NodeSearchDialog(this);
    if (d->exec()) {
      QString t = d->getText();
      if (! t.isEmpty()) {
        Place p = showNode(t,false);
        if (! p.isValid()) {
          QMessageBox msgBox;
          msgBox.setText(QString("%1 not found").arg(t));
          msgBox.exec();
        }
      }
    }
  }
  else if (k == "Quit") {
    on_actionExit();
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

  m_hForwardBtn = new QToolButton;
  m_hForwardBtn->setText("Forward");
  m_hForwardBtn->setDefaultAction(m_hForward);
  m_hForwardBtn->setPopupMode(QToolButton::MenuButtonPopup);
  m_hForwardBtn->setEnabled(false);
  m_hBackwardBtn = new QToolButton;
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

  m_notesBtn = new QPushButton(tr("Notes"));
  m_notesBtn->setEnabled(true);
  statusBar()->insertPermanentWidget(0,m_notesBtn,0);
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
  QString node = event->getNode();
  QString root = event->getRoot();
  GraphicsEntry * w = dynamic_cast<GraphicsEntry *>(m_tabs->widget(0));
  /// TODO fix for supplement
  w->getXmlForRoot(root,0,node);
  m_tabs->setTabText(0,root);
  w->setFocus();
}
void LanesLexicon::onHistoryBackward() {
  QAction * action = static_cast<QAction *>(QObject::sender());
  m_historyPos = action->data().toInt();
  HistoryEvent * event = m_history->getEvent(m_historyPos);
  setupHistory();
  QString node = event->getNode();
  QString root = event->getRoot();
  GraphicsEntry * w = dynamic_cast<GraphicsEntry *>(m_tabs->widget(0));
  /// TODO fix for supplement
  w->getXmlForRoot(root,0,node);
  m_tabs->setTabText(0,root);
  w->setFocus();
}
void LanesLexicon::on_actionExit()
{
  if (m_db.isOpen()) {
    m_db.close();
  }
  m_notes->close();
  delete m_notes;
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
Place LanesLexicon::showNode(const QString & node,bool createTab) {
  qDebug() << Q_FUNC_INFO << node << createTab;
  Place p;
  p.setNode(node);
 if (createTab) {
    /// turn history on as the user has clicked on something
    /// and the root is not already shown
    GraphicsEntry * w = new GraphicsEntry(this);
    setSignals(w);
    w->installEventFilter(this);
    if (w->hasPlace(p,true) == -1) {
      m_history->on();
      m_tabs->insertTab(m_tabs->currentIndex()+1,w,node);
      p = w->getXmlForNode(node);

      //      connect(w,SIGNAL(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)),
      //              this,SLOT(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)));

    }
  }
  else {
    GraphicsEntry * w = dynamic_cast<GraphicsEntry *>(m_tabs->widget(0));
    if (w->hasPlace(p,true) == -1) {
      m_history->on();
      p = w->getXmlForNode(node);
      m_tabs->setTabText(0,p.getNode());
      // this works but sets it for all tabs
      //m_tabs->setStyleSheet("QTabBar {font-family : Amiri;font-size : 16px}");
      // this sets it for all the items in graphicsentry
      // but not the tab title
      //    w->setStyleSheet("font-family : Amiri;font-size : 16px");
      w->setFocus();
    }
  }
 qDebug() << Q_FUNC_INFO << p.getRoot() << p.getWord() << p.getNode();
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
    GraphicsEntry * w = dynamic_cast<GraphicsEntry *>(m_tabs->widget(0));
    if (w->hasPlace(p,true) == -1) {
      np = w->getXmlForPlace(p);
      QString t = QString("<span class=\"ar\">%1</span>").arg(root);

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
      QString t = QString("<span class=\"ar\">%1</span>").arg(root);

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
  return p;
}
void LanesLexicon::focusItemChanged(QGraphicsItem * newFocus, QGraphicsItem * oldFocus, Qt::FocusReason reason) {
  EntryItem * item = dynamic_cast<EntryItem *>(newFocus);
  if (item) {
    QString node = item->getNode();
    QString word = item->getWord();
    QPointF p = item->pos();
    if (! node.isEmpty()) {
      QString txt = QString("%1  %2  At:%3,%4").arg(node).arg(word).arg(p.x()).arg(p.y());
      statusBar()->showMessage(txt);
      m_notesBtn->setEnabled(true);
      emit(nodeActivated(item->getNode(),item->getWord()));
    }
  }

}
/**
 * Ctrl-T moves focus to the tree
 * Ctrl-E moves focus to the current display widget
 *
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
  qDebug() << "keys" << settings.allKeys();
  settings.beginGroup("General");
  m_dbName = settings.value("Database","lexicon.sqlite").toString();
  QString ar = settings.value("Arabic font").toString();
  if (! ar.isEmpty()) {
    arFont.fromString(ar);
  }
  m_historyEnabled = settings.value("History",true).toBool();
  settings.endGroup();
  settings.beginGroup("Notes");
  m_notesDbName = settings.value("Database","notes.sqlite").toString();
  qDebug() << "Lexicon" << m_dbName;
  qDebug() << "Notesdb" << m_notesDbName;
}
void LanesLexicon::writeSettings() {
  QSettings settings;
  settings.beginGroup("General");
  QDateTime now = QDateTime::currentDateTime();
  settings.setValue("Run date",now);
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
    entry->getXmlForRoot(nroot);
  }
}
void LanesLexicon::findPrevRoot(const QString & root) {
  GraphicsEntry * entry = dynamic_cast<GraphicsEntry *>(QObject::sender());
  qDebug() << Q_FUNC_INFO  << root;
  QString nroot = m_tree->findPrevRoot(root);
  if (entry && ! nroot.isEmpty()) {
    entry->setPagingBackward();
    entry->getXmlForRoot(nroot);
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
  qDebug() << Q_FUNC_INFO;
  Place p(m_firstRoot,0);
  showPlace(p,false);
  m_tree->ensurePlaceVisible(p);
}
void LanesLexicon::on_actionLastRoot() {
  showRoot(m_lastRoot,false);
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
