#ifndef __ENSEARCHWIDGET_H__
#define __ENSEARCHWIDGET_H__
#include <QWidget>
#include <QVBoxLayout>
#include <QDebug>
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
 private:
  TextSearchWidget * m_search;
  public slots:
    void recordsRead(int);
};
#endif
