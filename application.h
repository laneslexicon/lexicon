#ifndef __MYAPP__H
#define __MYAPP__H
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QSettings>
#include <QFontDatabase>
#include <iostream>

class Lexicon : public QApplication {
  Q_OBJECT;
public:
  Lexicon(int & argc, char ** argv);
  QString getConfig() const;
  QSettings * getSettings();
  void setConfig(const QString & fileName);
  bool isOk() { return m_ok;}
 private:
  QString m_configFile;
  QString m_showFirst;
  bool m_ok;
};
#endif
