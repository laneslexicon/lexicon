#include "laneslexicon.h"
#include <QApplication>
#include <QFontDatabase>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include "QsLog.h"
#include "QsLogDest.h"
cmdOptions progOptions;
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
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Gabanjo");
    QCoreApplication::setOrganizationDomain("nowhere.com");
    QCoreApplication::setApplicationName("Lanes Lexicon");
    QCoreApplication::setApplicationVersion("0.1");

    QCommandLineParser parser;
    parser.setApplicationDescription("This is Lane's Arabic-English Lexicon");
    parser.addHelpOption();
    parser.addVersionOption();

    // A boolean option with a single name (-p)
    QCommandLineOption dumpOption("p", QCoreApplication::translate("main", "Show progress during copy"));
    parser.addOption(dumpOption);
     // An option with a value
    QCommandLineOption nodeOption(QStringList() <<"n" << "node","make the given node the initial display","node");
    parser.addOption(nodeOption);
    QCommandLineOption fontOption("f","List your systems Arabic fonts");
    parser.addOption(fontOption);
    //    a.setStyleSheet(".ar { font-family : Amiri;font-size : 16px}");

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


    progOptions.dump = parser.isSet(dumpOption);
    progOptions.node = parser.value(nodeOption);
    if (parser.isSet(fontOption) ) {
      QFontDatabase fdb;
      qDebug() << fdb.families(QFontDatabase::Arabic);
      return 0;
    }
    QFontDatabase::addApplicationFont("./resources/fonts/amiri/amiri-regular.ttf");
    //    qDebug() << "node = " << node << dump;
    //    qDebug() << "args = " << args;
    int ret;
    LanesLexicon * w = new LanesLexicon;
    if (w->isOk()) {
      w->show();
      ret = a.exec();
      delete w;
    }
    else {
      delete w;
      a.quit();
      return 0;
    }
    return ret;
}
