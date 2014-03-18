#include "helpviewer.h"
HelpBrowser::HelpBrowser(QWidget * parent) : QTextBrowser(parent) {
}
QVariant HelpBrowser::loadResource(int type, const QUrl &name)
{
  qDebug() << Q_FUNC_INFO << type << name;
  qDebug() << m_he->currentFilter();
  if (type < 4) {
    QStringList regs =  m_he->registeredDocumentations();
    QList<QUrl> files;
    files = m_he->files(regs[0],m_he->filterAttributes(),"png");
    for(int i=0;i < files.size();i++) {
      if (files[i].fileName() == name.fileName()) {
        qDebug() << "found" << files[i];
        QByteArray ba = m_he->fileData(files[i]);
        return ba;
      }
    }
  }
  return QTextBrowser::loadResource(type, name);
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
  m_browser->backward();
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
