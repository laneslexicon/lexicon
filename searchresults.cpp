#include "searchresults.h"
SearchResultsWidget::SearchResultsWidget(const QString & str,QWidget * parent) : QWidget(parent) {
  m_target = str;


  QVBoxLayout * layout = new QVBoxLayout;
  m_list = new QListWidget;
  m_text = new GraphicsEntry;
  QSplitter * splitter = new QSplitter(Qt::Vertical);
  splitter->addWidget(m_list);
  splitter->addWidget(m_text);
  splitter->setStretchFactor(0,0);
  splitter->setStretchFactor(1,1);
  layout->addWidget(splitter);

  qDebug() << Q_FUNC_INFO << str;
  bool ok = false;
  QString sql = QString("select * from xref where word = ?");
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
  while(m_query.next()) {
    QString t = m_query.value("node").toString();
    if (! nodes.contains(t)) {
      QListWidgetItem * item = new QListWidgetItem(t,m_list);
      nodes << t;
    }
  }
  setLayout(layout);
  connect(m_list,SIGNAL(currentItemChanged(QListWidgetItem * ,QListWidgetItem * )),
          this,SLOT(itemChanged(QListWidgetItem * ,QListWidgetItem * )));
}
void SearchResultsWidget::itemChanged(QListWidgetItem * item,QListWidgetItem * prev ) {
  QString node = item->text();
  m_nodeQuery.bindValue(0,node);
  m_nodeQuery.exec();
  /// missing node
  if ( ! m_nodeQuery.first()) {
    QLOG_WARN() << "No record for node" << node;
    return;
  }
  m_text->getXmlForNode(node);
  m_text->highlight(m_target);
}
