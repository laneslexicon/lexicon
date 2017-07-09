#ifndef __SYSTEMOPTIONS_H__
#define __SYSTEMOPTIONS_H__
#include "optionswidget.h"
#include <QFontDialog>
#include <QPushButton>
#include <QFileDialog>
#include <QRadioButton>
#include <QStyleFactory>
class SystemOptions : public OptionsWidget {
  Q_OBJECT

 public:
  SystemOptions(const QString & theme,QWidget * parent = 0);
  public slots:
    void writeSettings(const QString & fileName = QString());
    void readSettings(bool reload = false);
    bool isModified();
    void onSetFont();
    void onSetColor();
    void onSetDatabase();
    void onSetCss();
    void onSetNotesDatabase();
    void onSetHistoryDatabase();
    void onOfflineLocation();
    void onShowSplash();
 private:
    QCheckBox * m_contentsLinked;
    QLineEdit * m_lexicon;
    QCheckBox * m_docked;
    QCheckBox * m_importCheck;
    QCheckBox * m_importShow;
    QLineEdit * m_interval;
    QCheckBox * m_rootNavigation;
    // Icon size
    QCheckBox * m_minimalInterface;
    // Navigation
    QCheckBox * m_linksInCurrentTab;
    QCheckBox * m_restoreBookmarks;
    QCheckBox * m_restoreTabs;
    // Root mode
    QDateTimeEdit * m_runDate;
    QComboBox * m_qtStyle;
    // Save bookmarks ?
    QCheckBox * m_saveSettings;
    QCheckBox * m_saveTabs;
    QCheckBox * m_allowDuplicates;
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
    QLineEdit * m_splashDuration;
    QCheckBox * m_optionsWarning;
    QLineEdit * m_onlineUrl;
    QLineEdit * m_onlineCurrentPage;
    QLineEdit * m_offlineLocation;
    QLineEdit * m_offlineCurrentPage;
    QCheckBox * m_localDocs;
    QCheckBox * m_closeModal;
    QRadioButton * m_insertNewTab;
    QRadioButton * m_appendNewTab;

    bool m_allowNavMode;
};
#endif
