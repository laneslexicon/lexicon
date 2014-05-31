#include "searchresults.h"
#include "namespace.h"
#include "laneslexicon.h"
//extern LanesLexicon * getApp();
SearchResultsWidget::SearchResultsWidget(QWidget * parent) : QWidget(parent) {
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  settings->beginGroup("Search");
  QString f = settings->value("Results font",QString()).toString();
  if (! f.isEmpty()) {
    m_resultsFont.fromString(f);
  }
  delete settings;
  QVBoxLayout * layout = new QVBoxLayout;

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
  //QStyle * style = m_list->style();
  //  QLOG_DEBUG() << "style hint" << style->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus);
  m_resultsText = new QLabel("");
  m_resultsText->hide();
  containerlayout->addWidget(m_list);
  containerlayout->addWidget(m_resultsText);
  container->setLayout(containerlayout);
  m_text = new GraphicsEntry;
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
SearchResultsWidget::SearchResultsWidget(const QString & str,int options,QWidget * parent) : QWidget(parent) {
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
int SearchResultsWidget::count() {
  return m_list->rowCount();
}
void SearchResultsWidget::itemChanged(QTableWidgetItem * item,QTableWidgetItem * /* prev */) {
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
void SearchResultsWidget::itemDoubleClicked(QTableWidgetItem * item) {
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
bool SearchResultsWidget::eventFilter(QObject * target,QEvent * event) {

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
void SearchResultsWidget::search(const QString & target,int options) {
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
  sql = "select id,word,root,entry,node from xref where datasource = 1 ";
  if (options & Lane::Ignore_Diacritics) {
    qDebug() << "ignoring diacritics";
    if (options & Lane::Whole_Word_Match) {
      qDebug() << "whole word match";
      sql += "and bareword = ? ";
    }
    else {
      sql += "and instr(bareword,?) > 0";
    }
  }
  else {
    if (options & Lane::Whole_Word_Match) {
      qDebug() << "whole word match";
      sql += "and word = ? ";
    }
    else {
      sql += "and instr(word,?) > 0";
    }
  }
  sql += " order by root,entry asc";
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
  m_query.bindValue(0,m_target);
  m_query.exec();
  QMap<QString,int> nodes;

  QTableWidgetItem * item;
  /// TODO include count in table ?
  int count = 0;
  while(m_query.next()) {
    count++;
    QString t = m_query.value("node").toString();
    if (! nodes.contains(t)) {
      int row = m_list->rowCount();
      m_list->insertRow(row);
      QString word = m_query.value("entry").toString();

      item = new QTableWidgetItem(m_query.value("root").toString());
      item->setFont(m_resultsFont);
      item->setFlags(item->flags() ^ Qt::ItemIsEditable);
      m_list->setItem(row,0,item);

      item = new QTableWidgetItem(m_query.value("entry").toString());
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
  if (m_searchOptions & Lane::Whole_Word_Match)
    t += tr(", whole word match");
  m_resultsText->setText(t);
  m_resultsText->show();
  if (m_list->rowCount() > 0)
    m_list->itemDoubleClicked(m_list->item(0,0));
}
