#ifndef __BOOKMARKWIDGET_H__
#define __BOOKMARKWIDGET_H__
#include <QListWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QKeyEvent>
#include <QPushButton>
#include <QMap>
#include <QDialog>
#include <QFont>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QRegExp>
class Place;
class BookmarkWidget : public QDialog {
  Q_OBJECT
 public:
  BookmarkWidget(const QMap<QString,Place> & marks,QWidget * parent = 0);
  QString getSelected() { return m_mark;}
  QSize sizeHint() const;
  public slots:
    void setPlace();
    void jump(QTableWidgetItem *);
 private:
  void readSettings();
  QTableWidget * m_list;
  QString  m_mark;
  bool eventFilter(QObject *,QEvent *);
  QFont m_arFont;
};
#endif
