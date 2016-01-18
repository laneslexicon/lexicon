#ifndef _SHOWXML_H_
#define _SHOWXML_H_
#include <QDialog>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QFileDialog>
#include <QTextStream>
#include <QCheckBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
class ShowXmlDialog : public QDialog {
  Q_OBJECT

 public:
  enum XmlType { Xml, Html };
  ShowXmlDialog(const QString & node = QString(),QWidget * parent = 0);
  void setXml(const QString & xml);
  public slots:
    void onExport();
    void onSave();
    void onClose();
    void onUnlock(int);
 private:
  QPlainTextEdit * m_xml;
  QString m_node;
  QCheckBox * m_unlock;
  int m_type;
  QString m_originalXml;
  QDialogButtonBox * m_buttonBox;
  bool m_locked;
};

#endif
