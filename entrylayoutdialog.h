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
#include <QCheckBox>
#include <QTextStream>
#include <QGroupBox>
class NoteMaster;
class EntryLayoutDialog : public QDialog {
  Q_OBJECT

 public:
  EntryLayoutDialog(QWidget * parent = 0);
  ~EntryLayoutDialog();
 public slots:
  void onApply();
  void onReset();
  void onSave();
  void onRestore();
  void onClose();
  void onTextChanged();
private:
  QTabWidget * m_tabs;
  QPlainTextEdit  * m_cssEdit;
  QPlainTextEdit * m_xsltEdit;
  QPushButton * m_applyButton;
  QPushButton * m_resetButton;
  QPushButton * m_saveButton;
  QPushButton * m_restoreButton;
  QPushButton * m_closeButton;
  QCheckBox * m_testCssBox;
  QCheckBox * m_testXsltBox;

  QCheckBox * m_stateCssBox;
  QCheckBox * m_stateXsltBox;
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
