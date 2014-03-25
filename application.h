#ifndef __MYAPP__H
#define __MYAPP__H
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
class MyApp : public QApplication {
  Q_OBJECT;
public:
  MyApp(int & argc, char ** argv);
  QString getConfig() const;
  void setConfig(const QString & fileName);
 private:
  QString m_configFile;
};
#endif
