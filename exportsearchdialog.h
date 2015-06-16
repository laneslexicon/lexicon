#ifndef _EXPORTSEARCHDIALOG_H_
#define _EXPORTSEARCHDIALOG_H_
#include <QtCore>
#include <QFileDialog>
#include <QDialog>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QDir>
class ExportSearchDialog : public QDialog {
  Q_OBJECT

 public:
  ExportSearchDialog(const QStringList & columns,QWidget * parent = 0);
  QString separator() const;
  QString exportFileName() const;
  QStringList columns() const;
  bool markedRows() const;
  bool saveSettings() const;
  public slots:
   void onFileSelect();
  void readSettings();
  void writeSettings();
 protected:
   QSize sizeHint() const;
 private:
   QStringList m_columnLabels;
   QCheckBox * m_headers;
  QRadioButton * m_allRows;
  QRadioButton * m_markedRows;
  QList<QCheckBox *> m_columns;
  QRadioButton * m_tabSeparator;
  QRadioButton * m_commaSeparator;
  QRadioButton * m_otherSeparator;
  QLineEdit * m_charSeparator;
  QCheckBox * m_saveSettings;
  QLineEdit * m_fileName;
};
#endif
