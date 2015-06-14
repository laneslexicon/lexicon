#include "mainwindow.h"
#include "optionsdialog.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)

{
  QSettings * settings = new QSettings("default.ini",QSettings::IniFormat);
  settings->setIniCodec("UTF-8");
  QVBoxLayout * layout = new QVBoxLayout;
  QWidget * w = new QWidget;
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
void MainWindow::createActions() {
  m_exitAction = new QAction(tr("Exit"),this);
  connect(m_exitAction,SIGNAL(triggered()),this,SLOT(on_actionExit()));
  m_optionsAction = new QAction(tr("Options"),this);
  connect(m_optionsAction,SIGNAL(triggered()),this,SLOT(on_actionOptions()));
}
void MainWindow::createToolBar() {
  m_fileToolBar = addToolBar(tr("&File"));
  m_fileToolBar->addAction(m_exitAction);
  m_fileToolBar->addAction(m_optionsAction);
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
void MainWindow::on_actionOptions()
{
  OptionsDialog * d = new OptionsDialog("oxygen");
  d->exec();
  delete d;
}
