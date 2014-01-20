#include "searchresults.h"
SearchResultsWidget::SearchResultsWidget(const QString & str,QWidget * parent) : QWidget(parent) {
  m_target = str;


  QVBoxLayout * layout = new QVBoxLayout;
  m_list = new QListWidget;
  m_text = new QTextEdit;
  layout->addWidget(m_list);
  layout->addWidget(m_text);

  qDebug() << Q_FUNC_INFO << str;
  QString sql = QString("select * from xref where word = ?");
  if (m_query.prepare(sql)) {
    m_query.bindValue(0,m_target);
    m_query.exec();
  }
  else {
    m_text->setText("Error prepare SQL");
    return;
  }
  while(m_query.next()) {
    QListWidgetItem * item = new QListWidgetItem(m_query.value("node").toString(),m_list);
  }

  setLayout(layout);
}
