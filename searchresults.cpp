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
  m_list->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  QStringList headers;
  headers << tr("Root") << tr("Entry") << tr("Node");
  m_list->setHorizontalHeaderLabels(headers);
  m_list->horizontalHeader()->setStretchLastSection(true);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  m_list->installEventFilter(this);
  QStyle * style = m_list->style();
  QLOG_DEBUG() << "style hint" << style->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus);
  m_text = new GraphicsEntry;

  bool ok = false;
  /// TODO replace select *
  QString sql = QString("select * from xref where datasource = 1 and word = ? order by root,entry asc");
  if (m_query.prepare(sql)) {
    if (m_nodeQuery.prepare("select * from entry where datasource = 1 and nodeId = ?")) {
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
