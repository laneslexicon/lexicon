#ifndef _LOADPAGESETDIALOG_H_
#define _LOADPAGESETDIALOG_H_
#include <QtCore>
#include <QDialog>
#include <QDebug>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QFile>
#include <QLabel>
#include <QSettings>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QCheckBox>
#include <QFont>
#include <QWidget>
#include <QRegExp>
#include <QGroupBox>
#include <QRadioButton>
#include <QHeaderView>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QTabWidget>
#include <QMessageBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#ifdef __APPLE__
#include <QStyleFactory>
#endif
class PageEntryDialog;

class LoadPageSetDialog : public QDialog {
  Q_OBJECT

 public:
  LoadPageSetDialog(QWidget * parent = 0);
  ~LoadPageSetDialog();
  QString pageSetTitle() const;
  bool closeExisting() const ;
  int loadCount() const;
  QList<int> pages() const;
  private slots:
    void onSelectPages();
    void onSelectAll(int);
 private:
    void readSettings();
    void writeSettings();
    QMap<int,int> m_pagesets;
    QPushButton * m_loadButton;
  int loadTitles();
  void loadTitlesToTree();
  int m_action;
  QCheckBox * m_overwrite;
  QTableWidget * m_setlist;
  QString m_spanStyle;
  QTreeWidget * m_tree;
  QMap<int,PageEntryDialog *> m_pageDialogs;
  //  void createPageDialog(int);
};
#endif
