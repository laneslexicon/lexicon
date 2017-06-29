#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QTime>
#include <QLocale>
#include <QSettings>
#include <QSqlError>
#include <QPalette>
#include <QFont>
#include "QsLog.h"
#include "QsLogDest.h"
#include "definedsettings.h"
#include "definedsql.h"
#include "xsltsupport.h"
#include <iostream>
#include "textsearch.h"
#include "lanesupport.h"
#include "ensearchwidget.h"
#include "textsearchwidget.h"
bool showData;
LaneSupport * support = 0;
LaneSupport * getSupport() {
  return support;
}
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
/*
convert and \arb{...} sequences to their Buckwalter safe equivalent
 */
QString arbInput(QString & str) {
  TextSearch t;
  QString bc = t.buckwalterCharacters();
  QString pattern("\\\\arb{([" + bc + "]+)}");
  QRegularExpression rx(pattern);
  //  qDebug() << "input" << str << str.length();
  //  qDebug() << "pattern" << rx.pattern() << rx.pattern().length();
  //  qDebug() << "match" << rx.match(str).hasMatch();
  int pos = 0;
  QRegularExpressionMatch m = rx.match(str,pos);
  QList<QStringList> matches;
  while(m.hasMatch()) {
    matches << m.capturedTexts();
    pos = m.capturedEnd(0) + 1;
    m = rx.match(str,pos);
  }
  QString ostr = str;
  for(int i=0;i < matches.size();i++) {
    if (matches[i].size() == 2) {
      ostr.replace(matches[i][0],t.fromSafe(matches[i][1]));
    }
  }
  //  qDebug() << ostr;
  return ostr;

}
void test(QString & str) {
  if (0) {
  TextSearch t;
  QString bc = t.buckwalterCharacters();
  QString pattern("\\\\arb{([" + bc + "]+)}");
  QRegularExpression rx(pattern);
  qDebug() << "input" << str << str.length();
  qDebug() << "pattern" << rx.pattern() << rx.pattern().length();
  qDebug() << "match" << rx.match(str).hasMatch();
  int pos = 0;
  QRegularExpressionMatch m = rx.match(str,pos);
  QList<QStringList> matches;
  while(m.hasMatch()) {
    matches << m.capturedTexts();
    pos = m.capturedEnd(0) + 1;
    m = rx.match(str,pos);
  }
  QString ostr = str;
  for(int i=0;i < matches.size();i++) {
    if (matches[i].size() == 2) {
      ostr.replace(matches[i][0],t.fromSafe(matches[i][1]));
    }
  }
  qDebug() << ostr;
  }
  if (0) {
    TextSearch t;
    QStringList tests;
    tests << "graeme" << "فتح" << "وجد و وصل" << "وجدSو وصل";
    tests <<  t.fromSafe(str).trimmed();
    QRegularExpression rx("^[\u0600-\u06ff]+$");
    for(int i=0;i < tests.size();i++) {
      qDebug() << tests[i] << "has match" << rx.match(tests[i]).hasMatch();
    }
  }
  if (1) {
    QString str("but only the inf. n., namely دَقَعَ , of the verb in this sense");
    QRegularExpression rx1(QString("\\b%1\\b").arg("verb"));
    QRegularExpression rx2(QString("\\b%1\\b").arg("دَقَعَ"));
    QRegularExpression rx3(QString("\\b%1").arg("دَقَعَ"));
    QRegularExpression rx4(QString("%1\\b").arg("دَقَعَ"));
    rx1.setPatternOptions(QRegularExpression::UseUnicodePropertiesOption);
    rx2.setPatternOptions(QRegularExpression::UseUnicodePropertiesOption);
    rx3.setPatternOptions(QRegularExpression::UseUnicodePropertiesOption);
    rx4.setPatternOptions(QRegularExpression::UseUnicodePropertiesOption);
    /*
    qDebug() << rx1.isValid() << rx1.pattern() << rx1.match(str).hasMatch();
    qDebug() << rx2.isValid() << rx2.pattern() << rx2.match(str).hasMatch();
    qDebug() << rx3.isValid() << rx3.pattern() << rx3.match(str).hasMatch();
    qDebug() << rx4.isValid() << rx4.pattern() << rx4.match(str).hasMatch();
    */
    // this matches
    str = "   دقَع   ";
    rx1.setPattern(QString("\\b%1\\b").arg("دقَع"));
    qDebug() << rx1.isValid() << rx1.pattern() << rx1.match(str).hasMatch();
    // this does not
    str = "   دقَعَ   ";
    rx1.setPattern(QString("\\b%1\\b").arg("دقَعَ"));
    qDebug() << rx1.isValid() << rx1.pattern() << rx1.match(str).hasMatch();
    str = "   دقَعْ   ";
    rx1.setPattern(QString("\\b%1\\b").arg("دقَعْ"));
    qDebug() << rx1.isValid() << rx1.pattern() << rx1.match(str).hasMatch();
    str = "   دقَعُ   ";
    rx1.setPattern(QString("\\b%1\\b").arg("دقَعُ"));
    qDebug() << rx1.isValid() << rx1.pattern() << rx1.match(str).hasMatch();
    str = "   دقَعً   ";
    rx1.setPattern(QString("\\b%1\\b").arg("دقَعً"));
    qDebug() << rx1.isValid() << rx1.pattern() << rx1.match(str).hasMatch();

  }
  if (0) {
    TestRunner t("فَتح");
    t.add("graeme",false);
    t.add("فَتَح",false);
    t.add("فَتَح",true,false,false,true);
    t.run();

    TestRunner t1("graeme");
    t1.add("Graeme",false,false,true,false);
    t1.add("Graeme",true,false,false,false);
    t1.run();
  }
  return;

}
int main(int argc, char *argv[])
{
  QString str;
  bool ok;
  bool verbose;
  QApplication app(argc,argv);
  QCommandLineParser parser;
  parser.addHelpOption();
  parser.setApplicationDescription("Text search of Lane's Lexicon.\n\n\
If this application is run from its standard location it will locate the configuration file (config.ini) and use\n\
it to find the name of the database (lexicon.sqlite) and XSLT files (entry.xslt). It will also use the configuration\n\
file to determine the characters that are to be treated as diacritics.\n\n\
If run from elsewhere,either\n\
   use the -r options to specify the directory containing \"config.ini\" or\n\
   use the -b, -t options to set the filenames or\n\
   ensure that current directory contains \"lexicon.sqlite\" and \"entry.xslt\"\n\
and use -a to specify diacritics (if required).\n\n\
Usage examples:\n\n\
  ./ensearch fellow\n\n\
search the Lexicon for all occurrences of 'fellow', case-insensitively, as a whole word or as part of word.\n\n\
  ./ensearch -c -w fellow\n\n\
search case sensitively, whole word match only.\n\n\
  ./ensearch -d -a 64e,64f  -i ftH\n\n\
convert 'ftH' to its Arabic equivalent under the 'Buckwalter safe' transliteration, treat the characters 0x64e and 0x64f as diacritics, and search for that Arabic text, ignoring the specified diacritics.\n\n\
 ./ensearch -x \"friendship|fellowship\"\n\n\
regular expression search for \"friendship\" or \"fellowship\".\n\
 ");
  parser.addPositionalArgument("<search pattern>", QCoreApplication::translate("main", "Search pattern"));


  //
  // have paged table with option of number of rows in table
  //


  QCommandLineOption criticsOption(QStringList() <<"a" << "set-diacritics",QObject::tr("Comma separated list of code points (in hex)"),"critics");
  parser.addOption(criticsOption);

  QCommandLineOption dbOption(QStringList() <<"b" << "db-file",QObject::tr("Database file name relative to current directory"),"dbfile name");
  parser.addOption(dbOption);

  QCommandLineOption caseOption(QStringList() <<"c" << "case-sensitive",QObject::tr("Case sensitive"));
  parser.addOption(caseOption);

  QCommandLineOption diacriticsOption(QStringList() <<"d" << "ignore-diacritics",QObject::tr("Ignore diacritics (Arabic only)"));
  parser.addOption(diacriticsOption);

  QCommandLineOption noxrefOption(QStringList() <<"e" << "no-xref",QObject::tr("Do not use cross-reference table"));
  parser.addOption(noxrefOption);

  QCommandLineOption fieldOption(QStringList() <<"f" << "fields",QObject::tr("Specify output information: R(oot), H(ead word, N(ode), O(ffset),P(age), T(ext), V(olume). Defaults to all."),"fields");
  fieldOption.setDefaultValue("RHNOTVP");
  parser.addOption(fieldOption);

  QCommandLineOption guiOption(QStringList() <<"g" << "gui",QObject::tr("Show graphical interface"));
  parser.addOption(guiOption);
  parser.addHelpOption();
  QCommandLineOption buckOption(QStringList() <<"i" << "buckwalter-input",QObject::tr("Buckwalter safe input"));
  parser.addOption(buckOption);

  QCommandLineOption listOption(QStringList() <<"l" << "list-size",QObject::tr("Table rows for graphical interface"),"rows");
  listOption.setDefaultValue("10");
  parser.addOption(listOption);

  QCommandLineOption testOption(QStringList() <<"m" << "test",QObject::tr("Invoke test routine (Development only)"));
  parser.addOption(testOption);

 QCommandLineOption nodeOption(QStringList() <<"n" << "node",QObject::tr("Node or comma delimited list of nodes to be searched"),"node");
  parser.addOption(nodeOption);

  QCommandLineOption outputOption(QStringList() <<"o" << "output",QObject::tr("Send results to file"),"output file name");
  parser.addOption(outputOption);

  QCommandLineOption patternOption(QStringList() <<"p" << "pattern",QObject::tr("Search pattern, enclose multi-word patterns in quotes"),"pattern");
  parser.addOption(patternOption);

  QCommandLineOption resourcesOption(QStringList() <<"r" << "resources",QObject::tr("Location of Resources directory"),"Path to resources directory");
#ifdef __APPLE__
    resourcesOption.setDefaultValue(QCoreApplication::applicationDirPath() + "/../Resources");
#else
    resourcesOption.setDefaultValue(QCoreApplication::applicationDirPath() + QDir::separator() + "Resources");
#endif

  parser.addOption(resourcesOption);

  QCommandLineOption separatorOption(QStringList() <<"s" << "separator",QObject::tr("Output field separator,default \"|\""),"separator character");
  separatorOption.setDefaultValue("|");
  parser.addOption(separatorOption);


  QCommandLineOption xsltOption(QStringList() <<"t" << "xslt-filename",QObject::tr("XSLT file name relative to current directory"),"xslt-name");
  parser.addOption(xsltOption);

  QCommandLineOption verboseOption(QStringList() <<"v" << "verbose",QObject::tr("Show search progress"));
  parser.addOption(verboseOption);

  QCommandLineOption wholeOption(QStringList() <<"w" << "whole-word",QObject::tr("Whole word match"));
  parser.addOption(wholeOption);

  QCommandLineOption regexOption(QStringList() <<"x" << "regex",QObject::tr("Regular expression search"));
  parser.addOption(regexOption);

  QCommandLineOption sizeOption(QStringList() <<"z" << "size",QObject::tr("Show number of characters to the left and right of search hit"),"number");
  sizeOption.setDefaultValue("30");
  parser.addOption(sizeOption);


  parser.process(app);
  QStringList posargs = parser.positionalArguments();

  TextSearch searcher;

  QString resourcesDir = parser.value(resourcesOption);
  verbose = parser.isSet(verboseOption);
  QFileInfo configFile(resourcesDir,"config.ini");
  //
  if (configFile.exists()) {
    QSettings config(configFile.absoluteFilePath(),QSettings::IniFormat);
    config.beginGroup("System");
    QString currentTheme = config.value("Theme","default").toString();
    QString themeDirectory = config.value("Theme directory","themes").toString();
    QFileInfo sd(resourcesDir,themeDirectory);
    sd.setFile(sd.absoluteFilePath(),currentTheme);
    QFileInfo si(sd.absoluteFilePath(),"settings.ini");
    if (si.exists()) {
      searcher.setSettingsPath(si.absoluteFilePath());
      // set the XSLT file name
      QSettings settings(si.absoluteFilePath(),QSettings::IniFormat);
      settings.beginGroup("Resources");
      QString xsltdir = settings.value("XSLT","xslt").toString();
      settings.endGroup();
      settings.beginGroup("XSLT");
      QString xslt = settings.value(SID_XSLT_ENTRY,"entry.xslt").toString();
      settings.endGroup();
      QFileInfo xsltFile(sd.absoluteFilePath(),xsltdir);
      xsltFile.setFile(xsltFile.absoluteFilePath(),xslt);
      if (xsltFile.exists()) {
        searcher.setXsltFileName(xsltFile.absoluteFilePath());
      }
      // set the database name
      settings.beginGroup("System");
      QString db = settings.value(SID_SYSTEM_DATABASE).toString();
      settings.endGroup();
      QFileInfo dbFile(resourcesDir,db);
      if (dbFile.exists()) {
        searcher.setDbFileName(dbFile.absoluteFilePath());
      }
    }
  }
  QSqlDatabase db;
  if (parser.isSet(dbOption)) {
    QString dbname = parser.value(dbOption);
    QFileInfo fi(dbname);
    if (! fi.exists()) {
      std::cerr <<  qPrintable(QString("Cannot find supplied database file: %1").arg(dbname)) << std::endl;
      return 0;
    }
    else {
      searcher.setDbFileName(fi.absoluteFilePath());
    }
  }
  if (parser.isSet(xsltOption)) {
    QString str = parser.value(xsltOption);
    QFileInfo fi(str);
    if (! fi.exists()) {
      std::cerr <<  qPrintable(QString("Cannot find supplied XSLT file: %1").arg(str)) << std::endl;
      return 0;
    }
    else {
      searcher.setXsltFileName(fi.absoluteFilePath());
    }
  }
  /**
   * The minimum needed is the db file and xslt file. If we haven't got them, try
   * as a last resort the current directory
   */

  if (! searcher.ok()) {
    if (! QFileInfo::exists(searcher.dbFile())) {
      if (QFileInfo::exists("lexicon.sqlite")) {
        searcher.setDbFileName("lexicon.sqlite");
      }
    }
    if (! QFileInfo::exists(searcher.xsltFile())) {
      if (QFileInfo::exists("entry.xslt")) {
        searcher.setXsltFileName("entry.xslt");
      }
    }
  }
  if (! searcher.ok()) {
    if (! configFile.exists()) {
      std::cerr << qPrintable(QString("Cannot find \"config.ini\" in %1").arg(parser.value(resourcesOption))) << std::endl;
      std::cerr << "Use -r option to specify the directory containing \"config.ini\" or" << std::endl;
      std::cerr << "the -d and -t options for the full path names of the database file and the  XSLT file respectively" << std::endl;
      return 1;
    }
  }
  support = new LaneSupport(resourcesDir);
  if (parser.isSet(testOption)) {
    QString fileName;
    if (posargs.size() > 0) {
      fileName = posargs[0];
    }

    test(fileName);
    return 0;
  }

  if (db.isOpen()) {
    db.close();
  }
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(searcher.dbFile()); //dbname);
  db.open();
  if (! db.isOpen()) {
    std::cerr  << qPrintable(QString("Could not open database:%1").arg(searcher.dbFile())) << std::endl;
    if (support != 0) {
      delete support;
    }
    return 0;
  }

  QFileInfo fi;
  QString pattern;

  if (! parser.isSet(patternOption)) {
      if (posargs.size() > 0) {
        pattern = posargs[0];
      }
  }
  else {
    pattern = parser.value(patternOption);
  }
  if (pattern.length() == 0) {
    std::cerr << qPrintable(QString("No search pattern supplied")) << std::endl;
    parser.showHelp();
    return 0;
  }

  if (parser.isSet(buckOption) && ! parser.isSet(regexOption)) {
    QString t(pattern);
    pattern = searcher.fromSafe(pattern);
  }

  pattern = arbInput(pattern);

  QString fields = parser.value(fieldOption);
  QRegularExpression fx("[^RHOPNTV]+");
  if (fx.match(fields).hasMatch()) {
    std::cerr << qPrintable(QString("Unknown output field requested, ignored (use only RHNOPTV)")) << std::endl;
  }
  if (parser.isSet(criticsOption)) {
    searcher.setDiacritics(parser.value(criticsOption));
  }
  else {
    searcher.setDiacritics();
  }
  searcher.setVerbose(verbose);
  searcher.m_separator = parser.value(separatorOption);
  int padding = 30;

  if (parser.isSet(sizeOption)) {

    QString v = parser.value(sizeOption);
    int sz  = v.toInt(&ok);
    if (ok) {
      padding = sz;
      searcher.setPadding(padding);
    }
  }
  if (parser.isSet(noxrefOption)) {
    searcher.setIgnoreXref(true);
  }

  searcher.setFields(parser.value(fieldOption));
  searcher.setSearch(pattern,
                     parser.isSet(regexOption),
                     parser.isSet(caseOption),
                     parser.isSet(wholeOption),
                     parser.isSet(diacriticsOption));

  searcher.setNode(parser.value(nodeOption));
  QStringList nodes;
  QSqlQuery query;
  int rows = 0;

  str = parser.value(listOption);

  rows = str.toInt(&ok,10);
  if (! ok ) {
    rows = 10;
  }
  searcher.setListSize(rows);

  if (! parser.isSet(guiOption)) {
    SearchRunner * r = new SearchRunner;

    if (verbose) {
      QObject::connect(&searcher,SIGNAL(recordsRead(int)),r,SLOT(recordsRead(int)));
    }
    searcher.search();
    delete r;
    QFile of;
    if (parser.isSet(outputOption)) {
      searcher.toFile(parser.value(outputOption));
    }
    else {
      searcher.toFile();
    }
    searcher.setPages(rows);
  }
  else {
    EnsearchWidget * w = new EnsearchWidget(rows);
    if (parser.isSet(criticsOption)) {
      w->setDiacritics(parser.value(criticsOption));
    }
    else {
      w->setDiacritics();
    }
    w->setPadding(padding);
    w->setFields(fields);
    w->setNode(parser.value(nodeOption));
    w->searcher()->setIgnoreXref(parser.isSet(noxrefOption));
    w->setSearch(pattern,
                 parser.isSet(regexOption),
                 parser.isSet(caseOption),
                 parser.isSet(wholeOption),
                 parser.isSet(diacriticsOption));
    w->search();
    w->show();
    app.exec();
  }
  if (support != 0) {
    delete support;
  }
  db.close();
  return 0;
}
