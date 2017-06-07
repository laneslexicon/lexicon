#include <QCoreApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QTime>
#include <QLocale>
#include <QSettings>
#include <QSqlError>
#include "QsLog.h"
#include "QsLogDest.h"
#include "definedsettings.h"
#include "definedsql.h"
#include "xsltsupport.h"
#include <iostream>
#include "textsearch.h"
bool showData;
QString fixHtml(const QString & t) {
  QString html = t;


  /*
    QRegularExpression rxStart("<!--insert_start_(\\w+)-->");
    QRegularExpressionMatch m = rxStart.match(html);
    if (m.hasMatch()) {
    html.replace(m.captured(0),QString("<%1>").arg(m.captured(1)));
    }
    QRegularExpression rxEnd("<!--insert_end_(\\w+)-->");
    m = rxEnd.match(html);
    if (m.hasMatch()) {
    html.replace(m.captured(0),QString("</%1>").arg(m.captured(1)));
    }
    html = html.remove("\n");
  */
  QRegularExpression rxInsert("<!--insert{([^}]+)}-->",QRegularExpression::MultilineOption);
  QRegularExpressionMatchIterator iter = rxInsert.globalMatch(html);
  while(iter.hasNext()) {
    QRegularExpressionMatch m = iter.next();
    html.replace(m.captured(0),m.captured(1));
  }
  return html;
}
void testSplit(const QString & txt) {
  int rtlCount = 0;
  int ltrCount = 0;
  QString p;
  QString cp;
  for(int i=0;i < txt.length();i++) {
    switch(txt.at(i).direction()) {
    case QChar::DirAL:
    case QChar::DirR:
    case QChar::DirAN:
      rtlCount++;
      p += "r";
      cp = "r";
      break;
    case QChar::DirL:
    case QChar::DirEN:
      ltrCount++;
      p += "l";
      cp = "l";
      break;
    default:
      p += cp;
      break;
    }
  }

  //  qDebug() << QString("%1 : [%2]").arg(p.size()).arg(p);
  QStringList words;
  int pos = 0;
  for(int i=1;i < p.length();i++) {
    if (p.at(i) != p.at(i-1)) {
      words << txt.mid(pos, i - pos);
      pos = i;
    }
  }
  if (pos < p.length()) {
    words << txt.mid(pos);
  }
  qDebug() << QString("%1 : [%2]").arg(txt.size()).arg(txt);
  qDebug() << "words" << words;
}
void test() {
  TextSearch searcher;
  QList<QChar> dc;

  QString text("This is a test فتح for Graeme's crap code");
  QRegularExpression rx = searcher.buildRx("فَتَح",true,true,true);
  qDebug() << rx.pattern() << rx.isValid() << rx.match(text);
  rx = searcher.buildRx("graeme",true,true,true);
  qDebug() << rx.pattern() << rx.isValid() << rx.match(text);
  rx = searcher.buildRx("graeme",true,true,false);
  qDebug() << rx.pattern() << rx.isValid() << rx.match(text);
  rx = searcher.buildRx("rap",true,true,true);
  qDebug() << rx.pattern() << rx.isValid() << rx.match(text);
  //  QString diacritics = QString("[\\x%1]*").arg(points.join("\\x"));
  testSplit("graeme برتن is an idiot شَمس");
  qDebug() << searcher.splitText("graeme برتن is an idiot شَمس");
  testSplit("برتن is an idiot");
  testSplit("graeme");
  testSplit("برتن");
}
int main(int argc, char *argv[])
{
  QString settingsFile("Resources/themes/default/settings.ini");
  QCommandLineParser parser;
  parser.addHelpOption();
  QCommandLineOption nodeOption(QStringList() <<"n" << "node",QObject::tr("Node or comma delimited list of nodes to be searched"),"node");
  parser.addOption(nodeOption);

  parser.addHelpOption();

  QCommandLineOption patternOption(QStringList() <<"p" << "pattern",QObject::tr("Search pattern"),"pattern");
  parser.addOption(patternOption);

  QCommandLineOption outputOption(QStringList() <<"o" << "output",QObject::tr("Output file"),"output");
  parser.addOption(outputOption);

  QCommandLineOption separatorOption(QStringList() <<"s" << "separator",QObject::tr("Output file field separator"),"separator");
  separatorOption.setDefaultValue("|");
  parser.addOption(separatorOption);

  QCommandLineOption showOption(QStringList() <<"v" << "verbose",QObject::tr("Show matching entries"));
  parser.addOption(showOption);
  QCommandLineOption headOption(QStringList() <<"r" << "root",QObject::tr("Show root/headword for matching entries"));
  parser.addOption(headOption);

  QCommandLineOption testOption(QStringList() <<"t" << "test",QObject::tr("Invoke test routine"));
  parser.addOption(testOption);

  QCommandLineOption diacriticsOption(QStringList() <<"d" << "ignore-diacritics",QObject::tr("Ignore diacritics (Arabic only)"));
  parser.addOption(diacriticsOption);

  QCommandLineOption caseOption(QStringList() <<"i" << "ignore-case",QObject::tr("Ignore case"));
  parser.addOption(caseOption);

  QCommandLineOption wholeOption(QStringList() <<"w" << "whole-word",QObject::tr("Whole word match"));
  parser.addOption(wholeOption);

  QCommandLineOption regexOption(QStringList() <<"x" << "regex",QObject::tr("Regular expression search"));
  parser.addOption(regexOption);

  QCommandLineOption dbOption(QStringList() <<"f" << "file",QObject::tr("Database path relative to current directory"));
  parser.addOption(dbOption);

  QCoreApplication app(argc,argv);
  parser.process(app);

  if (parser.isSet(testOption)) {
    test();
    return 0;
  }
  QFile dbfile;
  QSqlDatabase db;
  int nodeCount = 0; // number of nodes read
  int wordCount = 0; // total number of pattern found
  QString dbname;
  //  QSettings settings(settingsFile,QSettings::IniFormat);
  //  settings.beginGroup("System");
  //  dbname = settings.value("Database",QString()).toString();
  //  if (dbname.length() == 0) {
  //    qDebug() << "Using hard-code dbname";
  if (! parser.isSet(dbOption)) {
    dbname = "Resources/lexicon.sqlite";
  }
  else {
    dbname = parser.value(dbOption);
  }
  QFileInfo fi(dbname);
  if (! fi.exists()) {
    dbname = "Resources/lexicon.sqlite";
    fi = QFileInfo(dbname);
  }
  if (! fi.exists()) {
    qDebug() << "Database not found:" << dbname;
  }


  QFile f(dbname);
  if (! f.exists()) {
    qDebug() << QString("Cannot find database: %1").arg(dbname);
    return 0;
  }


  dbfile.setFileName(fi.absoluteFilePath());

  if (! dbfile.exists() ) {
    QLOG_WARN() << QString(QObject::tr("Cannot find database : %1")).arg(dbname);
    return false;
  }
  if (db.isOpen()) {
    db.close();
  }
  showData = parser.isSet(showOption);
  bool showHead = parser.isSet(headOption);

  QStringList posargs = parser.positionalArguments();
  if (! parser.isSet(patternOption) && (posargs.size() == 0)) {
    qDebug() << "No pattern given";
    return 1;

  }
  QString pattern;
  if (parser.isSet(patternOption)) {
    pattern = parser.value(patternOption);
  }
  else {
    pattern = posargs[0];
  }
  if (pattern.isEmpty())  {
    qDebug() << "No pattern given";
    return 1;
  }
  TextSearch searcher;
  searcher.m_separator = parser.value(separatorOption);
  searcher.m_pattern = pattern;
  QRegularExpression rx;
  if (parser.isSet(regexOption)) {
    rx = searcher.buildRx(pattern,
                 parser.isSet(diacriticsOption),
                 parser.isSet(wholeOption),
                 parser.isSet(caseOption));
  }
  if (! rx.isValid()) {
    rx.setPattern("");
  }

  db = QSqlDatabase::addDatabase("QSQLITE");
  qDebug() << "Db" << dbname;
  db.setDatabaseName(dbname);
  db.open();
  if (! db.isOpen()) {
    qDebug() << "Could not open database";
    return 0;
  }
  QStringList nodes;
  QSqlQuery query;

  searcher.setNode(parser.value(nodeOption));
  searcher.search();
  /*
  QString root;
  QString headword;
  QString node;
  int maxFinds = 0;
  int maxReads = 0;
  int readCount = 0;
  int findCount = 0;
  bool finished = false;

  if (rx.pattern().length() > 0) {
    qDebug() << "Regular expression search" << rx.pattern();
  }
  else {
    qDebug() << "Text search" << pattern;
  }
  searcher.m_results.clear();
  QMap<int,QString> ret;
  QTime t;
  t.start();
  if (nodes.size() == 0) {
    if (! query.exec()) {
      qDebug() << "Error search all nodes" << query.executedQuery() << query.lastError();
    }
    while (query.next() && ! finished) {
      readCount++;
      QString xml = query.value("xml").toString();
      QString node = query.value("nodeid").toString();
      if (! node.startsWith("j")) {
        nodeCount++;
        if (maxReads && (nodeCount > maxReads)) {
          return 0;
        }
        ret = searcher.searchEntry(pattern,xml);
        if (ret.size() > 0) {
          SearchResult r;
          r.node = query.value("nodeid").toString();
          r.root = query.value("root").toString().toUtf8();
          r.head = query.value("word").toString().toUtf8();
          r.fragments = ret;
          searcher.m_results << r;
          findCount++;
          wordCount += ret.size();
        }
      }
      if (maxFinds && (findCount > maxFinds)) {
        finished = true;
      }
      if (maxReads && (readCount > maxReads)) {
        finished = true;
      }
    }
  }
  else {
    query.prepare(SQL_FIND_ENTRY_DETAILS);
    qDebug() << "nodes" << nodes;
    for(int i=0;(i < nodes.size()) && ! finished ;i++) {
      //query.prepare(SQL_FIND_ENTRY_DETAILS);
      query.bindValue(0,nodes[i]);
      if (! query.exec()) {
        qDebug() << "node query" << query.lastError();
        finished = true;
      }
      else {
        if (query.next()) {
          readCount++;
          QString xml = query.value("xml").toString();
          QString node = query.value("nodeid").toString();
          if (! node.startsWith("j")) {
            nodeCount++;
            if (maxReads && (nodeCount > maxReads)) {
              return 0;
            }
            ret = searcher.searchEntry(pattern,xml,rx);
            if (ret.size() > 0) {
              SearchResult r;
              r.node = query.value("nodeid").toString();
              r.root = query.value("root").toString().toUtf8();
              r.head = query.value("word").toString().toUtf8();
              r.fragments = ret;
              findCount++;
              wordCount += ret.size();
              searcher.m_results << r;
            }

            if (maxFinds && (findCount > maxFinds)) {
              finished = true;
            }
            if (maxReads && (readCount > maxReads)) {
              finished = true;
            }
          }
        }
      }
    }

  }
  searcher.m_time = t.elapsed();
  */
  dbfile.close();
  db.close();

  bool fileOutput = false;
  QFile of;
  if (parser.isSet(outputOption)) {
    searcher.toFile(parser.value(outputOption));
  }
  else {
    searcher.toFile();
  }

    /*
    of.setFileName(parser.value(outputOption));
    if (! of.open(QIODevice::WriteOnly)) {
      std::cerr << "Cannot open output file for writing: ";
      std::cerr << qPrintable(of.errorString()) << std::endl;
    }
    else {
      fileOutput = true;
    }
  }
  QTextStream out(&of);
  out.setCodec("UTF-8");
  for(int i=0;i < results.size();i++) {
    QMapIterator<int,QString> iter(results[i].fragments);
    while(iter.hasNext()) {
      iter.next();
      QStringList o;
      o << results[i].node << results[i].root << results[i].head  << QString("%1").arg(iter.key()) << iter.value();
      if (fileOutput) {
        out << o.join(parser.value(separatorOption));
        out << "\n";
      }
      else {
        std::cout << qPrintable(o.join(parser.value(separatorOption))) << std::endl;
      }
    }
  }
 QString summary =  QString("Search %1 : found in %2 entr%3, total count %4 (time %5ms)").arg(pattern).arg(findCount).arg(findCount > 1 ? "ies" : "y").arg(wordCount).arg(t.elapsed());
  of.close();
  std::cerr << qPrintable(summary) << std::endl;
    */
  return 0;
}
