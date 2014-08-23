#ifndef __ROOTSOPTIONS_H__
#define __ROOTSOPTIONS_H__
#include "optionswidget.h"
class RootsOptions : public OptionsWidget {
  Q_OBJECT

 public:
  RootsOptions(QSettings * settings = 0,QWidget * parent = 0);
  public slots:
    void writeSettings();
    void readSettings();
    bool isModified();
 private:

    //    QKeySequenceEdit * m_collapseAll;
    //    QKeySequenceEdit * m_collapseLetter;
    //    QKeySequenceEdit * m_syncToContents;
    //    QKeySequenceEdit * m_focusToContents;
    QLineEdit * m_moveDown;
    QLineEdit * m_moveUp;
    QLineEdit * m_expand;
    QCheckBox * m_singleClick;
    QCheckBox * m_doubleClick;
    QCheckBox * m_debug;
};
#endif
