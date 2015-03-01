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
    void onHeadDialog();
    void onFullDialog();
 private:
    /// full search
    QCheckBox * m_fullDebug;
    QSpinBox  *  m_fullFragmentSize;
    QCheckBox * m_fullIncludeHeads;
    QCheckBox * m_fullOneRow;
    QSpinBox  * m_fullStep;
    //    QLineEdit * m_fullViewerWidth;
    //    QLineEdit * m_fullViewerHeight;
    //    QLineEdit * m_fullXslt;
    QLineEdit * m_fullHeadColor;
    QLineEdit * m_fullHeadText;

    bool m_fullWholeWord;
    bool m_fullDiacritics;
    bool m_fullRegex;
    bool m_fullNewTab;
    bool m_fullGoTab;
    /// head word search
    QCheckBox * m_headDebug;
    QSpinBox  * m_headStep;
    QCheckBox * m_headVertical;
    QCheckBox * m_headFocusTable;
    bool m_headWholeWord;
    bool m_headDiacritics;
    bool m_headRegex;
    bool m_headNewTab;
    bool m_headGoTab;
    /// local search
    QCheckBox * m_localForce;
    QCheckBox * m_localIgnore;
    QCheckBox * m_localWholeWord;
    QCheckBox * m_localRegex;
    QCheckBox * m_localShowAll;
    QSize       m_viewerSize;
    QPoint      m_viewerPosition;

};
#endif
