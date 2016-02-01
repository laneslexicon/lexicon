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
    void readSettings(bool reload = false);
    bool isModified();
    void onSetFont();
    void onSetColor();
    void onSetFile();
    void onSetDirectory();
    void onSetIcon(const QString & sid,const QString & filename);
 private:
    QHBoxLayout * addLine(QLineEdit *, const QString &);
    QLineEdit * m_about;
    QLineEdit * m_directory;
    QLineEdit * m_back;
    QLineEdit * m_background; // new
    QLineEdit * m_bookmarks;
    QLineEdit * m_clear;
    QLineEdit * m_collapse;  // new
    QLineEdit * m_docs;
    QLineEdit * m_expand;   //new
    QLineEdit * m_exit;
    QLineEdit * m_first;
    QLineEdit * m_history;
    QLineEdit * m_insertLink;
    QLineEdit * m_keymaps;
    QLineEdit * m_keymapsDisabled;
    QLineEdit * m_last;
    QLineEdit * m_link;
    QLineEdit * m_list;  // new
    QLineEdit * m_load;  // new

    QLineEdit * m_localSearch;
    QLineEdit * m_localSearchNext;
    QLineEdit * m_logs;
    QLineEdit * m_narrow;
    QLineEdit * m_newTab; // new
    QLineEdit * m_next;
    QLineEdit * m_notes;
    QLineEdit * m_preferences;
    QLineEdit * m_print;
    QLineEdit * m_search;
    QLineEdit * m_syncLeft;
    QLineEdit * m_syncRight;
    QLineEdit * m_unlink;
    QLineEdit * m_widen;
    QLineEdit * m_zoom;
    QLineEdit * m_zoomIn;
    QLineEdit * m_zoomOut;

    void setIconFromField(const QLineEdit * w,const QString & );
};
#endif
