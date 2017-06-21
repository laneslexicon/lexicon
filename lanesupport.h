#ifndef __LANESUPPORT_H__
#define __LANESUPPORT_H__
#include <QFileInfo>
#include <QSettings>
#include <QFont>
#include <QDir>
#include <QDebug>
class LaneSupport : public QObject {
 public:
  LaneSupport(const QString & fileName);
  QString scanAndSpan(const QString & str,const QString & css,bool embed = false);
  QString spanArabic(const QString & ar,const QString & spanstyle);
  QString scanAndStyle(const QString & str,const QString & spanstyle);
  QString getResourceFilePath(int type, const QString & name = QString()) const;
  QString getSpanStyle(const QString & spanstyle);
  QString settingsFileName() const;
  QString xsltFileName() const;
  bool startsWithArabic(const QString & txt) const;
enum Resource { Stylesheet, Image,XSLT,Keyboard,Map,Splash,ThemeRoot,Log};
  QString logFilePath() const;
  ~LaneSupport();
 private:
  QSettings * m_settings;
  QSettings * m_config;
  QDir m_settingsDir;
  QString m_logFilePath;
};
#endif
