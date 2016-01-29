#ifndef __BOOKMARKOPTIONS_H__
#define __BOOKMARKOPTIONS_H__
#include "optionswidget.h"
#include <QFontDialog>
#include <QPushButton>
class BookmarkOptions : public OptionsWidget {
  Q_OBJECT

 public:
  BookmarkOptions(const QString & theme,QWidget * parent = 0);
  public slots:
    void writeSettings(const QString & fileName = QString());
    void readSettings(bool reload = false);
    bool isModified();
    void onSetFont();
    void onSetColor();
 private:
    QLineEdit * m_id;
    QLineEdit * m_format;
    QCheckBox * m_newTab;
    QCheckBox * m_goTab;
    QKeySequenceEdit * m_add;
    QKeySequenceEdit * m_input;
    QKeySequenceEdit * m_list;
    QKeySequenceEdit * m_jump;
    QKeySequenceEdit * m_revert;
    QKeySequenceEdit * m_clear;
};
#endif
