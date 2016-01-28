#ifndef __CHANGESDIALOG_H__
#define __CHANGESDIALOG_H__
#include <QtCore>
#include <QDialog>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QCheckBox>
#include <QTableWidget>
#include <QLabel>
#include <QDebug>
#include <QHeaderView>
#include <QGroupBox>
//class RootsOptions;

class ChangesDialog : public QDialog {
  Q_OBJECT

 public:
  ChangesDialog(bool cancel,QWidget * parent = 0);
  void setChanges(const QStringList &);
  int choice() const;
  bool always() const;
 protected:
  QSize sizeHint() const;
  public slots:
    void onSave();
    void onDiscard();
    void onCancel();
    void onDetails(int);
 private:
    int m_ret;
    QTableWidget * m_details;
    QStringList m_changes;
    QCheckBox * m_always;
};
#endif
