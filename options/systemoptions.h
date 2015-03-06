#ifndef __SYSTEMOPTIONS_H__
#define __SYSTEMOPTIONS_H__
#include "optionswidget.h"
#include <QFontDialog>
#include <QPushButton>
class SystemOptions : public OptionsWidget {
  Q_OBJECT

 public:
  SystemOptions(const QString & theme,QWidget * parent = 0);
  public slots:
    void writeSettings(const QString & fileName = QString());
    void readSettings();
    bool isModified();
    void onSetFont();
    void onSetColor();
 private:
    QCheckBox * m_linksActivate;
    QLineEdit * m_arabicFont;
    QLineEdit * m_color;
    QCheckBox * m_contentsLinked;
    QLineEdit * m_currentMap;
    QLineEdit * m_lexicon;
    QCheckBox * m_debug;
    QCheckBox * m_docked;
    QCheckBox * m_focusTab;
    // Icon size
    QCheckBox * m_mapsEnabled;
    QCheckBox * m_minimalInterface;
    // Navigation
    QLineEdit * m_nullMapName;
    QCheckBox * m_linksInCurrentTab;
    QCheckBox * m_restoreBookmarks;
    QCheckBox * m_restoreTabs;
    // Root mode
    QLineEdit * m_runData;
    // Save bookmarks ?
    QCheckBox * m_saveSettings;
    QCheckBox * m_saveTabs;
    QCheckBox * m_showInterfaceWarning;
    QLineEdit * m_css;
    QLineEdit * m_theme;
    QLineEdit * m_title;
    QCheckBox * m_toolbarText;
    QCheckBox * m_useNotes;

};
#endif
