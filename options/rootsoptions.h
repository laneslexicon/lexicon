#ifndef __ROOTSOPTIONS_H__
#define __ROOTSOPTIONS_H__
#include "optionswidget.h"
#include <QFontDialog>
#include <QPushButton>
class RootsOptions : public OptionsWidget {
  Q_OBJECT

 public:
  RootsOptions(QWidget * parent = 0);
  public slots:
    void writeSettings();
    void readSettings();
    bool isModified();
    void onSetFont();
 private:
    QPushButton * m_standardButton;
    QPushButton * m_arabicButton;
    QLineEdit * m_moveDown;
    QLineEdit * m_moveUp;
    QLineEdit * m_expand;
    QLineEdit * m_standardFont;
    QLineEdit * m_arabicFont;
    QCheckBox * m_singleClick;
    QCheckBox * m_doubleClick;
    QCheckBox * m_debug;
};
#endif
