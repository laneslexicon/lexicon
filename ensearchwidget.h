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
#include <QDesktopWidget>
class TextSearchWidget;
class TextSearch;
class EnsearchWidget : public QWidget {
  Q_OBJECT

 public:
  EnsearchWidget(QWidget * parent = 0);
  QSize sizeHint() const;
  int search();
  void setDiacritics(const QString & v = QString());
  void setSearch(const QString & pattern,bool regex,bool caseSensitive,bool wholeWord,bool diacritics);
  void setFields(const QString &);
  void setPadding(int);
  void setNode(const QString & n);
  void focusTable();
  TextSearch * searcher();
 private:
  TextSearchWidget * m_search;
  QLabel * m_summary;
  QPushButton * m_quit;
  QProgressDialog * m_pd;
  QEventLoop m_ep;
  public slots:
    void recordsRead(int);
    void onExit();
    void cancelSearch();
 signals:
    void showNode(const QString &,bool);
    void printNode(const QString &);
    void statusMessage(const QString &);
};
#endif
