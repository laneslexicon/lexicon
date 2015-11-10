#ifndef _LINKCHECKDIALOG_H_
#define _LINKCHECKDIALOG_H_
#include <QDialog>
#include <QComboBox>
#include <QCheckBox>
#include <QSqlRecord>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QDialogButtonBox>

class LinkCheckDialog : public QDialog {
  Q_OBJECT

 public:
  LinkCheckDialog(const QSqlRecord &,QWidget * parent = 0);
  int getStatus() const;
  QString getTargetNode() const;
  QString getText() const;
 private:
  QLineEdit * m_node;
  QPlainTextEdit * m_note;
  QComboBox * m_status;
};

#endif
