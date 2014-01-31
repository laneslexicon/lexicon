#ifndef __PLACEWIDGET_H__
#define __PLACEWIDGET_H__
#include <QtCore>
#include <QHBoxLayout>
#include <QWidget>
#include <QPaintEvent>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QLabel>
#include <QFont>
#include <QSettings>
#include "place.h"
class PlaceWidget : public QWidget {
  Q_OBJECT

 public:
  PlaceWidget(QWidget * parent = 0);
  void setPlace(const Place &);
  void setNode(const QString &);
  void setWord(const QString &);
  void setRoot(const QString &);
 protected:
  void paintEvent(QPaintEvent *);
 private:
  void readSettings();
  bool m_show;
  Place m_place;
  QFont m_rootFont;
  QFont m_wordFont;
  QLabel * m_label;
  QLabel * m_root;
  QLabel * m_word;
  QLabel * m_node;
  QLabel * m_page;

};
#endif
