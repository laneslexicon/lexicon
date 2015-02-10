#ifndef __OPTIONSTABWIDGET_H__
#define __OPTIONSTABWIDGET_H__
#include <QtCore>
#include <QDialog>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
class RootsOptions;

class OptionsTabWidget : public QWidget {
  Q_OBJECT

 public:
  OptionsTabWidget(const QString & theme,QWidget * parent = 0);
  ~OptionsTabWidget();
  public slots:
    void valueChanged(bool);
    void saveChanges();
    void resetChanges();
    void applyChanges();
    void currentChanged(int);
    void lock(bool);
 private:
    void setup();
    void writeSettings();
    void enableButtons();
    void setApplyReset(bool);
    QDialogButtonBox * m_buttons;
    QTabWidget * m_tabs;
};
#endif
