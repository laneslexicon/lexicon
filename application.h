#ifndef __MYAPP__H
#define __MYAPP__H
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QSettings>
class Lexicon : public QApplication {
  Q_OBJECT;
public:
  Lexicon(int & argc, char ** argv);
  QString getConfig() const;
  QSettings * getSettings();
  void setConfig(const QString & fileName);
 private:
  QString m_configFile;
};
#endif
