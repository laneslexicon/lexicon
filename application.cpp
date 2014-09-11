#include "application.h"
#include "QsLog.h"
#include "definedsettings.h"
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
    qDebug() << "No font directory";
  }
  else {
    scanForFonts(fonts);
  }

  addLibraryPath(resourceDir + "/lib");

  QCoreApplication::setOrganizationName("Gabanjo");
  QCoreApplication::setOrganizationDomain("theunwalledcity.com");
  QCoreApplication::setApplicationName("Lanes Lexicon");
  QCoreApplication::setApplicationVersion("0.1");

  connect(this,SIGNAL(focusChanged(QWidget *,QWidget *)),this,SLOT(onFocusChange(QWidget *,QWidget *)));
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
