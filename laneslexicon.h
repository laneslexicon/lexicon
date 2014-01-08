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
#include "contentswidget.h"
#include "graphicsentry.h"
#include "noteswidget.h"
#include "history.h"
#include "QsLog.h"

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
    void findNextRoot(const QString &);
    void findPrevRoot(const QString &);
    void on_actionExit();
    void on_actionTest();
    void on_actionNextRoot();
    void on_actionPrevRoot();
    void on_actionFirstRoot();
    void on_actionLastRoot();
    void onHistoryForward();
    void onHistoryBackward();
    void rootClicked(QTreeWidgetItem * , int);
    void focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason);
    void onNotesClicked();
    //    void historyItemActivated();
    void rootChanged(const QString & root,const QString & node);
private:
    QFont arFont;
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
    QAction * m_hForward;
    QAction * m_hBackward;
    // navigation
    QAction * m_rootForwardAction;
    QAction * m_rootBackwardAction;
    QAction * m_rootFirstAction;
    QAction * m_rootLastAction;
    // history
    QToolButton * m_hForwardBtn;
    QToolButton * m_hBackwardBtn;
    HistoryMaster * m_history;
 signals:
   void nodeActivated(const QString & node,const QString & word);
};


#endif // MAINWINDOW_H
