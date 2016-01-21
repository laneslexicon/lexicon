#ifndef _TEXTWIDTHDIALOG_H__
#define _TEXTWIDTHDIALOG_H__
#include <QDialog>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QMessageBox>
class TextwidthDialog : public QDialog {
  Q_OBJECT
 public:
  TextwidthDialog(int width,QWidget * parent = 0);
  int value() const;
  bool applyAll() const;
 private:
  QSpinBox * m_spinner;
  QCheckBox * m_applyTextwidth;
 private slots:
  void applyChanged(int);
  void onFromPage();
  void onToPage();
};
#endif
