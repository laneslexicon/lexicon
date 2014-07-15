#ifndef __LANESLEXICON_H__
#define __LANESLEXICON_H__
#include <QApplication>
#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QTreeWidgetItem>
#include <QGridLayout>
#include <QFormLayout>
#include <QToolBar>
#include <QStatusBar>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QStatusBar>
#include <QDebug>
#include <QSplitter>
#include <QLineEdit>
#include <QTextEdit>
#include <QStackedLayout>
#include <QSplitter>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSettings>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QTreeView>
#include <QSqlDatabase>
#include <QFile>
#include <QIcon>
#include <QLabel>
#include <QDialog>
#include <QKeySequenceEdit>
#include <QSettings>
#include <QDateTime>
#include <QShortcut>
#include <QSignalMapper>
#include <QMessageBox>
#include <QDockWidget>
#include <QInputDialog>
#include <QActionGroup>

#include <QUrl>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
class ArabicSearchDialog;
#include "application.h"
#include "inputmapper.h"
#include "contentswidget.h"
#include "graphicsentry.h"
#include "history.h"
#include "QsLog.h"
#include "place.h"
#include "bookmarkwidget.h"
#include "scripts.h"
#include "xsltsupport.h"
#include "menubar.h"
#include "helpviewer.h"
#include "notes.h"
#include "notebrowser.h"
#include "tabwidget.h"
#include "namespace.h"
class LanesLexicon : public QMainWindow
{
    Q_OBJECT

public:
    explicit LanesLexicon(QWidget *parent = 0);
    ~LanesLexicon();

    QSize sizeHint() const;
    void readSettings();
    void writeSettings();
    bool isOk() { return m_ok;}
    QString convertString(const QString &) const;
    QString getActiveKeymap() const;
    HistoryMaster * history();
    NoteMaster * notes();
    enum NavMode {ByRoot, ByPage };
    public slots:
    void gotoPlace(const Place &,int);
    int hasPlace(const Place & p,int searchtype,bool setFocus);
    void setStatus(const QString &);
    //    void saveNote(Note *);
    private slots:
      void testSlot();
      void showNoteBrowser();
      void shortcut(const QString &);
    void findNextRoot(const QString &);
    void findPrevRoot(const QString &);

    void moveNext(const Place &);
    void movePrevious(const Place &);
    void onCloseTab(int) ;
    void onGoToPage(const Place &);
    void on_actionExit();
    void on_actionTest();

    void on_actionNavNext();
    void on_actionNavPrev();
    void on_actionNavFirst();
    void on_actionNavLast();

    void on_actionNextRoot();
    void on_actionPrevRoot();
    void on_actionFirstRoot();
    void on_actionLastRoot();

    void searchForWord();
    void searchForPage();
    void searchForNode();
    void searchForRoot();
    void searchForEntry();

    void on_actionNextPage();
    void on_actionPrevPage();
    void on_actionFirstPage();
    void on_actionLastPage();
    void on_actionClearHistory();
    void onHistorySelection();
    void onKeymapChanged();
    void rootClicked(QTreeWidgetItem * , int);
    void entryActivated(QTreeWidgetItem * , int);
    void focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason);

    void rootChanged(const QString & root,const QString & node);
    void placeChanged(const Place &);
    void historyPositionChanged(int);
    void historyAddition();

    void bookmarkShortcut(const QString &);
    void bookmarkAdd();
    void bookmarkAdd(const QString & id,const Place & p);

    void onNavModeChanged();
    void on_actionDocs();
    void bookmarkRebuildMenu();

    void currentTabChanged(int);
    /// these apply to the current graphicsentry
    void pageZoomIn();
    void pageZoomOut();
    void pageWiden();
    void pageNarrow();
    void pagePrint();
    void pageSearch();
    void pageClear();
    void syncContents();
    /// from FullSearchWidget
    void showSearchNode(const QString &);
    void convertToEntry();
 protected:
    void closeEvent(QCloseEvent *);
private:
    void cleanup();
    void search(int searchType,ArabicSearchDialog *,const QString &);
    Place showPlace(const Place &,int);
    QAction * createIconAction(const QString imgdir,const QString & iconfile,const QString & text);
    /// look through all tabs for the given node, -1 if not found, else tab index
    int searchTabs(const QString & node);
    int m_defaultSearchOptions;
    QString m_configFile;
    QString m_interface;    // "default","minimal"
    QPrinter m_printer;
    ///
    void setupInterface();
    /// 0 - root mode, 1 - page mode
    int m_navMode;
    QLabel * m_navModeIndicator;
    QLabel * m_placeIndicator;
    QToolButton * m_keymapsButton;
    QAction * m_keymapsAction;
    void updateStatusBar();
    void updateMenu();
    bool m_ok;
    bool m_docked;          // whether to use docked widget
    bool m_valgrind;
    bool m_useNotes;

    bool m_treeKeepsFocus;

    QDockWidget * m_treeDock;
    QFont arFont;

    QString m_firstRoot;
    QString m_lastRoot;
    int m_firstPage;
    int m_lastPage;

    /// set by readSettings
    InputMapper * m_mapper;
    QString m_activeMap;
    QString m_dbName;
    QString m_notesDbName;
    QString m_historyDbName;
    bool m_historyEnabled;
    bool m_saveTabs;
    bool m_restoreTabs;
    bool m_searchNewTab;
    bool m_searchSwitchTab;
    QString m_navigationMode;
    void getFirstAndLast();
    void restoreTabs();
    QSignalMapper * m_signalMapper;

    // bookmarks
    QSignalMapper * m_bookmarkMap;
    QMap<QString, Place> m_bookmarks;
    bool m_saveBookmarks;
    bool m_restoreBookmarks;
    void restoreBookmarks();
    void setupBookmarkShortcuts();
    void bookmarkJump(const QString & id);
    QAction * m_bookmarkListAction;
    QAction * m_bookmarkAddAction;
    QAction * m_bookmarkJumpAction;
    QAction * m_bookmarkClearAction;
    QAction * m_bookmarkRevertAction;
    QAction * m_docAction;
    QMenu   * m_bookmarkMenu;
    QToolButton * m_bookmarkBtn;
    void addBookmarkMenuItem(const QString & id);
    void bookmarkClear();
    AppMenu * m_mainmenu;
    bool m_revertEnabled;
    QSize m_toolbarIconSize;

    void setupShortcuts();
    int m_historyPos;
    QMenu * m_historyMenu;
    void setSignals(GraphicsEntry *);
    void loadStyleSheet();
    void createActions();
    void createToolBar();
    void setupHistory(int startPos = -1);
    void createMenus();
    void createStatusBar();

    void enableKeymaps(bool);
    bool m_keymapsEnabled;
    bool eventFilter(QObject * target, QEvent *);
    bool openDatabase(const QString &);


    Place m_place;
    Place getCurrentPlace();
    ContentsWidget * m_tree;

    QSqlDatabase m_db;
    QSqlDatabase m_notesDb;
    TabWidget * m_tabs;
    QMenu * m_fileMenu;
    // actions
    QAction * m_exitAction;
    QAction * m_testAction;

    QAction * m_historyAction;
    QAction * m_bookmarkAction;
    QAction * m_navigationAction;

    QAction * m_searchAction;
    QToolButton * m_searchButton;
    QMenu * m_searchMenu;
    QAction * m_searchWordAction;
    QAction * m_searchPageAction;
    QAction * m_searchRootAction;
    QAction * m_searchNodeAction;
    /// Navigation
    QAction * m_navNextAction;
    QAction * m_navPrevAction;
    QAction * m_navFirstAction;
    QAction * m_navLastAction;
    QLabel * m_navText;
    QToolButton * m_navBtn;
    QMenu * m_navMenu;
    QAction * m_navModeRootAction;
    QAction * m_navModePageAction;

    // history
    QAction * m_clearHistoryAction;
    QToolButton * m_hBackwardBtn;
    HistoryMaster * m_history;

    // controls for the current graphics entry
    QAction * m_zoomInAction;
    QAction * m_zoomOutAction;
    QAction * m_widenAction;
    QAction * m_narrowAction;
    QAction * m_printAction;
    QAction * m_localSearchAction;
    // clear the highlights
    QAction * m_clearAction;
    // convert HeadSearch to GraphicsEntry
    QAction * m_convertToEntryAction;
    /// Notes
    NoteMaster * m_notes;
    ///
    int getSearchCount();
 signals:
   void nodeActivated(const QString & node,const QString & word);
};
#endif
