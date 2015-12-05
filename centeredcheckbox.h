#ifndef _CENTEREDCHECKBOX_H_
#define  _CENTEREDCHECKBOX_H_
#include <QWidget>
#include <QCheckBox>
#include <QHBoxLayout>
class CenteredCheckBox : public QWidget {
  Q_OBJECT
 public:
  CenteredCheckBox(QWidget * parent = 0);
  void setRow(int);
  int row() const;
  void setChecked(bool v);
  bool isChecked() const;
 private:
  QCheckBox * m_box;
  int m_row;
 signals:
  void stateChanged(int);
};
#endif
