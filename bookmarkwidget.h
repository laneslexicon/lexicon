#ifndef __BOOKMARKWIDGET_H__
#define __BOOKMARKWIDGET_H__
#include <QListWidget>
#include <QKeyEvent>

#include <QMap>
#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QRegExp>
class Place;
class BookmarkWidget : public QDialog {
   Q_OBJECT
 public:
   BookmarkWidget(const QMap<QString,Place> & marks,QWidget * parent = 0);
   QString getSelected() { return m_mark;}
   public slots:
     void setPlace();
     public slots:
       void jump(QListWidgetItem *);
 private:
   QListWidget * m_list;
   QString  m_mark;
   bool eventFilter(QObject *,QEvent *);
};
#endif
