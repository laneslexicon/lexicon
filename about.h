#ifndef __ABOUT_H__
#define __ABOUT_H__
#include <QDialog>
#include <QFormLayout>
#include <QTabWidget>
#include <QSqlQuery>
#include <QSqlError>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLineEdit>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QLabel>
#include <QCheckBox>
#include "QsLog.h"
class AboutDialog : public QDialog {
  Q_OBJECT

 public:
  AboutDialog(QWidget * parent = 0);
  QSize sizeHint() const;
 private:
  QWidget * buildTechTab();
  QWidget * buildSystemTab();
  QWidget * buildAuthorTab();
  QWidget * buildLicenseTab();
  QWidget * buildAckTab();
};
#endif
