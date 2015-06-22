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
  m_initialPage = true;
  m_timer = 0;
  m_progress = 0;

  connect(m_view,SIGNAL(linkClicked(const QUrl &)),this,SLOT(linkclick(const QUrl &)));
  connect(btns, SIGNAL(rejected()), this, SLOT(onClose()));
  connect(m_view,SIGNAL(loadProgress(int)),this,SLOT(loadProgress(int)));
  connect(m_view,SIGNAL(loadStarted()),this,SLOT(loadStarted()));
  connect(m_view,SIGNAL(loadFinished(bool)),this,SLOT(loadFinished(bool)));
  if (m_initialPage) {
    this->hide();
    m_view->hide();
  }

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
/**
 * The first load can take sometimes take a few seconds so we are showing
 * a progress dialog when
 *
 * @param x
 */
void HelpView::loadProgress(int x) {
  if (m_progress) {
    m_progress->setValue(x);
  }
}
void HelpView::loadStarted() {
  if (m_initialPage) {
    m_progress = new QProgressDialog("Loading ...", "Cancel", 0, 100);
    m_progress->setCancelButton(0);
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(perform()));
    m_timer->start(0);
    m_progress->show();
  }
}
void HelpView::loadFinished(bool ok) {
  if (m_initialPage) {
    m_view->show();
    m_initialPage = false;
    m_timer->stop();
    delete m_progress;
    delete m_timer;
    m_timer = 0;
    m_progress = 0;
  }
  this->show();
}
bool HelpView::isLoaded() {
  return ! m_initialPage;
}
