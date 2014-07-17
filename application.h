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

class Lexicon : public QApplication {
  Q_OBJECT;
public:
  Lexicon(int & argc, char ** argv);
  QString getConfig() const;
  QSettings * getSettings();
  void setConfig(const QString & fileName);
  bool isOk() { return m_ok;}
  void scanForFonts(const QDir &);
  public slots:
    void onFocusChange(QWidget *,QWidget *);
 private:
  QString m_configFile;
  QString m_showFirst;
  bool m_ok;
};
#endif
