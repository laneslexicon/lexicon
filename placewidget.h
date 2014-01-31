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
#include <QTextBrowser>
#include <QFile>
#include "place.h"
class PlaceWidget : public QWidget {
  Q_OBJECT

 public:
  PlaceWidget(QWidget * parent = 0);
  void setPlace(const Place &);
 protected:
  void paintEvent(QPaintEvent *);
 private:
  void readSettings();
  bool m_show;
  Place m_place;
  QFont m_rootFont;
  QFont m_wordFont;
  QTextBrowser * m_text;
  QString m_css;
  bool readCssFromFile(const QString & name);

  /*
  QLabel * m_label;
  QLabel * m_root;
  QLabel * m_word;
  QLabel * m_node;
  QLabel * m_page;
  */

};
#endif
