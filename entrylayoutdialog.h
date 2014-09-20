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
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QAbstractButton>
class NoteMaster;
class EntryLayoutDialog : public QDialog {
  Q_OBJECT

 public:
  EntryLayoutDialog(QWidget * parent = 0);
 public slots:
  void onTest(QAbstractButton *);
private:
  QTabWidget * m_tabs;
  QPlainTextEdit  * m_cssEdit;
  QPlainTextEdit * m_xsltEdit;
  QDialogButtonBox * m_testButtons;
  QDialogButtonBox * m_stateButtons;
  QString m_css;
  QString m_xslt;
  void readSettings();
  QString loadFromFile(int type,const QString &);
  QSize sizeHint() const;
 signals:
  void reload(const QString &,const QString &);
};
#endif
