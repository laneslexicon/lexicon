#include "mainwindow.h"
#include "notes.h"
#include "notebrowser.h"
#include "notedialog.h"
#include "place.h"
extern int random_in_range(unsigned int,unsigned int);
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)

{
  bool ok;
  QSettings settings("default.ini",QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  m_notes = new NoteMaster(&settings);
  settings.beginGroup("System");
  QString dbname = settings.value("Database","lexicon.sqlite").toString();
  QFile dbfile(dbname);

  if (! dbfile.exists()) {
    QLOG_WARN() << QString(tr("Cannot find database : %1")).arg(dbname);
  }
  else {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbname);
    ok = m_db.open();
    qDebug() << "db open" << ok;
  }
  m_browser = new NoteBrowser;
  setCentralWidget(m_browser);
  createActions();
  createToolBar();
  createMenus();
  createStatusBar();
}

MainWindow::~MainWindow()
{
  qDebug() << "main window destructor";
}
NoteMaster * MainWindow::getNotes() {
  return m_notes;
}
QSettings * MainWindow::getSettings() {
  return new QSettings("default.ini",QSettings::IniFormat);

}
void MainWindow::createActions() {
  m_exitAction = new QAction(tr("Exit"),this);
  connect(m_exitAction,SIGNAL(triggered()),this,SLOT(on_actionExit()));
  m_addNote = new QAction(tr("Add note"),this);
  connect(m_addNote,SIGNAL(triggered()),this,SLOT(onAddNote()));
  m_addNodeNote = new QAction(tr("Add node note"),this);
  connect(m_addNodeNote,SIGNAL(triggered()),this,SLOT(onAddNodeNote()));
}
void MainWindow::createToolBar() {
  m_fileToolBar = addToolBar(tr("&File"));
  m_fileToolBar->addAction(m_exitAction);
  m_fileToolBar->addAction(m_addNote);
  m_fileToolBar->addAction(m_addNodeNote);
}
void MainWindow::createMenus() {
  m_fileMenu = menuBar()->addMenu(tr("&File"));
  m_fileMenu->addAction(m_exitAction);
}
void MainWindow::createStatusBar() {
  statusBar()->showMessage(tr("Ready"));
}
QSize MainWindow::sizeHint() const {
  return QSize(800,600);
}
void MainWindow::on_actionExit()
{
    close();
}
void MainWindow::onAddNote() {
  Place p;
  QSqlQuery query;
  bool ok = query.prepare("select root,word,nodeid from entry where id = ?");
  if (! ok ) {
    QLOG_DEBUG() << "idiot....";
    return;
  }
  int max = 100;
  int c = 0;
  while(! p.isValid() && (c < max)) {
    query.bindValue(0,random_in_range(0,47000));
    query.exec();
    c++;
    if (query.first()) {
      p.setNode(query.value(2).toString());
      p.setWord(query.value(1).toString());
      p.setWord(query.value(0).toString());
    }
  }
  NoteDialog d(p);
  d.exec();
}
void MainWindow::onAddNodeNote() {
  Place p;
  QSqlQuery query;
  bool ok = query.prepare("select root,word,nodeid from entry where nodeid = ?");
  if (! ok ) {
    QLOG_DEBUG() << "idiot....";
    return;
  }
  int node = QInputDialog::getInt(this, tr("Add note to node"),
                                  tr("Node number:"), 0,0,47836);
  if (!ok) {
    return;
  }
  query.bindValue(0,QString("n%1").arg(node));
  query.exec();
  if (query.first()) {
      p.setNode(query.value(2).toString());
      p.setWord(query.value(1).toString());
      p.setWord(query.value(0).toString());
      NoteDialog d(p);
      if (d.exec()) {
        m_browser->loadNotes();
      }
  }
  else {
    QMessageBox::warning(this, tr("Add note to node"),
                         QString(tr("Node n%1 not found")).arg(node),
                         QMessageBox::Ok);
  }
}
