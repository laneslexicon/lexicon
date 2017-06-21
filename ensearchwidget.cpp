#include "ensearchwidget.h"
#include "textsearchwidget.h"
#include "textsearch.h"
#include "externs.h"
EnsearchWidget::EnsearchWidget(int rows,QWidget * parent) : QWidget(parent) {
  m_search = new TextSearchWidget(rows,false);
  m_pageSize = rows;
  setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(m_search);
  m_summary = new QLabel("");
  layout->addWidget(m_summary);
  QHBoxLayout * hlayout = new QHBoxLayout;
  m_quit = new QPushButton(tr("Quit"));
  hlayout->addWidget(m_quit);
  hlayout->addStretch();
  layout->addLayout(hlayout);
  setLayout(layout);
  connect(m_search->searcher(),SIGNAL(recordsRead(int)),this,SLOT(recordsRead(int)));
  connect(m_quit,SIGNAL(clicked()),this,SLOT(onExit()));
}
/*
TextSearchWidget * EnsearchWidget::searcher() {
  return m_search;
}
*/
QSize EnsearchWidget::sizeHint() const {
  return QSize(800,400);
}
void EnsearchWidget::search() {
  m_search->searcher()->search();

  m_pageCounts = m_search->searcher()->setPages(m_pageSize);
  // TODO make dependant on somethng
  m_search->setPages(m_pageCounts.second);
  m_search->loadPage(1);
  QString txt = m_search->searcher()->summary();
  m_summary->setText(getSupport()->scanAndSpan(txt,"searchsummary",true));//m_search->searcher()->summary());
}
void EnsearchWidget::recordsRead(int x) {

}
void EnsearchWidget::setSearch(const QString & pattern,bool regex,bool caseSensitive,bool wholeWord,bool diacritics) {
  //  qDebug() << Q_FUNC_INFO << pattern << regex << caseSensitive << wholeWord << diacritics;
  m_search->searcher()->setSearch(pattern,regex,caseSensitive,wholeWord,diacritics);
}
void EnsearchWidget::setFields(const QString & f) {
  m_search->searcher()->setFields(f);
}
void EnsearchWidget::setNode(const QString & n) {
  m_search->searcher()->setNode(n);
}
void EnsearchWidget::setPadding(int sz) {
  m_search->searcher()->setPadding(sz);
}
void EnsearchWidget::onExit() {
  this->close();
}
