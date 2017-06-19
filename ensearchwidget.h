#ifndef __ENSEARCHWIDGET_H__
#define __ENSEARCHWIDGET_H__
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QPushButton>
class TextSearchWidget;
class TextSearch;
class EnsearchWidget : public QWidget {
  Q_OBJECT

 public:
  EnsearchWidget(QWidget * parent = 0);
  QSize sizeHint() const;
  void search();
  void setSearch(const QString & pattern,bool regex,bool caseSensitive,bool wholeWord,bool diacritics);
  void setFields(const QString &);
  void setPadding(int);
  void setNode(const QString & n);
 private:
  TextSearchWidget * m_search;
  QLabel * m_summary;
  QPushButton * m_quit;
  public slots:
    void recordsRead(int);
    void onExit();
};
#endif
