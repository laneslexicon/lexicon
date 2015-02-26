#ifndef __FINDOPTIONS_H__
#define __FINDOPTIONS_H__
#include "optionswidget.h"
#include <QFontDialog>
#include <QPushButton>
class FindOptions : public OptionsWidget {
  Q_OBJECT

 public:
  FindOptions(const QString & theme,QWidget * parent = 0);
  public slots:
    void writeSettings();
    void readSettings();
    bool isModified();
    void onSetFont();
    void onSetColor();
 private:
    /// full search
    QCheckBox * m_fullDebug;
    QSpinBox *  m_fullFragmentSize;
    QCheckBox * m_fullIncludeHeads;
    QCheckBox * m_fullOneRow;
    QSpinBox * m_fullStep;
    QLineEdit * m_fullViewerWidth;
    QLineEdit * m_fullViewerHeight;
    QCheckBox * m_fullWholeWord;
    QLineEdit * m_fullXslt;
    QLineEdit * m_fullHeadColor;
    QLineEdit * m_fullHeadText;
    /// head word search
    QCheckBox * m_headDebug;
    QSpinBox * m_headStep;
    QCheckBox * m_headVertical;
    QCheckBox * m_headFocusTable;
    /// local search
    QCheckBox * m_localForce;
    QCheckBox * m_localIgnore;
    QCheckBox * m_localWholeWord;
    QCheckBox * m_localRegex;
    QCheckBox * m_localShowAll;

};
#endif
