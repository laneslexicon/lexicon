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

QString TextSearch::fixHtml(const QString & t) {
  QString html = t;
  QRegularExpression rxInsert("<!--insert{([^}]+)}-->",QRegularExpression::MultilineOption);
  QRegularExpressionMatchIterator iter = rxInsert.globalMatch(html);
  while(iter.hasNext()) {
    QRegularExpressionMatch m = iter.next();
    html.replace(m.captured(0),m.captured(1));
  }
  return html;
}

TextSearch::TextSearch() {
  m_padding = 30;
  m_verbose = false;
}
void TextSearch::setNode(const QString & node) {
    QRegularExpression nrx("^\\d+$");
    QStringList n = node.split(",");

    for(int i=0;i < n.size();i++) {
      if (nrx.match(n[i]).hasMatch()) {
       m_nodes <<  "n" + n[i];
      }
    }
}
void TextSearch::setPadding(int sz) {
  m_padding = sz;
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

QMap<int,QString> TextSearch::searchEntry(QString xml) {
  QMap<int,QString> results;
  //  QTextDocument doc(xml);
  //  int cnt = doc.characterCount();
  bool regex = (m_rx.pattern().length() > 0);

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
    c = doc.find(m_rx,0);
  }
  else {
    c = doc.find(m_pattern,0,f);
  }
  int start;
  int end;

  QString fragment;
  while (! c.isNull()) {
    fragment.clear();
    fc++;
    start = c.selectionStart() - m_padding;
    end = c.selectionEnd() + m_padding;
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
      c = doc.find(m_rx,c.selectionEnd());
    }
    else {
      c = doc.find(m_pattern,c.selectionEnd(),f);
    }
  }
  return results;
}
/*
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
*/
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
  m_rx = rx;
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
void TextSearch::searchAll() {
  QSqlQuery query;
  query.prepare(SQL_ALL_ENTRIES);
  if (! query.exec()) {
    qDebug() << "Error search all nodes" << query.executedQuery() << query.lastError();
    return;
  }
  int readCount = 0;
  int findCount = 0;
  bool finished = false;
  QMap<int,QString> ret;
  while (query.next() && ! finished) {
    readCount++;
    QString xml = query.value("xml").toString();
    QString node = query.value("nodeid").toString();
    if (! node.startsWith("j")) {
      ret = searchEntry(xml);
      if (ret.size() > 0) {
        SearchResult r;
        r.node = query.value("nodeid").toString();
        r.root = query.value("root").toString().toUtf8();
        r.head = query.value("word").toString().toUtf8();
        r.fragments = ret;
        m_results << r;
        findCount++;
      }
    }
    /*
    if (maxFinds && (findCount > maxFinds)) {
      finished = true;
    }
    if (maxReads && (readCount > maxReads)) {
      finished = true;
    }
    */
    //    if ((readCount % 500) == 0) {
    //      qDebug() << QString(".....%1....").arg(readCount);
    //    }
  }
}
void TextSearch::searchNodes() {
  int readCount = 0;
  int findCount = 0;
  bool finished = false;
  QSqlQuery query;
  query.prepare(SQL_FIND_ENTRY_DETAILS);
  QMap<int,QString> ret;
  for(int i=0;(i < m_nodes.size()) && ! finished ;i++) {
    //query.prepare(SQL_FIND_ENTRY_DETAILS);
    query.bindValue(0,m_nodes[i]);
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
          ret = searchEntry(xml);
          if (ret.size() > 0) {
            SearchResult r;
            r.node = query.value("nodeid").toString();
            r.root = query.value("root").toString().toUtf8();
            r.head = query.value("word").toString().toUtf8();
            r.fragments = ret;
            findCount++;
            m_results << r;
          }
          /*
          if (maxFinds && (findCount > maxFinds)) {
            finished = true;
          }
          if (maxReads && (readCount > maxReads)) {
            finished = true;
          }
          */
        }
      }
    }
  }

}

void TextSearch::search() {
  m_results.clear();
  QTime t;
  t.start();
  if (m_nodes.size() == 0) {
    this->searchAll();
  }
  else {
    this->searchNodes();
  }
  m_time = t.elapsed();
}
