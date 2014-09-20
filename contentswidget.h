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
#include <QSettings>
#include "QsLog.h"
#include "place.h"
#include "application.h"
#define ROOT_COLUMN 0
class ContentsWidget : public QTreeWidget {
  Q_OBJECT;
 public:
  ContentsWidget(QWidget * parent = 0);
  ~ContentsWidget();
  void loadContents();
  QString findNextRoot(const QString &);
  QString findPrevRoot(const QString &);
  Place getCurrentPlace();
  Place findNextPlace(const Place &);
  Place findPrevPlace(const Place &);
  QTreeWidgetItem * findPlace(const Place &) const;
  void ensurePlaceVisible(const Place & p, bool select = false);
  int addEntries(const QString & root,QTreeWidgetItem *);
  public slots:
    void nodeExpanded(QTreeWidgetItem * item);
    void nodeCollapsed(QTreeWidgetItem * item);
 protected:
  void focusInEvent(QFocusEvent *);
  void focusOutEvent(QFocusEvent *);
 private:
  void toggleExpand();
  void readSettings();
  bool m_debug;
  /// this is the background color of the select item when the window
  /// does not have focus
  QString m_backgroundColor;
  QSqlQuery m_entryQuery;
  QString m_moveUp;
  QString m_moveDown;
  QString m_expand;
  QFont m_itypeFont;
 protected:
  virtual void 	keyPressEvent(QKeyEvent * event);
 signals:
  void itemActivated(QTreeWidgetItem *,int /* not used */);
  void atEnd();
  void atStart();
};
#endif
