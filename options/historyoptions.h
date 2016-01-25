#ifndef __HISTORYOPTIONS_H__
#define __HISTORYOPTIONS_H__
#include "optionswidget.h"
#include <QFontDialog>
#include <QPushButton>
class HistoryOptions : public OptionsWidget {
  Q_OBJECT

 public:
  HistoryOptions(const QString & theme,QWidget * parent = 0);
  public slots:
    void writeSettings(const QString & fileName = QString());
    void readSettings();
    bool isModified();
    void onSetFont();
    void onSetColor();
 private:
    QCheckBox * m_enabled;
    QLineEdit * m_database;
    QLineEdit * m_menuFormat;
    QLineEdit   * m_menuFont;
    QCheckBox * m_newTab;
    QCheckBox * m_goTab;
    QLineEdit  * m_duplicateDepth;
    QLineEdit  * m_size;
    QLineEdit   * m_listFont;
    QPushButton * m_listButton;
    QPushButton * m_menuButton;
};
#endif
