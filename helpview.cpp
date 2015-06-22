#include "helpview.h"
#include "application.h"
#include "externs.h"
#include "definedsettings.h"
#include "QsLog.h"
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
  connect(m_view,SIGNAL(loadProgress(int)),this,SLOT(loadProgress(int)));

  readSettings();
  if (m_currentPage.isEmpty()) {
    if (m_helpRoot.endsWith(QDir::separator())) {
      m_helpRoot.chop(1);
    }
    QFileInfo fi(m_helpRoot + QDir::separator() + "index.html");

    m_view->load(QUrl("file:///" + fi.absoluteFilePath()));
  }
  m_view->load(m_currentPage);

  m_view->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  QLOG_DEBUG() << Q_FUNC_INFO << "Documentation opened at" << m_view->url();
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
  QUrl f("file:///" + str + QDir::separator() + "index.html");
  QLOG_DEBUG() << Q_FUNC_INFO << f;
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
  settings.setValue(SID_HELP_URL, m_view->url());
  settings.endGroup();

}
void HelpView::loadProgress(int x) {

  qDebug() << Q_FUNC_INFO << x;
}
