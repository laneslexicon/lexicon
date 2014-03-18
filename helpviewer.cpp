#include "helpviewer.h"
HelpBrowser::HelpBrowser(QWidget * parent) : QTextBrowser(parent) {
  connect(this,SIGNAL(anchorClicked(const QUrl &)),this,SLOT(getAnchor(const QUrl &)));
}
QVariant HelpBrowser::loadResource(int type, const QUrl &name)
{
  qDebug() << Q_FUNC_INFO << type << name;
  //  qDebug() << m_he->currentFilter();
  if (type < 4) {
    QStringList regs =  m_he->registeredDocumentations();
    QList<QUrl> files;
    files = m_he->files(regs[0],m_he->filterAttributes(),"png");
    for(int i=0;i < files.size();i++) {
      if (files[i].fileName() == name.fileName()) {
        QByteArray ba = m_he->fileData(files[i]);
        return ba;
      }
    }
  }
  return QTextBrowser::loadResource(type, name);
}
void HelpBrowser::getAnchor(const QUrl & url) {
  m_firstAnchor = url.toString();
  m_firstAnchor = m_firstAnchor.remove(0,1);
  qDebug() << url << m_firstAnchor;
  if ((this->backwardHistoryCount() == 0) && ! m_firstAnchor.isEmpty()) {
    qDebug() << "emitting backward available";
    emit(backwardAvailable(true));
  }
}
void HelpBrowser::toAnchor() {
  if ( ! m_firstAnchor.isEmpty()) {
    qDebug() << "scroll to anchor" << m_firstAnchor;
    this->scrollToAnchor(m_firstAnchor);
  }
}
HelpViewer::HelpViewer(QWidget * parent) : QWidget(parent) {

  QVBoxLayout * layout = new QVBoxLayout;
  m_browser = new HelpBrowser;
  m_backBtn = new QPushButton("Back");
  connect(m_browser,SIGNAL(backwardAvailable(bool)),this,SLOT(backwardAvailable(bool)));
  connect(m_backBtn,SIGNAL(clicked()),this,SLOT(goBack()));
  layout->addWidget(m_backBtn);
  layout->addWidget(m_browser);
  setLayout(layout);
}
void HelpViewer::backwardAvailable(bool available) {
  qDebug() << Q_FUNC_INFO << available;
}

void HelpViewer::goBack() {
  qDebug() << Q_FUNC_INFO;
  if (m_browser->backwardHistoryCount() == 0) {
    m_browser->toAnchor();
  }
  else {
    m_browser->backward();
  }
}
/*
 TRACE_OBJ
    QByteArray ba;
    if (type < 4) {
        const QUrl url = HelpEngineWrapper::instance().findFile(name);
        ba = HelpEngineWrapper::instance().fileData(url);
        if (url.toString().endsWith(QLatin1String(".svg"), Qt::CaseInsensitive)) {
            QImage image;
            image.loadFromData(ba, "svg");
            if (!image.isNull())
                return image;
        }
    }
    return ba;
*/
