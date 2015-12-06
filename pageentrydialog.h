#ifndef _PAGEENTRYDIALOG_H_
#define _PAGEENTRYDIALOG_H_
#include <QtCore>
#include <QDialog>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QLabel>
#include <QSettings>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QCheckBox>
#include <QFont>
#include <QWidget>
#include <QGroupBox>
#include <QRadioButton>
#include <QHeaderView>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QMessageBox>
#ifdef __APPLE__
#include <QStyleFactory>
#endif
class PageEntryDialog : public QDialog {
  Q_OBJECT

 public:
  PageEntryDialog(int pageset,QWidget * parent = 0);
  ~PageEntryDialog();
  int pageCount() const;
  QList<int> pages() const;
  QTableWidget * m_pages;
 protected:
  void showEvent(QShowEvent *);
  void done(int);
  private slots:
    void onSelectAll();
    void onClearSelections();
 private:
  void readSettings();
  void writeSettings();
};
#endif
