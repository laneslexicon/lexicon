#ifndef __LOGOPTIONS_H__
#define __LOGOPTIONS_H__
#include "optionswidget.h"
#include <QFontDialog>
#include <QPushButton>
class LogOptions : public OptionsWidget {
  Q_OBJECT

 public:
  LogOptions(const QString & theme,QWidget * parent = 0);
  public slots:
    void writeSettings(const QString & fileName = QString());
    void readSettings();
    bool isModified();
    void onSetFont();
    void onSetColor();
 private:
    QLineEdit * m_file;
    QLineEdit * m_maxSize;
    QSpinBox  * m_level;
    QSpinBox * m_archive;
    QLineEdit * m_maxLines;
    QLineEdit  * m_interval;
    QCheckBox * m_rotate;
    QLineEdit * m_warning;
    QLineEdit * m_error;
    QLineEdit * m_debug;
    QLineEdit * m_info;
};
#endif
