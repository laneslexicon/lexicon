#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include "QsLog.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
   // init the logging mechanism
   QsLogging::Logger& logger = QsLogging::Logger::instance();
   logger.setLoggingLevel(QsLogging::TraceLevel);
   const QString sLogPath(QDir(a.applicationDirPath()).filePath("log.txt"));

   QsLogging::DestinationPtr fileDestination(
      QsLogging::DestinationFactory::MakeFileDestination(sLogPath, true, 512, 2) );
   QsLogging::DestinationPtr debugDestination(
      QsLogging::DestinationFactory::MakeDebugOutputDestination() );
   logger.addDestination(debugDestination);
   logger.addDestination(fileDestination);
    MainWindow w;
    w.show();
    return a.exec();
}
