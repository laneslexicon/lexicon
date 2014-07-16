#include "mainwindow.h"
#include "imedit.h"
#include "imlineedit.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)

{
  QSettings * settings = new QSettings("default.ini",QSettings::IniFormat);
  QVBoxLayout * layout = new QVBoxLayout;
  m_edit = new ImEdit;
  m_edit->setDebug(true);
  m_edit->loadMap("buckwalter-1.4.js","Buckwalter");
  m_edit->activateMap("Buckwalter");
  QString ltr(QChar(0x202d));

  m_edit->setPlainText(ltr);
  m_lineEdit = new ImLineEdit;
  m_lineEdit->loadMap("buckwalter-1.4.js","Buckwalter");
  m_lineEdit->activateMap("Buckwalter");
  m_lineEdit->setForceLTR(true);
  m_lineEdit->setDebug(true);
  m_lineEdit->readSettings(settings);
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
  qDebug() << "main window destructor";
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
