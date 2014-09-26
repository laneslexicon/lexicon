#ifndef __LANESLEXICON_H__
#define __LANESLEXICON_H__
#include <QApplication>
#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QTreeWidgetItem>
#include <QGridLayout>
#include <QFormLayout>
#include <QComboBox>
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
#include "place.h"
#include "searchoptionswidget.h"
#ifndef qStrip
#define qStrip qDebug()
#endif

class ArabicSearchDialog;
class NoteMaster;
class HistoryMaster;
class Place;
class ArabicSearchDialog;
class AppMenu;
class InputMapper;
class GraphicsEntry;
class ContentsWidget;
class TabWidget;
class EntryLayoutDialog;
class LogViewer;
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
    public slots:
    void gotoPlace(const Place &,int);
    int hasPlace(const Place & p,int searchtype,bool setFocus);
    void setStatus(const QString &);
    //    void saveNote(Note *);
    private slots:
      void onLinkChanged();
      void testSlot();
      void sync();
      void showNoteBrowser();
      void shortcut(const QString &);
      void deleteSearch();
      void localSearch();
      void localSearchNext();
      void localSearchClear();
      void localSearchShow();
      void tabsChanged();

      void reloadEntry(const QString &,const QString &);
      void revertEntry();
    void findNextRoot(const QString &);
    void findPrevRoot(const QString &);

    void moveNext(const Place &);
    void movePrevious(const Place &);
    void onCloseTab(int) ;
    void onGoToPage(const Place &);
    void onExit();
    void onTest();

    void onNavigationChanged(int);
    void onNavNext();
    void onNavPrev();
    void onNavFirst();
    void onNavLast();
    void onNextRoot();
    void onPrevRoot();
    void onFirstRoot();
    void onLastRoot();
    void onNextPage();
    void onPrevPage();
    void onFirstPage();
    void onLastPage();

    void searchForWord();
    void searchForPage();
    void searchForNode();
    void searchForRoot();
    void searchForEntry();

    void onClearHistory();
    void onHistorySelection();

    void onKeymapChanged();

    void onEditView();

    void onOptions();

    void onSetInterface();
    void rootClicked(QTreeWidgetItem * , int);
    void entryActivated(QTreeWidgetItem * , int);
    void focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason);

    void rootChanged(const QString & root,const QString & node);
    void placeChanged(const Place &);
    void historyPositionChanged(int);
    void historyAddition(const Place &);

    void bookmarkShortcut(const QString &);
    void bookmarkAdd();
    void bookmarkAdd(const QString & id,const Place & p);


    void onDocs();
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
    void printNode(const QString &);
 protected:
    void closeEvent(QCloseEvent *);
private:
    void printCurrentPage(const QString & node = QString());
    void restoreSavedState();
    void cleanup();
    void search(int searchType,ArabicSearchDialog *,const QString &);
    Place showPlace(const Place &,int);
    //    QAction * createIconAction(const QString imgdir,const QString & iconfile,const QString & text);
    /// look through all tabs for the given node, -1 if not found, else tab index
    int searchTabs(const QString & node);
    SearchOptions m_defaultSearchOptions;
    QString m_configFile;

    QString m_interface;    // "default","minimal"
    QAction * m_minimalAction;

    QAction * m_optionsAction;

    QString m_applicationCssFile;
    EntryLayoutDialog * m_entryLayout;

    LogViewer * m_logview;
    QPrinter m_printer;
    QString m_printPdfLocation;
    bool m_printToPdf;
    bool m_printerReUse;
    ///
    //    void setupInterface();
    /// 0 - root mode, 1 - page mode
    int m_navMode;

    QLabel * m_placeIndicator;
    QToolButton * m_keymapsButton;
    QAction * m_keymapsAction;
    QToolButton * m_linkButton;
    QAction * m_linkAction;
    bool m_linkContents;

    void updateStatusBar();
    void updateMenu();
    bool m_ok;
    bool m_docked;          // whether to use docked widget
    bool m_valgrind;
    bool m_useNotes;

    bool m_treeKeepsFocus;
    /// toolbars
    QToolBar * m_mainbar;
    QToolBar * m_navigation;
    QToolBar * m_entrybar;

    QString m_iconTheme;
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
    bool m_saveSettings;
    QString m_startupNode;
    QString m_startupRoot;
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
    void setIcon(QAction *,const QString &,const QString &);
    void setIcons(const QString & theme = QString());
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

    SearchOptions m_searchOptions;

    QSqlDatabase m_db;
    QSqlDatabase m_notesDb;
    TabWidget * m_tabs;
    QMenu * m_fileMenu;
    // actions
    QAction * m_exitAction;
    QAction * m_testAction;

    QAction * m_editViewAction;

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
    QAction * m_searchEntryAction;
    /// Navigation
    QAction * m_navNextAction;
    QAction * m_navPrevAction;
    QAction * m_navFirstAction;
    QAction * m_navLastAction;
    //    QLabel * m_navText;
    QToolButton * m_navBtn;
    QAction * m_navboxAction;
    QComboBox * m_navBy;
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
