#ifndef _PAGESETDIALOG_H_
#define _PAGESETDIALOG_H_
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
class CenteredCheckBox : public QWidget {
  Q_OBJECT
 public:
  CenteredCheckBox(QWidget * parent = 0);
  void setChecked(bool v);
  bool isChecked() const;
 private:
  QCheckBox * m_box;
};
class PageSetDialog : public QDialog {
  Q_OBJECT

 public:
  PageSetDialog(QTabWidget *,QWidget * parent = 0);
  ~PageSetDialog();
  QString pageSetTitle() const;
  QList<int> requestedTabs() const;
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
  int m_action;
  QCheckBox * m_overwrite;
  QRadioButton * m_selectAll;
  QRadioButton * m_selectNone;
  QTableWidget * m_setlist;
  QTableWidget * m_tablist;
  QLineEdit * m_name;
  QString m_spanStyle;
};
#endif
