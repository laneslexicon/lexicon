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
    void writeSettings(const QString & fileName = QString());
    void readSettings(bool reload = false);
    bool isModified();
    void onSetFont();
    void onSetColor();
    void onHeadDialog();
    void onFullDialog();
    void onLocalDialog();
 private:
    /// full search
    QCheckBox * m_textDebug;
    QSpinBox  *  m_textFragmentSize;
    QCheckBox * m_textSummary;
    QCheckBox * m_textCurrentPage;
    QSpinBox  * m_textStep;
    QSpinBox  * m_textPageSize;
    QComboBox * m_textRepeat;  // default button when search fails
    QLineEdit * m_contextStyle;

    bool m_textWholeWord;
    bool m_textDiacritics;
    bool m_textRegex;
    bool m_textNewTab;
    bool m_textGoTab;
    bool m_textCase;
    bool m_textKeymap;
    /// head word search
    QCheckBox * m_headPhrase;
    QSpinBox  * m_headStep;
    QCheckBox * m_headVertical;
    QCheckBox * m_headFocusTable;
    QCheckBox * m_nodeinfoClose;
    QCheckBox * m_nodeinfoForce;
    bool m_headWholeWord;
    bool m_headDiacritics;
    bool m_headRegex;
    bool m_headNewTab;
    bool m_headGoTab;
    bool m_headKeymap;

    /// local search
    bool m_localKeymap;
    bool m_localWholeWord;
    bool m_localRegex;
    bool m_localDiacritics;
    bool m_ignoreCase;

    QCheckBox * m_localShowAll;


    QCheckBox * m_nodeNew;
    QCheckBox * m_nodeGo;
    QCheckBox * m_pageNew;
    QCheckBox * m_pageGo;
    QCheckBox * m_rootNew;
    QCheckBox * m_rootGo;
    QCheckBox * m_rootKeymap;

};
#endif
