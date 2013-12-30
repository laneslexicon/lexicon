#include "laneslexicon.h"
#include <QApplication>
#include "QsLog.h"
#include "QsLogDest.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
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

    LanesLexicon w;
    w.show();
    return a.exec();
}
