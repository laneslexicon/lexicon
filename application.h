#ifndef __LEXICON_H__
#define __LEXICON_H__
#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QDebug>
#include <QSettings>
#include <QFontDatabase>
#include <QWidget>
#include <QLineEdit>
#include <QRegularExpression>
#include <QDateTime>
#include <QSysInfo>
#include <iostream>
#include "QsLog.h"
class Lexicon : public QApplication {
  Q_OBJECT;
public:
  Lexicon(int & argc, char ** argv);
  enum Status { Ok, ResourceDirError, NoThemeDirectory, ThemeNotFound , SettingsNotFound};
  enum Resource { Stylesheet, Image,XSLT,Keyboard,Map,Splash,ThemeRoot};
  enum Stylesheet { Application , Entry, Print , Node };
  QString getConfig() const;
  void setOptions(const QMap<QString,QString> &);
  QMap<QString,QString> getOptions() const;
  QSettings * getSettings();
  QString settingsFileName(const QString & theme = QString());
  QString editorSettingsFileName(const QString & theme = QString());
  QVariant getValue(const QString & group,const QString & key);
  bool     getBool(const QString & group,const QString & key);
  QString currentTheme() const { return m_currentTheme;}
  bool isOk() const;
  int setTheme(const QString &);
  void scanForFonts(const QDir &);
  QStringList getThemes() const;
  QStringList getKeyboards(int type = 0) ;
  QString     getDefaultKeyboard();
  bool        setDefaultKeyboard(const QString &);
  QString getStylesheetFilePath(int type);
  QString copyToTemp(const QString &);
  //  QString getResourcePath(int type);
  QString getResourceFilePath(int type,const QString & = QString());
  QString spanArabic(const QString &,const QString & which = QString());
  QString scanAndSpan(const QString &,const QString & css = QString("arabic"));
  QString scanAndStyle(const QString &,const QString & css = QString());
  QDir themeDirectory() { return m_settingsDir; }
  void setCursorPosition(QWidget *,int start=0);
  QString imageDirectory();
  QString errorPath() const { return m_errorFilePath; }
  QString errorFile() const { return m_errorFile; }
  void startLogging();
  QString takeLastError();
  QMap<QString,int> getUsedFont();
  QStringList setArabicFont(const QString & family = QString());
  QStringList changeFontInSettings(const QString & family,int size = -1);
  QStringList changeFontInStylesheet(const QString & fileName,const QString & family,int size = -1);
  QStringList getFontInSettings(const QString & selector) const;
  QStringList getFontInStylesheet(const QString & fileName,const QString & selector) const;
  void adjustHeight(QWidget *);
  QString getCssSpecification(const QString & selector) ;
  QFont fontFromCss(const QString & css);
  void setEditFont(QWidget *,const QString & selector,const QString & sample = QString(),int margin = 4);

  public slots:
    void onFocusChange(QWidget *,QWidget *);
 private:
    QString setCssFont(const QString & css,const QString & family,int fontSize) const;
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
