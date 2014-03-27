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
#include <QtWebKitWidgets/QWebView>
#include <QUrl>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include "application.h"
#include "inputmapper.h"
#include "contentswidget.h"
#include "graphicsentry.h"
#include "noteswidget.h"
#include "history.h"
#include "QsLog.h"
#include "place.h"
#include "searchwidget.h"
#include "bookmarkwidget.h"
#include "scripts.h"
#include "searchresults.h"
#include "xsltsupport.h"
#include "menubar.h"
#include "helpviewer.h"
struct cmdOptions {
  bool dump;
  QString node;
  QString config;
};
extern QSettings * getSettings();
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
    HistoryMaster * history();
    enum NavMode {ByRoot, ByPage };
    private slots:
      void testSlot();
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

    void on_actionNextPage();
    void on_actionPrevPage();
    void on_actionFirstPage();
    void on_actionLastPage();
    void on_actionClearHistory();
    void onHistorySelection();
    void rootClicked(QTreeWidgetItem * , int);
    void focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason);
    void onNotesClicked();
    //    void historyItemActivated();
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

    /// these apply to the current graphicsentry
    void pageZoomIn();
    void pageZoomOut();
    void pageWiden();
    void pageNarrow();
    void pagePrint();
    void pageSearch();
private:
    Place showPlace(const Place &,bool newTab = false);
    QAction * createIconAction(const QString imgdir,const QString & iconfile,const QString & text);
    QString m_configFile;
    QString m_interface;    // "default","minimal"
    QPrinter m_printer;
    void setupInterface();
    /// 0 - root mode, 1 - page mode
    int m_navMode;
    QLabel * m_navModeIndicator;
    QLabel * m_placeIndicator;
    void updateStatusBar();
    void updateMenu();
    bool m_ok;
    bool m_docked;          // whether to use docked widget
    bool m_valgrind;
    bool m_useNotes;
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
    bool eventFilter(QObject * target, QEvent *);
    bool openDatabase(const QString &);


    Place m_place;
    Place getCurrentPlace();
    QPushButton * m_notesBtn;
    ContentsWidget * m_tree;
    //    GraphicsEntry * m_entry;
    NotesWidget * m_notes;
    QSqlDatabase m_db;
    QTabWidget * m_tabs;
    QToolBar  * m_tools;
    QToolBar * m_fileToolBar;
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

    // root navigation
    QAction * m_rootForwardAction;
    QAction * m_rootBackwardAction;
    QAction * m_rootFirstAction;
    QAction * m_rootLastAction;
    // page navigation
    QAction * m_pageForwardAction;
    QAction * m_pageBackwardAction;
    QAction * m_pageFirstAction;
    QAction * m_pageLastAction;
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
 signals:
   void nodeActivated(const QString & node,const QString & word);
};


#endif // MAINWINDOW_H
