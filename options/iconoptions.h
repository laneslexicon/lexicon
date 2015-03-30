#ifndef __ICONOPTIONS_H__
#define __ICONOPTIONS_H__
#include "optionswidget.h"
#include <QFontDialog>
#include <QPushButton>
class IconOptions : public OptionsWidget {
  Q_OBJECT

 public:
  IconOptions(const QString & theme,QWidget * parent = 0);
  public slots:
    void writeSettings(const QString & fileName = QString());
    void readSettings();
    bool isModified();
    void onSetFont();
    void onSetColor();
    void onSetFile();
    void onSetIcon(const QString & sid,const QString & filename);
 private:
    QLineEdit * m_about;
    QLineEdit * m_directory;
    QLineEdit * m_back;
    QString m_imageDirectory;
    void setIconFromField(const QLineEdit * w,const QString & );
};
#endif
