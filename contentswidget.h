#ifndef __CONTENTSWIDGET_H__
#define __CONTENTSWIDGET_H__
#include <QTreeWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <QHeaderView>
#include <QDebug>
#include <QFont>
#include <QKeyEvent>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include "QsLog.h"
#include "place.h"
class ContentsWidget : public QTreeWidget {
  Q_OBJECT;
 public:
  ContentsWidget(QWidget * parent = 0);
  void loadContents();
  QString findNextRoot(const QString &);
  QString findPrevRoot(const QString &);
  Place findNextPlace(const Place &);
  Place findPrevPlace(const Place &);
  void ensureVisible(const QString & root, int supp = 0,bool select = false);
  void ensurePlaceVisible(const Place & p, bool select = false);
 private:
  bool m_debug;
 protected:
  virtual void 	keyPressEvent(QKeyEvent * event);
 signals:
  void itemActivated(QTreeWidgetItem *,int /* not used */);
  void atEnd();
  void atStart();
};
#endif
