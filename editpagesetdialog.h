#ifndef _EDITPAGESETDIALOG_H_
#define _EDITPAGESETDIALOG_H_
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
#include <QInputDialog>
#ifdef __APPLE__
#include <QStyleFactory>
#endif
class ColumnarTableWidget;
class PageEntryDialog;
class EditPageSetDialog : public QDialog {
  Q_OBJECT

 public:
  EditPageSetDialog(QWidget * parent = 0);
  ~EditPageSetDialog();
  QString pageSetTitle() const;
  int loadCount() const;
  QList<int> pages() const;
  static bool deletePageSet(int);
  private slots:
    void onSelectPages();
    void onSelectAll(int);
    void onApply();
    void onItemDoubleClicked(QTableWidgetItem * item);
 private:
    bool m_dirty;
    void readSettings();
    void writeSettings();
    QMap<int,int> m_pagesets;
    QPushButton * m_loadButton;
  int loadTitles();
  int m_action;
  ColumnarTableWidget * m_setlist;
  QString m_spanStyle;
  QMap<int,PageEntryDialog *> m_pageDialogs;
  //  void createPageDialog(int);
};
#endif
