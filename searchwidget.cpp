#include "searchwidget.h"
#include "searchoptions.h"
#include "graphicsentry.h"
#include "application.h"
#include "namespace.h"
#include "laneslexicon.h"
#include "imlineedit.h"
//extern LanesLexicon * getApp();
SearchWidget::SearchWidget(QWidget * parent) : QWidget(parent) {
  qDebug() << Q_FUNC_INFO << "enter";
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  settings->beginGroup("Search");
  QString f = settings->value("Results font",QString()).toString();
  if (! f.isEmpty()) {
    m_resultsFont.fromString(f);
  }
  delete settings;
  qDebug() << "at" << 1;
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
  qDebug() << "at" << 2;

  QWidget * container = new QWidget;
  QVBoxLayout * containerlayout = new QVBoxLayout;
  m_list = new QTableWidget;
  m_list->setColumnCount(4);
  m_list->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  QStringList headers;
  headers << tr("Root") << tr("Entry") << tr("Node") << tr("Count");
  m_list->setHorizontalHeaderLabels(headers);
  m_list->horizontalHeader()->setStretchLastSection(true);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  m_list->installEventFilter(this);

  m_rxlist = new QTableWidget;
  m_rxlist->setColumnCount(4);
  m_rxlist->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_rxlist->setSelectionMode(QAbstractItemView::SingleSelection);

  qDebug() << "at" << 3;

  m_rxlist->setHorizontalHeaderLabels(headers);
  m_rxlist->horizontalHeader()->setStretchLastSection(true);
  m_rxlist->setSelectionMode(QAbstractItemView::SingleSelection);
  m_rxlist->installEventFilter(this);
  //QStyle * style = m_list->style();
  //  QLOG_DEBUG() << "style hint" << style->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus);
  m_resultsText = new QLabel("");
  m_resultsText->hide();
  containerlayout->addLayout(targetlayout);
  containerlayout->addWidget(m_search);
  QHBoxLayout * listlayout = new QHBoxLayout;
  listlayout->addWidget(m_list);
  listlayout->addWidget(m_rxlist);
  containerlayout->addLayout(listlayout);
  containerlayout->addWidget(m_resultsText);
  container->setLayout(containerlayout);

  m_text = new GraphicsEntry;
  m_text->hide();
  qDebug() << "at" << 4;

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
  qDebug() << Q_FUNC_INFO << "exit";

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
  if (options & Lane::Ignore_Diacritics) {
    qDebug() << "ignoring diacritics";
    if (options & Lane::Whole_Word) {
      qDebug() << "whole word match";
      sql += "and bareword = ? ";
    }
    else {
      sql += "and instr(bareword,?) > 0";
    }
  }
  else {
    if (options & Lane::Whole_Word) {
      qDebug() << "whole word match";
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
    if (options & Lane::Full) {
      t = m_query.value("node").toString();
      word = m_query.value("entry").toString();
    }
    else {
      t = m_query.value("nodeid").toString();
      word = m_query.value("word").toString();
    }
    m_nodes << t;
    if (! nodes.contains(t)) {
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
    else {
      nodes[t] = nodes.value(t) + 1;
    }
  }
  for(int i=0; i < m_list->rowCount();i++) {
    QString t = m_list->item(i,NODE_COLUMN)->text();
    if (! t.isEmpty() && nodes.contains(t)) {
      m_list->item(i,3)->setText(QString("%1").arg(nodes.value(t)));
    }
  }
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
  QString ar("[\\x064b\\x064c\\x064d\\x064e\\x064f\\x0650\\x0651\\x0652\\x0670\\x0671]*");
  this->search(m_findTarget->text(),m_search->getOptions());
  this->regexSearch(m_findTarget->text(),m_search->getOptions());
  m_text->show();
  bool ok = true;
  if (m_nodes.size() == m_rxnodes.size()) {

    for(int i=0;i < m_nodes.size();i++) {
      if (! m_rxnodes.contains(m_nodes[i]))
        ok = false;
    }
  }
  qDebug() << "Search match" << ok;
}
/**
 *
 *
 * @param target
 * @param options
 */
void SearchWidget::regexSearch(const QString & target,int options) {
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
  if (options & Lane::Full) {
  sql = "select id,word,root,entry,node from xref where datasource = 1 ";
  }
  else {
    sql = "select id,word,root,nodeid,nodenum from entry where datasource = 1 ";
  }
  QRegExp rx;
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
    pattern += "\\b" + pattern + "\\b";
  }
  qDebug() << "Pattern" << pattern;
  rx.setPattern(pattern);
  if (options & Lane::Full) {
    sql += " order by root,entry asc";
  }
  else {
    sql += " order by root,nodenum asc";
  }
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
  m_rxlist->setRowCount(0);
#define NODE_COLUMN 2
  qint64 st = QDateTime::currentMSecsSinceEpoch();
  //  m_nodquery.bindValue(0,m_target);
  m_query.exec();
  QMap<QString,int> nodes;

  QTableWidgetItem * item;
  /// TODO include count in table ?
  int count = 0;
  int readCount = 0;
  while(m_query.next()) {
    readCount++;
    QString t;
    QString word;
    if (options & Lane::Full) {
      t = m_query.value("node").toString();
      word = m_query.value("entry").toString();
    }
    else {
      t = m_query.value("nodeid").toString();
      word = m_query.value("word").toString();
    }
    if (word.indexOf(rx) != -1) {
      count++;
      m_rxnodes << t;
      if (! nodes.contains(t)) {
        int row = m_rxlist->rowCount();
        m_rxlist->insertRow(row);
        item = new QTableWidgetItem(m_query.value("root").toString());
        item->setFont(m_resultsFont);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        m_rxlist->setItem(row,0,item);

        item = new QTableWidgetItem(word);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        item->setFont(m_resultsFont);
        m_rxlist->setItem(row,1,item);

        item = new QTableWidgetItem(t);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        m_rxlist->setItem(row,NODE_COLUMN,item);
        nodes.insert(t,1);
        m_rxlist->setItem(row,3,new QTableWidgetItem("1"));
      }
      else {
        nodes[t] = nodes.value(t) + 1;
      }
    }
  }
  for(int i=0; i < m_rxlist->rowCount();i++) {
    QString t = m_rxlist->item(i,NODE_COLUMN)->text();
    if (! t.isEmpty() && nodes.contains(t)) {
      m_rxlist->item(i,3)->setText(QString("%1").arg(nodes.value(t)));
    }
  }
  //  emit(searchResult(QString(tr("Found %1 items")).arg(count)));
  QString t = QString(tr("Search for %1, find count %2 ")).arg(m_target).arg(count);
  if (m_searchOptions & Lane::Ignore_Diacritics)
    t += tr(", ignoring diacritics");
  if (m_searchOptions & Lane::Whole_Word)
    t += tr(", whole word match");
  m_resultsText->setText(t);
  m_resultsText->show();
  if (m_rxlist->rowCount() > 0)
    m_rxlist->itemDoubleClicked(m_rxlist->item(0,0));
  qDebug() << "read count" << count << "time" <<   (QDateTime::currentMSecsSinceEpoch() - st);
}
