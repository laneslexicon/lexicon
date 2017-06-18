#include "ensearchwidget.h"
#include "textsearchwidget.h"
#include "textsearch.h"

EnsearchWidget::EnsearchWidget(QWidget * parent) : QWidget(parent) {
  m_search = new TextSearchWidget(10,false);
  setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(m_search);
  setLayout(layout);
  connect(m_search->searcher(),SIGNAL(recordsRead(int)),this,SLOT(recordsRead(int)));
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
  QPair<int,int> pages = m_search->searcher()->setPages(10);
  qDebug() << Q_FUNC_INFO << pages;
  m_search->setPages(pages.second);
  m_search->loadPage(1);

}
void EnsearchWidget::recordsRead(int x) {

}
void EnsearchWidget::setSearch(const QString & pattern,bool regex,bool caseSensitive,bool wholeWord,bool diacritics) {
  qDebug() << Q_FUNC_INFO << pattern << regex << caseSensitive << wholeWord << diacritics;
  m_search->searcher()->setSearch(pattern,regex,caseSensitive,wholeWord,diacritics);
}
void EnsearchWidget::setFields(const QString & f) {
  m_search->searcher()->setFields(f);
}
void EnsearchWidget::setPadding(int sz) {
  m_search->searcher()->setPadding(sz);
}
