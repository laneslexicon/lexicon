#ifndef __FOCUSTABLE_H__
#define __FOCUSTABLE_H__
#include <QTableWidget>
#include <QFocusEvent>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
class CheckBoxTableItem : public QWidget {
  Q_OBJECT
 public:
  CheckBoxTableItem(QWidget * parent = 0);
  bool isChecked() const;
 private:
  QCheckBox * m_checkbox;

};
class FocusTable : public QTableWidget {
    Q_OBJECT
 public:
    FocusTable(QWidget * parent = 0);
    public slots:
      void exportResults();
 protected:
   void focusInEvent(QFocusEvent *);
   void focusOutEvent(QFocusEvent *);
 private:
   QString removeSpan(const QString &);

};
#endif
