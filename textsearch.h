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
class   SearchResult {
public:
  QString root;
  QString node;
  QString head;
  QMap<int,QString> fragments;
};

class TextSearch : public QObject {
  Q_OBJECT
 public:
  TextSearch();
  QString transform(int type,const QString & xsl,const QString & xml);
  QList<QPair<QString,QString> > splitText(const QString & txt);
  QMap<int,QString> searchEntry(QString xml);
  QString getDiacritics(QList<QChar> & points);
  QRegularExpression buildRx(QString target,bool ignorediacritics,bool wholeword,bool ignorecase);
  QString fixHtml(const QString & t);
  void toFile(const QString & fileName = QString()) const;
  void search();
  void searchAll();
  void searchNodes();
  void setNode(const QString & node);
  void setPadding(int);
  QList<SearchResult>  m_results;
  QString m_pattern;
  qint64 m_time;
  QString m_separator;
  QStringList m_nodes;
  QRegularExpression m_rx;
  bool m_verbose;
  int m_padding;
};
#endif
