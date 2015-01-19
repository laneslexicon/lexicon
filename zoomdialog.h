#ifndef _ZOOMDIALOG_H__
#define _ZOOMDIALOG_H__
#include <QDialog>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QVBoxLayout>
class ZoomDialog : public QDialog {
  Q_OBJECT
 public:
  ZoomDialog(qreal scale,QWidget * parent = 0);
  double value();
 private:
  QDoubleSpinBox * m_spinner;
};
#endif
