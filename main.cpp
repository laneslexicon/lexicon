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
#include "optionsdialog.h"
#include "externs.h"
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
Lexicon * getLexicon() {
  return qobject_cast<Lexicon *>(qApp);
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
void statusMessage(const QString & t) {
  getApp()->setStatus(t);
}
void viewLogsMessage() {
  getApp()->setStatus(QObject::tr("An error occurred, please see the log files for details."));
}
int main(int argc, char *argv[])
{
    Lexicon mansur(argc, argv);
    QCommandLineParser parser;
    parser.setApplicationDescription("\nEdward William Lane's Arabic-English Lexicon\n\nAny options specifiying a relative file path, will be relative to the Resources directory.");
    parser.addHelpOption();
    parser.addVersionOption();
     // An option with a value
    QCommandLineOption nodeOption(QStringList() <<"n" << "node",QObject::tr("Show the supplied node"),"node");
    parser.addOption(nodeOption);

    QCommandLineOption rootOption(QStringList() <<"r" << "root",QObject::tr("Show the supplied root"),"root");
    parser.addOption(rootOption);

    QCommandLineOption configOption(QStringList() <<"c" << "config",QObject::tr("Run configuration"));
    parser.addOption(configOption);

    QCommandLineOption fontOption(QStringList() << "f" << "fonts",QObject::tr("List your systems Arabic fonts"));
    parser.addOption(fontOption);

    QCommandLineOption dbOption(QStringList() << "d" << "dbname",QObject::tr("Use the specified database"),"db");
    parser.addOption(dbOption);


    QCommandLineOption langOption(QStringList() <<"e" << "lang",QObject::tr("Use the specified language"),"lang");
    parser.addOption(langOption);

    QCommandLineOption nosaveOption(QStringList() << "x" << "no-save",QObject::tr("Do not save the settings"));
    parser.addOption(nosaveOption);

    QCommandLineOption notabsOption(QStringList() << "s" << "no-restore",QObject::tr("Do not restore tabs"));
    parser.addOption(notabsOption);

    QCommandLineOption themeOption(QStringList() << "t" << "theme",QObject::tr("Use given theme"),"theme");
    parser.addOption(themeOption);

    QCommandLineOption textWidthOption(QStringList() << "w" << "text-width",QObject::tr("Set textwidth"),"textwidth");
    parser.addOption(textWidthOption);

    QCommandLineOption nosplashOption(QStringList() << "p" << "no-splash",QObject::tr("Do not show splash screen"));
    parser.addOption(nosplashOption);

    QCommandLineOption logOption(QStringList() << "l" << "log-level",QObject::tr("Set logging level 0 (most) - 6 (none)"),"loglevel");
    parser.addOption(logOption);

    QCommandLineOption enableTestOption(QStringList() << "b" << "enable-test",QObject::tr("enable test"));
    parser.addOption(enableTestOption);

    parser.addOption(enableTestOption);
    QCommandLineOption noRestoreState(QStringList() << "a" << "no-restore-state",QObject::tr("Do not restore saved state"));

    parser.addOption(noRestoreState);
    // Process the actual command line arguments
    parser.process(mansur);
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
    /// store the options we were interested in
    /// and pass them to the Lexicon
    QMap<QString,QString> options;
    if (parser.isSet(nodeOption)) {
      options.insert(nodeOption.valueName(),parser.value(nodeOption));
    }
    if (parser.isSet(rootOption)) {
      options.insert(rootOption.valueName(),parser.value(rootOption));
    }
    /*
    if (parser.isSet(configOption)) {
      options.insert(configOption.valueName(),parser.value(configOption));
    }
    */
    if (parser.isSet(dbOption)) {
      options.insert(dbOption.valueName(),parser.value(dbOption));
    }
    if (parser.isSet(langOption)) {
      options.insert(langOption.valueName(),parser.value(langOption));
    }
    if (parser.isSet(themeOption)) {
      options.insert(themeOption.valueName(),parser.value(themeOption));
    }
    if (parser.isSet(nosaveOption)) {
      options.insert("nosave","");
    }
    if (parser.isSet(notabsOption)) {
      options.insert("notabs","");
    }
    if (parser.isSet(logOption)) {
      options.insert("loglevel",parser.value(logOption));
    }
    if (parser.isSet(textWidthOption)) {
      options.insert(textWidthOption.valueName(),parser.value(textWidthOption));
    }
    if (parser.isSet(enableTestOption)) {
      options.insert("enabletest","");
    }
    if (parser.isSet(noRestoreState)) {
      options.insert("nostate","");
    }
    mansur.setOptions(options);
    if (parser.isSet(themeOption)) {
      mansur.setTheme(parser.value(themeOption));
    }
    mansur.startLogging();
    /// we should be in the Resources directory
    mansur.scanForFonts(QDir("fonts"));
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
      mansur.installTranslator(&translator);
    }
    else if (options.contains("lang")) {
      QLOG_WARN() << QString(QObject::tr("Could not find translation file %1 for language %2")).arg(trfile).arg(options.value("lang"));
    }
    int ret;
    SplashScreen * splash = 0;
    int splashDelay = 0;
    bool makeSplash = false;
    QString splashDir = mansur.getResourceFilePath(Lexicon::Splash);
    if (splashDir.isEmpty() || ! QFileInfo::exists(splashDir)) {
      makeSplash = false;
    }
    else {
      SETTINGS

      settings.beginGroup("Splash");
      splashDelay = settings.value(SID_SPLASH_DELAY,5).toInt();
      makeSplash = settings.value(SID_SPLASH_ENABLED,true).toBool();

      if (parser.isSet(nosplashOption)) {
        makeSplash = false;
      }
    }
    if (parser.isSet(configOption)) {
      makeSplash = false;
    }
    if (makeSplash) {
      QDir d(splashDir);
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
        splash->setWindowTitle(QObject::tr("Lane's Arabic-English Lexicon"));
        splash->show();
      }
      else {
        makeSplash = false;
      }
    }
    mansur.processEvents();
    if (parser.isSet(configOption)) {
      OptionsDialog * d = new OptionsDialog(mansur.currentTheme());
      d->show();
      ret = mansur.exec();
    }
    else {
      LanesLexicon *  w = new LanesLexicon;
      if (w->isOk()) {
        w->show();
        if (makeSplash) {
          splash->setWidget(w);
          QTimer::singleShot(splashDelay * 1000, splash, SLOT(pausedFinish()));
        }
        ret = mansur.exec();
      }
      else {
        mansur.quit();
        return 0;
      }
    }
    return ret;
}
