#ifndef __TABLISTDIALOG_H__
#define __TABLISTDIALOG_H__
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
#include <QCheckBox>
#include <QDialogButtonBox>
class Place;
class QShowEvent;
class ColumnarTableWidget;

class TabListDialog : public QDialog {
  Q_OBJECT
 public:
  TabListDialog(QWidget * parent = 0);
  ~TabListDialog();
 public slots:
   void switchTab(int,int);
 protected:
  void showEvent(QShowEvent *);
  QSize sizeHint() const;
 private:
  void loadTabs();
    ColumnarTableWidget * m_list;
    bool eventFilter(QObject *,QEvent *);
};
#endif
