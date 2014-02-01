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
#include "contentswidget.h"
#include "graphicsentry.h"
#include "noteswidget.h"
#include "history.h"
#include "QsLog.h"
#include "place.h"
#include "placewidget.h"
#include "searchwidget.h"
#include "scripts.h"
#include "searchresults.h"
#include "xsltsupport.h"
class LanesLexicon : public QMainWindow
{
    Q_OBJECT

public:
    explicit LanesLexicon(QWidget *parent = 0);
    ~LanesLexicon();
    QSize sizeHint() const;
    void readSettings();
    void writeSettings();
    HistoryMaster * history();
    private slots:
      void shortcut(const QString &);
    void findNextRoot(const QString &);
    void findPrevRoot(const QString &);
    void onCloseTab(int) ;
    void onGoToPage(int);
    void on_actionExit();
    void on_actionTest();

    void on_actionNextRoot();
    void on_actionPrevRoot();
    void on_actionFirstRoot();
    void on_actionLastRoot();

    void on_actionNextPage();
    void on_actionPrevPage();
    void on_actionFirstPage();
    void on_actionLastPage();

    void onHistoryForward();
    void onHistoryBackward();
    void rootClicked(QTreeWidgetItem * , int);
    void focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason);
    void onNotesClicked();
    //    void historyItemActivated();
    void rootChanged(const QString & root,const QString & node);
    void placeChanged(const Place &);
    // shortcuts
    void shortcutActivated();
    void ambiguousShortcut();
private:
    Place showPlace(const Place &,bool newTab = false);
    Place showRoot(const QString &,int supp = 0,bool newTab = false);
    Place showNode(const QString &,bool nodeOnly = false,bool newTab = false);
    QString m_interface;    // "default","minimal"
    void setupInterface();
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

    QString m_dbName;
    QString m_notesDbName;
    bool m_historyEnabled;
    bool m_saveTabs;
    bool m_restoreTabs;
    QString m_navigationMode;
    void getFirstAndLast();
    void restoreTabs();
    QSignalMapper * m_signalMapper;
    void setupShortcuts();
    int m_historyPos;
    void setSignals(GraphicsEntry *);
    void loadStyleSheet();
    void createActions();
    void createToolBar();
    void setupHistory();
    void createMenus();
    void createStatusBar();
    bool eventFilter(QObject * target, QEvent *);
    bool openDatabase(const QString &);
    /// this is in the status bar
    QLabel * m_currentRoot;
    Place m_place;
    QPushButton * m_notesBtn;
    ContentsWidget * m_tree;
    //    GraphicsEntry * m_entry;
    NotesWidget * m_notes;
    PlaceWidget * m_pwidget;
    QSqlDatabase m_db;
    QTabWidget * m_tabs;
    QToolBar  * m_tools;
    QToolBar * m_fileToolBar;
    QMenu * m_fileMenu;
    // actions
    QAction * m_exitAction;
    QAction * m_testAction;
    QAction * m_hForward;
    QAction * m_hBackward;
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
    QToolButton * m_hForwardBtn;
    QToolButton * m_hBackwardBtn;
    HistoryMaster * m_history;
 signals:
   void nodeActivated(const QString & node,const QString & word);
};


#endif // MAINWINDOW_H
