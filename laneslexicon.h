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
#include "contentswidget.h"
#include "graphicsentry.h"
class LanesLexicon : public QMainWindow
{
    Q_OBJECT

public:
    explicit LanesLexicon(QWidget *parent = 0);
    ~LanesLexicon();
    QSize sizeHint() const;
private slots:
    void on_actionExit();
    void rootClicked(QTreeWidgetItem * , int);
    void focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason);
private:
    void createActions();
    void createToolBar();
    void createMenus();
    void createStatusBar();
    bool openDatabase(const QString &);
    ContentsWidget * m_tree;
    GraphicsEntry * m_entry;
    QSqlDatabase m_db;
    QTabWidget * m_tabs;
    QToolBar  * m_tools;
    QToolBar * m_fileToolBar;
    QMenu * m_fileMenu;
    // actions
    QAction * m_exitAction;
};
#endif // MAINWINDOW_H
