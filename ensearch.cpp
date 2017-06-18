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
void test(QString & fileName) {
  /*
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
  */
  QRegularExpression crx("Reliance");

  QTextDocument doc;
  doc.setPlainText("This is reliance");
  QTextCursor c = doc.find(crx,0);
  qDebug() << "Find case mismatch" <<  c.position();
  crx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
  c = doc.find(crx,0);
  qDebug() << "Find case insensitive" << c.position();

  return;

}
int main(int argc, char *argv[])
{


  bool verbose;
  QApplication app(argc,argv);
  QString settingsFile("Resources/themes/default/settings.ini");
  QCommandLineParser parser;
  parser.addHelpOption();
  //  parser.setApplicationDescription("Full text search of Lane's Lexicon.");
  parser.addPositionalArgument("<search pattern>", QCoreApplication::translate("main", "Search pattern"));
 QCommandLineOption nodeOption(QStringList() <<"n" << "node",QObject::tr("Node or comma delimited list of nodes to be searched"),"node");
  parser.addOption(nodeOption);

  parser.addHelpOption();

  QCommandLineOption patternOption(QStringList() <<"p" << "pattern",QObject::tr("Search pattern, enclose multi-word patterns in quotes"),"pattern");
  parser.addOption(patternOption);

  QCommandLineOption outputOption(QStringList() <<"o" << "output",QObject::tr("Send results to file"),"output file name");
  parser.addOption(outputOption);

  QCommandLineOption separatorOption(QStringList() <<"s" << "separator",QObject::tr("Output field separator,default \"|\""),"separator character");
  separatorOption.setDefaultValue("|");
  parser.addOption(separatorOption);

  QCommandLineOption verboseOption(QStringList() <<"v" << "verbose",QObject::tr("Show search progress"));
  parser.addOption(verboseOption);
  QCommandLineOption resourcesOption(QStringList() <<"r" << "resources",QObject::tr("Location of Resources directory"),"Path to resources directory");
#ifdef __APPLE__
    resourcesOption.setDefaultValue(QCoreApplication::applicationDirPath() + "/../Resources");
#else
    resourcesOption.setDefaultValue(QCoreApplication::applicationDirPath() + QDir::separator() + "Resources");
#endif

  parser.addOption(resourcesOption);

  QCommandLineOption testOption(QStringList() <<"t" << "test",QObject::tr("Invoke test routine"));
  parser.addOption(testOption);

  QCommandLineOption diacriticsOption(QStringList() <<"d" << "ignore-diacritics",QObject::tr("Ignore diacritics (Arabic only)"));
  parser.addOption(diacriticsOption);

  QCommandLineOption caseOption(QStringList() <<"c" << "case-sensitive",QObject::tr("Case sensitive"));
  parser.addOption(caseOption);

  QCommandLineOption wholeOption(QStringList() <<"w" << "whole-word",QObject::tr("Whole word match"));
  parser.addOption(wholeOption);

  QCommandLineOption regexOption(QStringList() <<"x" << "regex",QObject::tr("Regular expression search"));
  parser.addOption(regexOption);

  QCommandLineOption dbOption(QStringList() <<"b" << "db-file",QObject::tr("Database file name relative to current directory"),"dbfile name");
  parser.addOption(dbOption);

  QCommandLineOption sizeOption(QStringList() <<"z" << "size",QObject::tr("Show number of characters to the left and right of search hit"),"number");
  parser.addOption(sizeOption);

  QCommandLineOption buckOption(QStringList() <<"i" << "buckwalter-input",QObject::tr("Buckwalter safe input"));
  parser.addOption(buckOption);

  //
  // have paged table with option of number of rows in table
  //
  QCommandLineOption guiOption(QStringList() <<"g" << "gui",QObject::tr("Show graphical interface"));
  parser.addOption(guiOption);


  QCommandLineOption fieldOption(QStringList() <<"f" << "fields",QObject::tr("Specify output information: R(oot), H(ead word, N(ode), O(ffset),P(age), T(ext), V(olume). Defaults to all.)"),"RHNOTVP");
  fieldOption.setDefaultValue("RHNOTVP");
  parser.addOption(fieldOption);


  parser.process(app);
  QStringList posargs = parser.positionalArguments();

  if (parser.isSet(testOption)) {
    QString fileName;
    if (posargs.size() > 0) {
      fileName = posargs[0];
    }

    test(fileName);
    return 0;
  }
  TextSearch searcher;

  QString resourcesDir = parser.value(resourcesOption);
  verbose = parser.isSet(verboseOption);
  QFileInfo configFile(resourcesDir,"config.ini");

  if (! configFile.exists()) {
    std::cerr << qPrintable(QString("Cannot find \"config.ini\" in %1").arg(parser.value(resourcesOption))) << std::endl;
    std::cerr << "Use -r option to specify the directory containing \"config.ini\"" << std::endl;
    return 1;
  }
  support = new LaneSupport(resourcesDir);
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

  //  QFile dbfile;
  QSqlDatabase db;
///  QString dbname;

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


  //  dbfile.setFileName(searcher.dbFile());

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
  //  fi.setFile(parser.value(iniOption));
  //  if (fi.exists()) {
  //    searcher.setSettingsPath(fi.absoluteFilePath());
  //  }
  QString fields = parser.value(fieldOption);
  QRegularExpression fx("[^RHOPNTV]+");
  if (fx.match(fields).hasMatch()) {
    std::cerr << qPrintable(QString("Unknown output field requested, ignored (use only RHNOPTV)")) << std::endl;
  }
  searcher.setVerbose(verbose);
  searcher.m_separator = parser.value(separatorOption);
  //  searcher.m_pattern = pattern;
  if (parser.isSet(sizeOption)) {
    bool ok = true;
    QString v = parser.value(sizeOption);
    int sz = v.toInt(&ok);
    if (ok) {
      searcher.setPadding(sz);
    }
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
    searcher.setPages(10);
    qDebug() << searcher.getPage(1,false);
    //    searcher.dumpPages(false);
  }
  else {
    EnsearchWidget * w = new EnsearchWidget;
    w->setPadding(20);
    w->setFields("RHOPNTV");
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
