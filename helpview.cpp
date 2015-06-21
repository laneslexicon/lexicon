#include "helpview.h"
#include "application.h"
#include "externs.h"
#include "definedsettings.h"
HelpView::HelpView(QWidget * parent) : QWidget(parent) {
  QVBoxLayout * layout = new QVBoxLayout;
  m_view = new QWebView(this);

  QDialogButtonBox * btns = new QDialogButtonBox(QDialogButtonBox::Close);
  layout->addWidget(m_view);
  layout->addWidget(btns);
  setLayout(layout);
  m_view->show();

  connect(m_view,SIGNAL(linkClicked(const QUrl &)),this,SLOT(linkclick(const QUrl &)));
  connect(btns, SIGNAL(rejected()), this, SLOT(onClose()));
  readSettings();

  QFileInfo fi(m_helpRoot + "/index.html");
  qDebug() << fi.absoluteFilePath();
  //m_view->load(QUrl("file:///site/lane/preface/index.html"));
  m_view->load(QUrl("file://" + fi.absoluteFilePath()));
  m_view->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  // QUrl::fromLocalFile("file:///hello.html"));
  qDebug() << m_view->url();

}
HelpView::~HelpView() {
  qDebug() << Q_FUNC_INFO;
  writeSettings();
}
void HelpView::linkclick(const QUrl & url) {
  qDebug() << Q_FUNC_INFO << url;

  QString str = url.toLocalFile();
  if (str.endsWith(QDir::separator())) {
    str.chop(1);
  }
  QUrl f("file://" + str + QDir::separator() + "index.html");
  qDebug() << f;
  m_view->load(f);

}
void HelpView::onClose() {
  this->hide();
}
void HelpView::readSettings() {
  SETTINGS

  settings.beginGroup("Help");
  m_helpRoot = settings.value(SID_HELP_LOCATION,"site").toString();
  m_currentPage = settings.value(SID_HELP_URL,QUrl()).toUrl();
  resize(settings.value(SID_HELP_SIZE,QSize(800,600)).toSize());
  move(settings.value(SID_HELP_POS,QSize(450,20)).toPoint());
}
void HelpView::writeSettings() {
  SETTINGS

  settings.beginGroup("Help");
  settings.setValue(SID_HELP_SIZE, size());
  settings.setValue(SID_HELP_POS, pos());
  settings.endGroup();

}
