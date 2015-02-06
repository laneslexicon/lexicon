#ifndef __LEXICON_H__
#define __LEXICON_H__
#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>
#include <QSettings>
#include <QFontDatabase>
#include <QWidget>
#include <iostream>
#include "QsLog.h"
class Lexicon : public QApplication {
  Q_OBJECT;
public:
  Lexicon(int & argc, char ** argv);
  enum Status { Ok, ResourceDirError, NoThemeDirectory, ThemeNotFound , SettingsNotFound};
  enum Resource { Stylesheet, Image,XSLT,Keyboard,Map,Splash };
  QString getConfig() const;
  void setOptions(const QMap<QString,QString> &);
  QMap<QString,QString> getOptions() const;
  QSettings * getSettings();
  QVariant getValue(const QString & group,const QString & key);
  bool     getBool(const QString & group,const QString & key);
  QString currentTheme() const { return m_currentTheme;}
  bool isOk() const;
  int setTheme(const QString &);
  void scanForFonts(const QDir &);
  QStringList getThemes() const;
  QString getResourcePath(int type,const QString & name = QString());
  QString spanArabic(const QString &,const QString & which = QString());
  QString scanAndSpan(const QString &,const QString & css = QString("ar"));
  QDir themeDirectory() { return m_settingsDir; }
  QString imageDirectory();
  QString errorPath() const { return m_errorFilePath; }
  QString errorFile() const { return m_errorFile; }
  void startLogging();
  QString takeLastError();
  public slots:
    void onFocusChange(QWidget *,QWidget *);
 private:
    QStringList m_errors;
    QString m_errorFilePath;
    QString m_errorFile;
    QString m_configFile;
    QString m_showFirst;
    QString m_themeDirectory;
    QString m_currentTheme;
    QDir m_settingsDir;
    int m_status;
    QMap<QString,QString> m_options;
};
#endif
