#include "ensearchwidget.h"
#include "textsearchwidget.h"
#include "textsearch.h"
#include "externs.h"
#include "nodeview.h"
EnsearchWidget::EnsearchWidget(int rows,QWidget * parent) : QWidget(parent) {
  m_search = new TextSearchWidget(rows,false);
  m_pageSize = rows;
  setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(m_search);
  m_summary = new QLabel("");
  layout->addWidget(m_summary);
#ifndef LANE
  QHBoxLayout * hlayout = new QHBoxLayout;
  m_quit = new QPushButton(tr("Quit"));
  hlayout->addWidget(m_quit);
  hlayout->addStretch();
  layout->addLayout(hlayout);
  connect(m_quit,SIGNAL(clicked()),this,SLOT(onExit()));
#endif
  setLayout(layout);
  connect(m_search->searcher(),SIGNAL(recordsRead(int)),this,SLOT(recordsRead(int)));
  connect(m_search,SIGNAL(showNode(const QString &,bool)),this,SIGNAL(showNode(const QString &,bool)));
  connect(m_search,SIGNAL(printNode(const QString &)),this,SIGNAL(printNode(const QString &)));
}
/*
TextSearchWidget * EnsearchWidget::searcher() {
  return m_search;
}
*/
QSize EnsearchWidget::sizeHint() const {
  return QSize(800,400);
}
TextSearch * EnsearchWidget::searcher() {
  return m_search->searcher();
}

int EnsearchWidget::search() {
  int max = m_search->searcher()->readSize();
  m_pd = new QProgressDialog("Searching...", "Cancel", 0,max, this);
  m_pd->setWindowTitle(tr("Text Search"));
  connect(m_pd,SIGNAL(canceled()),this,SLOT(cancelSearch()));
  m_pd->setWindowModality(Qt::WindowModal);
  m_pd->show();
  this->recordsRead(1);
  int findCount = m_search->searcher()->search();
  delete m_pd;
  m_pageCounts = m_search->searcher()->setPages(m_pageSize);
  // TextSearchWidget needs to readSettings for:
  // summary/full
  // rows per table
  // fragment size
  // fields
  m_search->setPages(m_pageCounts.second);
  m_search->loadPage(1);
  QString txt = m_search->searcher()->summary();
  m_summary->setText(getSupport()->scanAndSpan(txt,"searchsummary",true));
  return findCount;
}
void EnsearchWidget::recordsRead(int x) {
  m_pd->setValue(x);
  m_ep.processEvents();
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
void EnsearchWidget::cancelSearch() {
  searcher()->setCancel(true);
}
void EnsearchWidget::focusTable() {
  m_search->focusTable();
}
void EnsearchWidget::setDiacritics(const QString & v) {
  if (v.length() == 0) {
    m_search->searcher()->setDiacritics(); // get from settings ini
  }
  else {
    m_search->searcher()->setDiacritics(v); // get from commandline
  }
}
