#ifndef __ROOTSOPTIONS_H__
#define __ROOTSOPTIONS_H__
#include "optionswidget.h"
#include <QFontDialog>
#include <QPushButton>
class RootsOptions : public OptionsWidget {
  Q_OBJECT

 public:
  RootsOptions(const QString & theme,QWidget * parent = 0);
  public slots:
    void writeSettings(const QString & fileName = QString());
    void readSettings(bool reload = false);
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
    QCheckBox * m_showEntryColumn;
    QCheckBox * m_showHeadColumn;
    QCheckBox * m_showNodeColumn;
    QCheckBox * m_romanItypes;
    QCheckBox * m_singleClick;
    QCheckBox * m_doubleClick;
    QCheckBox * m_debug;
    QCheckBox * m_activateTab;
    QCheckBox * m_newTab;
    QCheckBox * m_toggleButton;
};
#endif
