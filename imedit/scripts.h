#ifndef __SCRIPTS_H__
#define __SCRIPTS_H__
#include <QList>
#include <QStringList>
#include <QRegExp>
#include <QDebug>
/**
 * splits string on change of UCD script
 * storing list of <script,substring>
 *
 *
 */
class UcdScripts : public QObject {
 public:
  UcdScripts() {}
  UcdScripts(const QString & );
  int size() { return m_scripts.size();}
  QString getScriptAt(int i);
  QString getStringAt(int i);
  static bool isScript(const QString &,const QString &);
  static void getScripts(QStringList &);
  static QString getScript(int);
  static QString getScript(const QString & );
  static QString getScript(const QString & ,int *);
 private:
  bool m_skipEntities;
  bool m_trim;
  QList<QPair<QString,QString> > m_scripts;
  QString m_target;
  void split();
  QList<QRegExp> m_remove;
  QRegExp m_skip;
};
#endif
