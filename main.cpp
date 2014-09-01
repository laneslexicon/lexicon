#include "laneslexicon.h"
#include <QApplication>
#include <QFontDatabase>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QSplashScreen>
#include <QImageReader>
#include <QTimer>
#include <QPixmap>
#include <QLocale>
#include <QTranslator>
#include "QsLog.h"
#include "QsLogDest.h"
#include "application.h"
#include "splashscreen.h"
#include "definedsettings.h"
#include <iostream>
#ifdef __APPLE__
int RandomUnder(int topPlusOne)
{
  srandomdev();
  unsigned two31 = 1U << 31;
  unsigned maxUsable = (two31 / topPlusOne) * topPlusOne;

  while(1)
    {
      unsigned num = random();
      if(num < maxUsable)
        return num % topPlusOne;
    }
}
#endif

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
    return random_in_range(min, max);
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
QSettings * getSettings() {
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  return app->getSettings();
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

    QCommandLineOption rootOption(QStringList() <<"r" << "root","make the given root the initial display","root");
    parser.addOption(rootOption);

    QCommandLineOption configOption(QStringList() <<"c" << "config","use the given INI file","config");
    parser.addOption(configOption);

    QCommandLineOption fontOption(QStringList() << "f" << "fonts","List your systems Arabic fonts");
    parser.addOption(fontOption);

    QCommandLineOption dbOption(QStringList() << "d" << "dbname","use the specified database","db");
    parser.addOption(dbOption);


    QCommandLineOption langOption(QStringList() <<"l" << "lang","use the specified language","lang");
    parser.addOption(langOption);

    QCommandLineOption nosaveOption(QStringList() << "x" << "no-save","do not save the settings");
    parser.addOption(nosaveOption);

    QCommandLineOption notabsOption(QStringList() << "t" << "no-tabs","do not restore tabs");
    parser.addOption(notabsOption);

    QCommandLineOption textWidthOption(QStringList() << "w" << "text-width","set textwidth","textwidth");
    parser.addOption(textWidthOption);

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
   // Process the actual command line arguments
    parser.process(a);

    const QStringList args = parser.positionalArguments();

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
    /// TODO remove this
    /// and have Lexicon use the m_options values
    QString configFile;
    if (parser.isSet(configOption)) {
        configFile = parser.value(configOption);
        a.setConfig(configFile);
    }
    else {
      a.setConfig("default.ini");
    }
    QMap<QString,QString> options;
    if (parser.isSet(nodeOption)) {
      options.insert(nodeOption.valueName(),parser.value(nodeOption));
    }
    if (parser.isSet(rootOption)) {
      options.insert(rootOption.valueName(),parser.value(rootOption));
    }
    if (parser.isSet(configOption)) {
      options.insert(configOption.valueName(),parser.value(configOption));
    }
    if (parser.isSet(dbOption)) {
      options.insert(dbOption.valueName(),parser.value(dbOption));
    }
    if (parser.isSet(langOption)) {
      options.insert(langOption.valueName(),parser.value(langOption));
    }
    if (parser.isSet(nosaveOption)) {
      options.insert("nosave","");
    }
    if (parser.isSet(notabsOption)) {
      options.insert("notabs","");
    }
    if (parser.isSet(textWidthOption)) {
      options.insert(textWidthOption.valueName(),parser.value(textWidthOption));
    }
    a.setOptions(options);
    qDebug() << options;
    QTranslator translator;
    QString trfile;
    if (options.contains("lang")) {
      trfile = QString("laneslexicon_%1.qm").arg(options.value("lang"));
    }
    else {
      trfile = QString("laneslexicon_%1.qm").arg(QLocale::system().name());
    }
    QFileInfo fi(trfile);
    if (fi.exists()) {
      translator.load(trfile);
      a.installTranslator(&translator);
    }
    else {
      QLOG_DEBUG() << "Could not find translation file" << trfile;
    }
    int ret;
    SplashScreen * splash = 0;
    QSettings * settings = a.getSettings();
    settings->beginGroup("Splash");
    int splashDelay = settings->value(SID_SPLASH_DELAY,5).toInt();
    QString splashDir = settings->value(SID_SPLASH_LOCATION,"images/splash").toString();
    bool makeSplash = settings->value(SID_SPLASH_ENABLED,true).toBool();
    delete settings;
    QDir d(splashDir);
    if (! d.exists()) {
      makeSplash = false;
    }
    if (makeSplash) {
      QList<QByteArray> formats =  QImageReader::supportedImageFormats();
      QStringList m;
      for(int i=0;i < formats.size();i++) {
        m << "*." + QString(formats[i]);
        m << "*." + QString(formats[i]).toUpper();
      }
      QStringList images = d.entryList(m);
      int ix = -1;
      if (images.size() > 0) {

#ifdef __APPLE__
        ix = RandomUnder(images.size());
#else
        ix = random_in_range(0,images.size() - 1);
#endif
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
