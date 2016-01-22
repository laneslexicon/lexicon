#ifndef __ENTRYOPTIONS_H__
#define __ENTRYOPTIONS_H__
#include "optionswidget.h"
#include <QFontDialog>
#include <QPushButton>
class DebugOptionsDialog : public QDialog {
  Q_OBJECT
 public:
  DebugOptionsDialog(bool,bool,bool,const QString &,QWidget * parent = 0);
  QCheckBox * m_xml;
  QCheckBox * m_html;
  QCheckBox * m_outputHtml;
  QLineEdit *  m_path;
  private slots:
    void onPathSelect();
};

class EntryOptions : public OptionsWidget {
  Q_OBJECT

 public:
  EntryOptions(const QString & theme,QWidget * parent = 0);
  public slots:
    void writeSettings(const QString & fileName = QString());
    void readSettings();
    bool isModified();
    void onSetFont();
    void onSetColor();
    void onAdvanced();
 private:
    QLineEdit * m_back;
    QLineEdit * m_css;
    QLineEdit * m_printCss;
    QLineEdit * m_entryXslt;
    QCheckBox * m_nodeinfoClose;
    QLineEdit * m_clean;
    QCheckBox * m_debug;
    QLineEdit * m_find;
    QLineEdit * m_findNext;
    QLineEdit * m_forward;
    QLineEdit * m_home;
    QLineEdit * m_margin;
    QLineEdit * m_mark;
    QLineEdit * m_focusUp;
    QLineEdit * m_focusDown;
    QLineEdit * m_narrow;
    QLineEdit * m_reload;
    QCheckBox * m_saveHtml;
    QCheckBox * m_saveXml;
    QCheckBox * m_saveOutputHtml;
    QLineEdit * m_outputPath;
    QCheckBox * m_offPage;
    QLineEdit * m_show;
    QCheckBox * m_showLinkWarning;
    QLineEdit * m_widenStep;
    QLineEdit * m_scaleStep;
    QLineEdit * m_highlightColor;
    QLineEdit * m_textWidth;
    QLineEdit * m_widen;
    QDoubleSpinBox * m_zoom;
    QLineEdit * m_zoomIn;
    QLineEdit * m_zoomOut;
};
#endif
