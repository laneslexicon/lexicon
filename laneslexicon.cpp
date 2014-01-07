#include "laneslexicon.h"
LanesLexicon::LanesLexicon(QWidget *parent) :
    QMainWindow(parent)

{
  loadStyleSheet();
  QSplitter * w = new QSplitter;
  m_tree = new ContentsWidget(this);
  m_tree->setObjectName("treeRoots");
  m_tree->installEventFilter(this);
  m_tabs = new QTabWidget(this);
  GraphicsEntry * entry = new GraphicsEntry(this);
  entry->installEventFilter(this);

  m_notes = new NotesWidget(this);
  //  m_notes->setObjectName("notes");
  w->addWidget(m_tree);
  w->addWidget(m_tabs);
  m_tabs->addTab(entry,tr(""));
  m_tabs->addTab(m_notes,"Notes");
  setCentralWidget(w);
  createActions();
  createToolBar();
  createMenus();
  createStatusBar();

  readSettings();
  if (openDatabase("lexicon.sqlite")) {
    statusBar()->showMessage(tr("Ready"));
    m_tree->loadContents();
    entry->prepareQueries();
    /// at the end of the history, but we should be able to restore from settings
    m_historyPos = 9;
    m_history = new HistoryMaster("notes.sqlite");
    setupHistory();
  }
  else {
    statusBar()->showMessage(tr("Failed to open database"));
  }
  connect(m_tree,SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)),this,SLOT(rootClicked(QTreeWidgetItem *,int)));

  connect(m_tree,SIGNAL(itemActivated(QTreeWidgetItem *,int)),this,SLOT(rootClicked(QTreeWidgetItem *,int)));


  connect(entry,SIGNAL(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)),
          this,SLOT(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)));
  connect(entry,SIGNAL(nextRoot(const QString &)),this,SLOT(findNextRoot(const QString &)));
  connect(entry,SIGNAL(prevRoot(const QString &)),this,SLOT(findPrevRoot(const QString &)));

  connect(entry,SIGNAL(rootChanged(const QString & ,const QString & )),this,SLOT(rootChanged(const QString &, const QString &)));

  connect(this,SIGNAL(nodeActivated(const QString & ,const QString & )),
          m_notes,SLOT(setActiveNode(const QString & ,const QString & )));

  connect(m_notesBtn,SIGNAL(clicked()),this,SLOT(onNotesClicked()));
 }

LanesLexicon::~LanesLexicon()
{
  QLOG_DEBUG() << "main window destructor";
}
/**
 * load the application level stylesheet, stripping out lines
 * beginning with #
 *
 */
void LanesLexicon::loadStyleSheet() {
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

  m_rootForwardAction = new QAction(tr("Next Root"),this);
  m_rootBackwardAction = new QAction(tr("Prev Root"),this);
  m_rootFirstAction = new QAction(tr("First Root"),this);
  m_rootLastAction = new QAction(tr("Last Root"),this);

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
  m_notesBtn->setEnabled(false);
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
  w->getXmlForRoot(root,node);
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
  w->getXmlForRoot(root,node);
  m_tabs->setTabText(0,root);
  w->setFocus();
}
void LanesLexicon::on_actionExit()
{
  if (m_db.isOpen()) {
    m_db.close();
  }
  close();
}
void LanesLexicon::onNotesClicked() {
  m_tabs->setCurrentWidget(m_notes);
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
  QString root = item->text(0);


  if (QApplication::keyboardModifiers() & Qt::ShiftModifier) {
    /// turn history on as the user has clicked on something
    m_history->on();
    GraphicsEntry * w = new GraphicsEntry(this);
    w->prepareQueries();
    m_tabs->insertTab(m_tabs->currentIndex()+1,w,root);
    w->getXmlForRoot(root);

    connect(w,SIGNAL(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)),
          this,SLOT(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)));
    w->installEventFilter(this);
  }
  else {
    GraphicsEntry * w = dynamic_cast<GraphicsEntry *>(m_tabs->widget(0));
    if (w->hasRoot(root,true) == -1) {
      m_history->on();
      w->getXmlForRoot(root);
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
  QLOG_DEBUG() << "Get root" << QApplication::keyboardModifiers() << root;
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
  QString ar = settings.value("Arabic font").toString();
  if (! ar.isEmpty()) {
    qDebug() << "set font" << ar;
    arFont.fromString(ar);
  }
}
void LanesLexicon::writeSettings() {
  QSettings settings;
  QDateTime now = QDateTime::currentDateTime();
  settings.setValue("Run date",now);
}
HistoryMaster * LanesLexicon::history() {
  return m_history;
}
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
void LanesLexicon::on_actionNextRoot() {
  qDebug() << Q_FUNC_INFO;
}
void LanesLexicon::on_actionPrevRoot() {
  qDebug() << Q_FUNC_INFO;
}
void LanesLexicon::on_actionFirstRoot() {
  qDebug() << Q_FUNC_INFO;
}
void LanesLexicon::on_actionLastRoot() {
  qDebug() << Q_FUNC_INFO;
}
void LanesLexicon::rootChanged(const QString & root,const QString & node) {
  m_currentRoot->setText(root);
}
