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
#include "ensearch.h"
bool showData;

/*
  QFile dbfile;
  QSqlDatabase db;
  bool openDatabase(const QString & dbname) {
  QLOG_DEBUG() << Q_FUNC_INFO << dbname;
  dbfile.setFileName(dbname);

  if (! dbfile.exists() ) {
  QLOG_WARN() << QString(QObject::tr("Cannot find database : %1")).arg(dbname);
  return false;
  }
  if (db.isOpen()) {
  db.close();
  }
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(dbname);
  return db.open();
  }
*/
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
TextSearch::TextSearch() {
}
QString TextSearch::transform(int type,const QString & xsl,const QString & xml) {
  int ok;
  if (xsl.isEmpty()) {
    qDebug() << "No XSLT supplied";
    return QString();
  }
  if (type == ENTRY_XSLT && ! QFileInfo::exists(xsl)) {
    qDebug() << QString("Cannot find XSLT file: %1").arg(QDir::current().relativeFilePath(xsl));
    return QString();
  }

  ok = compileStylesheet(type,xsl);
  if (ok == 0) {
    QString html = xsltTransform(type,xml);
    if (! html.isEmpty()) {

      return fixHtml(html);
    }
    else {
      QLOG_WARN() << "Transform returned no HTML";
      return QString();
    }
  }
  else {
    qDebug() << "XSLT compile errors";
  }
  QStringList errors = getParseErrors();
  if (errors.size() > 0) {
    qDebug() << "XSLT errors" << errors.size();
    /// TODO fix this
    //  errors.prepend("Errors when processing entry styesheet:");
    //  QMessageBox msgBox;
    //  msgBox.setText(errors.join("\n"));
    //  msgBox.exec();
    clearParseErrors();
  }
  return QString();

}
QList<QPair<QString,QString> > TextSearch::splitText(const QString & txt) {
  QList<QPair<QString,QString>> texts;
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
  QStringList css;
  int pos = 0;
  for(int i=1;i < p.length();i++) {
    if (p.at(i) != p.at(i-1)) {
      words << txt.mid(pos, i - pos);
      if (p.at(i-1) == 'r') {
        css << "ar";
      }
      else {
        css << "en";
      }
      texts << qMakePair(words[words.size() - 1],css[css.size() - 1]);
      pos = i;
    }

  }
  if (pos < p.length()) {
    words << txt.mid(pos);
    if (p.at(pos) == 'r') {
      css << "ar";
    }
    else {
      css << "en";
    }
    texts << qMakePair(words[words.size() - 1],css[css.size() - 1]);
  }
  return texts;
}

QMap<int,QString> TextSearch::searchEntry(QString pattern,QString xml,QRegularExpression rx) {
  QMap<int,QString> results;
  //  QTextDocument doc(xml);
  //  int cnt = doc.characterCount();
  bool regex = (rx.pattern().length() > 0);

  xml = QString("<word>%1</word>").arg(xml);
  QString html =
    transform(ENTRY_XSLT,"Resources/themes/default/xslt/entry.xslt",xml);
  if (html.isEmpty()) {
    return results;
  }
  //  html = fixHtml(html);
  //  qDebug() << html;
  QTextDocument doc;
  QTextDocument xdoc(xml);
  //  qDebug() << html;
  doc.setHtml(QString(xml.toUtf8()));//"<html><body>fuck</body></html>");

  //  QTextCursor xc = xdoc.find("camel");
  //  if (! xc.isNull() && c.isNull()) {
  //    qDebug() << "find in xml, not in html";
  int fc = 0;
  QTextDocument::FindFlags f;
  f = QTextDocument::FindCaseSensitively;
  f |= QTextDocument::FindWholeWords;
  QTextCursor c;
  if (regex) {
    c = doc.find(rx,0);
  }
  else {
    c = doc.find(pattern,0,f);
  }
  int start;
  int end;
  int fsize = 20;
  QString fragment;
  while (! c.isNull()) {
    fragment.clear();
    fc++;
    start = c.selectionStart() - fsize;
    end = c.selectionEnd() + fsize;
    if (start < 0) {
      start = 0;
    }
    if (end > doc.characterCount()) {
      end = doc.characterCount();
    }
    for(int i=start;i < end;i++) {
      fragment += QString(doc.characterAt(i));
    }
    results[c.selectionStart()] = fragment;
    if (regex) {
      c = doc.find(rx,c.selectionEnd());
    }
    else {
      c = doc.find(pattern,c.selectionEnd(),f);
    }
  }
  if (fc && showData) {
    qDebug() << html;
    qDebug() << xml;
  }

  return results;
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
/*
  Return the regex to make the diacritics optional and fill the passed array with the characters
  so the calling function can remove them from the search pattern
*/
QString TextSearch::getDiacritics(QList<QChar> & points) {
  QSettings settings("Resources/themes/default/settings.ini",QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup("Diacritics");
  QStringList keys = settings.childKeys();
  QString v;
  QString rx("[");
  bool ok;
  for(int i=0;i < keys.size();i++) {
    if (keys[i].startsWith("Char")) {
      v = settings.value(keys[i],QString()).toString();
      if (ok) {
        rx += QString("0x%1").arg(v);
      }
      points << QChar(v.toInt(&ok,16));
    }
  }
  rx += "]*";
  return rx;
}
QRegularExpression TextSearch::buildRx(QString target,bool ignorediacritics,bool wholeword,bool ignorecase) {
  QList<QChar> dc;
  QString diacritics = getDiacritics(dc);//("[0x6710x64c0x6500x651]*");
  QString pattern;
  for(int i=0;i < dc.size();i++) {
    target.remove(dc[i]);
  }
  for(int i=0;i < target.size();i++) {
    QChar sp = target.at(i);
    pattern += QString(sp);
    if ( sp.isLetter() ) {
      /// if it's in the Arabic block, append to allow for optional diacritics
      if (ignorediacritics && ((sp.unicode() >= 0x600) && (sp.unicode() <= 0x6ff))) {
        pattern += diacritics;
      }
    }
  }
  if (wholeword) {
    pattern = "\\b" + pattern + "\\b";
  }
  QRegularExpression rx(pattern);
  if (ignorecase) {
    rx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
  }
  return rx;
}
void TextSearch::toFile(const QString & fileName) const {
  bool fileOutput = false;
  QFile of;

  if (fileName.length() > 0) {
  of.setFileName(fileName);
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
  int entryCount = m_results.size();
  int findCount = 0;
  for(int i=0;i < m_results.size();i++) {
    QMapIterator<int,QString> iter(m_results[i].fragments);
    findCount += m_results[i].fragments.size();
    while(iter.hasNext()) {
      iter.next();
      QStringList o;
      o << m_results[i].node << m_results[i].root << m_results[i].head  << QString("%1").arg(iter.key()) << iter.value();
      if (fileOutput) {
        out << o.join(m_separator);
        out << "\n";
      }
      else {
        std::cout << qPrintable(o.join(m_separator)) << std::endl;
      }
    }
  }
 QString summary =  QString("Search %1 : found in %2 entr%3, total count %4 (time %5ms)").arg(m_pattern).arg(entryCount).arg(entryCount > 1 ? "ies" : "y").arg(findCount).arg(m_time);
  of.close();
  std::cerr << qPrintable(summary) << std::endl;
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
  if (! parser.isSet(nodeOption)) {
    query.prepare(SQL_ALL_ENTRIES);
  }
  else {
    QString node = parser.value(nodeOption);
    QRegularExpression nrx("^\\d+$");
    QStringList n = node.split(",");

    for(int i=0;i < n.size();i++) {
      if (nrx.match(n[i]).hasMatch()) {
        nodes <<  "n" + n[i];
      }
    }

  }
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
        ret = searcher.searchEntry(pattern,xml,rx);
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
      //    if ((readCount % 500) == 0) {
      //      qDebug() << QString(".....%1....").arg(readCount);
      //    }
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
