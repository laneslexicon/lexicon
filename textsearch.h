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
#define COUT  std::cout
#define CERR  std::cerr
#define ENDL  std::endl
#else
#define COUT  QLOG_INFO()
#define CERR  QLOG_WARN()
#define ENDL  "\n"
#endif
/*
class  SearchFragment {
  int pos;
  bool headMatch;
  QString fragment;
};
*/
class SearchHit {
 public:
  QString root;
  QString node;
  QString head;
  int vol;
  int page;
  int ix;                // hit count
  QString fragment;
  friend QDebug operator<<(QDebug, const SearchHit &);
};
class   SearchResult {
public:
  QString root;
  QString node;
  QString head;
  int vol;
  int page;
  QMap<int,QString> fragments;
  friend QDataStream &operator<<(QDataStream &, const SearchResult &);
  friend QDebug operator<<(QDebug, const SearchResult &);
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
  QString m_pattern;
  QString m_separator;
  QList<SearchHit> getHits(int start,int sz,bool summary = false) const;
  QList<SearchHit> getPage(int page,bool summary) const;
  QString buckwalterCharacters();

  int rows(bool summary) const;
  QPair<int,int> setPages(int pagesize);
  void dumpPages(bool);
 private:
  QList<SearchResult>  m_results;
  void setOffsets();
  qint64 m_time;
  QStringList m_nodes;
  QString m_fields;
  QRegularExpression m_rx;
  bool m_verbose;
  bool m_caseSensitive;
  bool m_wholeWord;
  int m_padding;
  bool m_diacritics;
  bool m_regex;
  QString m_iniFileName; // do we need to keep these three?
  QString m_xsltFile;    //
  QString m_dbFile;      //
  int m_pageSize;
  QMap<int,int> m_pages;
  QMap<int,QPair<int,int> > m_summaryPages;
  QMap<int,QPair<int,int> > m_fullPages;
  QMap<QChar,QChar> m_safe;
 signals:
  void recordsRead(int);
  void fatalError(const QString &);
};
#endif
