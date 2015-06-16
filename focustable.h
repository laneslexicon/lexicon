#ifndef __FOCUSTABLE_H__
#define __FOCUSTABLE_H__
#include <QTableWidget>
#include <QFocusEvent>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
class FocusTable : public QTableWidget {
    Q_OBJECT
 public:
    FocusTable(QWidget * parent = 0);
    public slots:
      void exportResults();
 protected:
   void focusInEvent(QFocusEvent *);
   void focusOutEvent(QFocusEvent *);
};
#endif
