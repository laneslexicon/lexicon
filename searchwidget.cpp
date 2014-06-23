#include "searchwidget.h"
#include "searchoptions.h"
#include "graphicsentry.h"
#include "application.h"
#include "namespace.h"
#include "laneslexicon.h"
#include "imlineedit.h"
#include "nodeview.h"
//extern LanesLexicon * getApp();
SearchWidget::SearchWidget(QWidget * parent) : QWidget(parent) {
  readSettings();
  setMaxRecords();
  QVBoxLayout * layout = new QVBoxLayout;
  /// add the target
  m_findTarget = new ImLineEdit;
  m_findButton = new QPushButton(tr("Find"));
  m_hideOptionsButton = new QPushButton(tr("Hide options"));
  m_hideOptionsButton->setCheckable(true);

  connect(m_findButton,SIGNAL(clicked()),this,SLOT(findTarget()));
  connect(m_hideOptionsButton,SIGNAL(clicked()),this,SLOT(hideOptions()));
  QHBoxLayout * targetlayout = new QHBoxLayout;
  targetlayout->addWidget(m_findTarget);
  targetlayout->addWidget(m_findButton);
  targetlayout->addWidget(m_hideOptionsButton);

  m_search = new SearchOptions(Lane::Word);
  QWidget * container = new QWidget;
  QVBoxLayout * containerlayout = new QVBoxLayout;
  m_list = new QTableWidget;
  m_list->setColumnCount(4);
  m_list->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  QStringList headers;
  headers << tr("Root") << tr("Entry") <<  tr("Node") << tr("Position") << tr("Context");
  m_list->setHorizontalHeaderLabels(headers);
  m_list->horizontalHeader()->setStretchLastSection(true);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  m_list->installEventFilter(this);

  m_rxlist = new QTableWidget;
  m_rxlist->setColumnCount(5);
  m_rxlist->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_rxlist->setSelectionMode(QAbstractItemView::SingleSelection);

  m_rxlist->setHorizontalHeaderLabels(headers);
  m_rxlist->horizontalHeader()->setStretchLastSection(true);
  m_rxlist->setSelectionMode(QAbstractItemView::SingleSelection);
  m_rxlist->installEventFilter(this);
  //QStyle * style = m_list->style();
  //  QLOG_DEBUG() << "style hint" << style->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus);
  m_progress = new QProgressBar;
  m_progress->setMaximum(m_maxRecordCount);
  m_progress->hide();
  m_resultsText = new QLabel("");
  m_resultsText->hide();
  containerlayout->addLayout(targetlayout);
  containerlayout->addWidget(m_search);
  QHBoxLayout * listlayout = new QHBoxLayout;
  listlayout->addWidget(m_list);
  listlayout->addWidget(m_rxlist);
  containerlayout->addLayout(listlayout);
  containerlayout->addWidget(m_resultsText);
  containerlayout->addWidget(m_progress);
  container->setLayout(containerlayout);

  m_text = new GraphicsEntry;
  m_text->hide();

  QSplitter * splitter = new QSplitter(Qt::Vertical);
  splitter->addWidget(container);
  splitter->addWidget(m_text);
  splitter->setStretchFactor(0,0);
  splitter->setStretchFactor(1,1);
  layout->addWidget(splitter);
  m_list->adjustSize();//resizeColumnsToContents();

  setLayout(layout);
  //connect(m_list,SIGNAL(currentItemChanged(QTableWidgetItem * ,QTableWidgetItem * )),
  //      this,SLOT(itemChanged(QTableWidgetItem * ,QTableWidgetItem * )));
  connect(m_rxlist,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
          this,SLOT(itemDoubleClicked(QTableWidgetItem * )));

  initXslt();
  //  if (m_list->rowCount() > 0)
  //    m_list->itemDoubleClicked(m_list->item(0,0));

  m_list->hide();
  m_search->setOptions(m_defaultOptions);
}
SearchWidget::~SearchWidget() {
  qDebug() << Q_FUNC_INFO;
}
int SearchWidget::count() {
  return m_list->rowCount();
}
void SearchWidget::itemChanged(QTableWidgetItem * item,QTableWidgetItem * /* prev */) {
  /// get the node
  item = item->tableWidget()->item(item->row(),2);
  QString node = item->text();
  m_nodeQuery.bindValue(0,node);
  m_nodeQuery.exec();
  /// missing node
  if ( ! m_nodeQuery.first()) {
    QLOG_WARN() << "No record for node" << node;
    return;
  }
  /// TODO make this a QSettings option or dialog option
  Place np;
  np.setNode(node);
  //  np.setNodeOnly(true);
  Place p = m_text->getXmlForRoot(np);
  if (p.isValid()) {
    m_text->highlight(m_target);
  }
  else {
    QLOG_DEBUG() << "Invalid place returned for node" << node;
  }
}
void SearchWidget::itemDoubleClicked(QTableWidgetItem * item) {
  /// get the node
  item = item->tableWidget()->item(item->row(),2);
  QString node = item->text();
  m_nodeQuery.bindValue(0,node);
  m_nodeQuery.exec();
  /// missing node
  if ( ! m_nodeQuery.first()) {
    QLOG_WARN() << "No record for node" << node;
    return;
  }
  /// TODO make this a QSettings option or dialog option
  QString xml = m_nodeQuery.value("xml").toString();
  QString html = this->transform(xml);
  NodeView * v = new NodeView(this);
  v->setCSS(m_currentCSS);
  v->setHeader(m_nodeQuery.value("root").toString(),m_nodeQuery.value("word").toString());
  v->document()->setHtml(html);
  v->show();
  v->raise();
  v->activateWindow();
  /*
  Place np;
  np.setNode(node);
  //  np.setNodeOnly(true);
  Place p = m_text->getXmlForRoot(np);
  if (p.isValid()) {
    m_text->highlight(m_target);
  }
  else {
    QLOG_DEBUG() << "Invalid place returned for node" << node;
  }
  */
}
bool SearchWidget::eventFilter(QObject * target,QEvent * event) {

  if (event->type() == QEvent::KeyPress) {
    QLOG_DEBUG() << Q_FUNC_INFO;
    QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
    switch(keyEvent->key()) {
      case Qt::Key_Enter: {
        QLOG_DEBUG() << "hit enter on table";
        if (keyEvent->modifiers() && Qt::ControlModifier) {
          //          m_tree->setFocus();
          return true;
        }
        break;
      }
    case Qt::Key_Return:
    case Qt::Key_Space: {
        QLOG_DEBUG() << "hit return on table";
        //        if (keyEvent->modifiers() && Qt::ControlModifier) {
        QTableWidgetItem * item = m_list->currentItem();
        if (item)
          m_list->itemDoubleClicked(item);

        break;
      }
      case Qt::Key_E: {
        if (keyEvent->modifiers() && Qt::ControlModifier) {
          //          if (target == m_tree)
            //          m_tabs->currentWidget()->setFocus();
          return true;
        }
        break;
      }
    default:
      break;
    }
  }
  return QWidget::eventFilter(target,event);
}
void SearchWidget::search(const QString & target,int options) {
  m_target = target;
  m_searchOptions = options;
  m_nodes.clear();
  QString sql;
  //  whole word with diacritics
  //   select id,node where word = ?
  //  part word with diacritics
  //   select id,node from xref where instr(word,?) > 0;
  //  whole word without diacritics
  //   select where bareword = ?
  //  part word without diacritics
  //   select where instr(bareword,?) > 0
  sql = buildSearchSql(options);
  qDebug() << "search sql" << sql;
  bool ok = false;
  /// TODO replace select *
  //   sql = QString("select * from xref where datasource = 1 and word = ? order by root,entry asc");
  if (m_query.prepare(sql)) {
    if (m_nodeQuery.prepare("select * from entry where datasource = 1 and nodeId = ?")) {
      ok = true;
    }
  }
  if (! ok ) {
    QLOG_WARN() << "Error prepare SQL";
    return;
  }
  m_list->setRowCount(0);
#define NODE_COLUMN 2

  qint64 st = QDateTime::currentMSecsSinceEpoch();
  m_query.bindValue(0,m_target);
  m_query.exec();
  QMap<QString,int> nodes;

  QTableWidgetItem * item;
  /// TODO include count in table ?
  int count = 0;
  while(m_query.next()) {
    count++;
    QString t;
    QString word;
    QString entry;
    if (options & Lane::Full) {
      t = m_query.value("node").toString();
      word = m_query.value("word").toString();
    }
    else {
      t = m_query.value("nodeid").toString();
      word = m_query.value("word").toString();
    }
    if ( ! m_nodes.contains(t))
      m_nodes << t;
    else {
      nodes[t] = nodes.value(t) + 1;
    }
    int row = m_list->rowCount();
    m_list->insertRow(row);
    item = new QTableWidgetItem(m_query.value("root").toString());
    item->setFont(m_resultsFont);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_list->setItem(row,0,item);

    item = new QTableWidgetItem(word);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    item->setFont(m_resultsFont);
    m_list->setItem(row,1,item);

    item = new QTableWidgetItem(t);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_list->setItem(row,NODE_COLUMN,item);


    nodes.insert(t,1);
    m_list->setItem(row,3,new QTableWidgetItem("1"));
  }
  /*
  for(int i=0; i < m_list->rowCount();i++) {
    QString t = m_list->item(i,NODE_COLUMN)->text();
    if (! t.isEmpty() && nodes.contains(t)) {
      m_list->item(i,3)->setText(QString("%1").arg(nodes.value(t)));
    }
  }
  */
  //  emit(searchResult(QString(tr("Found %1 items")).arg(count)));
  QString t = QString(tr("Search for %1, find count %2 ")).arg(m_target).arg(count);
  if (m_searchOptions & Lane::Ignore_Diacritics)
    t += tr(", ignoring diacritics");
  if (m_searchOptions & Lane::Whole_Word)
    t += tr(", whole word match");
  m_resultsText->setText(t);
  m_resultsText->show();
  if (m_list->rowCount() > 0)
    m_list->itemDoubleClicked(m_list->item(0,0));

  qDebug() << "readcount" << count << "time" <<   (QDateTime::currentMSecsSinceEpoch() - st);

}
void SearchWidget::hideOptions() {
  if (m_hideOptionsButton->isChecked()) {
    m_search->hide();
    m_hideOptionsButton->setText("Show options");
  }
  else {
    m_search->show();
    m_hideOptionsButton->setText("Hide options");
  }
}
void SearchWidget::findTarget() {
  qDebug() << Q_FUNC_INFO;
  int options = m_search->getOptions();
  //  this->search(m_findTarget->text(),options);
  m_progress->setValue(0);
  m_progress->show();
  this->regexSearch(m_findTarget->text(),options);

  m_progress->hide();

}
/**
 * There are two ways of doing the search if ignore diacritics are set
 *    (1)  build a regex with with each with each character in the search string
 *         followed by an optional character class of all the diacritics
 *
 *    (2)  For each search word from the db, replace all characters from the
 *         the diacritics class by an empty string and then do the search
 *         i.e strip the diacritics and then attempt to match
 *
 *    In the code below, if replaceSearch is true, the 2nd method is used
 *    otherwise the first.
 *
 *  On my timings there second method adds about 200ms.
 * @param target
 * @param options
 */
void SearchWidget::regexSearch(const QString & target,int options) {
  bool replaceSearch = true;
  qDebug() << Q_FUNC_INFO;
  m_target = target;
  m_searchOptions = options;


  QRegExp rx;
  QString pattern;
  QRegExp rxclass("[\\x064b\\x064c\\x064d\\x064e\\x064f\\x0650\\x0651\\x0652\\x0670\\x0671]*");

  QString sql = "select id,word,root,entry,node,nodenum from xref where datasource = 1 order by nodenum asc";

  if (!replaceSearch) {
    if (options & Lane::Ignore_Diacritics) {
      QString ar("[\\x064b\\x064c\\x064d\\x064e\\x064f\\x0650\\x0651\\x0652\\x0670\\x0671]*");
      QStringList cc = target.split("");
      QString brx = "";
      for(int i=0;i < cc.size();i++) {
        pattern += cc[i] + ar;
      }
      m_target = pattern;
    }
  }
  if (options & Lane::Whole_Word) {
    pattern = "\\b" + m_target + "\\b";
  }
  else {
    pattern = m_target;
  }
  rx.setPattern(pattern);


  bool ok = false;
  if (m_query.prepare(sql)) {
    if (m_nodeQuery.prepare("select root,word,xml from entry where datasource = 1 and nodeId = ?")) {
      ok = true;
    }
  }
  if (! ok ) {
    QLOG_WARN() << "Error prepare SQL";
    return;
  }
  if (m_debug) {
    m_rxlist->showColumn(2);
    m_rxlist->showColumn(3);
  }
  else {
    m_rxlist->hideColumn(2);
    m_rxlist->hideColumn(3);
  }
  if (options & Lane::Head)
    m_rxlist->hideColumn(4);
  else
    m_rxlist->showColumn(4);


  m_rxlist->setRowCount(0);
#define NODE_COLUMN 2
  qint64 st = QDateTime::currentMSecsSinceEpoch();
  //  m_nodquery.bindValue(0,m_target);


  int headCount = 0;
  int textCount = 0;
  int readCount = 0;
  int entryCount = 0;
  QString node;
  QString root;
  QString headword;

  //  qDebug() << "Pattern" << pattern;
  //  qDebug() << "Sql" << sql;

  m_query.exec();
  while(m_query.next()) {
    readCount++;
    if ((readCount % 1000) == 0) {
      m_progress->setValue(readCount);
    }

    QString word = m_query.value("word").toString();
    /// strip diacritics if required
    if (replaceSearch) {
      if (options & Lane::Ignore_Diacritics)
        word =  word.replace(rxclass,QString());
    }
    if ((word.indexOf(rx) != -1) && (node != m_query.value("node").toString())) {
      entryCount++;
      node = m_query.value("node").toString();
      m_nodeQuery.bindValue(0,node);
      if ( m_nodeQuery.exec() &&  m_nodeQuery.first()) {
        root = m_nodeQuery.value("root").toString();
        headword = m_nodeQuery.value("word").toString();
        /// strip diacritics if required
        if (replaceSearch) {
          if (options & Lane::Ignore_Diacritics)
            headword =  headword.replace(rxclass,QString());
        }
        if (headword.indexOf(rx) != -1) {
          addRow(root,m_nodeQuery.value("word").toString(),node,"Head word",0);
          headCount++;
        }
        headword = m_nodeQuery.value("word").toString();
        if (options & Lane::Full) {
          QString xml = m_nodeQuery.value("xml").toString();
          QTextDocument * doc  = fetchDocument(xml);
          if (doc->characterCount() > 0) {
            getTextFragments(doc,target,options);
            for(int i=0;i < m_fragments.size();i++) {
              addRow(root,headword,node,m_fragments[i],m_positions[i]);
            }
            textCount += m_fragments.size();
          }
        }
      }
      else {
        qDebug() << "Error in node Query sql";
      }
    }
  }

  //  emit(searchResult(QString(tr("Found %1 items")).arg(count)));
  QString t;
  if (Lane::Full)
    t = QString(tr("Search for %1, found %2 in %3 entries")).arg(target).arg(textCount + headCount).arg(entryCount);
  else
    t=  QString(tr("Search for %1, found %2 entries")).arg(target).arg(headCount);
  if (m_searchOptions & Lane::Ignore_Diacritics)
    t += tr(", ignoring diacritics");
  if (m_searchOptions & Lane::Whole_Word)
    t += tr(", whole word match");
  m_resultsText->setText(t);
  m_resultsText->show();
  //  if (m_rxlist->rowCount() > 0)
  //    m_rxlist->itemDoubleClicked(m_rxlist->item(0,0));
  m_rxlist->resizeColumnToContents(0);
  m_rxlist->resizeColumnToContents(2);
  m_rxlist->resizeColumnToContents(3);
  m_rxlist->resizeColumnToContents(4);
}
void SearchWidget::addRow(const QString & root,const QString & headword, const QString & node, const QString & text,int pos) {
  QTableWidgetItem * item;
  int row = m_rxlist->rowCount();
  m_rxlist->insertRow(row);
  item = new QTableWidgetItem(root);
  item->setFont(m_resultsFont);
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  m_rxlist->setItem(row,0,item);

  item = new QTableWidgetItem(headword);
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  item->setFont(m_resultsFont);
  m_rxlist->setItem(row,1,item);

  item = new QTableWidgetItem(node);
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  m_rxlist->setItem(row,NODE_COLUMN,item);

  m_rxlist->setItem(row,3,new QTableWidgetItem(QString("%1").arg(pos)));

  if (text.size() > 0) {

    ///
    /// Force left justification, since the text could begin with arabic or english. From:
    /// https://stackoverflow.com/questions/10998105/qt-how-to-change-the-direction-of-placeholder-in-a-qlineedit
    ///
    ///
    /*
    QLineEdit * e = new QLineEdit(text);
    QKeyEvent ke(QEvent::KeyPress, Qt::Key_Direction_L, 0);
    QApplication::sendEvent(e, &ke);
    m_rxlist->setCellWidget(row,4,e);
    */
    QTextEdit * d = new QTextEdit(this);
    d->setHtml("<html><body>" + text + "</body></html>");
    d->setReadOnly(true);
    d->setMaximumHeight(30);
    m_rxlist->setCellWidget(row,4,d);
  }
  else
    m_rxlist->setItem(row,4,new QTableWidgetItem(text));


}
/**
 * build SQL for search using DB style search
 *
 * @param options
 *
 * @return
 */
QString SearchWidget::buildSearchSql(int options) {
  QString sql;
  if (options & Lane::Full) {
    sql = "select id,word,root,entry,node from xref where datasource = 1 ";
  }
  else {
    sql = "select id,word,root,nodeid,nodenum from entry where datasource = 1 ";
  }
  if (options & Lane::Ignore_Diacritics) {
    if (options & Lane::Whole_Word) {
      sql += "and bareword = ? ";
    }
    else {
      sql += "and instr(bareword,?) > 0";
    }
  }
  else {
    if (options & Lane::Whole_Word) {
      sql += "and word = ? ";
    }
    else {
      sql += "and instr(word,?) > 0";
    }
  }
  if (options & Lane::Full) {
    sql += " order by root,entry asc";
  }
  else {
    sql += " order by root,nodenum asc";
  }
  return sql;
}
/**
 * build SQL for search using regex
 *
 * @param options
 *
 * @return
 */
QString SearchWidget::buildRxSql(int options) {
  QString sql;
  //  whole word with diacritics
  //   select id,node where word = ?
  //  part word with diacritics
  //   select id,node from xref where instr(word,?) > 0;
  //  whole word without diacritics
  //   select where bareword = ?
  //  part word without diacritics
  //   select where instr(bareword,?) > 0
  if (options & Lane::Full) {
  sql = "select id,word,root,entry,node from xref where datasource = 1 ";
  }
  else {
    sql = "select id,word,root,nodeid,nodenum from entry where datasource = 1 ";
  }
  return sql;
}
QTextDocument * SearchWidget::fetchDocument(const QString & xml) {
  /*
  if (1) {
    QFileInfo fi(QDir::tempPath(),QString("/tmp/%1.xml").arg(node));
    QFile f(fi.filePath());
    if (f.open(QIODevice::WriteOnly)) {
      QTextStream out(&f);
      out.setCodec("UTF-8");
      out << xml;
    }
  }
  */
  QString html = transform(xml);
  //  qDebug() <<  html;
  m_nodeDoc.setHtml(html);
  QTextDocument * doc = new QTextDocument;
  doc->setHtml(html);
  return doc;
}
QString SearchWidget::transform(const QString & xml) {
  int ok = compileStylesheet(1,m_xsltSource);
  if (ok == 0) {
    QString html = xsltTransform(1,xml);
    if (! html.isEmpty()) {
      return html;
    }
  }
  /// could be errors in stylesheet or in the xml
  QStringList errors = getParseErrors();
  if (ok != 0) {
    errors.prepend("Errors when processing stylesheet:");
  }
  else {
    errors.prepend("Errors when processing entry:");
  }
  QMessageBox msgBox;
  msgBox.setText(errors.join("\n"));
  msgBox.exec();
  clearParseErrors();
  return QString();
}
void SearchWidget::readSettings() {
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  bool ok;
  settings->setIniCodec("UTF-8");
  settings->beginGroup("Entry");
  QString css = settings->value("CSS",QString("entry.css")).toString();
  readCssFromFile(css);
  //m_xsltSource = settings->value("XSLT",QString("entry.xslt")).toString();
  settings->endGroup();
  settings->beginGroup("Search");
  QString f = settings->value("Results font",QString()).toString();
  if (! f.isEmpty()) {
    m_resultsFont.fromString(f);
  }
  m_xsltSource = settings->value("XSLT",QString("node.xslt")).toString();
  m_defaultOptions = 0;
  QString v;
  v  = settings->value("Where",QString("full")).toString();
  if (v == "full")
    m_defaultOptions |= Lane::Full;
  else
    m_defaultOptions |= Lane::Head;

  v  = settings->value("Type",QString("normal")).toString();
  if (v == "normal")
    m_defaultOptions |= Lane::Normal;
  else
    m_defaultOptions |= Lane::Regex;

  v = settings->value("Ignore diacritics",QString("yes")).toString();
  if (v == "yes")
    m_defaultOptions |= Lane::Ignore_Diacritics;

  v = settings->value("Whole word",QString("no")).toString();
  if (v == "yes")
    m_defaultOptions |= Lane::Whole_Word;


  m_fragmentSize = settings->value("Fragment size",40).toInt();

  m_debug = settings->value("Debug",false).toBool();
  delete settings;
}
void SearchWidget::getTextFragments(QTextDocument * doc,const QString & target,int options) {
  QRegExp rx;
  QString pattern;
  QRegExp rxclass("[\\x064b\\x064c\\x064d\\x064e\\x064f\\x0650\\x0651\\x0652\\x0670\\x0671]*");

  if (options & Lane::Regex) {
    pattern = target;
  }
  else {
    if (options & Lane::Ignore_Diacritics) {
      QString ar("[\\x064b\\x064c\\x064d\\x064e\\x064f\\x0650\\x0651\\x0652\\x0670\\x0671]*");
      QStringList cc = target.split("");
      QString brx = "";
      for(int i=0;i < cc.size();i++) {
        pattern += cc[i] + ar;
      }
    }
    else {
      pattern = target;
    }
    if (options & Lane::Whole_Word) {
      pattern = "\\b" + pattern + "\\b";
    }
  }
  //  qDebug() << "Pattern" << pattern;
  rx.setPattern(pattern);
  m_fragments.clear();
  m_positions.clear();
  int position = 0;
  QTextCursor c = doc->find(rx,position);
  QString src = doc->toPlainText();
  QStringList f;
  int sx;
  int ex;
  int sz = m_fragmentSize;
  while(! c.isNull()) {
    //    qDebug() << "cursor pos" << c.position() <<  "offset in string" << src.indexOf(rx,position);
    position = c.position();
    if (position > sz)
      sx = position - sz;
    else
      sx = 0;

    if ((position + sz) > src.size())
      ex = src.size();
    else
      ex = position + sz;
    //    f << src.mid(sx,ex - sx);
    m_positions << position;
    m_fragments << src.mid(sx,ex - sx);
    //    qDebug() << "fragment size" << (ex - sx);  //QString("[%1][%2][%3][%4]").arg(position).arg(sx).arg(ex).arg(src);
    c = doc->find(rx,position);
  }
}
void SearchWidget::setMaxRecords() {
  bool ok = false;
  QSqlQuery maxq("select id from xref order by id desc limit 1");
  if (maxq.exec() && maxq.next())
    m_maxRecordCount = maxq.value(0).toInt(&ok);

  if (! ok )
    m_maxRecordCount = 544695;
}
/**
 * lines beginning with - are omitted
 *
 */
bool SearchWidget::readCssFromFile(const QString & name) {
  QFile f(name);
  if (! f.open(QIODevice::ReadOnly)) {
    QLOG_WARN()  << "Cannot open CSS file for reading: " << name
                 << f.errorString();
    return false;

  }
  QTextStream in(&f);
  QString css;
  while( ! in.atEnd()) {
    if (! css.startsWith("-")) {
      css += in.readLine();
    }
  }
  f.close();
  if (! css.isEmpty()) {
    m_currentCSS = css;
  }
  qDebug() << m_currentCSS;
  return true;
}
