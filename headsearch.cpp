#include "graphicsentry.h"
#include "application.h"
#include "headsearch.h"
#include "laneslexicon.h"
#include "searchoptionswidget.h"
#include "definedsettings.h"
#include "definedsql.h"
#include "externs.h"
#include "scripts.h"
#include "columnartablewidget.h"
#include "nodeinfo.h"
#include "xsltsupport.h"
#include "centeredcheckbox.h"
#define SELECT_COLUMN 0
#define ROOT_COLUMN 1
#define HEAD_COLUMN 2
#define ENTRY_COLUMN 3
#define NODE_COLUMN 4
#define VOL_COLUMN 5
#define COLUMN_COUNT 6
extern LanesLexicon * getApp();
HeadSearchWidget::HeadSearchWidget(QWidget * parent) : QWidget(parent) {
  readSettings();
  QVBoxLayout * layout = new QVBoxLayout;
  setWindowTitle(tr("Search for Head Word"));
  setObjectName("headsearchwidget");

  m_heads = new ColumnarTableWidget(QStringList() << tr("Mark") << tr("Root") << tr("Head word") << tr("Entry") << tr("Node") << tr("Vol/Page"));

  m_heads->setKey(ColumnarTableWidget::STATE,SID_HEADSEARCH_STATE);
  m_heads->setDefaultWidth(100);
  m_heads->setObjectName("headsearchlist");
  m_heads->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_heads->setSelectionMode(QAbstractItemView::SingleSelection);
  m_heads->setMarkColumn(SELECT_COLUMN);
  m_heads->setFixedRowHeight(m_rowHeight);
  m_heads->installEventFilter(this);

  m_searchTitle = new QLabel("");
  m_searchTitle->hide();
  m_resultsText = new QLabel("");
  m_resultsText->hide();

  m_exportButton = new QPushButton(tr("&Export results"));
  m_exportButton->hide();

  connect(m_exportButton,SIGNAL(clicked()),this,SLOT(onExport()));

  layout->addWidget(m_searchTitle);
  layout->addWidget(m_heads);

  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addWidget(m_resultsText);
  hlayout->addStretch();
  hlayout->addWidget(m_exportButton);

  layout->addLayout(hlayout);

  setLayout(layout);
  connect(m_heads,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
          this,SLOT(onItemDoubleClicked(QTableWidgetItem * )));
  connect(m_heads,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(onCellDoubleClicked(int,int)));


  SETTINGS

  settings.beginGroup("HeadSearch");
  m_heads->readConfiguration(settings);
  settings.endGroup();

  if (! m_nodeQuery.prepare(SQL_ENTRY_FOR_NODE)) {
    QLOG_WARN() << QString("SQL prepare error on SQL_ENTRY_FOR_NODE [%1], error:%2").arg(SQL_ENTRY_FOR_NODE)
      .arg(m_nodeQuery.lastError().text());
  }
}
int HeadSearchWidget::count() {
  return m_heads->rowCount();
}
void HeadSearchWidget::onItemDoubleClicked(QTableWidgetItem * item) {
  QLOG_DEBUG() << Q_FUNC_INFO << "row" << item->row();

  this->showNode(m_heads->currentRow());
}
void HeadSearchWidget::onCellDoubleClicked(int row,int /* col */) {
  QLOG_DEBUG() << Q_FUNC_INFO << "row";
  this->showNode(row);

}
bool HeadSearchWidget::eventFilter(QObject * target,QEvent * event) {
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
    switch(keyEvent->key()) {
      /// TODO Do we need this
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
      this->onCellDoubleClicked(m_heads->currentRow(),NODE_COLUMN);
      break;
    }
    default:
      break;
    }
  }
  return QWidget::eventFilter(target,event);
}
void HeadSearchWidget::search(const QString & searchpattern,const SearchOptions & options) {
  QLOG_DEBUG() << Q_FUNC_INFO;
  QTableWidgetItem * item;
  int count = 0;

  QRegExp rx;
  QRegExp rxclass(m_diacritics);
  QString pattern = searchpattern;
  pattern.remove(QChar(0x202d));

  bool replaceSearch = true;

  m_target = pattern;
  m_searchOptions = options;

  rx = SearchOptionsWidget::buildRx(pattern,m_diacritics,options);
  m_currentRx = rx;

  if (! m_query.prepare(SQL_ALL_ENTRIES)) {
      QLOG_WARN() << QString(tr("SQL prepare error %1: %2"))
        .arg("SQL_ALL_ENTRIES")
        .arg(m_query.lastError().text());
     return;
  }

  m_heads->clearContents();
  m_heads->setRowCount(0);
  m_query.exec();
  QEventLoop ep;

  QProgressDialog *  pd = new QProgressDialog(tr("Searching..."), tr("Cancel"), 0,48000, getApp());
#ifdef __APPLE__
  pd->setStyle(QStyleFactory::create("Fusion"));
#endif
  connect(pd,SIGNAL(canceled()),this,SLOT(cancelSearch()));

  pd->setWindowModality(Qt::WindowModal);
  int width = pd->frameGeometry().width();
  int height = pd->frameGeometry().height();

  QDesktopWidget wid;

  int screenWidth = wid.screen()->width();
  int screenHeight = wid.screen()->height();

  pd->setGeometry((screenWidth/2)-(width/2),(screenHeight/2)-(height/2),width,height);
  pd->show();

  m_cancelSearch = false;

  Place p;
  QLabel * label;
  QString searchtarget;

  while(m_query.next() & ! m_cancelSearch) {
    count++;

    if (options.headPhrase()) {
      searchtarget = m_query.value("word").toString();
    }
    else {
      searchtarget = m_query.value("headword").toString();
    }
    /// strip diacritics if required
    if (options.getSearchType() == SearchOptions::Normal) {
      if (replaceSearch) {
        if (options.ignoreDiacritics())
          searchtarget =  searchtarget.replace(rxclass,QString());
      }
    }
    if (searchtarget.indexOf(rx) != -1) {
      p = Place::fromEntryRecord(m_query.record());


      int row = m_heads->rowCount();
      m_heads->insertRow(row);

      m_heads->setCellWidget(row,SELECT_COLUMN,new CenteredCheckBox);

      label = m_heads->createLabel(p.m_root,"headsearchlist");
      label->setAlignment(Qt::AlignCenter);
      m_heads->setCellWidget(row,ROOT_COLUMN,label);

      label = m_heads->createLabel(p.m_head,"headsearchlist");
      label->setAlignment(Qt::AlignCenter);
      m_heads->setCellWidget(row,HEAD_COLUMN,label);

      label = m_heads->createLabel(p.m_word,"headsearchlist");
      label->setAlignment(Qt::AlignCenter);
      m_heads->setCellWidget(row,ENTRY_COLUMN,label);

      label = m_heads->createLabel(p.format("%V/%P"));
      label->setAlignment(Qt::AlignCenter);
      m_heads->setCellWidget(row,VOL_COLUMN,label);

      item = new QTableWidgetItem(p.node());
      item->setFlags(item->flags() ^ Qt::ItemIsEditable);
      m_heads->setItem(row,NODE_COLUMN,item);

    }
    if ((count % m_stepCount) == 0) {
      pd->setValue(count);
      ep.processEvents();
    }
  }
  QString ar = getLexicon()->spanArabic(m_target,"headsearchresults");
  /// need the <body> otherwise it's treated as plain text
  QString html =  QString(tr("<html><body><p>Search for:%1</p></body></html>")).arg(ar);

  m_heads->resizeColumnToContents(SELECT_COLUMN);
  m_searchTitle->setText(html);
  m_searchTitle->show();
  m_resultsText->setText(this->buildText(options));
  m_resultsText->show();

  if (m_heads->rowCount() > 0) {
    if (m_resizeRows) {
      m_heads->resizeRowsToContents();
    }
    m_heads->selectRow(0);
    m_exportButton->setEnabled(true);
    m_exportButton->setVisible(true);
  }
  else {
    m_exportButton->setEnabled(false);
    m_exportButton->setVisible(false);
  }
  delete pd;

}

QString HeadSearchWidget::buildText(const SearchOptions & options) {
  QLOG_DEBUG() << Q_FUNC_INFO;
  QString t;
  QString p1;
  QString p2;
  int findCount = m_heads->rowCount();

  //t = QString(tr("Search for %1")).arg(m_target);
  QString targetText;
  if (options.getSearchType() != SearchOptions::Regex) {
    if (UcdScripts::isScript(m_target,"Arabic")) {
      targetText = getLexicon()->spanArabic(m_target,"headsearchresults");
  }
    else {
      targetText = m_target;
    }
  }

  switch(findCount) {
  case 0:
    p1 = QString(tr("No items found"));
    break;
  case 1:
    p1 = QString(tr("Found 1 match"));
    break;
  default:
    p1 = QString(tr("Found %1 matches")).arg(findCount);
    break;
  }
  if (options.getSearchType() == SearchOptions::Regex) {
    p2 += tr("Regular expression search");
  }
  else {
    if (m_searchOptions.ignoreDiacritics()) {
      p2 += tr("Ignoring diacritics");
    }
    if (m_searchOptions.wholeWordMatch()) {
      if (p2.isEmpty()) {
        p2 += tr("Whole word match");
      }
      else {
        p2 += tr(", whole word match");
      }
    }
  }
  if (findCount > 0) {
    if (options.headPhrase()) {
      p2 += tr(" [Scope : head phrase]");
    }
    else {
      p2 += tr(" [Scope : head word]");
    }
  }
  return (t + "\n" + p1 + "\n" + p2);
}
void HeadSearchWidget::focusTable() {
  m_heads->setFocus();
}
void HeadSearchWidget::selectFocus() {
  if (m_heads->rowCount() > 0) {
    m_heads->setFocus();

  }
  else {
    //    m_findTarget->setFocus();
  }
}
void HeadSearchWidget::focusInEvent(QFocusEvent * event) {
  QLOG_DEBUG() << Q_FUNC_INFO << event;
  this->selectFocus();
  QWidget::focusInEvent(event);
}
void HeadSearchWidget::focusOutEvent(QFocusEvent * event) {
  QLOG_DEBUG() << Q_FUNC_INFO << event;
  QWidget::focusOutEvent(event);
}
void HeadSearchWidget::cancelSearch() {
  m_cancelSearch = true;
  statusMessage(tr("Search cancelled"));
}
void HeadSearchWidget::readSettings() {
  QString v;

  SETTINGS

  settings.beginGroup("HeadSearch");

  m_stepCount = settings.value(SID_HEADSEARCH_STEP,100).toInt();
  m_headPhrase = settings.value(SID_HEADSEARCH_USE_PHRASE,false).toBool();
  m_focusTable = settings.value(SID_HEADSEARCH_FOCUS_TABLE,true).toBool();
  m_nodeinfoClose = settings.value(SID_HEADSEARCH_NODEINFO_CLOSE,true).toBool();
  m_nodeinfoForce = settings.value(SID_HEADSEARCH_NODEINFO_FORCE,false).toBool();

  m_resizeRows = settings.value(SID_HEADSEARCH_RESIZE_ROWS,true).toBool();
  m_rowHeight  = settings.value(SID_HEADSEARCH_ROW_HEIGHT,40).toInt();;

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
  ///
  /// For the NodeInfo transform
  ///

  settings.beginGroup("XSLT");
  m_xsltSource = settings.value(SID_XSLT_ENTRY,QString("entry.xslt")).toString();
  m_xsltSource = getLexicon()->getResourceFilePath(Lexicon::XSLT,m_xsltSource);
  settings.endGroup();

  settings.beginGroup("Entry");
  QString css = settings.value(SID_ENTRY_CSS,QString("entry.css")).toString();
  css = getLexicon()->getResourceFilePath(Lexicon::Stylesheet,css);
  readCssFromFile(css);

}
void HeadSearchWidget::onExport() {
  statusMessage(m_heads->exportResults(SID_HEADSEARCH_COLUMNS));
}
void HeadSearchWidget::showNode(int row)  {
  QLOG_DEBUG() << Q_FUNC_INFO << row;
  QTableWidgetItem * nodeItem = m_heads->item(row,NODE_COLUMN);
  QString node;
  if (nodeItem) {
    node = nodeItem->text();
  }

  if (node.isEmpty()) {
    return;
  }

  m_nodeQuery.bindValue(0,node);
  m_nodeQuery.exec();
  /// missing node
  if ( ! m_nodeQuery.first()) {
    QLOG_WARN() << Q_FUNC_INFO << "No record for node" << node;
    return;
  }
  /// TODO make this a QSettings option or dialog option
  QString xml = m_nodeQuery.value("xml").toString();
  QString html = this->transform(xml);
  NodeInfo * v = new NodeInfo(this);
  v->setAttribute(Qt::WA_DeleteOnClose);
  v->setWindowTitle(QString(tr("Viewing item %1").arg(row+1)));
  v->setCss(m_css);
  v->setHeader(Place::fromEntryRecord(m_nodeQuery.record()));
  v->setHtml(html);
  v->setAlwaysLoad(m_nodeinfoForce);
  v->setCloseOnLoad(m_nodeinfoClose);
  v->show();
  v->raise();
  v->activateWindow();
  connect(v,SIGNAL(openNode(const QString &,bool)),this,SLOT(openNode(const QString &,bool)));

}
QString HeadSearchWidget::transform(const QString & xml) const {
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
/**
 *
 */
bool HeadSearchWidget::readCssFromFile(const QString & name) {
  QFile f(name);
  if (! f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QLOG_WARN()  << QString(tr("Error reading CSS file :%1, error: %2 ")).arg(name).arg(f.errorString());
    return false;

  }
  QTextStream in(&f);
  m_css = in.readAll();
  f.close();
  return true;
}
void HeadSearchWidget::openNode(const QString & node,bool v ) {
  NodeInfo * viewer = qobject_cast<NodeInfo *>(sender());
  if (viewer != NULL) {
    v = viewer->alwaysLoad();
  }
  emit(showNode(node,v));
  this->setFocus();
}
