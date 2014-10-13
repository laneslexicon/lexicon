#include "application.h"
#include "QsLog.h"
#include "definedsettings.h"
#include "version.h"
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
  QDir fonts(resourceDir + "/fonts");
  if (! fonts.exists()) {
    QLOG_DEBUG() << "No font directory";
  }
  else {
    scanForFonts(fonts);
  }

  addLibraryPath(resourceDir + "/lib");

  QCoreApplication::setOrganizationName("Gabanjo");
  QCoreApplication::setOrganizationDomain("theunwalledcity.com");
  QCoreApplication::setApplicationName("Lanes Lexicon");
  QCoreApplication::setApplicationVersion(buildVersion());

  connect(this,SIGNAL(focusChanged(QWidget *,QWidget *)),this,SLOT(onFocusChange(QWidget *,QWidget *)));
}
void Lexicon::startLogging() {
  QSettings * settings = getSettings();
  settings->beginGroup("Logging");
  QString logfile = settings->value(SID_LOGGING_FILE,"log.txt").toString();
  int loglevel = settings->value(SID_LOGGING_LEVEL,2).toInt();
  int maxsize = settings->value(SID_LOGGING_MAXSIZE,64000).toInt();
  int archiveCount = settings->value(SID_LOGGING_ARCHIVES,4).toInt();
  bool rotate = settings->value(SID_LOGGING_ROTATE,true).toBool();
  delete settings;

    QsLogging::Logger& logger = QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::TraceLevel);
    switch(loglevel) {
    case 0  : { logger.setLoggingLevel(QsLogging::TraceLevel) ; break; }
    case 1  : { logger.setLoggingLevel(QsLogging::DebugLevel) ; break; }
    case 2  : { logger.setLoggingLevel(QsLogging::InfoLevel) ; break; }
    case 3  : { logger.setLoggingLevel(QsLogging::WarnLevel) ; break; }
    case 4  : { logger.setLoggingLevel(QsLogging::ErrorLevel) ; break; }
    case 5  : { logger.setLoggingLevel(QsLogging::FatalLevel) ; break; }
    case 6  : { logger.setLoggingLevel(QsLogging::OffLevel) ; break; }
    default : { logger.setLoggingLevel(QsLogging::InfoLevel) ; break; }
    }
    //    const QString sLogPath(QDir(applicationDirPath()).filePath(logfile));
    const QString sLogPath(QDir::current().filePath(logfile));
    /// path, rotatation enabled,bytes to rotate after,nbr of old logs to keep
   QsLogging::DestinationPtr fileDestination(
      QsLogging::DestinationFactory::MakeFileDestination(sLogPath, rotate, maxsize, archiveCount) );
   QsLogging::DestinationPtr debugDestination(
      QsLogging::DestinationFactory::MakeDebugOutputDestination() );
   logger.addDestination(debugDestination);
   logger.addDestination(fileDestination);

   QLOG_INFO() << "Program started";
   QLOG_INFO() << "Built with Qt" << QT_VERSION_STR << "running on" << qVersion();
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

void Lexicon::setOptions(const QMap<QString,QString> & options) {
  m_options = options;
  if (m_options.contains("config")) {
    m_configFile = m_options.value("config");
  }
  else {
    m_configFile = "default.ini";
  }
}
QMap<QString,QString> Lexicon::getOptions() const  {
  return m_options;
}

QSettings * Lexicon::getSettings() {
  if (m_configFile.isEmpty()) {
    return new QSettings;
  }
  QSettings * settings = new QSettings(m_configFile,QSettings::IniFormat);
  settings->setIniCodec("UTF-8");
  return settings;
}
QVariant Lexicon::getValue(const QString & group,const QString & key) {
  QSettings s(m_configFile,QSettings::IniFormat);
  s.setIniCodec("UTF-8");
  s.beginGroup(group);
  return s.value(key);
}
bool Lexicon::getBool(const QString & group,const QString & key) {
  QSettings s(m_configFile,QSettings::IniFormat);
  s.setIniCodec("UTF-8");
  s.beginGroup(group);
  return s.value(key).toBool();
}
void Lexicon::onFocusChange(QWidget * old, QWidget * now) {
  if (old && now) {
    QLOG_DEBUG() << old->metaObject()->className() << "--->" << now->metaObject()->className();
  }
}
void Lexicon::scanForFonts(const QDir & dir)
{
  QDir d(dir);
  QStringList filters;
  QStringList fonts;
  filters << "*.otf" << "*.ttf";
  d.setNameFilters(filters);

  fonts << d.entryList();

   QDirIterator iterator(d.absolutePath(), QDirIterator::Subdirectories);
   while (iterator.hasNext()) {
      iterator.next();
      QDir d(iterator.path());
      //      d.setNameFilters(filters);
      if (! iterator.fileInfo().isDir()) {
        QString f = iterator.filePath(); //<< d.entryList();
        if (f.endsWith(".ttf") || f.endsWith(".otf")) {
          fonts << f;

        }
      }
   }
   while(fonts.size() > 0) {
     QFontDatabase::addApplicationFont(fonts.takeFirst());
   }
}
QString Lexicon::spanArabic(const QString & ar) {
  QSettings s(m_configFile,QSettings::IniFormat);
  s.setIniCodec("UTF-8");
  s.beginGroup("Arabic");
  QString fontname = s.value(SID_ARABIC_FONT_NAME,QString()).toString();
  QString fontsize = s.value(SID_ARABIC_FONT_SIZE,QString()).toString();
  QString style;
  if ( ! fontname.isEmpty()) {
    style = QString("font-family : %1").arg(fontname);
  }
  if (! fontsize.isEmpty()) {
    if (! style.isEmpty()) {
      style += ";";
    }
    style += QString("font-size : %1").arg(fontsize);
  }
  return QString("<span style=\"%1\">%2</span>").arg(style).arg(ar);
}
