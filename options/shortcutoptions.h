#ifndef __SHORTCUTOPTIONS_H__
#define __SHORTCUTOPTIONS_H__
#include "optionswidget.h"
#include <QStringList>
class ShortcutOptions : public OptionsWidget {
  Q_OBJECT

 public:
  ShortcutOptions(const QString & theme,QWidget * parent = 0);
  public slots:
    void writeSettings(const QString & fileName = QString());
    void readSettings(bool reload = false);
    bool isModified();
 private:
    QMap<QString,QString> m_values;
    QMap<QString,QString> m_labels;
    QTabWidget * m_tabs;
    void addTab(const QString &,const QStringList &);
};
#endif
