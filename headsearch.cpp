#include "graphicsentry.h"
#include "application.h"
#include "headsearch.h"
#include "focustable.h"
#include "namespace.h"
#include "laneslexicon.h"
extern LanesLexicon * getApp();
HeadSearchWidget::HeadSearchWidget(QWidget * parent) : QWidget(parent) {
  readSettings();
  QVBoxLayout * layout = new QVBoxLayout;

  QWidget * container = new QWidget;
  QVBoxLayout * containerlayout = new QVBoxLayout;
  m_list = new FocusTable;
  m_list->setColumnCount(3);
  m_list->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  QStringList headers;
  headers << tr("Root") << tr("Entry") << tr("Node");
  m_list->setHorizontalHeaderLabels(headers);
  m_list->horizontalHeader()->setStretchLastSection(true);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  m_list->installEventFilter(this);
  //QStyle * style = m_list->style();
  //  QLOG_DEBUG() << "style hint" << style->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus);
  m_resultsText = new QLabel("");
  m_resultsText->hide();
  containerlayout->addWidget(m_list);
  containerlayout->addWidget(m_resultsText);
  container->setLayout(containerlayout);
  m_text = new GraphicsEntry;
  m_text->installEventFilter(this);
  //  qDebug() << "result count" << count;
  //  this->search(str,options);
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
  connect(m_list,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
          this,SLOT(itemDoubleClicked(QTableWidgetItem * )));
  /// show the first item in the list
  /// TODO decide who gets focus and select the first row
  /// TODO if table loses focus, change the background selection color
  if (m_list->rowCount() > 0)
    m_list->itemDoubleClicked(m_list->item(0,0));

}
/// TODO can remove this
HeadSearchWidget::HeadSearchWidget(const QString & str,int options,QWidget * parent) : QWidget(parent) {
  m_target = str;
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  settings->beginGroup("Search");
  QString f = settings->value("Results font",QString()).toString();
  if (! f.isEmpty()) {
    m_resultsFont.fromString(f);
  }
  delete settings;
  QVBoxLayout * layout = new QVBoxLayout;
  m_list = new FocusTable;
  m_list->setColumnCount(4);
  m_list->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  QStringList headers;
  headers << tr("Root") << tr("Entry") << tr("Node") << tr("Count");
  m_list->setHorizontalHeaderLabels(headers);
  m_list->horizontalHeader()->setStretchLastSection(true);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  m_list->installEventFilter(this);
  QStyle * style = m_list->style();
  QLOG_DEBUG() << "style hint" << style->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus);
  m_text = new GraphicsEntry;
  //  qDebug() << "result count" << count;
  this->search(str,options);
  QSplitter * splitter = new QSplitter(Qt::Vertical);
  splitter->addWidget(m_list);
  splitter->addWidget(m_text);
  splitter->setStretchFactor(0,0);
  splitter->setStretchFactor(1,1);
  layout->addWidget(splitter);
  m_list->adjustSize();//resizeColumnsToContents();

  setLayout(layout);
  //connect(m_list,SIGNAL(currentItemChanged(QTableWidgetItem * ,QTableWidgetItem * )),
  //      this,SLOT(itemChanged(QTableWidgetItem * ,QTableWidgetItem * )));
  connect(m_list,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
          this,SLOT(itemDoubleClicked(QTableWidgetItem * )));
  /// show the first item in the list
  /// TODO decide who gets focus and select the first row
  /// TODO if table loses focus, change the background selection color
  if (m_list->rowCount() > 0)
    m_list->itemDoubleClicked(m_list->item(0,0));

}
int HeadSearchWidget::count() {
  return m_list->rowCount();
}
void HeadSearchWidget::itemChanged(QTableWidgetItem * item,QTableWidgetItem * /* prev */) {
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
void HeadSearchWidget::itemDoubleClicked(QTableWidgetItem * item) {
  /// get the node
  qDebug() << Q_FUNC_INFO << "row" << item->row();
  item = item->tableWidget()->item(item->row(),2);
  QString node = item->text();
  m_nodeQuery.bindValue(0,node);
  m_nodeQuery.exec();
  /// missing node
  if ( ! m_nodeQuery.first()) {
    QLOG_WARN() << "No record for node" << node;
    return;
  }
  if (m_text->focusNode(node))
    return;

  Place np;
  np.setNode(node);
  np = m_text->getXmlForRoot(np);
  if (np.isValid()) {
    m_text->focusNode(node);
  }
  else {
    QLOG_DEBUG() << "Invalid place returned for node" << node;
  }
}
bool HeadSearchWidget::eventFilter(QObject * target,QEvent * event) {
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
    switch(keyEvent->key()) {
    case Qt::Key_Escape: {
      qDebug() << Q_FUNC_INFO << "escape hit" << target;
      GraphicsEntry * e = qobject_cast<GraphicsEntry *>(target);
      if (e) {
        m_list->setFocus();
        return true;
      }
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
      QTableWidget *e = qobject_cast<QTableWidget *>(target);
      if ( e) {
        QTableWidgetItem * item = m_list->currentItem();
        if (item) {
          m_list->itemDoubleClicked(item);
          /// Need this - seems <return> handling is different from space
          /// without out the table keeps focus
          return true;
        }
      }
      break;
    }
    case Qt::Key_Tab: {
      QTableWidget *e = qobject_cast<QTableWidget *>(target);
      if ( e) {
        m_text->setFocus();
      }
      break;
    }
    case Qt::Key_E: {
      QTableWidget *e = qobject_cast<QTableWidget *>(target);
      if ( e) {
        if (keyEvent->modifiers() && Qt::ControlModifier) {
          //          if (target == m_tree)
          //          m_tabs->currentWidget()->setFocus();
          return true;
        }
      }
      break;
    }
    default:
      break;
    }
  }
  return QWidget::eventFilter(target,event);
}
void HeadSearchWidget::search(const QString & target,int options) {
  QRegExp rx;
  QRegExp rxclass("[\\x064b\\x064c\\x064d\\x064e\\x064f\\x0650\\x0651\\x0652\\x0670\\x0671]*");

  bool replaceSearch = true;
  m_target = target;
  m_searchOptions = options;
  QString sql;
  //  whole word with diacritics
  //   select id,node where word = ?
  //  part word with diacritics
  //   select id,node from xref where instr(word,?) > 0;
  //  whole word without diacritics
  //   select where bareword = ?
  //  part word without diacritics
  //   select where instr(bareword,?) > 0
  sql = "select id,word,root,nodeid,nodenum from entry where datasource = 1 order by nodenum asc";
  QString pattern;
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
  bool ok = false;
  if (m_query.prepare(sql)) {
    if (m_nodeQuery.prepare("select * from entry where datasource = 1 and nodeId = ?")) {
      ok = true;
    }
  }
  if (! ok ) {
    QLOG_WARN() << "Error prepare SQL" << sql;
    return;
  }
  m_list->setRowCount(0);
#define NODE_COLUMN 2
  if (m_debug) {
    m_list->showColumn(NODE_COLUMN);
  }
  else {
    m_list->hideColumn(NODE_COLUMN);
  }

  m_query.exec();
  QTableWidgetItem * item;
  int count = 0;
  QProgressDialog *  pd = new QProgressDialog("Searching...", "Cancel", 0,48000, getApp());
  connect(pd,SIGNAL(canceled()),this,SLOT(cancelSearch()));
  pd->setWindowModality(Qt::WindowModal);
  //  pd->setMinimumDuration(0);
  pd->show();
  m_cancelSearch = false;
  QString node;
  QString word;
  while(m_query.next() & ! m_cancelSearch) {
    count++;
    node = m_query.value("nodeid").toString();
    word = m_query.value("word").toString();
    /// strip diacritics if required
    if (replaceSearch) {
      if (options & Lane::Ignore_Diacritics)
        word =  word.replace(rxclass,QString());
    }
    if (word.indexOf(rx) != -1) {
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

      item = new QTableWidgetItem(node);
      item->setFlags(item->flags() ^ Qt::ItemIsEditable);
      m_list->setItem(row,NODE_COLUMN,item);
    }
  if ((count % m_stepCount) == 0) {
    pd->setValue(count);
  }
  }
  m_resultsText->setText(this->buildText(options));
  m_resultsText->show();
  if (m_list->rowCount() > 0) {
    m_list->selectRow(0);
    m_list->setFocus();
  }
  delete pd;
}
void HeadSearchWidget::showFirst() {
  if (m_list->rowCount() > 0) {
    QTableWidgetItem * item = m_list->item(0,0);
    this->itemChanged(item,0);
    QFocusEvent * e = new QFocusEvent(QEvent::FocusOut);
    QApplication::postEvent(m_list,e);
    //    m_list->selectRow(0);
    //    m_list->setFocus();

  }
}
QString HeadSearchWidget::buildText(int options) {
  QString t;
  QString p1;
  QString p2;
  int findCount = m_list->rowCount();

  t = QString("Search for %1: ").arg(m_target);
  switch(findCount) {
  case 0:
    p1 = QString(tr("No items found"));
    break;
  case 1:
    p1 = QString(tr("1 entry found"));
    break;
  default:
    p1 = QString("%1 entries found").arg(findCount);
    break;
  }
  t += p1;
  if (m_searchOptions & Lane::Ignore_Diacritics)
    t += tr(", ignoring diacritics");
  if (m_searchOptions & Lane::Whole_Word)
    t += tr(", whole word match");

  return t;
}
void HeadSearchWidget::focusTable() {
  m_list->setFocus();
}
void HeadSearchWidget::focusInEvent(QFocusEvent * event) {
  qDebug() << Q_FUNC_INFO << event;
  if (event->reason() == Qt::OtherFocusReason) {
    if (m_list->rowCount() > 0) {
      m_list->setFocus();

    }
    //    else {
    //      m_findTarget->setFocus();
    //    }
  }
  QWidget::focusInEvent(event);
}
void HeadSearchWidget::focusOutEvent(QFocusEvent * event) {
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
void HeadSearchWidget::cancelSearch() {
  m_cancelSearch = true;
}
void HeadSearchWidget::readSettings() {
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  settings->beginGroup("HeadSearch");
  QString f = settings->value("Results font",QString()).toString();
  if (! f.isEmpty()) {
    m_resultsFont.fromString(f);
  }
  m_stepCount = settings->value("Step",100).toInt();
  m_debug = settings->value("Debug",false).toBool();
  delete settings;
}
