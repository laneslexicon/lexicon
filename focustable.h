#ifndef __FOCUSTABLE_H__
#define __FOCUSTABLE_H__
#include <QTableWidget>
#include <QFocusEvent>
class FocusTable : public QTableWidget {
    Q_OBJECT
 public:
    FocusTable(QWidget * parent = 0);

 protected:
   void focusInEvent(QFocusEvent *);
   void focusOutEvent(QFocusEvent *);
};
#endif
