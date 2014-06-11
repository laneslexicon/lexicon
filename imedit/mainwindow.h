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
#include <QLabel>
class ImEdit;
class ImLineEdit;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QSize sizeHint() const;
private slots:
    void on_actionExit();
private:
    void createActions();
    void createToolBar();
    void createMenus();
    void createStatusBar();
    QToolBar  * m_tools;
    QToolBar * m_fileToolBar;
    QMenu * m_fileMenu;
    // actions
    QAction * m_exitAction;
    ImEdit * m_edit;
    ImLineEdit * m_lineEdit;
};
#endif // MAINWINDOW_H
