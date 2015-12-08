#ifndef __SYSTEMOPTIONS_H__
#define __SYSTEMOPTIONS_H__
#include "optionswidget.h"
#include <QFontDialog>
#include <QPushButton>
#include <QFileDialog>
#include <QRadioButton>
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
    void onSetDatabase();
    void onSetCss();
    void onSetNotesDatabase();
    void onSetHistoryDatabase();
    void onOfflineLocation();
 private:
    QCheckBox * m_contentsLinked;
    QLineEdit * m_lexicon;
    QCheckBox * m_debugOption;
    QCheckBox * m_docked;
    QLineEdit * m_focusTab;
    QCheckBox * m_rootNavigation;
    // Icon size
    QCheckBox * m_minimalInterface;
    // Navigation
    QCheckBox * m_linksInCurrentTab;
    QCheckBox * m_restoreBookmarks;
    QCheckBox * m_restoreTabs;
    // Root mode
    QDateTimeEdit * m_runDate;
    // Save bookmarks ?
    QCheckBox * m_saveSettings;
    QCheckBox * m_saveTabs;
    QCheckBox * m_showInterfaceWarning;
    QLineEdit * m_css;
    QLineEdit * m_historyDb;
    QCheckBox * m_history;
    QComboBox * m_theme;
    QComboBox * m_keyboard;
    QLineEdit * m_title;
    QCheckBox * m_toolbarText;
    QCheckBox * m_useNotes;
    QLineEdit * m_notesDb;
    QCheckBox * m_splashScreen;

    QLineEdit * m_onlineUrl;
    QLineEdit * m_onlineCurrentPage;
    QLineEdit * m_offlineLocation;
    QLineEdit * m_offlineCurrentPage;
    QCheckBox * m_localDocs;

    QRadioButton * m_insertNewTab;
    QRadioButton * m_appendNewTab;
};
#endif
