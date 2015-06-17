#include "fullsearch.h"
#include "searchoptionswidget.h"
#include "graphicsentry.h"
#include "application.h"
#include "namespace.h"
#include "laneslexicon.h"
#include "imlineedit.h"
#include "nodeview.h"
#include "focustable.h"
#include "keyboard.h"
#include "keyboardwidget.h"
#include "xsltsupport.h"
#include "htmldelegate.h"
#include "definedsettings.h"
#include "definedsql.h"
#include "externs.h"
#define SELECT_COLUMN 0
#define ROOT_COLUMN 1
#define HEAD_COLUMN 2
#define NODE_COLUMN 3
#define VOL_COLUMN 4
#define POSITION_COLUMN 5
#define CONTEXT_COLUMN 6
#define COLUMN_COUNT 7


/// TODO
/// some function pass SearchOptions - can we use the class member
/**
 *
 *
 * @param parent
 */
FullSearchWidget::FullSearchWidget(QWidget * parent) : QWidget(parent) {
  readSettings();
  m_attached = false;
  m_findTarget = new ImLineEdit;
  getLexicon()->adjustHeight(m_findTarget);
  QString mapname = getApp()->getActiveKeymap();
  QString mapfile = getApp()->getKeymapFileName(mapname);
  if (! mapfile.isEmpty()) {
    m_findTarget->loadMap(mapfile,mapname);
    m_findTarget->setCurrentMap(mapname);
  }

  //  m_findTarget->readSettings(getLexicon()->settingsFileName());

  m_findButton = new QPushButton(tr("F&ind"));
  m_findButton->setDefault(true);
  m_hideOptionsButton = new QPushButton(tr("Sho&w options"));
  //  m_hideOptionsButton->setCheckable(true);

  m_keyboardButton  = new QPushButton(tr("Show &keyboard"));
  m_keyboardButton->setAutoDefault(false);
  m_keyboardButton->setCheckable(true);

  m_keyboard = new KeyboardWidget(getLexicon()->getResourceFilePath(Lexicon::Keyboard),m_keyboardConfig,this);
  connect(m_findButton,SIGNAL(clicked()),this,SLOT(findTarget()));
  connect(m_hideOptionsButton,SIGNAL(clicked()),this,SLOT(hideOptions()));
  connect(m_keyboardButton, SIGNAL(clicked()),this,SLOT(showKeyboard()));

  QHBoxLayout * targetlayout = new QHBoxLayout;
  targetlayout->addWidget(m_findTarget);
  targetlayout->addWidget(m_findButton);
  targetlayout->addWidget(m_keyboardButton);
  targetlayout->addWidget(m_hideOptionsButton);

  QStringList headers = this->columnHeadings();
  m_defaultOptions.setSearchScope(SearchOptions::Word);
  m_search = new SearchOptionsWidget(m_defaultOptions,this);

  connect(m_search,SIGNAL(force(bool)),m_findTarget,SLOT(setForceLTR(bool)));

  //  QWidget * container = new QWidget;
  m_container = new QVBoxLayout;
  m_rxlist = new FocusTable;
  m_rxlist->setColumnCount(COLUMN_COUNT);
  m_rxlist->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_rxlist->setSelectionMode(QAbstractItemView::SingleSelection);

  m_rxlist->setHorizontalHeaderLabels(headers);
  m_rxlist->horizontalHeader()->setStretchLastSection(true);
  m_rxlist->setSelectionMode(QAbstractItemView::SingleSelection);
  m_rxlist->installEventFilter(this);
  HtmlDelegate * d = new HtmlDelegate(m_rxlist);
  if ( ! m_spanStyle.isEmpty()) {
    d->setStyleSheet(m_spanStyle);
  }
  //  d->setStyleSheet(".ar { font-family : Amiri;font-size : 16px}");
  m_rxlist->setItemDelegateForColumn(CONTEXT_COLUMN,d);

  //QStyle * style = m_list->style();
  //  QLOG_DEBUG() << "style hint" << style->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus);
  m_progress = new QProgressBar;
  m_progress->hide();
  QHBoxLayout * resultslayout = new QHBoxLayout;
  m_resultsText = new QLabel("");
  m_resultsText->hide();
  m_exportButton = new QPushButton(tr("Export results"));
  resultslayout->addWidget(m_resultsText);
  resultslayout->addStretch();
  resultslayout->addWidget(m_exportButton);
  m_container->addLayout(targetlayout);
  m_container->addWidget(m_search);
  m_spacer = new QSpacerItem(0, 20,QSizePolicy::Ignored, QSizePolicy::MinimumExpanding);
  m_container->addWidget(m_progress);
  m_container->addSpacerItem(m_spacer);

  m_container->addWidget(m_rxlist);
  m_container->addLayout(resultslayout);

  setLayout(m_container);
  connect(m_rxlist,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
          this,SLOT(itemDoubleClicked(QTableWidgetItem * )));
  initXslt();
  //  m_search->setOptions(m_defaultOptions);
  m_search->setVisible(false);
  m_rxlist->hide();
  this->setFocus();
  connect(m_exportButton,SIGNAL(clicked()),this,SLOT(onExport()));
}
FullSearchWidget::~FullSearchWidget() {
  QLOG_DEBUG() << Q_FUNC_INFO;
}

QStringList FullSearchWidget::columnHeadings() {
  QStringList h;
  for(int i=0;i < COLUMN_COUNT;i++) {
    h << "";
  }
  h[SELECT_COLUMN] = "";
  h[ROOT_COLUMN] = tr("Root");

  h[HEAD_COLUMN] = tr("Entry");
  h[NODE_COLUMN] =  tr("Node");
  if (m_singleRow) {
    h[POSITION_COLUMN] = tr("Occurs");
  }
  else {
    h[POSITION_COLUMN] = tr("Position");
  }
  h[CONTEXT_COLUMN] = tr("Context");
  h[VOL_COLUMN] = tr("Vol/Page");
  return h;
}
void FullSearchWidget::itemChanged(QTableWidgetItem * /* item */,QTableWidgetItem * /* prev */) {
  QLOG_DEBUG() << Q_FUNC_INFO << __LINE__ << "NOSHOW we should not be here";

}

void FullSearchWidget::itemDoubleClicked(QTableWidgetItem * item) {
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
    QLOG_WARN() << Q_FUNC_INFO << "No record for node" << node;
    return;
  }
  item = item->tableWidget()->item(item->row(),POSITION_COLUMN);
  if (!item) {
    return;
  }
  int pos = item->data(Qt::UserRole).toInt();
  if (m_singleRow) {
    pos = 0;
  }
  /// TODO make this a QSettings option or dialog option
  QString xml = m_nodeQuery.value("xml").toString();
  QString html = this->transform(xml);
  NodeView * v = new NodeView(this);
  v->setWindowTitle(QString(tr("Showing result %1 in search for %2")
                            .arg(item->row() + 1)
                            .arg(m_target)));
  v->setPattern(m_currentRx);
  v->setCSS(m_currentCSS);
  /**
   * get the page, check it is for a valid volume and pass it if it is, otherwise passs 0
   *
   */

  int page = m_nodeQuery.value("page").toInt();
  if (Place::volume(page) == 0) {
    page = 0;
  }
  v->setHeader(m_nodeQuery.value("root").toString(),m_nodeQuery.value("word").toString(),node,page);
  /// has to be call before setHtml otherwise it will select the first occurence
  if (! isHead ) {
    v->setStartPosition(pos);
  }
  v->setHtml(html);
  v->findFirst();
  v->show();
  v->raise();
  v->activateWindow();
  connect(v,SIGNAL(openNode(const QString &)),this,SIGNAL(showNode(const QString &)));
  connect(v,SIGNAL(printNode(const QString &)),this,SIGNAL(printNode(const QString &)));
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
    case Qt::Key_Enter:
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
void FullSearchWidget::setSearch(const QString & searchFor,const SearchOptions & options) {
  m_target = searchFor;
  m_search->setOptions(options);
  m_findTarget->setText(m_target);
}
void FullSearchWidget::hideOptions() {
  QLOG_DEBUG() << Q_FUNC_INFO << m_search->isVisible();
  m_search->hideTabOptions();
  if ( m_search->isVisible()) {
    m_search->hide();
    m_search->setVisible(false);
    m_hideOptionsButton->setText("Show options");
  }
  else {
    m_search->setVisible(true);
    m_search->showMore(true);
    m_search->show();
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
  QLOG_DEBUG() << Q_FUNC_INFO << showProgress;
  m_showProgressDialog = showProgress;
  SearchOptions options;
  m_search->getOptions(options);
  /// this shows text in progressbar
#ifdef __APPLE__
  m_progress->setStyle(QStyleFactory::create("Fusion"));
#endif
  m_progress->setValue(0);
  if (! showProgress) {
    m_progress->show();
  }
  else {
    //    m_hideOptionsButton->setChecked(true);
    //    this->hideOptions();
  }
  /// TODO check if only arabic characters
  ///      and use appropriate
  QString t = m_findTarget->text();
  t.remove(QChar(0x202d));

  QRegExp rx("[a-z]+");
  if ((options.getSearchType() == SearchOptions::Regex) || (rx.indexIn(t,0) != -1)) {
    this->regexSearch(t,options);
  }
  else {
    this->textSearch(m_findTarget->text(),options);
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
void FullSearchWidget::textSearch(const QString & target,const SearchOptions & options) {
  bool replaceSearch = true;
  QLOG_DEBUG() << Q_FUNC_INFO;
  m_target = target;
  m_searchOptions = options;
  QRegExp rx;
  QRegExp rxclass(m_diacritics);

  QString pattern;
  m_currentRx = rx = SearchOptionsWidget::buildRx(target,m_diacritics,options);
  QString sql(SQL_FIND_XREF_ENTRIES);
  if (m_query.prepare(sql)) {
    sql = SQL_FIND_ENTRY_DETAILS;
    if (! m_nodeQuery.prepare(sql)) {
      QLOG_WARN() << QString("SQL prepare error %1 : %2")
        .arg(sql)
        .arg(m_nodeQuery.lastError().text());
      return;
    }
  }
  else {
    QLOG_WARN() << QString("SQL prepare error %1 : %2")
      .arg(sql)
      .arg(m_query.lastError().text());
    return;
  }
  if (m_debug) {
    m_rxlist->showColumn(NODE_COLUMN);
  }
  else {
    m_rxlist->hideColumn(NODE_COLUMN);
  }
  m_rxlist->setRowCount(0);
  int headCount = 0;
  int textCount = 0;
  int readCount = 0;
  int entryCount = 0;
  QString node;
  QString root;
  QString headword;
  int page;
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
    page = m_query.value("page").toInt();
    /// strip diacritics if required
    if (replaceSearch && (options.getSearchType() == SearchOptions::Normal)) {
      if (options.ignoreDiacritics())
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
          if (options.ignoreDiacritics())
            headword =  headword.replace(rxclass,QString());
        }
        if (headword.indexOf(rx) != -1) {
          if (options.includeHeads()) {
            int row = addRow(root,m_nodeQuery.value("word").toString(),node,m_headText,-1,page);
            QTableWidgetItem * item = m_rxlist->item(row,NODE_COLUMN);
            item->setData(Qt::UserRole,true);
            headCount++;
            if (m_headBackgroundColor.isValid()) {
              QBrush b(m_headBackgroundColor);
              for(int i=0;i < m_rxlist->columnCount();i++) {
                m_rxlist->item(row,i)->setBackground(b);
              }
            }
          }
        }
        headword = m_nodeQuery.value("word").toString();

        QString xml = m_nodeQuery.value("xml").toString();
        QTextDocument * doc  = fetchDocument(xml);
          if (doc->characterCount() > 0) {
            getTextFragments(doc,target,options);
            if (m_fragments.size() > 0) {
              if (m_singleRow) {
                QString html = qobject_cast<Lexicon *>(qApp)->scanAndSpan(m_fragments[0]);
                addRow(root,headword,node,html,m_fragments.size(),page);
              }
              else {
                for(int i=0;i < m_fragments.size();i++) {
                  if (m_fragments[i].size() > 0) {
                    QString html = qobject_cast<Lexicon *>(qApp)->scanAndSpan(m_fragments[i]);
                    addRow(root,headword,node,html,i,page);
                  }
                }
              }
              textCount += m_fragments.size();
            }
          }
      }
      else {
        QLOG_DEBUG() << "Error in node Query sql";
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
    m_exportButton->setEnabled(true);
  }
  else {
    m_exportButton->setEnabled(false);
  }


  m_resultsText->setText(buildText(entryCount,headCount,textCount,et - st));
  m_resultsText->show();

  m_rxlist->resizeColumnToContents(SELECT_COLUMN);
  m_rxlist->resizeColumnToContents(ROOT_COLUMN);
  m_rxlist->resizeColumnToContents(NODE_COLUMN);
  m_rxlist->resizeColumnToContents(POSITION_COLUMN);
  m_rxlist->resizeColumnToContents(CONTEXT_COLUMN);
  m_rxlist->resizeColumnToContents(VOL_COLUMN);
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
QString FullSearchWidget::buildText(int entryCount,int headCount,int bodyCount,int ms) {
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
    p1 ="es";
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
    t = QString(tr("Search for \"%1\", found %2 match%3 in %4 entr%5"))
      .arg(m_target)
      .arg(findCount)
      .arg(p1)
      .arg(entryCount)
      .arg(p2);
  }
  if (m_searchOptions.getSearchType() == SearchOptions::Regex) {
    t += tr(", regular expression search");
  }
  else {
    if (m_searchOptions.ignoreDiacritics())
      t += tr(", ignoring diacritics");
    if (m_searchOptions.wholeWordMatch())
      t += tr(", whole word match");
  }
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
int FullSearchWidget::addRow(const QString & root,const QString & headword, const QString & node, const QString & text,int pos,int page) {
  QTableWidgetItem * item;

  int row = m_rxlist->rowCount();
  m_rxlist->insertRow(row);
  /*
  QWidget * w = new QWidget;
  QHBoxLayout * boxlayout = new QHBoxLayout;
  boxlayout->addWidget(new QCheckBox);
  boxlayout->setAlignment(Qt::AlignCenter);
  boxlayout->setContentsMargins(0,0,0,0);
  w->setLayout(boxlayout);

  m_rxlist->setCellWidget(row,SELECT_COLUMN,w);
  */
  m_rxlist->setCellWidget(row,SELECT_COLUMN,new CheckBoxTableItem);

  //  m_rxlist->setCellWidget(row,SELECT_COLUMN,new QCheckBox);
  item = new QTableWidgetItem(root);
  item->setFont(m_resultsFont);
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  m_rxlist->setItem(row,ROOT_COLUMN,item);

  item = new QTableWidgetItem(headword);
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  item->setFont(m_resultsFont);
  m_rxlist->setItem(row,HEAD_COLUMN,item);

  item = new QTableWidgetItem(node);
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  m_rxlist->setItem(row,NODE_COLUMN,item);

  int c = pos;
  ///
  /// for pos = -1
  ///
  if (pos == -1) {
    m_rxlist->setItem(row,POSITION_COLUMN,new QTableWidgetItem(""));
  }
  else {
    if (!m_singleRow) {
      c = 0;
      if ((pos >= 0) && (pos < m_positions.size())) {
        c = m_positions[pos];
      }
    }
    item = new QTableWidgetItem(QString("%1").arg(c));
    item->setData(Qt::UserRole,pos);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_rxlist->setItem(row,POSITION_COLUMN,item);
  }
  if (text.size() > 0) {
    item = new QTableWidgetItem(text);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_rxlist->setItem(row,CONTEXT_COLUMN,item);
  }
  else {
    m_rxlist->setItem(row,CONTEXT_COLUMN,new QTableWidgetItem(text));
  }
  int vol = Place::volume(page);
  if (vol > 0) {
    item = new QTableWidgetItem(QString("%1/%2").arg(vol).arg(page));
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_rxlist->setItem(row,VOL_COLUMN,item);
  }

  return row;
}
QTextDocument * FullSearchWidget::fetchDocument(const QString & xml) {
  QString html = transform(xml);
  m_nodeDoc.setHtml(html);
  QTextDocument * doc = new QTextDocument;
  doc->setHtml(html);
  return doc;
}
QString FullSearchWidget::transform(const QString & xml) {
  int ok = compileStylesheet(NODE_XSLT,m_xsltSource);
  if (ok == 0) {
    QString html = xsltTransform(NODE_XSLT,xml);
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
  SETTINGS
  settings.beginGroup("Entry");
  QString css = settings.value(SID_ENTRY_CSS,QString("entry.css")).toString();
  css = getLexicon()->getResourceFilePath(Lexicon::Stylesheet,css);
  readCssFromFile(css);
  //m_xsltSource = settings.value("XSLT",QString("entry.xslt")).toString();
  settings.endGroup();
  settings.beginGroup("FullSearch");
  m_singleRow = settings.value(SID_FULLSEARCH_ONE_ROW,true).toBool();
  QString f = settings.value(SID_FULLSEARCH_RESULTS_FONT,QString()).toString();
  if (! f.isEmpty()) {
    m_resultsFont.fromString(f);
  }
  m_debug = settings.value(SID_FULLSEARCH_DEBUG,false).toBool();
  m_fragmentSize = settings.value(SID_FULLSEARCH_FRAGMENT_SIZE,40).toInt();

  m_defaultOptions.setIncludeHeads(settings.value(SID_FULLSEARCH_INCLUDE_HEADS,false).toBool());
  m_stepCount = settings.value(SID_FULLSEARCH_STEP,500).toInt();
  v = settings.value(SID_FULLSEARCH_HEAD_BACKGROUND).toString();
  m_headBackgroundColor.setNamedColor(v);
  m_headText = settings.value(SID_FULLSEARCH_HEAD_TEXT).toString();
  if (settings.value(SID_FULLSEARCH_TYPE_REGEX,false).toBool()) {
    m_defaultOptions.setSearchType(SearchOptions::Regex);
  }
  else {
    m_defaultOptions.setSearchType(SearchOptions::Normal);
  }
  m_defaultOptions.setIgnoreDiacritics(settings.value(SID_FULLSEARCH_DIACRITICS,true).toBool());
  m_defaultOptions.setWholeWordMatch(settings.value(SID_FULLSEARCH_WHOLE_WORD,false).toBool());
  m_spanStyle = settings.value(SID_FULLSEARCH_CONTEXT_STYLE,QString()).toString();
  settings.endGroup();


  settings.beginGroup("XSLT");
  m_xsltSource = settings.value(SID_XSLT_NODE,QString("node.xslt")).toString();
  m_xsltSource = getLexicon()->getResourceFilePath(Lexicon::XSLT,m_xsltSource);
  settings.endGroup();

  settings.beginGroup("Diacritics");
  QStringList keys = settings.childKeys();
  QStringList points;
  for(int i=0;i < keys.size();i++) {
    if (keys[i].startsWith("Char")) {
      v = settings.value(keys[i],QString()).toString();
      points << v;
    }
  }
  m_diacritics = QString("[\\x%1]*").arg(points.join("\\x"));

  settings.endGroup();
  settings.beginGroup("Keyboards");
  m_keyboardConfig = settings.value("Config","keyboard.ini").toString();

}
void FullSearchWidget::getTextFragments(QTextDocument * doc,const QString & target,const SearchOptions & options,const QRegExp & regex) {
  QRegExp rx;
  QString pattern;
  QRegExp rxclass(m_diacritics);

  if (options.getSearchType() == SearchOptions::Regex) {
    pattern = target;
  }
  else {
    if (options.ignoreDiacritics()) {
      QString ar(m_diacritics);
      QStringList cc = target.split("");
      QString brx = "";
      for(int i=0;i < cc.size();i++) {
        pattern += cc[i] + ar;
      }
    }
    else {
      pattern = target;
    }
    if (options.wholeWordMatch()) {
      pattern = "\\b" + pattern + "\\b";
    }
  }
  //  QLOG_DEBUG() << "Pattern" << pattern;
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
    //    QLOG_DEBUG() << "cursor pos" << c.position() <<  "offset in string" << src.indexOf(rx,position);
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
    //    QLOG_DEBUG() << "fragment size" << (ex - sx);  //QString("[%1][%2][%3][%4]").arg(position).arg(sx).arg(ex).arg(src);
    c = doc->find(rx,position);
  }
  //  if (m_positions.size() > 0) {
  //    QLOG_DEBUG() << Q_FUNC_INFO << m_positions;
  //  }
}
int FullSearchWidget::getMaxRecords(const QString & table) {
  bool ok = false;
  QString sql = QString(SQL_FIND_MAXIMUM).arg(table);
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
  QString css = in.readAll();
  f.close();
  if (! css.isEmpty()) {
    m_currentCSS = css;
  }
  return true;
}
void FullSearchWidget::focusInEvent(QFocusEvent * event) {
  QLOG_DEBUG() << Q_FUNC_INFO << event;
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
  QLOG_DEBUG() << Q_FUNC_INFO << event;
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
  if (! m_attached) {
    m_keyboard->attach(m_findTarget);
    m_keyboardButton->setText(tr("Hide &keyboard"));
    QPoint p;
    p = m_keyboard->currentPosition();
    if (p.isNull()) {
      p = m_findTarget->pos();
      p = m_findTarget->mapToGlobal(p);
      p.setX(p.x() - 50);
      p.setY(p.y() + 50);
    }
    m_keyboard->autoScale();
    m_keyboard->move(p);
    m_attached = true;
  }
  else {
    m_keyboard->detach();
    m_keyboardButton->setText(tr("Show &keyboard"));
    m_attached = false;
  }
}
void FullSearchWidget::regexSearch(const QString & target,const SearchOptions & options) {
  QLOG_DEBUG() << Q_FUNC_INFO;
  m_target = target;
  m_searchOptions = options;


  QRegExp rx;

  QRegExp rxclass(m_diacritics);
  QString pattern;
  if (options.getSearchType() == SearchOptions::Normal) {
    if (options.ignoreDiacritics()) {
      m_target = m_target.replace(QRegExp(rxclass),QString());
      for(int i=0;i < target.size();i++) {
        QChar sp = m_target.at(i);
        pattern += QString(sp);
        if ( sp.isLetter() ) {
          /// if it's in the Arabic block, append to allow for optional diacritics
          if ((sp.unicode() >= 0x600) && (sp.unicode() <= 0x6ff)) {
            pattern += m_diacritics;
          }
        }
      }
    }
    else {
      pattern = target;
    }
    if (options.wholeWordMatch()) {
      pattern = "\\b" + m_target + "\\b";
    }
    rx.setPattern(pattern);
  }
  else {
    rx.setPattern(target);
  }
  QLOG_DEBUG() << Q_FUNC_INFO << "regex pattern" << rx.pattern();
  m_currentRx = rx;
  QString sql(SQL_REGEX_FIND_ENTRY_DETAILS);
  if (m_query.prepare(sql)) {
    sql = SQL_FIND_ENTRY_DETAILS;// "select root,word,xml,page from entry where datasource = 1 and nodeid = ?";
    if (! m_nodeQuery.prepare(sql)) {
      QLOG_WARN() << QString("SQL prepare error %1 : %2")
        .arg(sql)
        .arg(m_nodeQuery.lastError().text());
      return;
    }
  }
  else {
    QLOG_WARN() << QString("SQL prepare error %1 : %2")
      .arg(sql)
      .arg(m_query.lastError().text());
    return;
  }
  if (m_debug) {
    m_rxlist->showColumn(NODE_COLUMN);
  }
  else {
    m_rxlist->hideColumn(NODE_COLUMN);
  }
  //  m_rxlist->showColumn(4);
  m_rxlist->setRowCount(0);
  //  m_rxlist->hide();

  //  m_nodquery.bindValue(0,m_target);


  int headCount = 0;
  int textCount = 0;
  int readCount = 0;
  int entryCount = 0;
  QString node;
  QString root;
  QString headword;
  int page;
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
    readCount++;
    if ((readCount % m_stepCount) == 0) {
      m_progress->setValue(readCount);
    }
    if (pd) {
        pd->setValue(readCount);
    }
    ep.processEvents();
    headword = m_query.value("word").toString();
    root = m_query.value("root").toString();
    node = m_query.value("nodeid").toString();
    page = m_query.value("page").toInt();
    if (headword.indexOf(rx) != -1) {
      if (options.includeHeads()) {
        int row = addRow(root,headword,node,m_headText,0,page);
        headCount++;
        if (m_headBackgroundColor.isValid()) {
          QBrush b(m_headBackgroundColor);
          for(int i=0;i < m_rxlist->columnCount();i++) {
            m_rxlist->item(row,i)->setBackground(b);
          }
        }
      }
    }
    QString xml = m_query.value("xml").toString();
    QTextDocument * doc  = fetchDocument(xml);
    if (doc->characterCount() > 0) {
            getTextFragments(doc,target,options);
            if (m_fragments.size() > 0) {
              entryCount++;
              if (m_singleRow) {
                QString html = qobject_cast<Lexicon *>(qApp)->scanAndSpan(m_fragments[0]);
                addRow(root,headword,node,html,m_fragments.size(),page);
              }
              else {
                for(int i=0;i < m_fragments.size();i++) {
                  if (m_fragments[i].size() > 0) {
                    QString html = qobject_cast<Lexicon *>(qApp)->scanAndSpan(m_fragments[i]);
                    addRow(root,headword,node,html,i,page);
                  }
                }
              }
              textCount += m_fragments.size();
            }
    }

  }
  m_rxlist->setUpdatesEnabled(true);
  qint64 et = QDateTime::currentMSecsSinceEpoch();
  QLOG_DEBUG() << "Search time ms:" << et - st;
  if (pd) {
    delete pd;
  }

  if (m_rxlist->rowCount() > 0) {
    m_container->removeItem(m_spacer);
    m_rxlist->show();
    m_rxlist->selectRow(0);
    m_rxlist->setFocus();
    m_exportButton->setEnabled(true);
  }
  else {
    m_exportButton->setEnabled(false);
  }
  //  m_resultsText->setText(QString("Search for %1, returned %2 results").arg(target).arg(m_rxlist->rowCount()));
  m_resultsText->setText(buildText(entryCount,headCount,textCount,et - st));
  m_resultsText->show();
  m_rxlist->resizeColumnToContents(SELECT_COLUMN);
  m_rxlist->resizeColumnToContents(ROOT_COLUMN);
  m_rxlist->resizeColumnToContents(NODE_COLUMN);
  m_rxlist->resizeColumnToContents(POSITION_COLUMN);
  m_rxlist->resizeColumnToContents(CONTEXT_COLUMN);
  m_rxlist->resizeColumnToContents(VOL_COLUMN);
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
void FullSearchWidget::setForceLTR(bool v) {
   m_findTarget->setForceLTR(v);
 }
void FullSearchWidget::onExport() {
  m_rxlist->exportResults();
}
