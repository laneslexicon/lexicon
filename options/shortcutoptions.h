#ifndef __SHORTCUTOPTIONS_H__
#define __SHORTCUTOPTIONS_H__
#include "optionswidget.h"
#include <QStringList>
class ShortcutOptions : public OptionsWidget {
  Q_OBJECT

 public:
  ShortcutOptions(QSettings * settings = 0,QWidget * parent = 0);
  public slots:
    void writeSettings();
    void readSettings();
    bool isModified();
 private:
    QMap<QString,QString> m_values;
    QTabWidget * m_tabs;
    void addTab(const QString &,const QStringList &);
};
#endif
