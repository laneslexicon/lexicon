#include "fullsearch.h"
#include "searchoptions.h"
#include "graphicsentry.h"
#include "application.h"
#include "namespace.h"
#include "laneslexicon.h"
#include "imlineedit.h"
#include "nodeview.h"
#include "focustable.h"
#include "keyboard.h"
#define NODE_COLUMN 2
#define POSITION_COLUMN 3
#define CONTEXT_COLUMN 4
extern LanesLexicon * getApp();
/**
 *
 *
 * @param parent
 */
FullSearchWidget::FullSearchWidget(QWidget * parent) : QWidget(parent) {
  readSettings();
  //  setMaxRecords();
  m_attached = false;
  QVBoxLayout * layout = new QVBoxLayout;
  /// add the target
  m_findTarget = new ImLineEdit;
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  m_findTarget->readSettings(settings);

  m_findButton = new QPushButton(tr("F&ind"));
  m_findButton->setDefault(true);
  m_hideOptionsButton = new QPushButton(tr("Sho&w options"));
  m_hideOptionsButton->setCheckable(true);

  m_keyboardButton  = new QPushButton(tr("Show &keyboard"));
  m_keyboardButton->setAutoDefault(false);
  m_keyboardButton->setCheckable(true);

  m_keyboard = new KeyboardWidget(this);

  connect(m_findButton,SIGNAL(clicked()),this,SLOT(findTarget()));
  connect(m_hideOptionsButton,SIGNAL(clicked()),this,SLOT(hideOptions()));
  connect(m_keyboardButton, SIGNAL(clicked()),this,SLOT(showKeyboard()));

  QHBoxLayout * targetlayout = new QHBoxLayout;
  targetlayout->addWidget(m_findTarget);
  targetlayout->addWidget(m_findButton);
  targetlayout->addWidget(m_keyboardButton);
  targetlayout->addWidget(m_hideOptionsButton);

  QStringList headers;
  headers << tr("Root") << tr("Entry") <<  tr("Node") << tr("Position") << tr("Context");

  m_search = new SearchOptions(Lane::Word,this);
  QWidget * container = new QWidget;
  m_container = new QVBoxLayout;
  m_rxlist = new FocusTable;
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
  m_progress->hide();
  m_resultsText = new QLabel("");
  m_resultsText->hide();
  m_container->addLayout(targetlayout);
  m_container->addWidget(m_search);
  m_spacer = new QSpacerItem(0, 20,QSizePolicy::Ignored, QSizePolicy::MinimumExpanding);
  m_container->addWidget(m_progress);
  m_container->addSpacerItem(m_spacer);

  m_container->addWidget(m_rxlist);
  m_container->addWidget(m_resultsText);

  container->setLayout(m_container);

  m_text = new GraphicsEntry;
  m_text->hide();

  QSplitter * splitter = new QSplitter(Qt::Vertical);
  splitter->addWidget(container);
  splitter->addWidget(m_text);
  splitter->setStretchFactor(0,0);
  splitter->setStretchFactor(1,1);
  layout->addWidget(splitter);

  setLayout(layout);
  connect(m_rxlist,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
          this,SLOT(itemDoubleClicked(QTableWidgetItem * )));

  initXslt();
  m_search->setOptions(m_defaultOptions);
  m_rxlist->hide();
  this->setFocus();
}
FullSearchWidget::~FullSearchWidget() {
  qDebug() << Q_FUNC_INFO;
}

void FullSearchWidget::itemChanged(QTableWidgetItem * item,QTableWidgetItem * /* prev */) {
  qDebug() << Q_FUNC_INFO << "we should not be here";
/*  bool isHead = false;
  /// get the node
  item = item->tableWidget()->item(item->row(),NODE_COLUMN);
  if (item->data(Qt::UserRole).toBool()) {
    isHead = true;
  }
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
*/
}

void FullSearchWidget::itemDoubleClicked(QTableWidgetItem * item) {
  bool ok;
  bool isHead = false;
  /// get the node
  item = item->tableWidget()->item(item->row(),NODE_COLUMN);
  if (item->data(Qt::UserRole).toBool()) {
    isHead = true;
  }
  QString node = item->text();
  m_nodeQuery.bindValue(0,node);
  m_nodeQuery.exec();
  /// missing node
  if ( ! m_nodeQuery.first()) {
    QLOG_WARN() << "No record for node" << node;
    return;
  }
  item = item->tableWidget()->item(item->row(),POSITION_COLUMN);
  int pos =  item->text().toInt(&ok);
  if ( !ok )
    pos = 0;
  /// TODO make this a QSettings option or dialog option
  QString xml = m_nodeQuery.value("xml").toString();
  QString html = this->transform(xml);
  NodeView * v = new NodeView(this);
  v->setWindowTitle(QString(tr("Showing result %1 in search for %2")
                            .arg(item->row() + 1)
                            .arg(m_target)));
  v->setPattern(m_currentRx);
  v->setCSS(m_currentCSS);
  v->setHeader(m_nodeQuery.value("root").toString(),m_nodeQuery.value("word").toString(),node);
  /// has to be call before setHtml otherwise it will select the first occurence
  if (! isHead ) {
    v->setStartPosition(pos);
  }
  v->setHtml(html);
  v->show();
  v->raise();
  v->activateWindow();
  connect(v,SIGNAL(openNode(const QString &)),this,SIGNAL(showNode(const QString &)));
}
bool FullSearchWidget::eventFilter(QObject * target,QEvent * event) {
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
    switch(keyEvent->key()) {
    case Qt::Key_Tab: {
      m_findTarget->setFocus();
      break;
    }
    case Qt::Key_Escape: {
      QWidget * w = qobject_cast<QWidget *>(target);
      while(w) {
        QTabWidget * tabw = qobject_cast<QTabWidget *>(w);
        if (tabw)  {
          tabw->tabBar()->setFocus();
          return true;
        }
        w = w->parentWidget();
      }
      break;
    }
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
        //        if (keyEvent->modifiers() && Qt::ControlModifier) {
        QTableWidgetItem * item = m_rxlist->currentItem();
        if (item)
          m_rxlist->itemDoubleClicked(item);

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
void FullSearchWidget::setSearch(const QString & searchFor,int options) {
  m_target = searchFor;
  m_search->setOptions(options);
  m_findTarget->setText(m_target);
}
void FullSearchWidget::setOptionsHidden(bool hide) {
  m_hideOptionsButton->setChecked(hide);
  hideOptions();
}

void FullSearchWidget::hideOptions() {
  if (m_hideOptionsButton->isChecked()) {
    m_search->hide();
    m_hideOptionsButton->setText("Show options");
  }
  else {
    m_search->show();
    m_search->showMore(true);
    m_hideOptionsButton->setText("Hide options");
  }
}
/**
 *  for regex search do this:
 *
 TODO Check for regex special characters in buckwalter transliteration

QString escaped = pattern;
    escaped.replace("\\", "\\\\");
    escaped.replace("\"", "\\\"");
    escaped.prepend("\"");
    escaped.append("\"");

 */
void FullSearchWidget::findTarget(bool showProgress) {
  qDebug() << Q_FUNC_INFO << showProgress;
  m_showProgressDialog = showProgress;
  int options = m_search->getOptions();
  /// this shows text in progressbar
#ifdef __APPLE__
  m_progress->setStyle(QStyleFactory::create("Fusion"));
#endif
  m_progress->setValue(0);
  if (! showProgress) {
    m_progress->show();
  }
  else {
    m_hideOptionsButton->setChecked(true);
    this->hideOptions();
  }
  QString t = m_findTarget->text();
  QRegExp rx("[a-z]+");
  if (rx.indexIn(t,0) != -1) {
    this->textSearch(t,options);
  }
  else {
    this->regexSearch(m_findTarget->text(),options);
  }

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
void FullSearchWidget::regexSearch(const QString & target,int options) {
  bool replaceSearch = true;
  qDebug() << Q_FUNC_INFO;
  m_target = target;
  m_searchOptions = options;


  QRegExp rx;

  QRegExp rxclass("[\\x064b\\x064c\\x064d\\x064e\\x064f\\x0650\\x0651\\x0652\\x0670\\x0671]*");

  QString sql = "select id,word,root,entry,node,nodenum from xref where datasource = 1 order by nodenum asc";

  QString pattern;
  if (options & Lane::Normal_Search) {
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
    m_currentRx.setPattern(pattern);

    pattern.clear();
    if (options & Lane::Whole_Word) {
      pattern = "\\b" + m_target + "\\b";
    }
    else {
      pattern = m_target;
    }
    rx.setPattern(pattern);
  }
  else {
    qDebug() << "regex pattern" << rx.pattern();
    rx.setPattern(target);
  }

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
  m_rxlist->showColumn(4);


  m_rxlist->setRowCount(0);
  //  m_rxlist->hide();
#define NODE_COLUMN 2

  //  m_nodquery.bindValue(0,m_target);


  int headCount = 0;
  int textCount = 0;
  int readCount = 0;
  int entryCount = 0;
  QString node;
  QString root;
  QString headword;

  /// Added QEventLoop because under OSX nothing was shown
  /// the loop was finished
  QEventLoop ep;
  QProgressDialog * pd = 0;
  int max = this->getMaxRecords("xref");
  if (m_showProgressDialog) {
    pd = new QProgressDialog("Searching...", "Cancel", 0,max, getApp());
    connect(pd,SIGNAL(canceled()),this,SLOT(cancelSearch()));
    pd->setWindowModality(Qt::WindowModal);
    pd->show();
  }
  m_progress->setMaximum(max);
  m_cancelSearch = false;
  m_query.exec();
  //  m_rxlist->setUpdatesEnabled(false);
  qint64 st = QDateTime::currentMSecsSinceEpoch();
  while(m_query.next() && ! m_cancelSearch) {
    ok = false;
    readCount++;
    if ((readCount % 500) == 0) {
      m_progress->setValue(readCount);
      if (pd) {
        pd->setValue(readCount);
      }
      //      emit(setProgressValue(readCount));
      ep.processEvents();
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
          if (options & Lane::Include_Heads) {
            int row = addRow(root,m_nodeQuery.value("word").toString(),node,m_headText,0);
            QTableWidgetItem * item = m_rxlist->item(row,NODE_COLUMN);
            item->setData(Qt::UserRole,true);
            headCount++;
            if (m_headBackgroundColor.isValid()) {
              QBrush b(m_headBackgroundColor);
              for(int i=0;i < m_rxlist->columnCount();i++) {
                m_rxlist->item(row,i)->setBackground(b);
              }
            }
            else {
              qDebug() << "invalid color";
            }
          }
        }
        headword = m_nodeQuery.value("word").toString();

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
      else {
        qDebug() << "Error in node Query sql";
      }
    }
  }
  qint64 et = QDateTime::currentMSecsSinceEpoch();
  //  m_rxlist->setUpdatesEnabled(true);
  if (pd) {
    delete pd;
  }

  if (m_rxlist->rowCount() > 0) {
    m_container->removeItem(m_spacer);
    m_rxlist->show();
    m_rxlist->selectRow(0);
    m_rxlist->setFocus();
  }


  m_resultsText->setText(buildText(entryCount,headCount,textCount,et - st,options));
  m_resultsText->show();

  m_rxlist->resizeColumnToContents(0);
  m_rxlist->resizeColumnToContents(2);
  m_rxlist->resizeColumnToContents(3);
  m_rxlist->resizeColumnToContents(4);
  this->show();
  /*
  else {
    QMessageBox msgBox;
    msgBox.setObjectName("wordnotfound");
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(QString(tr("Word not found")));
    msgBox.exec();
  }
  */
}
/**
 *
 *
 * @param entryCount the number of unique entries i.e. nodes
 * @param bodyCount  the number of occurrences within XML
 * @param options
 *
 * @return
 */
QString FullSearchWidget::buildText(int entryCount,int headCount,int bodyCount,int ms,int options) {
  QString t;
  QString p1;
  QString p2;
  int findCount = bodyCount + headCount;

  switch(findCount) {
  case 0 :
    p1 = "no items found";
    break;
  case 1:
    p1 = "";
    break;
  default:
    p1 ="s";
  }
  if (entryCount == 1) {
    p2 = "y";
  }
  else {
    p2 = "ies";
  }
  /// TODO allow for Arabic font
  if (findCount == 0) {
    t = QString(tr("Search for \"%1\", %2")).arg(m_target).arg(p1);
  }
  else {
    t = QString(tr("Search for \"%1\", found %2 item%3 in %4 entr%5"))
      .arg(m_target)
      .arg(findCount)
      .arg(p1)
      .arg(entryCount)
      .arg(p2);
  }
  if (options & Lane::Ignore_Diacritics)
    t += tr(", ignoring diacritics");
  if (options & Lane::Whole_Word)
    t += tr(", whole word match");

  if (ms != -1) {
    qreal x = (ms/1000) + 0.5;
    int y = static_cast<int>(x);
    t += QString(" (%1 sec").arg(y);
    if (y != 1) {
      t += "s";
    }
    t += ")";
  }
  return t;
}
int FullSearchWidget::addRow(const QString & root,const QString & headword, const QString & node, const QString & text,int pos) {
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

  item = new QTableWidgetItem(QString("%1").arg(pos));
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  m_rxlist->setItem(row,3,item);

  if (text.size() > 0) {

    /// For QLineEdit
    /// force left justification, since the text could begin with arabic or english. From:
    /// https://stackoverflow.com/questions/10998105/qt-how-to-change-the-direction-of-placeholder-in-a-qlineedit
    ///
    ///
    if (0) {
      QLineEdit * e = new QLineEdit(text);
      e->setReadOnly(true);
      QKeyEvent ke(QEvent::KeyPress, Qt::Key_Direction_L, 0);
      QApplication::sendEvent(e, &ke);
      m_rxlist->setCellWidget(row,CONTEXT_COLUMN,e);
    }
    if (0) {
      QTextEdit * d = new QTextEdit(this);
      d->setHtml("<html><body>" + text + "</body></html>");
      d->setReadOnly(true);
      d->setMaximumHeight(30);
      m_rxlist->setCellWidget(row,CONTEXT_COLUMN,d);
    }
    if (1) {
      item = new QTableWidgetItem(text);
      item->setFlags(item->flags() ^ Qt::ItemIsEditable);
      m_rxlist->setItem(row,CONTEXT_COLUMN,item);
    }
  }
  else
    m_rxlist->setItem(row,4,new QTableWidgetItem(text));

  return row;
}
/**
 * build SQL for search using DB style search
 *
 * @param options
 *
 * @return
 */
QString FullSearchWidget::buildSearchSql(int options) {
  QString sql = "select id,word,root,entry,node from xref where datasource = 1  order by root,entry asc";
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
  return sql;
}
/**
 * build SQL for search using regex
 *
 * @param options
 *
 * @return
 */
QString FullSearchWidget::buildRxSql(int options) {
  QString sql;
  //  whole word with diacritics
  //   select id,node where word = ?
  //  part word with diacritics
  //   select id,node from xref where instr(word,?) > 0;
  //  whole word without diacritics
  //   select where bareword = ?
  //  part word without diacritics
  //   select where instr(bareword,?) > 0
  sql = "select id,word,root,entry,node from xref where datasource = 1 ";
  return sql;
}
QTextDocument * FullSearchWidget::fetchDocument(const QString & xml) {
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
QString FullSearchWidget::transform(const QString & xml) {
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
void FullSearchWidget::readSettings() {

  QString v;
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  bool ok;
  settings->setIniCodec("UTF-8");
  m_defaultOptions = 0;

  settings->beginGroup("Entry");
  QString css = settings->value("CSS",QString("entry.css")).toString();
  readCssFromFile(css);
  //m_xsltSource = settings->value("XSLT",QString("entry.xslt")).toString();
  settings->endGroup();
  settings->beginGroup("FullSearch");
  QString f = settings->value("Results font",QString()).toString();
  if (! f.isEmpty()) {
    m_resultsFont.fromString(f);
  }
  m_xsltSource = settings->value("XSLT",QString("node.xslt")).toString();
  m_debug = settings->value("Debug",false).toBool();
  m_fragmentSize = settings->value("Fragment size",40).toInt();
  if (settings->value("Include heads",false).toBool()) {
    m_defaultOptions |= Lane::Include_Heads;
  }
  v = settings->value("Head word background").toString();
  m_headBackgroundColor.setNamedColor(v);
  m_headText = settings->value("Head text").toString();
  settings->endGroup();

  settings->beginGroup("Search");
  v  = settings->value("Type",QString("normal")).toString();
  if (v == "normal")
    m_defaultOptions |= Lane::Normal_Search;
  else
    m_defaultOptions |= Lane::Regex_Search;

  v = settings->value("Ignore diacritics",QString("yes")).toString();
  if (v == "yes")
    m_defaultOptions |= Lane::Ignore_Diacritics;

  v = settings->value("Whole word",QString("no")).toString();
  if (v == "yes")
    m_defaultOptions |= Lane::Whole_Word;

  v = settings->value("Target",QString("Arabic")).toString();

  if (v == "Arabic")
    m_defaultOptions |= Lane::Arabic;
  else
    m_defaultOptions |= Lane::Buckwalter;



  delete settings;
}
void FullSearchWidget::getTextFragments(QTextDocument * doc,const QString & target,int options,const QRegExp & regex) {
  QRegExp rx;
  QString pattern;
  QRegExp rxclass("[\\x064b\\x064c\\x064d\\x064e\\x064f\\x0650\\x0651\\x0652\\x0670\\x0671]*");

  if (options & Lane::Regex_Search) {
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
  if (! regex.isEmpty()) {
    rx.setPattern(regex.pattern());
  }
  else {
  rx.setPattern(pattern);
  }

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
int FullSearchWidget::getMaxRecords(const QString & table) {
  bool ok = false;
  QString sql = QString("select id from %1 order by id desc limit 1").arg(table);
  QSqlQuery maxq(sql);
  int max;
  if (maxq.exec() && maxq.next())
    max = maxq.value(0).toInt(&ok);

  if (! ok ) {
    if (table == "xref") {
      max = 544695;
    }
    else {
      max = 49000;
    }
  }
  return max;
}

/**
 * lines beginning with - are omitted
 *
 */
bool FullSearchWidget::readCssFromFile(const QString & name) {
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
void FullSearchWidget::focusInEvent(QFocusEvent * event) {
  qDebug() << Q_FUNC_INFO << event;
  if (event->reason() == Qt::OtherFocusReason) {
    if (m_rxlist->rowCount() > 0) {
      m_rxlist->setFocus();

    }
    else {
      m_findTarget->setFocus();
    }
  }
  QWidget::focusInEvent(event);
}
void FullSearchWidget::focusOutEvent(QFocusEvent * event) {
  qDebug() << Q_FUNC_INFO << event;
  /*
  if (event->reason() == Qt::OtherFocusReason) {
    if (m_rxlist->rowCount() > 0)
      m_rxlist->setStyleSheet("QTableView { selection-background-color : green}"); //%1}").arg("lightgray"));
    m_rxlist->repaint();
  }
  */
  QWidget::focusOutEvent(event);
}
void FullSearchWidget::focusTable() {
  m_rxlist->setFocus();
}
void FullSearchWidget::cancelSearch() {
  m_cancelSearch = true;
}
void FullSearchWidget::showKeyboard() {
  m_keyboard->attach(m_findTarget);
  m_attached = ! m_attached;
  if (m_attached) {
    m_keyboardButton->setText(tr("Hide keyboard"));
    QPoint p;
    QPoint gp;
    p = m_keyboard->currentPosition();
    if (p.isNull()) {
      p = m_findTarget->pos();
      gp =  m_findTarget->mapToGlobal(p);
      QRect r = m_findTarget->frameGeometry();
      int x = gp.x();
      int y = gp.y() + r.height();
      m_keyboard->move(getApp()->mapFromGlobal(QPoint(x,y)));
    }
    else {
      gp =  m_findTarget->mapToGlobal(p);
      m_keyboard->move(p);
    }
  }
  else
    m_keyboardButton->setText(tr("Show keyboard"));

}
void FullSearchWidget::textSearch(const QString & target,int options) {
  bool replaceSearch = true;
  qDebug() << Q_FUNC_INFO;
  m_target = target;
  m_searchOptions = options;


  QRegExp rx;

  QRegExp rxclass("[\\x064b\\x064c\\x064d\\x064e\\x064f\\x0650\\x0651\\x0652\\x0670\\x0671]*");
  QString pattern;
  if (options & Lane::Normal_Search) {
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
    m_currentRx.setPattern(pattern);

    pattern.clear();
    if (options & Lane::Whole_Word) {
      pattern = "\\b" + m_target + "\\b";
    }
    else {
      pattern = m_target;
    }
    rx.setPattern(pattern);
  }
  else {
    rx.setPattern(target);
    qDebug() << Q_FUNC_INFO << "regex pattern" << rx.pattern();
  }
  bool ok = false;
  if (m_query.prepare("select root,word,nodeid,xml from entry where datasource = 1 order by nodenum asc")) {
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
  m_rxlist->showColumn(4);


  m_rxlist->setRowCount(0);
  //  m_rxlist->hide();
#define NODE_COLUMN 2

  //  m_nodquery.bindValue(0,m_target);


  int headCount = 0;
  int textCount = 0;
  int readCount = 0;
  int entryCount = 0;
  QString node;
  QString root;
  QString headword;

  /// Added QEventLoop because under OSX nothing was shown
  /// the loop was finished
  QEventLoop ep;
  QProgressDialog * pd = 0;
  int max = this->getMaxRecords("entry");
  m_progress->setMaximum(max);
  if (m_showProgressDialog) {
    pd = new QProgressDialog("Searching...", "Cancel", 0,max, getApp());
    connect(pd,SIGNAL(canceled()),this,SLOT(cancelSearch()));
    pd->setWindowModality(Qt::WindowModal);
    pd->show();
  }
  m_cancelSearch = false;
  m_query.exec();
  qint64 st = QDateTime::currentMSecsSinceEpoch();
  m_rxlist->setUpdatesEnabled(false);
  while(m_query.next() && ! m_cancelSearch) {
    ok = false;
    readCount++;
    if ((readCount % 500) == 0) {
      m_progress->setValue(readCount);
    }
    if (pd) {
        pd->setValue(readCount);
    }
    ep.processEvents();
    QString headword = m_query.value("word").toString();
    if (headword.indexOf(rx) != -1) {
      if (options & Lane::Include_Heads) {
        addRow(m_query.value("root").toString(),
               m_query.value("word").toString(),
               m_query.value("nodeid").toString(),
               "head word",0);
        headCount++;
        ok = true;
      }
    }
    QString xml = m_query.value("xml").toString();
    QTextDocument * doc  = fetchDocument(xml);
    if (doc->characterCount() > 0) {
      getTextFragments(doc,target,options,rx);
      if (m_fragments.size() == 0) {
      }
      for(int i=0;i < m_fragments.size();i++) {
        addRow(m_query.value("root").toString(),
               m_query.value("word").toString(),
               m_query.value("nodeid").toString(),
               m_fragments[i],m_positions[i]);
      }
      if (m_fragments.size() > 0) {
        textCount += m_fragments.size();
        ok = true;
      }
    }
    if (ok) {
      entryCount++;
    }
  }
  m_rxlist->setUpdatesEnabled(true);
  qint64 et = QDateTime::currentMSecsSinceEpoch();
  qDebug() << "Search time ms:" << et - st;
  if (pd) {
    delete pd;
  }

  if (m_rxlist->rowCount() > 0) {
    m_container->removeItem(m_spacer);
    m_rxlist->show();
    m_rxlist->selectRow(0);
    m_rxlist->setFocus();
  }
  //  m_resultsText->setText(QString("Search for %1, returned %2 results").arg(target).arg(m_rxlist->rowCount()));
  m_resultsText->setText(buildText(entryCount,headCount,textCount,et - st,options));
  m_resultsText->show();

  m_rxlist->resizeColumnToContents(0);
  m_rxlist->resizeColumnToContents(2);
  m_rxlist->resizeColumnToContents(3);
  m_rxlist->resizeColumnToContents(4);
  this->show();
  /*
  else {
    QMessageBox msgBox;
    msgBox.setObjectName("wordnotfound");
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(QString(tr("Word not found")));
    msgBox.exec();
  }
  */
}
