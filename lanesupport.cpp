#include "lanesupport.h"
#include <iostream>
#include "definedsettings.h"
// absolute path to config.ini
LaneSupport::LaneSupport(const QString & resourcesDir) {
  QFileInfo configFile(resourcesDir,"config.ini");
  if (configFile.exists()) {
    m_config = new QSettings(configFile.absoluteFilePath(), QSettings::IniFormat);
    m_config->beginGroup("System");
    QString currentTheme = m_config->value("Theme","default").toString();
    QString themeDirectory = m_config->value("Theme directory","themes").toString();
    QFileInfo sd(resourcesDir,themeDirectory);
    sd.setFile(sd.absoluteFilePath(),currentTheme);
    QFileInfo si(sd.absoluteFilePath(),"settings.ini");
    if (si.exists()) {
      m_settings = new QSettings(si.absolutePath(), QSettings::IniFormat);
      m_settingsDir = si.dir();
    }
  }
  else {
    m_config = 0;
    m_settings = 0;
  }
}
LaneSupport::~LaneSupport() {
  if (m_settings != 0) {
    delete m_settings;
  }
  if (m_config != 0) {
    delete m_config;
  }
  std::cerr << "Delete LaneSupport" << std::endl;
}
QString LaneSupport::settingsFileName() const {
  return m_settings->fileName();
}
QString LaneSupport::scanAndSpan(const QString & str,const QString & css) {
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
QString LaneSupport::spanArabic(const QString & ar,const QString & spanstyle) {
  QString fontFamily;
  int fontSize = 10;
  QString style;

  if ( ! spanstyle.isEmpty() ) {
    m_settings->beginGroup("SpannedText");
    m_settings->beginGroup("Arabic");
    QString style = m_settings->value(spanstyle,QString()).toString();
    if (! style.isEmpty()) {
      return QString("<span style=\"%1\">%2</span>").arg(style).arg(ar);
    }
    m_settings->endGroup();
    m_settings->endGroup();
  }
  m_settings->beginGroup("System");
  QString fontString = m_settings->value(SID_SYSTEM_ARABIC_FONT,QString()).toString();
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
  style  = QString("<span style=\"font-family : %1;font-size : %2px\">%3</span>")
    .arg(fontFamily)
    .arg(fontSize)
    .arg(ar);
  return style;
}
/**
 * This is for mixed language text and will wrap any Arabic in a <span>
 * with the style included in the "style" attribute.
 *
 * Used for mixed text in widgets that don't support QTextDocument stylesheets eg QMessageBox
 *
 * @param str
 * @param style
 *
 * @return
 */
QString LaneSupport::scanAndStyle(const QString & str,const QString & spanstyle) {
  QString fontFamily;
  int fontSize = 10;
  QString style;

  if ( ! spanstyle.isEmpty() ) {
    m_settings->beginGroup("SpannedText");
    m_settings->beginGroup("Arabic");
    style = m_settings->value(spanstyle,QString()).toString();
    m_settings->endGroup();
    m_settings->endGroup();
  }
  QString ar = str;

  bool inArabic = false;
  QString html;
  for(int i=0;i < ar.size();i++) {
    QChar c = ar.at(i);
    if ((c.unicode() >= 0x600) && (c.unicode() <= 0x6ff)) {
      if (! inArabic) {
        html.append(QString("<span style=\"%1\">").arg(style));
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
QString LaneSupport::getResourceFilePath(int type, const QString & name) const {
  QFile file;
  QString d(".");
  switch(type) {
  case LaneSupport::Log : {
    return logFilePath();
  }
  case LaneSupport::ThemeRoot : {
    QDir d = QDir::current();
    QString t;
    QScopedPointer<QSettings> settings(new QSettings("config.ini",QSettings::IniFormat));
    settings->setIniCodec("UTF-8");
    settings->beginGroup("System");
    t = settings->value("Theme directory","themes").toString();
    if (t.isEmpty()) {
      t = "themes";
    }
    return d.absolutePath() + QDir::separator() + t + QDir::separator() + name;
    break;
  }
  case LaneSupport::Stylesheet : {
    d = m_settings->value("Stylesheet","css").toString();
    if (d.isEmpty()) {
      d = "css";
    }
    break;
  }
  case LaneSupport::Image : {
    d = m_settings->value("Image","images").toString();
    if (d.isEmpty()) {
      d = "images";
    }
    break;
  }
  case LaneSupport::XSLT : {
    d = m_settings->value("XSLT","xslt").toString();
    if (d.isEmpty()) {
      d = "xslt";
    }
    break;
  }
  case LaneSupport::Keyboard : {
    d = m_settings->value("Keyboard","keyboards").toString();
    if (d.isEmpty()) {
      d = "keyboards";
    }
    break;
  }
  case LaneSupport::Map : {
    d = m_settings->value("Map","maps").toString();
    if (d.isEmpty()) {
      d = "maps";
    }
    break;
  }
  case LaneSupport::Splash : {
    d = m_settings->value("Splash","images/splash").toString();
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
      return QString("Error:%1:%2:%3")
        .arg(QDir::current().relativeFilePath(rd.absolutePath()))
        .arg(name)
        .arg("Resource not found");
    }
  }
  else {
      return QString("Error:%1:%2:%3").arg(m_settingsDir.absolutePath()).arg("").arg("Directory not found");
  }
  return QString();
}
QString LaneSupport::logFilePath() const {
  return m_logFilePath;
}
bool LaneSupport::startsWithArabic(const QString & txt) const {
  for(int i=0;i < txt.length();i++) {
    switch(txt.at(i).direction()) {
    case QChar::DirAL:
    case QChar::DirR:
    case QChar::DirAN:
      return true;
      break;
    case QChar::DirL:
    case QChar::DirEN:
      return false;
      break;
    default:
      break;
    }
  }
  return false;
}
QString LaneSupport::xsltFileName() const {
  QString fileName;
  m_settings->beginGroup("XSLT");
  fileName = m_settings->value(SID_XSLT_ENTRY,QString("entry.xslt")).toString();
  if (fileName.isEmpty()) {
    fileName = "entry.xslt";
  }
  QString xsltDir = getResourceFilePath(LaneSupport::XSLT);
  QFileInfo fi(xsltDir,fileName);
  return fi.absoluteFilePath();
}
