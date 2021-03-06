#include "mainwindow.h"
#include "imedit.h"
#include "imlineedit.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)

{
  QVBoxLayout * layout = new QVBoxLayout;
  m_edit = new ImEdit;
  m_edit->setDebug(true);
  m_edit->loadMap("buckwalter-1.4.js","Buckwalter");
  m_edit->setCurrentMap("Buckwalter");
  QString ltr(QChar(0x202d));

  m_edit->setPlainText(ltr);
  m_lineEdit = new ImLineEdit;
  m_lineEdit->setDebug(true);
  m_lineEdit->loadMap("../buckwalter-1.6.js","Buckwalter");
  m_lineEdit->loadMap("../perseus.js","Perseus");
  m_lineEdit->loadMap("../ugaritic.js","Ugaritic");
  m_lineEdit->setCurrentMap("Ugaritic");
  m_lineEdit->setForceLTR(false);
  //  m_lineEdit->readSettings("default.ini");
  connect(m_lineEdit,SIGNAL(logMessage(const QString &)),this,SLOT(logMessage(const QString &)));
  QFont f("Amiri",30);
  m_lineEdit->setFont(f);
  QWidget * w = new QWidget;
  layout->addWidget(m_lineEdit);
  layout->addWidget(m_edit);
  w->setLayout(layout);
  setCentralWidget(w);
  createActions();
  createToolBar();
  createMenus();
  createStatusBar();
}

MainWindow::~MainWindow()
{
}
void MainWindow::logMessage(const QString & t) {
  qDebug() << t;
}
void MainWindow::createActions() {
  m_exitAction = new QAction(tr("Exit"),this);
  connect(m_exitAction,SIGNAL(triggered()),this,SLOT(on_actionExit()));
}
void MainWindow::createToolBar() {
  m_fileToolBar = addToolBar(tr("&File"));
  m_fileToolBar->addAction(m_exitAction);
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
