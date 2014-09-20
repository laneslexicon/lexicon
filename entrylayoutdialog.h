#ifndef __ENTRYLAYOUTDIALOG_H__
#define __ENTRYLAYOUTDIALOG_H__
#include <QDialog>
#include <QTabWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QString>
#include <QScopedPointer>
#include <QSettings>
#include <QMap>
#include <QFile>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QAbstractButton>
#include <QCheckBox>
class NoteMaster;
class EntryLayoutDialog : public QDialog {
  Q_OBJECT

 public:
  EntryLayoutDialog(QWidget * parent = 0);
  ~EntryLayoutDialog();
 public slots:
  void onTest(QAbstractButton *);
  void onState(QAbstractButton *);
  void onTabChange(int);
private:
  QTabWidget * m_tabs;
  QPlainTextEdit  * m_cssEdit;
  QPlainTextEdit * m_xsltEdit;
  QDialogButtonBox * m_testButtons;
  QDialogButtonBox * m_stateButtons;
  QCheckBox * m_which;
  QString m_css;
  QString m_xslt;
  QString m_cssFileName;
  QString m_xsltFileName;
  void readSettings();
  QString loadFromFile(int type,const QString &);
  QSize sizeHint() const;
 signals:
  void reload(const QString &,const QString &);
  void revert();
};
#endif
