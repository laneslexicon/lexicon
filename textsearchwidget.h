#ifndef __TEXTSEARCHWIDGET_H__
#define __TEXTSEARCHWIDGET_H__
#include <QWidget>
#include <QVBoxLayout>
class TextSearch;
class ColumnarTableWidget;
class Place;
class TextSearchWidget : public QWidget {
 public:
  TextSearchWidget(bool summary,QWidget * parent = 0);
  void load(const TextSearch &);
 private:
  bool m_summary;
  QString m_contextStyle;
  ColumnarTableWidget * m_results;
  int addRow(const Place & p, const QString & text,int pos);

};
#endif
