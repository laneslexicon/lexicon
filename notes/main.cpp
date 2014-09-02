#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include "QsLog.h"
#include "QsLogDest.h"
#ifdef __APPLE__
int random_in_range(unsigned int min, unsigned int max) {
  return RandomUnder(max);
}
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
#else
int random_in_range(unsigned int min, unsigned int max)
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
#endif
NoteMaster * getNotes() {
  foreach(QWidget *widget, qApp->topLevelWidgets()) {
    if(widget->inherits("QMainWindow")) {
      MainWindow * w  = dynamic_cast<MainWindow *>(widget);
      return w->getNotes();
    }
  }
  return NULL;
}
QSettings  * getSettings() {
  foreach(QWidget *widget, qApp->topLevelWidgets()) {
    if(widget->inherits("QMainWindow")) {
      MainWindow * w  = dynamic_cast<MainWindow *>(widget);
      return w->getSettings();
    }
  }
  return NULL;
}

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
    MainWindow w;
    w.show();
    return a.exec();
}
