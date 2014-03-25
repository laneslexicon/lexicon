#include "searchresults.h"
SearchResultsWidget::SearchResultsWidget(const QString & str,QWidget * parent) : QWidget(parent) {
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
  m_list->setColumnCount(3);
  QStringList headers;
  headers << tr("Root") << tr("Entry") << tr("Node");
  m_list->setHorizontalHeaderLabels(headers);
  m_list->horizontalHeader()->setStretchLastSection(true);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  m_text = new GraphicsEntry;

  bool ok = false;
  /// TODO replace select *
  QString sql = QString("select * from xref where word = ? order by root,entry asc");
  if (m_query.prepare(sql)) {
    if (m_nodeQuery.prepare("select * from entry where nodeId = ?")) {
      ok = true;
    }
  }
  if (! ok ) {
    QLOG_WARN() << "Error prepare SQL";
    return;
  }
  m_query.bindValue(0,m_target);
  m_query.exec();
  QStringList nodes;

  QTableWidgetItem * item;
  /// TODO include count in table ?
  while(m_query.next()) {
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
      m_list->setItem(row,2,item);
      nodes << t;
    }
  }
  QSplitter * splitter = new QSplitter(Qt::Vertical);
  splitter->addWidget(m_list);
  splitter->addWidget(m_text);
  splitter->setStretchFactor(0,0);
  splitter->setStretchFactor(1,1);
  layout->addWidget(splitter);
  m_list->adjustSize();//resizeColumnsToContents();

  setLayout(layout);
  connect(m_list,SIGNAL(currentItemChanged(QTableWidgetItem * ,QTableWidgetItem * )),
          this,SLOT(itemChanged(QTableWidgetItem * ,QTableWidgetItem * )));
}
int SearchResultsWidget::count() {
  return m_list->rowCount();
}
void SearchResultsWidget::itemChanged(QTableWidgetItem * item,QTableWidgetItem * prev ) {
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
