#ifndef __BOOKMARKWIDGET_H__
#define __BOOKMARKWIDGET_H__
#include <QListWidget>
#include "place.h"
#include <QMap>
#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QRegExp>
class BookmarkWidget : public QDialog {
   Q_OBJECT
 public:
   BookmarkWidget(const QMap<QString,Place> & marks,QWidget * parent = 0);
   QString getSelected() { return m_mark;}
   public slots:
     void setPlace();
 private:
   QListWidget * m_list;
   QString  m_mark;
};
#endif
