#include "application.h"
MyApp::MyApp(int & argc, char ** argv) : QApplication(argc,argv) {
  QString resourceDir;
#ifdef __APPLE__
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
  QString MyApp::getConfig() const {
  return "fuck you";
}
  void MyApp::setConfig(const QString & fileName) {
}
