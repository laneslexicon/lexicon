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
  QByteArray a;
  QByteArray b;
  QMap<QChar,QChar> safe;
  safe.insert(QChar('C'),QChar(0x621));
  safe.insert(QChar('M'),QChar(0x622));
  safe.insert(QChar('O'),QChar(0x623));
  safe.insert(QChar('W'),QChar(0x624));
  safe.insert(QChar('I'),QChar(0x625));
  safe.insert(QChar('Q'),QChar(0x626));
  safe.insert(QChar('A'),QChar(0x627));
  safe.insert(QChar('b'),QChar(0x628));
  safe.insert(QChar('p'),QChar(0x629));
  safe.insert(QChar('t'),QChar(0x62A));
  safe.insert(QChar('v'),QChar(0x62B));
  safe.insert(QChar('j'),QChar(0x62C));
  safe.insert(QChar('H'),QChar(0x62D));
  safe.insert(QChar('x'),QChar(0x62E));
  safe.insert(QChar('d'),QChar(0x62F));
  safe.insert(QChar('V'),QChar(0x630));
  safe.insert(QChar('r'),QChar(0x631));
  safe.insert(QChar('z'),QChar(0x632));
  safe.insert(QChar('s'),QChar(0x633));
  safe.insert(QChar('c'),QChar(0x634));
  safe.insert(QChar('S'),QChar(0x635));
  safe.insert(QChar('D'),QChar(0x636));
  safe.insert(QChar('T'),QChar(0x637));
  safe.insert(QChar('Z'),QChar(0x638));
  safe.insert(QChar('E'),QChar(0x639));
  safe.insert(QChar('g'),QChar(0x63A));
  safe.insert(QChar('f'),QChar(0x641));
  safe.insert(QChar('q'),QChar(0x642));
  safe.insert(QChar('k'),QChar(0x643));
  safe.insert(QChar('l'),QChar(0x644));
  safe.insert(QChar('m'),QChar(0x645));
  safe.insert(QChar('n'),QChar(0x646));
  safe.insert(QChar('h'),QChar(0x647));
  safe.insert(QChar('w'),QChar(0x648));
  safe.insert(QChar('Y'),QChar(0x649));
  safe.insert(QChar('y'),QChar(0x64A));
  safe.insert(QChar('F'),QChar(0x64B));
  safe.insert(QChar('N'),QChar(0x64C));
  safe.insert(QChar('K'),QChar(0x64D));
  safe.insert(QChar('a'),QChar(0x64E));
  safe.insert(QChar('u'),QChar(0x64F));
  safe.insert(QChar('i'),QChar(0x650));
  safe.insert(QChar('~'),QChar(0x651));
  safe.insert(QChar('e'),QChar(0x670));
  safe.insert(QChar('L'),QChar(0x671));
  safe.insert(QChar('_'),QChar(0x640));
  safe.insert(QChar(','),QChar(0x60C));
  safe.insert(QChar('-'),QChar(0x0AD));
  safe.insert(QChar(';'),QChar(0x61b));
  safe.insert(QChar('?'),QChar(0x60f));
  safe.insert(QChar('P'),QChar(0x67E));
  safe.insert(QChar('J'),QChar(0x686));
  safe.insert(QChar('B'),QChar(0x6A4));
  safe.insert(QChar('G'),QChar(0x6AF));

  QString t("fataHa");
  QString ot;
  for(int i=0;i < t.length();i++) {
    if (safe.contains(t.at(i))) {
      ot.append(safe.value(t.at(i)));
    }
    else {
      ot.append(t.at(i));
    }
  }
  qDebug() << ot;
  QRegularExpression rx("[\u0600-\u06ff]+"); // seems to work but the backslashes
  //QRegularExpression rx("[0x0600-0x06ff]+");
  QRegularExpression en("[a-zA-Z]+");
  QStringList tests;
  tests << QString("termed خبب. (TA");
  tests << "english test";
  tests << QString(" خبب.");
  tests << QString("خبب");
  tests << QString("خَبَب");
  bool ok;
  QString tee("0074");
  qDebug() << QString("%1").arg(QChar(tee.toInt(&ok,16)));

  qDebug() << rx.isValid() << rx.pattern();

  for (int i=0;i < tests.length();i++) {
    qDebug() << "Check rx" << tests[i] << rx.match(tests[i]).hasMatch() << en.match(tests[i]).hasMatch();
  }
  QRegularExpression dx("خ[\u064e\u064f\u0650]*ب");
  qDebug() << "diacritic test :true" << dx.match(tests[tests.size() - 1]).hasMatch() << dx.pattern();
  QString r = tests[tests.size() - 1];

  TextSearch ts;
  QRegularExpression q = ts.buildRx(r,true,false,false);
  qDebug() << "buildRx" << q.isValid() << q.pattern();
  QString src("it is termed خبب. (TA");
  qDebug() << q.match(src).hasMatch();
  return;

  if (fileName.length() == 0) {
    fileName = "test_node.html";
  }
  QString css;
  QFile f("test.css");
  if (f.open(QIODevice::ReadOnly)) {
    QTextStream in(&f);
    css = in.readAll();
    f.close();
  }
  f.setFileName(fileName);
  if (f.open(QIODevice::ReadOnly)) {
    QTextStream in(&f);
    QString txt = in.readAll();
    qDebug() << css << "\n" << txt;
    QTextDocument doc;
    if (! css.isEmpty()) {
      //      doc.setDefaultStyleSheet(css);
    }
    //    QFont font("Amiri");
    //    doc.setDefaultFont(font);
    doc.setHtml(txt);
  }
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

    //    test(fileName);
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
  searcher.m_pattern = pattern;
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

  QStringList nodes;
  QSqlQuery query;
  SearchRunner * r = new SearchRunner;
  searcher.setNode(parser.value(nodeOption));
  if (verbose) {
    QObject::connect(&searcher,SIGNAL(recordsRead(int)),r,SLOT(recordsRead(int)));
  }
  searcher.search();
  delete r;
  db.close();

  bool fileOutput = false;
  QFile of;
  if (parser.isSet(outputOption)) {
    searcher.toFile(parser.value(outputOption));
  }
  else {
    searcher.toFile();
  }
  if (parser.isSet(guiOption)) {

  }
  return 0;
}
