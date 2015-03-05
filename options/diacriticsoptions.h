#ifndef __DIACRITICSSOPTIONS_H__
#define __DIACRITICSSOPTIONS_H__
#include "optionswidget.h"
#include <QGridLayout>
#include <QTextEdit>
#include <QTextStream>
class DiacriticsOptions : public OptionsWidget {
  Q_OBJECT

 public:
  DiacriticsOptions(const QString &,QWidget * parent = 0);
  public slots:
    void writeSettings(const QString & fileName = QString());
    void readSettings();
    bool isModified();
 private:
    QMap<int,QString> m_chars;
};
#endif
