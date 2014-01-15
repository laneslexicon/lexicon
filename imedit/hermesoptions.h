#ifndef __HERMESOPTIONS__
#define __HERMESOPTIONS__
#include <QSettings>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QKeySequence>
class UnicodeEntry {
 public:
  //  UnicodeEntry() {}
  QString m_key;      // from .conf
  QString m_name;     // user defined name
  int m_value;
  QString m_uname;    // unicode name
  QString m_iconFilename;
};

class ScriptFont {
 public:
  ScriptFont() {}
  void setFontString(const QString & f) {
    m_fontString = f;
  }
  QString getFontString() { return m_fontString;}
  void setEnabled(bool v) {
    m_enabled = v;
  }
  bool getEnabled() { return m_enabled;}
 private:
  QString m_fontString;
  bool m_enabled;
};
typedef QMap<QString,ScriptFont *>  FontSettings;
class ScriptMap {
 public:
  ScriptMap() {}
  ScriptMap(const QString &);
  QString getFileName() { return m_fileName;}
  QString getScript() { return m_script;}
  QString getShortCut() { return m_shortcut;}
  bool getEnabled() { return m_enabled;}
  void setScript(const QString & s) {
    m_script = s;
  }
  void setEnabled(bool v) {
    m_enabled = v;
  }
  void setFileName(const QString & f) {
    m_fileName = f;
  }
  void setShortCut(const QString & f) {
    m_shortcut = f;
  }
 private:
  QString m_script;
  QString m_fileName;
  QString m_shortcut;
  bool m_enabled;
};
typedef QMap<QString,ScriptMap *> MapSettings;

typedef QMap<QString,UnicodeEntry *> UniSettings;
typedef QMap<QString,UnicodeEntry *> UnicodeSettings;
class HermesOptions : public QObject {
  Q_OBJECT
 public:
  HermesOptions() {}
  ~HermesOptions();
  void readSettings();
  void writeSettings();
  void addMap(const QString & script,ScriptMap *);
  void addFont(const QString &,ScriptFont *);
  void addFlashFont(const QString &,ScriptFont *);
  void addTreeFont(const QString &,ScriptFont *);
  MapSettings getMapSettings();
  FontSettings getFontSettings();
  const UnicodeSettings getUnicodeSettings() { return m_unicode;}
  void setMaps(const MapSettings &);
  void setFonts(const FontSettings &);
  QString isShortcut(const QKeySequence &);
  QString getOption(const QString &);
  QString getFontString(const QString &);
  QString getFlashFontString(const QString &);
  QString getTreeFontString(const QString &);
 private:
  void setupKeys();
  QMap<QString,ScriptFont *> m_fonts;
  QMap<QString,ScriptFont *> m_flashfonts;
  QMap<QString,ScriptFont *> m_treefonts;
  QMap<QString,ScriptMap *> m_maps;
  QMap<QString,QString> m_keys;
  QMap<QString,QString> m_options; // other options
  QMap<QString,UnicodeEntry *> m_unicode;
};

#endif
