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
    QString c = m_firstAnchor;
    c = "j" + c.remove(0,1);
    qDebug() << "scroll to anchor" << m_firstAnchor << " --->"  << c;
    this->scrollToAnchor(c);
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
HelpViewer::~HelpViewer() {
  qDebug() << Q_FUNC_INFO;
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
HelpWidget::HelpWidget(QWidget * parent) : QWidget(parent) {
  readSettings();
  m_he = new QHelpEngine("./help/lanedocs.qhc");
  m_viewer = new HelpViewer(this);
  m_viewer->browser()->setHelpEngine(m_he);
  //    QHelpContentModel *contentModel = he->contentModel();


    QHelpContentWidget *contentWidget = m_he->contentWidget();
    QHelpContentModel *contentModel =
        qobject_cast<QHelpContentModel*>(contentWidget->model());
    connect(contentModel, SIGNAL(contentsCreated()), this, SLOT(contentsCreated()));
  m_he->setupData();
    QHelpIndexModel* indexModel = m_he->indexModel();
    QHelpIndexWidget* indexWidget = m_he->indexWidget();

    QSplitter* splitter = new QSplitter();
    splitter->addWidget(contentWidget);
    //    splitter->addWidget(indexWidget);
    splitter->addWidget(m_viewer);
    splitter->setStretchFactor(0,0);
    splitter->setStretchFactor(1,1);

    indexWidget->setModel(indexModel);
    QVBoxLayout * layout = new QVBoxLayout;
    layout->addWidget(splitter);
    setLayout(layout);
   connect(contentWidget,SIGNAL(linkActivated(const QUrl &)),this,SLOT(helpLinkActivated(const QUrl &)));
   if (! m_currentUrl.isEmpty()) {
     helpLinkActivated(m_currentUrl);
   }
}
HelpWidget::~HelpWidget() {
  writeSettings();
}
void HelpWidget::contentsCreated() {
  qDebug() << Q_FUNC_INFO;

  // get registered docs
  qDebug() << "Collection file" << m_he->collectionFile();
   QStringList regs =  m_he->registeredDocumentations();
  qDebug() << "Registered documentation" << regs;

  qDebug() << "Current filter" << m_he->currentFilter();
  QList<QStringList> fa = m_he->filterAttributeSets(regs[0]);
  qDebug() << "Filter attribute sets" << fa;

  QList<QUrl> files;
  files = m_he->files(regs[0],fa[0]);

  for(int i=0;i < files.size();i++) {
    QByteArray ba = m_he->fileData(files[i]);
    qDebug() << files[i] << ba.size();
  }

  m_he->setCurrentFilter("Lanes Lexicon 1.0");
  qDebug() << "Filter attributes" << m_he->filterAttributes();
  QMap<QString,QUrl> links = m_he->linksForIdentifier(QLatin1String("MyApplication::config"));
  qDebug() << "Links for ID size:" << links.size();
  QMapIterator<QString, QUrl> i(links);
  while (i.hasNext()) {
    i.next();
    qDebug() << i.key() << ": " << i.value();
  }
  links = m_he->linksForIdentifier(QLatin1String("lanex"));
  qDebug() << "Links for ID size:" << links.size();
  QMapIterator<QString, QUrl> it(links);
  while (it.hasNext()) {
    it.next();
    qDebug() << it.key() << ": " << it.value();
  }
  m_he->contentWidget()->expandAll();
}
void HelpWidget::helpLinkActivated(const QUrl & url)  {
  qDebug() << Q_FUNC_INFO << url << "fragment" << url.fragment();
  QByteArray helpData = m_he->fileData(url);//.constBegin().value());
  m_viewer->browser()->setHtml(helpData);
  if (url.hasFragment()) {
    m_viewer->browser()->scrollToAnchor(url.fragment());
  }
  m_viewer->setFocus(Qt::OtherFocusReason);
  m_currentUrl = url;
}

void HelpWidget::writeSettings() {
  QSettings settings;
  settings.setIniCodec("UTF-8");
  settings.beginGroup("Help");
  settings.setValue("Current page",m_currentUrl);
}
void HelpWidget::readSettings() {
  QSettings settings;
  settings.setIniCodec("UTF-8");


  settings.beginGroup("Help");
  m_helpCollection = settings.value("Help collection","lanedocs.qhc").toString();
  m_currentUrl = settings.value("Current page").toUrl();
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
