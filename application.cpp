#include "application.h"
#include "QsLog.h"
#include "definedsettings.h"
#include "version.h"
Lexicon::Lexicon(int & argc, char ** argv) : QApplication(argc,argv) {
  QString resourceDir;
  m_status = Lexicon::Ok;
  m_configFile = "settings.ini";
#ifdef __APPLE__
  resourceDir = QCoreApplication::applicationDirPath() + "/../Resources";
#else
  resourceDir = QCoreApplication::applicationDirPath() + QDir::separator() + "Resources";
#endif

  if ( ! QDir::setCurrent(resourceDir)) {
    QString errmsg  = QString(QObject::tr("Warning failed to change application working directory to : %1")).arg(resourceDir);
    std::cout << errmsg.toLocal8Bit().data() << std::endl;
    m_status = Lexicon::ResourceDirError;
  }
  QDir fonts(resourceDir + QDir::separator() + "fonts");
  if (fonts.exists()) {
    scanForFonts(fonts);
  }

  addLibraryPath(resourceDir + QDir::separator() + "lib");

  QCoreApplication::setOrganizationName("Gabanjo");
  QCoreApplication::setOrganizationDomain("theunwalledcity.com");
  QCoreApplication::setApplicationName("Lanes Lexicon");
  QCoreApplication::setApplicationVersion(buildVersion());

  connect(this,SIGNAL(focusChanged(QWidget *,QWidget *)),this,SLOT(onFocusChange(QWidget *,QWidget *)));

  QScopedPointer<QSettings> settings(new QSettings("config.ini",QSettings::IniFormat));
  settings->beginGroup("System");
  m_themeDirectory = settings->value("Theme directory","themes").toString();
  m_currentTheme =  settings->value("Theme","oxygen").toString();
  /// check the theme directory exists
  QDir d = QDir::current();
  if (! d.cd(m_themeDirectory)) {
    QString errmsg  = QString(QObject::tr("Warning: Theme directory not found : %1")).arg(m_themeDirectory);
    std::cout << errmsg.toLocal8Bit().data() << std::endl;
    m_status = Lexicon::NoThemeDirectory;
    return;
  }
  /// check the theme exists
  if (!d.cd(m_currentTheme)) {
    QString errmsg  = QString(QObject::tr("Warning: specified theme directory not found : %1")).arg(m_currentTheme);
    std::cout << errmsg.toLocal8Bit().data() << std::endl;
    m_status = Lexicon::ThemeNotFound;
    return;
  }
  m_settingsDir = d;
}
bool Lexicon::isOk() const {
  return (m_status == Lexicon::Ok);
}
/**
 * Returns the absolute path to the requested file
 *
 * @param type
 * @param name If empty return the path to the directory of the required item type
 *
 * @return
 */
QString Lexicon::getResourcePath(int type) {
  QFile file;
  QFileInfo f(m_settingsDir,m_configFile);
  QSettings settings(f.absoluteFilePath(),QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup("Resources");
  QString d(".");
  switch(type) {
  case Lexicon::ThemeRoot : {
    QDir d = QDir::current();
    QString t;
    QScopedPointer<QSettings> settings(new QSettings("config.ini",QSettings::IniFormat));
    settings->beginGroup("System");
    t = settings->value("Theme directory","themes").toString();
    if (t.isEmpty()) {
      t = "themes";
    }
    return d.absolutePath() + QDir::separator() + t;
    break;
  }
  case Lexicon::Stylesheet : {
    d = settings.value("Stylesheet","css").toString();
    if (d.isEmpty()) {
      d = "css";
    }
    break;
  }
  case Lexicon::Image : {
    d = settings.value("Image","images").toString();
    if (d.isEmpty()) {
      d = "images";
    }
    break;
  }
  case Lexicon::XSLT : {
    d = settings.value("XSLT","xslt").toString();
    if (d.isEmpty()) {
      d = "xslt";
    }
    break;
  }
  case Lexicon::Keyboard : {
    d = settings.value("Keyboard","keyboards").toString();
    if (d.isEmpty()) {
      d = "keyboards";
    }
    break;
  }
  case Lexicon::Map : {
    d = settings.value("Map","maps").toString();
    if (d.isEmpty()) {
      d = "maps";
    }
    break;
  }
  case Lexicon::Splash : {
    d = settings.value("Splash","images/splash").toString();
    if (d.isEmpty()) {
      d = "images/splash";
    }
  }
  default : { break; }
  }
  if (m_settingsDir.exists(d)) {
    QDir rd = m_settingsDir;
    rd.cd(d);
    return rd.absolutePath();
  }
  else {
    m_errors << QString(tr("Settings directory not found: %1")).arg(m_settingsDir.absolutePath());
  }
  return QString();
}
QString Lexicon::getResourceFilePath(int type, const QString & name) {
  QFile file;
  QFileInfo f(m_settingsDir,m_configFile);
  QSettings settings(f.absoluteFilePath(),QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup("Resources");
  QString d(".");
  switch(type) {
  case Lexicon::ThemeRoot : {
    QDir d = QDir::current();
    QString t;
    QScopedPointer<QSettings> settings(new QSettings("config.ini",QSettings::IniFormat));
    settings->beginGroup("System");
    t = settings->value("Theme directory","themes").toString();
    if (t.isEmpty()) {
      t = "themes";
    }
    return d.absolutePath() + QDir::separator() + t + QDir::separator() + name;
    break;
  }
  case Lexicon::Stylesheet : {
    d = settings.value("Stylesheet","css").toString();
    if (d.isEmpty()) {
      d = "css";
    }
    break;
  }
  case Lexicon::Image : {
    d = settings.value("Image","images").toString();
    if (d.isEmpty()) {
      d = "images";
    }
    break;
  }
  case Lexicon::XSLT : {
    d = settings.value("XSLT","xslt").toString();
    if (d.isEmpty()) {
      d = "xslt";
    }
    break;
  }
  case Lexicon::Keyboard : {
    d = settings.value("Keyboard","keyboards").toString();
    if (d.isEmpty()) {
      d = "keyboards";
    }
    break;
  }
  case Lexicon::Map : {
    d = settings.value("Map","maps").toString();
    if (d.isEmpty()) {
      d = "maps";
    }
    break;
  }
  case Lexicon::Splash : {
    d = settings.value("Splash","images/splash").toString();
    if (d.isEmpty()) {
      d = "images/splash";
    }
  }
  default : { break; }
  }
  if (m_settingsDir.exists(d)) {
    QDir rd = m_settingsDir;
    rd.cd(d);
    if (name.isEmpty()) {
      return rd.absolutePath();
    }
    QFileInfo r(rd,name);
    if (r.exists()) {
      return r.absoluteFilePath();
    }
    else {
      m_errorFilePath = rd.absolutePath();
      m_errorFile = name;
      m_errors << QString(tr("Resource not found: %1")).arg(name);
    }
  }
  else {
    m_errors << QString(tr("Settings directory not found: %1")).arg(m_settingsDir.absolutePath());
  }
  return QString();
}
QString Lexicon::takeLastError() {
  if (m_errors.size() > 0) {
    return m_errors.takeLast();
  }
  return QString();
}
/**
 * This is not required
 *
 *
 * @return
 */
QString Lexicon::imageDirectory() {
  QLOG_DEBUG() << Q_FUNC_INFO << "NOSHOW we should not be here";
  QFileInfo f(m_settingsDir,m_configFile);
  QSettings settings(f.absoluteFilePath(),QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup("Icons");
  QString imageDirectory = settings.value("Directory","images").toString();
  QFileInfo img(m_settingsDir,imageDirectory);
  return img.absoluteFilePath();

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
  if (m_options.contains("loglevel")) {
    QString v = m_options.value("loglevel");
    bool ok = true;
    int x = v.toInt(&ok);
    if (ok) {
      loglevel = x;
    }
  }
  /// we need some settings that make sense
  if (logfile.isEmpty()) {
    logfile = "log.txt";
  }
  if (maxsize < 1000) {
    maxsize = 64000;
  }
    QsLogging::Logger& logger = QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::OffLevel);
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

   QLOG_INFO() << "Built with Qt" << QT_VERSION_STR << "running on" << qVersion();
   QLOG_INFO() << "Program started";
}
QString Lexicon::getConfig() const {
  return m_configFile;
}
/*
void Lexicon::setConfig(const QString & fileName) {
  QFileInfo fi(fileName);
  if (fi.exists()) {
    m_configFile = fileName;
  }
}
*/
int Lexicon::setTheme(const QString & theme) {
  QDir d = QDir::current();
  if (! d.cd(m_themeDirectory)) {
    return Lexicon::NoThemeDirectory;
  }
  if (! d.cd(theme)) {
    return Lexicon::ThemeNotFound;
  }
  if (! d.exists(m_configFile)) {
    return Lexicon::SettingsNotFound;
  }
  m_currentTheme = theme;
  m_settingsDir = d;

  QScopedPointer<QSettings> settings(new QSettings("config.ini",QSettings::IniFormat));
  settings->beginGroup("System");
  settings->setValue("Theme",theme);
  return Lexicon::Ok;
}
void Lexicon::setOptions(const QMap<QString,QString> & options) {
  m_options = options;
  if (m_options.contains("config")) {
    m_configFile = m_options.value("config");
  }
  else {
    m_configFile = "settings.ini";
  }
}
QMap<QString,QString> Lexicon::getOptions() const  {
  return m_options;
}

QSettings * Lexicon::getSettings() {
  if (m_configFile.isEmpty()) {
    return new QSettings;
  }
  QFileInfo f(m_settingsDir,m_configFile);
  QSettings * settings = new QSettings(f.absoluteFilePath(),QSettings::IniFormat);
  settings->setIniCodec("UTF-8");
  return settings;
}
/**
 * returns the name of imeditor's ini file
 *
 * @param theme
 *
 * @return
 */
QString Lexicon::editorSettingsFileName(const QString & theme) {
  if (theme.isEmpty()) {
    QFileInfo f(m_settingsDir,"editor.ini");
    return f.absoluteFilePath();
  }
  QDir d = QDir::current();
  if (! d.cd(m_themeDirectory)) {
    return QString();
  }
  return d.absoluteFilePath("editor.ini");
}
/**
 * Returns the filename of the active settings file if no theme is supplied
 * otherwise returns the filename of supplied theme
 *
 * @param theme
 *
 * @return
 */
QString Lexicon::settingsFileName(const QString & theme) {
  if (theme.isEmpty()) {
    QFileInfo f(m_settingsDir,m_configFile);
    return f.absoluteFilePath();
  }
  QDir d = QDir::current();
  if (! d.cd(m_themeDirectory)) {
    return QString();
  }
  if (! d.cd(theme)) {
    return QString();
  }
  if (! d.exists(m_configFile)) {
    return QString();
  }
  return d.absoluteFilePath(m_configFile);
}
QVariant Lexicon::getValue(const QString & group,const QString & key) {
  QFileInfo f(m_settingsDir,m_configFile);
  QSettings s(f.absoluteFilePath(),QSettings::IniFormat);
  s.setIniCodec("UTF-8");
  s.beginGroup(group);
  return s.value(key);
}
bool Lexicon::getBool(const QString & group,const QString & key) {
  QFileInfo f(m_settingsDir,m_configFile);
  QSettings s(f.absoluteFilePath(),QSettings::IniFormat);
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
/**
 * This is for mixed language text and will wrap any Arabic in a <span>
 * with the class given by the css parameter. It embeds the style information
 * in the "class" attribute and so can only be used widgets that support
 * stylesheets.
 *
 * @param str
 * @param css
 *
 * @return
 */
QString Lexicon::scanAndSpan(const QString & str,const QString & css) {
  QString teststr("You say, كَتَبَ إِلَىَّ يَسْتَبْطِئُنِى He wrote وَجَدَ هَ          and thats it");
  QString ar = str;
  //  ar = teststr;
  bool inArabic = false;
  QString html;
  for(int i=0;i < ar.size();i++) {
    QChar c = ar.at(i);
    if ((c.unicode() >= 0x600) && (c.unicode() <= 0x6ff)) {
      if (! inArabic) {
        html.append(QString("<span class=\"%1\">").arg(css));
      }
      html.append(c);
      inArabic = true;
    }
    else if (c.isPunct() || c.isNumber()) {
      html.append(c);
    }
    else if (c.isSpace()) {
      if (inArabic) {
        bool arabicBlock = true;
        for(int j=i+1;arabicBlock && (j < ar.size());j++) {
          QChar sp = ar.at(j);
          if ( sp.isLetter() ) {
            if ((sp.unicode() >= 0x600) && (sp.unicode() <= 0x6ff)) {
              j = ar.size();
            }
            else {
              arabicBlock = false;
            }
          }
        }
        if (! arabicBlock ) {
          html.append("</span>");
          html.append(c);
          inArabic = false;
        }
        else {
          html.append(c);
        }
      }
      else {
        html.append(c);
      }
    }
    else {
           if (inArabic) {
                   html.append("</span>");
                   inArabic = false;
                 }
      html.append(c);
    }
  }
  if (inArabic) {
    html.append("</span>");
  }
  return html;
}
/**
 * This is used to wrap Arabic only text. It embeds the style information
 * in the "style" attribute - it does NOT use CSS classes because for some
 * widgets these are not available.
 *
 *
 * The spanstyle parameter should be a key in the spanArabic section
 * of the INI file. This allows for different font/sizes to be used.
 *
 * If no spanstyle supplied it looks at the "System/Arabic font" settings and get the default
 * font and size from there.

 * @param ar                the text to wrap
 * @param style
 *
 * @return
 */
QString Lexicon::spanArabic(const QString & ar,const QString & spanstyle) {
  QFileInfo f(m_settingsDir,m_configFile);
  QSettings s(f.absoluteFilePath(),QSettings::IniFormat);
  QString fontFamily;
  int fontSize = 10;
  QString style;
  s.setIniCodec("UTF-8");
  if ( ! spanstyle.isEmpty() ) {
    s.beginGroup("SpannedText");
    s.beginGroup("Arabic");
    QString style = s.value(spanstyle,QString()).toString();
    if (! style.isEmpty()) {
      return QString("<span style=\"%1\">%2</span>").arg(style).arg(ar);
    }
    s.endGroup();
    s.endGroup();
  }
  s.beginGroup("System");
  QString fontString = s.value(SID_SYSTEM_ARABIC_FONT,QString()).toString();
  if (! fontString.isEmpty()) {
    QFont f;
    f.fromString(fontString);
    fontFamily = f.family();
    int  sz = f.pixelSize();
    if (sz <= 0) {
      sz = f.pointSize();
    }
    if (sz > 0) {
      fontSize = sz;
    }
  }
  style  = QString("<span style=\"font-family : %1;font-size : %2\">%3</span>")
    .arg(fontFamily)
    .arg(fontSize)
    .arg(ar);
  return style;
  /*
  s.beginGroup("Arabic");
  QString fontname = s.value(SID_ARABIC_FONT_NAME,QString()).toString();
  QString fontsize = s.value(SID_ARABIC_FONT_SIZE,QString()).toString();
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
  */
}
QStringList Lexicon::getThemes() const {
  QDir d(m_themeDirectory);
  return d.entryList(QStringList(),QDir::NoDotAndDotDot| QDir::Dirs);

}
/**
 *
 *
 * @param type 0 - default,return name of keyboard
 *             1 - return full name of ini file
 *
 * @return
 */
QStringList Lexicon::getKeyboards(int type)  {
  QStringList keyboards;
  QString k = getResourceFilePath(Lexicon::Keyboard);
  QDir keydir(k);
  QStringList filters;
  filters << "*.ini";
  QFileInfoList info = keydir.entryInfoList(filters);
  for(int i=0;i < info.size();i++) {
    if (type == 1) {
      keyboards << info[i].absoluteFilePath();
    }
    else {
      if (info[i].fileName() != "keyboard.ini") {
        QSettings settings(info[i].absoluteFilePath(),QSettings::IniFormat);
        settings.setIniCodec("UTF-8");
        settings.beginGroup("Header");
        keyboards << settings.value("name","<Unnamed keyboard>").toString();
      }
    }
  }
  return keyboards;
}
bool Lexicon::setDefaultKeyboard(const QString & name) {
  QString k = getResourceFilePath(Lexicon::Keyboard);
  QString currentKeyboard;
  QDir keydir(k);
  QStringList filters;
  filters << "*.ini";
  QFileInfoList info = keydir.entryInfoList(filters);
  for(int i=0;i < info.size();i++) {
    if (info[i].fileName() == "keyboard.ini") {
      QSettings settings(info[i].absoluteFilePath(),QSettings::IniFormat);
      settings.setIniCodec("UTF-8");
      settings.beginGroup("System");
      settings.setValue("Default",name);
      return true;
    }
  }
  return false;
}
QString Lexicon::getDefaultKeyboard()  {
  QString k = getResourceFilePath(Lexicon::Keyboard);
  QString currentKeyboard;
  QDir keydir(k);
  QStringList filters;
  filters << "*.ini";
  QFileInfoList info = keydir.entryInfoList(filters);
  for(int i=0;i < info.size();i++) {
    if (info[i].fileName() == "keyboard.ini") {
      QSettings settings(info[i].absoluteFilePath(),QSettings::IniFormat);
      settings.setIniCodec("UTF-8");
      settings.beginGroup("System");
      return settings.value("Default",QString()).toString();
    }
  }
  return currentKeyboard;
}
/**
 * This assumes the following:
 *
 * For entries in QSettings the key contains the word arabic (ignoring case)
 * QSettings entries are either in the form that QFont::toString() produces
 * or simple CSS statements containing "font-family : <font name>"
 *
 * For entries in any of the CSS files, the css selector for any Arabic text
 * contains the word "Arabic" (ignoring case)  and that any font name is
 * in a 'font-family' clause.
 *
 * For Qt stylesheet the whole selector is scanned for 'arabic'
 *
 * All other entries will be ignored.
 *
 * @param family
 */
QStringList Lexicon::changeFontInStylesheet(const QString & fileName,const QString & family) {
  QStringList changedEntries;
  QRegularExpression reCss("(.+){(.+)}");
  QRegularExpressionMatch m;
  QStringList css;

  QFile file(fileName);
  if ( ! file.open(QIODevice::ReadOnly)) {
    return changedEntries;
  }
  QTextStream in(&file);
  in.setCodec("UTF-8");
  QString selector;
  QString clause;
  while(! in.atEnd()) {
    QString line = in.readLine();
    m = reCss.match(line);
    if (m.lastCapturedIndex() == 2) {
      selector = m.captured(1);
      clause = m.captured(2);
    }
    if (selector.contains("arabic",Qt::CaseInsensitive)) {
      QString t = setCssFont(clause,family);
      line = QString("%1 { %2 }").arg(selector,t);
      if (family.isEmpty()) {
        changedEntries << t;
      }
      else {
        changedEntries << line;
        css << line;
      }
    }
    else {
      css << line;
    }
  }
  file.close();
  if (family.isEmpty()) {
    return changedEntries;
  }
  if (! file.open(QIODevice::WriteOnly)) {
    return QStringList();
  }
  QTextStream out(&file);
  out.setCodec("UTF-8");
  for(int i=0;i < css.size();i++) {
    out << css[i] << endl;
  }
  file.close();
  return changedEntries;
}
QStringList Lexicon::changeFontInSettings(const QString & family) {
  QStringList changedKeys;
  QFileInfo f(m_settingsDir,m_configFile);
  QSettings settings(f.absoluteFilePath(),QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  QStringList keys = settings.allKeys();
  QString v;
  QFont font;
  QRegularExpression re("Arabic", QRegularExpression::CaseInsensitiveOption);
  QRegularExpression reFontString("\\d+,\\d+,\\d++");
  QRegularExpression reCssFont("font-family", QRegularExpression::CaseInsensitiveOption);

  QRegularExpressionMatch match;
  for(int i=0;i < keys.size();i++) {
    if (re.match(keys[i]).hasMatch()) {
      v = settings.value(keys[i]).toString();
      if (reFontString.match(v).hasMatch()) {
        font.fromString(v);
        if (family.isEmpty()) {
          changedKeys << font.family();
        }
        else {
          font.setFamily(family);
          v = font.toString();
          settings.setValue(keys[i],v);
          changedKeys << keys[i];
        }
      }
      else if (reCssFont.match(v).hasMatch()) {
        v  = setCssFont(v,family);
        if (family.isEmpty()) {
          changedKeys << v;
        }
        else {
          changedKeys << keys[i];
          settings.setValue(keys[i],v);
        }
      }
    }
  }
  return changedKeys;
}
/**
 * If family is empty, return the name of font-family
 *
 * @param src
 * @param family
 *
 * @return
 */
QString Lexicon::setCssFont(const QString & src,const QString & family) const {
  QRegularExpressionMatch m;
  QString r;
  QStringList parts = src.split(";",QString::SkipEmptyParts);
  for(int i=0;i < parts.size();i++) {
    parts[i] = parts[i].trimmed();
    if (! parts[i].isEmpty()) {
    QStringList kv = parts[i].split(":");
    if (kv.size() == 2) {
      QString k = kv[0];
      QString v = kv[1];
      if (kv[0].toLower().trimmed() == "font-family") {
        if (family.isEmpty()) {
          return v.trimmed();
        }
        v = "\"" + family + "\"";
      }
      r += k + ":" + v + ";";
    }
    else {
      r += parts[i] + ";";
    }
    }
  }
  return r;
}
QStringList Lexicon::getUsedFont() {
  QStringList changes = changeFontInSettings(QString());
  QDir cssDirectory(getResourceFilePath(Lexicon::Stylesheet));
  QStringList filters;
  filters << "*.css";
  QFileInfoList files =  cssDirectory.entryInfoList(filters);
  for(int i=0;i < files.size();i++) {
    changes << changeFontInStylesheet(files[i].absoluteFilePath(),QString());
  }
  for(int i=0;i < changes.size();i++) {
    changes[i] = changes[i].remove(QChar('"'));
  }
  changes.removeDuplicates();
  return changes;
}
QString Lexicon::getStylesheetFilePath(int type) {
  QString fileName;
  QFileInfo f(m_settingsDir,m_configFile);
  QSettings settings(f.absoluteFilePath(),QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  if (type == Lexicon::Application) {
    settings.beginGroup("System");
    fileName = settings.value(SID_SYSTEM_STYLESHEET,"application.css").toString();
  }
  if (type == Lexicon::Entry) {
    settings.beginGroup("Entry");
    fileName = settings.value(SID_ENTRY_CSS,"entry.css").toString();
  }
  if (type == Lexicon::Print) {
    settings.beginGroup("Entry");
    fileName = settings.value(SID_ENTRY_PRINT_CSS,"entry_print.css").toString();
  }
  if (fileName.isEmpty()) {
    return fileName;
  }
  QDir cssDirectory(getResourceFilePath(Lexicon::Stylesheet));
  f.setFile(cssDirectory,fileName);
  if (f.exists()) {
    return f.absoluteFilePath();
  }
  return QString();
}
void Lexicon::setCursorPosition(QWidget * w,int pos) {
  QList<QLineEdit *> edits = w->findChildren<QLineEdit *>();
  foreach(QLineEdit *  widget,edits) {
    widget->setCursorPosition(pos);
  }
}
QFont Lexicon::fontFromCss(const QString & src) {
  QFont f;
  QString family;
  QString fontSize;
  QString css = src;

  css.remove("{");
  css.remove("}");
  QStringList parts = css.split(";",QString::SkipEmptyParts);
  for(int i=0;i < parts.size();i++) {
    parts[i] = parts[i].trimmed();
    if (! parts[i].isEmpty()) {
      QStringList kv = parts[i].split(":");
      if (kv.size() == 2) {
        QString k = kv[0];
        QString v = kv[1];
        if (kv[0].toLower().trimmed() == "font-family") {
          family =  v.trimmed();
        }
        if (kv[0].toLower().trimmed() == "font-size") {
          fontSize =  v.trimmed();
        }
      }
    }
  }
  family.remove("\"");
  QRegularExpression sizeRx("(\\d+)([^\\d]*)");
  QRegularExpressionMatch m;
  m = sizeRx.match(fontSize);
  if (m.hasMatch()) {
    QStringList captured = m.capturedTexts();
    bool ok = false;
    int sz = captured[1].toInt(&ok);
    if (ok) {
      if (captured.size() == 3) {
        if (captured[2].contains("px")) {
          f.setPixelSize(sz);
        }
        else if (captured[2].contains("pt")) {
          f.setPointSize(sz);
        }
        else {
          f.setPixelSize(sz);
        }
      }
    }
    f.setFamily(family);
  }

  return f;
}
QString Lexicon::getCssSpecification(const QString & selector) {
  QString css;
  QString fileName = getStylesheetFilePath(Lexicon::Application);
  QFile file(fileName);
  if (! file.open(QIODevice::ReadOnly)) {
    return css;
  }
  QTextStream in(&file);
  QString clause;
  bool ok = false;
  QRegularExpression rx("(^[^\\s]+)\\s");
  QRegularExpressionMatch m;
  while(! in.atEnd() && ! ok ) {
    QString s;
    QString line = in.readLine();
    m = rx.match(line);
    if (m.hasMatch()) {
      s = m.captured(1);
    }
    if (s.contains(selector)) {
      while(! line.contains("{") && ! line.contains("}") && ! in.atEnd()) {
        line += in.readLine();
      }
      if (line.contains("{") &&  line.contains("}")) {
        ok = true;
        css = line.remove(s).trimmed();
      }
    }
  }
  file.close();
  if (! ok ) {
    return css;
  }
  return css;
}
/**
 * This is because on OSX QLineEdit does not automatically adjust its height
 * so that the text is all visible
 * @param w
 * @param t
 */
void Lexicon::adjustHeight(QWidget * w) {
  QFileInfo fi(m_settingsDir,m_configFile);
  QSettings settings(fi.absoluteFilePath(),QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup("HeightAdjustment");
  QStringList groups = settings.childGroups();
  for(int i=0;i < groups.size();i++) {
    settings.beginGroup(groups[i]);
    QStringList selectors = settings.value("Selectors").toStringList();
    QString sampleText = settings.value("Sample").toString();
    for(int j=0;j < selectors.size();j++) {
      if (! selectors[j].isEmpty()) {
        setEditFont(w,selectors[j],sampleText);
      }
    }
    settings.endGroup();
  }
}
void Lexicon::setEditFont(QWidget * w,const QString & selector,const QString & t) {
  QString css = getCssSpecification("ImLineEdit");
  if (css.isEmpty()) {
    return;
  }
  QString sample = t;
  if (sample.isEmpty()) {
    sample = "أٌل";
  }
  QLineEdit * edit = qobject_cast<QLineEdit *>(w);
  if (edit) {
    QFont f = fontFromCss(css);
    QFontMetrics fm(f);
    QSize sz = fm.size(Qt::TextSingleLine,sample);
    if (sz.height() > 0) {
      edit->setMinimumHeight(sz.height() + 4);
    }
  }
  /*
  QList<QLineEdit *> edits = w->findChildren<QLineEdit *>();
  foreach(QLineEdit *  widget,edits) {
    widget->setFont(f);
  }
  */
}
