#include "application.h"
#include "QsLog.h"
Lexicon::Lexicon(int & argc, char ** argv) : QApplication(argc,argv) {
  QString resourceDir;
  m_ok = true;
#ifdef __APPLE__
  resourceDir = QCoreApplication::applicationDirPath() + "/../Resources";
#else
  resourceDir = QCoreApplication::applicationDirPath() + "/Resources";
#endif

  if ( ! QDir::setCurrent(resourceDir)) {
    QString errmsg  = QString(QObject::tr("Warning failed to change application working directory to : %1")).arg(resourceDir);
    std::cout << errmsg.toLocal8Bit().data() << std::endl;
    m_ok = false;
  }
  QFontDatabase::addApplicationFont(resourceDir + "/fonts/amiri/amiri-regular.ttf");
  QFontDatabase::addApplicationFont(resourceDir + "/fonts/FiraSans/OTF/FiraSans-Regular.otf");
  if (QFontDatabase::addApplicationFont(resourceDir + "/fonts/FiraSans/OTF/FiraSans-Book.otf") == -1)
    std::cout << "Could not load FiraSans-Book font" << std::endl;
  addLibraryPath(resourceDir + "/lib");



  QCoreApplication::setOrganizationName("Gabanjo");
  QCoreApplication::setOrganizationDomain("theunwalledcity.com");
  QCoreApplication::setApplicationName("Lanes Lexicon");
  QCoreApplication::setApplicationVersion("0.1");

  connect(this,SIGNAL(focusChanged(QWidget *,QWidget *)),this,SLOT(onFocusChange(QWidget *,QWidget *)));
}
QString Lexicon::getConfig() const {
  return m_configFile;
}
void Lexicon::setConfig(const QString & fileName) {
  QFileInfo fi(fileName);
  if (fi.exists()) {
    m_configFile = fileName;
  }
}
QSettings * Lexicon::getSettings() {
  if (m_configFile.isEmpty()) {
    return new QSettings;
  }
  return new QSettings(m_configFile,QSettings::IniFormat);
}
QString Lexicon::getFontName(const QString & type) {
  /*
  QString k = "name_" + type;
  if (m_fonts.contains(k)) {
    return m_fonts.value(k);
  }
  */
  return QString();
}
QString Lexicon::getFontSize(const QString & type) {
  /*
  QString k = "size_" + type;
  if (m_fonts.contains(k)) {
    return m_fonts.value(k);
  }
  */
  return QString();
}
void Lexicon::onFocusChange(QWidget * old, QWidget * now) {
  if (old && now)
    QLOG_DEBUG() << old->metaObject()->className() << "--->" << now->metaObject()->className();
}
