#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QApplication>
#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QGridLayout>
#include <QFormLayout>
#include <QToolBar>
#include <QStatusBar>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QStatusBar>
#include <QDebug>
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
#include <QSqlDatabase>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
class NoteMaster;
class NoteBrowser;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QSize sizeHint() const;
    NoteMaster * getNotes();
    QSettings * getSettings();
private slots:
    void on_actionExit();
    void onAddNote();
    void onAddNodeNote();
private:
    QSqlDatabase m_db;
    void createActions();
    void createToolBar();
    void createMenus();
    void createStatusBar();
    QToolBar  * m_tools;
    QToolBar * m_fileToolBar;
    QMenu * m_fileMenu;
    NoteMaster * m_notes;
    // actions
    QAction * m_exitAction;
    QAction * m_addNote;
    QAction * m_addNodeNote;
    NoteBrowser * m_browser;
};
#endif // MAINWINDOW_H
