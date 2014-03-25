#include "application.h"
Lexicon::Lexicon(int & argc, char ** argv) : QApplication(argc,argv) {
  QString resourceDir;

#ifdef Q_WS_MAC
  resourceDir = QCoreApplication::applicationDirPath() + "/../Resources";
#else
  resourceDir = QCoreApplication::applicationDirPath() + "/Resources";
#endif

  if ( ! QDir::setCurrent(resourceDir)) {
    qDebug() << "Failed to changed current directory" << resourceDir;
  }
  addLibraryPath("./images");
  QCoreApplication::setOrganizationName("Gabanjo");
  QCoreApplication::setOrganizationDomain("theunwalledcity.com");
  QCoreApplication::setApplicationName("Lanes Lexicon");
  QCoreApplication::setApplicationVersion("0.1");
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
