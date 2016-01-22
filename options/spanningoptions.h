#ifndef _SPANNINGOPTIONS_H_
#define _SPANNINGOPTIONS_H_
#include "optionswidget.h"
#include "definedsettings.h"
#include "QsLog.h"
class SpanningOptions : public OptionsWidget {
  Q_OBJECT

 public:
  SpanningOptions(const QString & theme,QWidget * parent = 0);
  public slots:
    void writeSettings(const QString & fileName = QString());
    void readSettings();
    bool isModified();
    void onSetFont();
 private:
  QMap<QString,QString> m_names;
  QMap<QString,QLineEdit *> m_edits;
  QStringList m_keys;
};
#endif
