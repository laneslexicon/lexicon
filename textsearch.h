#ifndef __TEXTSEARCH_H__
#define __TEXTSEARCH_H__
#include <QObject>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTextDocument>
#include <QTextCursor>
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#ifndef LANE
class SearchRunner : public QObject {
  Q_OBJECT
 public:
  SearchRunner();
 public slots:
  void recordsRead(int);
};
#endif
/*
class  SearchFragment {
  int pos;
  bool headMatch;
  QString fragment;
};
*/
class   SearchResult {
public:
  QString root;
  QString node;
  QString head;
  QMap<int,QString> fragments;
  //  QList<SearchFragment> f;
};

class TextSearch : public QObject {
  Q_OBJECT
 public:
  TextSearch();
  QString transform(int type,const QString & xsl,const QString & xml);
  QList<QPair<QString,QString> > splitText(const QString & txt);
  QMap<int,QString> searchEntry(QString xml,QString headword,QString node = QString());
  QString getDiacritics(QList<QChar> & points);
  QRegularExpression buildRx(QString target,bool ignorediacritics,bool wholeword,bool ignorecase);
  QString fixHtml(const QString & t);
  void setSearch(const QString & pattern,bool regex,bool caseSensitive,bool wholeWord,bool diacritics);
  void setSettingsPath(const QString &);
  void setXsltFileName(const QString &);
  void setDbFileName(const QString &);
  void setFields(const QString &);
  QString  dbFile() const;
  void toFile(const QString & fileName = QString()) const;
  QString fromSafe(const QString & v);
  void search();
  void searchAll();
  void searchNodes();
  void setVerbose(bool);
  void setNode(const QString & node);
  void setPadding(int);
  void setCaseSensitive(bool);
  void setWholeWord(bool);
  void setDiacritics(bool);
  void setSearchType(bool);
  QString summary() const;
  QList<SearchResult>  m_results;

  QString m_pattern;
  qint64 m_time;
  QString m_separator;
  QStringList m_nodes;
  QString m_fields;
  QRegularExpression m_rx;
  bool m_verbose;
  bool m_caseSensitive;
  bool m_wholeWord;
  int m_padding;
  bool m_diacritics;
  bool m_regex;
  QString m_iniFileName;
  QString m_xsltFile;
  QString m_dbFile;
  QMap<QChar,QChar> m_safe;
 signals:
  void recordsRead(int);
  void fatalError(const QString &);
};
#endif
