#ifndef _SHOWXML_H_
#define _SHOWXML_H_
#include <QDialog>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QDebug>
#include <QFileDialog>
#include <QTextStream>
class ShowXmlDialog : public QDialog {
  Q_OBJECT

 public:
  ShowXmlDialog(QWidget * parent = 0);
  void setXml(const QString & xml);
  public slots:
    void onSave();
    void onClose();
 private:
  QPlainTextEdit * m_xml;
  int m_type;
};

#endif
