#ifndef __OPTIONSDIALOG_H__
#define __OPTIONSDIALOG_H__
#include <QtCore>
#include <QDialog>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
class RootsOptions;

class OptionsDialog : public QDialog {
  Q_OBJECT

 public:
  OptionsDialog(QWidget * parent = 0);
  public slots:
    void valueChanged(bool);
    void saveChanges();
    void resetChanges();
    void applyChanges();
 private:
    void enableButtons();
  QDialogButtonBox * m_buttons;
  QTabWidget * m_tabs;
};
#endif
