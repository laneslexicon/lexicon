#include "laneslexicon.h"
#include <QApplication>
#include <QFontDatabase>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QSplashScreen>
#include <QPixmap>
#include "QsLog.h"
#include "QsLogDest.h"
#include "application.h"
#include "splashscreen.h"
#include <iostream>
int random_in_range (unsigned int min, unsigned int max)
{
  int base_random = rand(); /* in [0, RAND_MAX] */
  if (RAND_MAX == base_random) return random_in_range(min, max);
  /* now guaranteed to be in [0, RAND_MAX) */
  int range       = max - min,
      remainder   = RAND_MAX % range,
      bucket      = RAND_MAX / range;
  /* There are range buckets, plus one smaller interval
     within remainder of RAND_MAX */
  if (base_random < RAND_MAX - remainder) {
    return min + base_random/bucket;
  } else {
    return random_in_range (min, max);
  }
}

LanesLexicon * getApp() {
  foreach(QWidget *widget, qApp->topLevelWidgets()) {
    if(widget->inherits("QMainWindow"))
      return dynamic_cast<LanesLexicon *>(widget);
  }
  return NULL;
}
HistoryMaster * getHistory() {
  LanesLexicon * app = getApp();
  return app->history();
}
NoteMaster * getNotes() {
  LanesLexicon * app = getApp();
  return app->notes();
}
int main(int argc, char *argv[])
{
    Lexicon a(argc, argv);
    //    if ( ! a.isOk() ) {
    //      return 0;
    //    }
    QCommandLineParser parser;
    parser.setApplicationDescription("This is Lane's Arabic-English Lexicon");
    parser.addHelpOption();
    parser.addVersionOption();

     // An option with a value
    QCommandLineOption nodeOption(QStringList() <<"n" << "node","make the given node the initial display","node");
    parser.addOption(nodeOption);

    QCommandLineOption rootOption(QStringList() <<"r" << "root","make the given root the initial display","node");
    parser.addOption(rootOption);

    QCommandLineOption configOption(QStringList() <<"c" << "config","use the given INI file","config");
    parser.addOption(configOption);

    QCommandLineOption fontOption(QStringList() << "f" << "fonts","List your systems Arabic fonts");
    parser.addOption(fontOption);

    QsLogging::Logger& logger = QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::TraceLevel);
    const QString sLogPath(QDir(a.applicationDirPath()).filePath("log.txt"));
    /// path, rotatation enabled,bytes to rotate after,nbr of old logs to keep
   QsLogging::DestinationPtr fileDestination(
      QsLogging::DestinationFactory::MakeFileDestination(sLogPath, true, 512 * 64, 5) );
   QsLogging::DestinationPtr debugDestination(
      QsLogging::DestinationFactory::MakeDebugOutputDestination() );
   logger.addDestination(debugDestination);
   logger.addDestination(fileDestination);

   QLOG_INFO() << "Program started";
   QLOG_INFO() << "Built with Qt" << QT_VERSION_STR << "running on" << qVersion();
 // Process the actual command line arguments given by the user
    parser.process(a);

    const QStringList args = parser.positionalArguments();
    // source is args.at(0), destination is args.at(1)


    //    progOptions.node = parser.value(nodeOption);
    //    QFontDatabase::addApplicationFont("fonts/amiri/amiri-regular.ttf");


    if (parser.isSet(fontOption) ) {
      QFontDatabase fdb;
      QStringList fonts = fdb.families(QFontDatabase::Arabic);
      std::cout << "Arabic fonts available:" << std::endl;
      std::cout << "=======================" << std::endl;
      for(int i=0;i < fonts.size(); i++) {
        std::cout << "\t" << fonts[i].toLocal8Bit().data() << std::endl;
      }
      return 0;
    }
    QString configFile;
    if (parser.isSet(configOption)) {
        configFile = parser.value(configOption);
        a.setConfig(configFile);
    }
    else {
      a.setConfig("default.ini");
    }
    int ret;
    SplashScreen * splash = 0;
    QSettings * settings = a.getSettings();
    settings->beginGroup("Splash");
    int splashDelay = settings->value("Delay",5).toInt();
    QString splashDir = settings->value("Location","images/splash").toString();
    bool makeSplash = settings->value("Enabled",true).toBool();
    delete settings;
    QDir d(splashDir);
    if (! d.exists()) {
      makeSplash = false;
    }
    if (makeSplash) {

      QStringList images = d.entryList(QStringList() << "*.png" << "*.jpg");
      int ix = -1;
      if (images.size() > 0) {
        ix = random_in_range(0,images.size() - 1);
        QString f = d.absolutePath()  + QDir::separator() + images[ix];
        QPixmap pixmap(f);
        splash = new SplashScreen(pixmap);
        splash->show();
      }
      else {
        makeSplash = false;
      }
    }
    a.processEvents();
    LanesLexicon *  w = new LanesLexicon;
    /*
    if (splash) {
      splash->setWidget(w);

    }
    */
    if (w->isOk()) {
      w->show();
      if (makeSplash) {
        splash->setWidget(w);
        QTimer::singleShot(splashDelay * 1000, splash, SLOT(pausedFinish()));
      //        splash->finish(w);
         }
      ret = a.exec();
      //      delete w;
    }
    else {
      //delete w;
      a.quit();
      return 0;
    }
    return ret;
}
