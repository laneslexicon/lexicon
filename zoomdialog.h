#ifndef _ZOOMDIALOG_H__
#define _ZOOMDIALOG_H__
#include <QDialog>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QMessageBox>
class ZoomDialog : public QDialog {
  Q_OBJECT
 public:
  ZoomDialog(qreal scale,QWidget * parent = 0);
  double value() const;
  bool applyAll() const;
 private:
  QDoubleSpinBox * m_spinner;
  QCheckBox * m_applyZoom;
 private slots:
  void applyChanged(int);
  void onFromPage();
  void onToPage();
 signals:
    void valueChanged(double);
};
#endif
