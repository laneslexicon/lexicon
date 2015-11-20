#include "linkcheckdialog.h"
#include "externs.h"
#include "application.h"
LinkCheckDialog::LinkCheckDialog(const QSqlRecord & record, QWidget * parent) : QDialog(parent) {


  /*
  QMap<int,QString> linkType;
  linkType.insert(0,"Single word, isolated");
  linkType.insert(1,"Multi-word link,isolated");
  linkType.insert(2,"Foreign before, no after");
  linkType.insert(3,"Multiple foreigns before,no after");
  linkType.insert(4,"Foreign after,foreign or none before");
  linkType.insert(-1,"Unknown");
  */
  QVBoxLayout * layout = new QVBoxLayout;

  QString linkText = record.value("link").toString();
  QString fromnode = record.value("fromnode").toString();
  QString tonode = record.value("tonode").toString();

  QString orthid = record.value("orthid").toString();
  int orthindex = record.value("orthindex").toInt();
  int orthtype = record.value("orthfixtype").toInt();
  QString pattern = record.value("orthpattern").toString();
  int status = record.value("status").toInt();
  int matchtype =  record.value("matchtype").toInt();

  setWindowTitle(QString(tr("Link Status : %1")).arg(orthid));

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
  formlayout->addRow(tr("Link id:"),new QLabel(QString("%1").arg(orthid)));
  formlayout->addRow(tr("Index:"),new QLabel(QString("%1").arg(orthindex)));
  formlayout->addRow(tr("Pattern:"),new QLabel(QString("%1").arg(pattern)));
  formlayout->addRow(tr("Type:"),new QLabel(QString("%1").arg(orthtype)));
  formlayout->addRow(tr("Match type:"),new QLabel(QString("%1").arg(matchtype)));

  m_status = new QComboBox;
  m_status->addItem("Not checked",-1);
  m_status->addItem("Checked,Ok",0);
  m_status->addItem("Word order wrong",1);
  m_status->addItem("Other text problem",2);
  m_status->addItem("Fixed Ok",3);
  int ix;
  switch(status) {
  case -1 : ix = 0;break;
  case  0 : ix = 1;break;
  case  1 : ix = 2;break;
  case  2 : ix = 3;break;
  case  3 : ix = 3;break;
  default : ix = 0;
  }
  m_status->setCurrentIndex(ix);

  formlayout->addRow(tr("Status:"),m_status);
  m_note = new QPlainTextEdit;
  formlayout->addRow(tr("Note:"),m_note);
  m_note->setPlainText(record.value("note").toString());

  layout->addLayout(formlayout);
  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Save
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
 QString LinkCheckDialog::getText() const {
   return m_note->toPlainText();
 }
