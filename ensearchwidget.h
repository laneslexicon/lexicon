#ifndef __ENSEARCHWIDGET_H__
#define __ENSEARCHWIDGET_H__
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QEventLoop>
#include <QProgressDialog>
class TextSearchWidget;
class TextSearch;
class EnsearchWidget : public QWidget {
  Q_OBJECT

 public:
  EnsearchWidget(int rows = 10,QWidget * parent = 0);
  QSize sizeHint() const;
  int search();
  void setSearch(const QString & pattern,bool regex,bool caseSensitive,bool wholeWord,bool diacritics);
  void setFields(const QString &);
  void setPadding(int);
  void setNode(const QString & n);
  TextSearch * searcher();
 private:
  int m_pageSize;
  TextSearchWidget * m_search;
  QLabel * m_summary;
  QPushButton * m_quit;
  QProgressDialog * m_pd;
  QEventLoop m_ep;
  QPair<int,int> m_pageCounts;  // first is summary,second is full
  public slots:
    void recordsRead(int);
    void onExit();
    void cancelSearch();
};
#endif
