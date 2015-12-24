#ifndef _SAVEPAGESETDIALOG_H_
#define _SAVEPAGESETDIALOG_H_
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
#ifdef __APPLE__
#include <QStyleFactory>
#endif
class ColumnarTableWidget;
class SavePageSetDialog : public QDialog {
  Q_OBJECT

 public:
  SavePageSetDialog(QTabWidget *,QWidget * parent = 0);
  ~SavePageSetDialog();
  QString pageSetTitle() const;
  QList<int> requestedTabs() const;
  int overwriteId() const;
  bool overwrite() const ;
  private slots:
    void onSave();
    void setTitleFromTable();
    void selectionToggled(bool);
 private:
    void readSettings();
    void writeSettings();

  void loadTitles();
  void loadTabs(QTabWidget *);
  int m_overwriteId;
  QCheckBox * m_overwrite;
  QRadioButton * m_selectAll;
  QRadioButton * m_selectNone;
  ColumnarTableWidget * m_setlist;
  ColumnarTableWidget * m_tablist;
  QLineEdit * m_name;
  QString m_spanStyle;
};
#endif
