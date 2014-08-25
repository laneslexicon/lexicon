#ifndef __SHORTCUTOPTIONS_H__
#define __SHORTCUTOPTIONS_H__
#include "optionswidget.h"
class ShortcutOptions : public OptionsWidget {
  Q_OBJECT

 public:
  ShortcutOptions(QSettings * settings = 0,QWidget * parent = 0);
  public slots:
    void writeSettings();
    void readSettings();
    bool isModified();
 private:
    QKeySequenceEdit * m_bookmarkAdd;
    QKeySequenceEdit * m_collapseAll;
    QKeySequenceEdit * m_collapseLetter;
    QKeySequenceEdit * m_deleteTab;
    QKeySequenceEdit * m_focusContents;
    QKeySequenceEdit * m_focusTree;
    QKeySequenceEdit * m_goTab;
    QKeySequenceEdit * m_historyBack;
    QKeySequenceEdit * m_historyNext;
    QKeySequenceEdit * m_keymapsEnable;
    QKeySequenceEdit * m_keymapsDisable;
    QKeySequenceEdit * m_localSearch;
    QKeySequenceEdit * m_localSearchClear;
    QKeySequenceEdit * m_localSearchFind;
    QKeySequenceEdit * m_localSearchNext;
    QKeySequenceEdit * m_localSearchShow;
    QKeySequenceEdit * m_navFirst;
    QKeySequenceEdit * m_navNext;
    QKeySequenceEdit * m_navBack;
    QKeySequenceEdit * m_navLast;
    QKeySequenceEdit * m_quit;
    //    QKeySequenceEdit * m_searchDelete;
    //    QKeySequenceEdit * m_searchEntry;
    QKeySequenceEdit * m_searchHead;
    QKeySequenceEdit * m_searchNode;
    QKeySequenceEdit * m_searchRoot;
    QKeySequenceEdit * m_searchWord;
    QKeySequenceEdit * m_searchPage;
    QKeySequenceEdit * m_selectAll;
    QKeySequenceEdit * m_selectEntry;
    QKeySequenceEdit * m_showNotes;
    QKeySequenceEdit * m_sync;
    QKeySequenceEdit * m_interface;


};
#endif
