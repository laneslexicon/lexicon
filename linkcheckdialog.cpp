#include "linkcheckdialog.h"
#include "externs.h"
#include "application.h"
LinkCheckDialog::LinkCheckDialog(const QSqlRecord & record, QWidget * parent) : QDialog(parent) {
  setWindowTitle(tr("Link Status"));

  QMap<int,QString> linkType;
  linkType.insert(0,"Single word, isolated");
  linkType.insert(1,"Multi-word link,isolated");
  linkType.insert(2,"Foreign before, no after");
  linkType.insert(3,"Multiple foreigns before,no after");
  linkType.insert(4,"Foreign after,foreign or none before");
  linkType.insert(-1,"Unknown");
  QVBoxLayout * layout = new QVBoxLayout;

  QString linkText = record.value("link").toString();
  QString fromnode = record.value("fromnode").toString();
  QString tonode = record.value("tonode").toString();

  int linkId = record.value("linkid").toInt();
  int orthindex = record.value("orthindex").toInt();
  int orthtype = record.value("orthtype").toInt();
  int status = record.value("status").toInt();
  QFormLayout * formlayout = new QFormLayout;
  formlayout->addRow(tr("From:"),new QLabel(fromnode));

  m_node = new QLineEdit;
  m_node->setText(tonode);
  formlayout->addRow(tr("To:"),m_node);

  QString html1 = getLexicon()->spanArabic(linkText,"linktext");

  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addWidget(new QLabel(html1));
  hlayout->addStretch();
  formlayout->addRow(tr("Text:"),hlayout);
  formlayout->addRow(tr("Link id:"),new QLabel(QString("%1").arg(linkId)));
  formlayout->addRow(tr("Index:"),new QLabel(QString("%1").arg(orthindex)));
  QString t;
  if (linkType.contains(orthtype)) {
    t = QString("%1 (%2)").arg(linkType.value(orthtype)).arg(orthtype);
  }
  else {
    t = "Unknown";
  }
  formlayout->addRow(tr("Type:"),new QLabel(t));

  m_status = new QComboBox;
  m_status->addItem("Not checked",-1);
  m_status->addItem("Checked Ok",0);

  if (status == 0) {
    m_status->setCurrentIndex(1);
  }
  else {
    m_status->setCurrentIndex(0);
  }
  formlayout->addRow(tr("Status"),m_status);

  layout->addLayout(formlayout);
  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  layout->addWidget(buttonBox);
  setLayout(layout);
  m_status->setFocus();
}
int LinkCheckDialog::getStatus() const {
  return m_status->currentData().toInt();
}
QString LinkCheckDialog::getTargetNode() const {
  return m_node->text();

}
