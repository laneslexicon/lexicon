#include "laneslexicon.h"
LanesLexicon::LanesLexicon(QWidget *parent) :
    QMainWindow(parent)

{

  QSplitter * w = new QSplitter;
  m_tree = new ContentsWidget(this);
  m_entry = new GraphicsEntry;
  w->addWidget(m_tree);
  w->addWidget(m_entry);
  setCentralWidget(w);
  createActions();
  createToolBar();
  createMenus();
  createStatusBar();

  if (openDatabase("lexicon.sqlite")) {
    statusBar()->showMessage(tr("Ready"));
    m_tree->loadContents();
    m_entry->prepareQueries();
  }
  else {
    statusBar()->showMessage(tr("Failed to open database"));
  }
  connect(m_tree,SIGNAL(itemClicked(QTreeWidgetItem *,int)),this,SLOT(rootClicked(QTreeWidgetItem *,int)));
}

LanesLexicon::~LanesLexicon()
{
  qDebug() << "main window destructor";
}
void LanesLexicon::createActions() {
  m_exitAction = new QAction(tr("Exit"),this);
  connect(m_exitAction,SIGNAL(triggered()),this,SLOT(on_actionExit()));
}
void LanesLexicon::createToolBar() {
  m_fileToolBar = addToolBar(tr("&File"));
  m_fileToolBar->addAction(m_exitAction);
}
void LanesLexicon::createMenus() {
  m_fileMenu = menuBar()->addMenu(tr("&File"));
  m_fileMenu->addAction(m_exitAction);
}
void LanesLexicon::createStatusBar() {
  statusBar()->showMessage(tr("Ready"));
}
QSize LanesLexicon::sizeHint() const {
  return QSize(800,600);
}
void LanesLexicon::on_actionExit()
{
  if (m_db.isOpen()) {
    m_db.close();
  }
  close();
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
  return ok;
}
void LanesLexicon::rootClicked(QTreeWidgetItem * item,int column) {
  QString root = item->text(0);
  qDebug() << "requested root" << root;
  m_entry->getXmlForRoot(root);
}
