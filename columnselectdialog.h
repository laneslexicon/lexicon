#ifndef _COLUMNSELECT_H_
#define  _COLUMNSELECT_H_
#include <QMap>
#include <QWidget>
#include <QDialog>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
class ColumnSelectDialog : public QDialog {
  Q_OBJECT
 public:
  ColumnSelectDialog(QMap<int,QString> cols,QWidget * parent = 0);
  void setState(QList<bool>);
  QList<bool> state() const;
 private:
  QList<QCheckBox *> m_cols;
  QGridLayout * m_grid;
};
#endif
