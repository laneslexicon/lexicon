#ifndef __HTMLDELEGATE_H__
#define __HTMLDELEGATE_H__
#include <QtGui>
#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QTableWidget>
class HtmlDelegate : public QStyledItemDelegate {
  Q_OBJECT
 public:
  HtmlDelegate(QObject * parent = 0);
  ~HtmlDelegate();
  void setStyleSheet(const QString &);
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
 private:
  QString m_stylesheet;
};
#endif
