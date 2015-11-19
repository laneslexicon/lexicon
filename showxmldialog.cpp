#include "showxmldialog.h"
#include "application.h"
#include "externs.h"
#include "definedsettings.h"
ShowXmlDialog::ShowXmlDialog(QWidget * parent) :QDialog(parent) {
  QVBoxLayout * layout = new QVBoxLayout;
  m_xml = new QPlainTextEdit;

  QDialogButtonBox * btns = new QDialogButtonBox(QDialogButtonBox::Save|QDialogButtonBox::Close);
  QPushButton * btn = btns->button(QDialogButtonBox::Save);
  btn->setText(tr("Export"));
  connect(btns,SIGNAL(accepted()),this,SLOT(onSave()));
  connect(btns,SIGNAL(rejected()),this,SLOT(onClose()));

  layout->addWidget(m_xml);
  layout->addWidget(btns);
  setLayout(layout);

  SETTINGS

  settings.beginGroup("Entry");
  resize(settings.value(SID_ENTRY_XML_VIEWER_SIZE,QSize(600,400)).toSize());
  move(settings.value(SID_ENTRY_XML_VIEWER_POS,QPoint(450,20)).toPoint());

}
void ShowXmlDialog::setXml(const QString & xml) {
  m_xml->setPlainText(xml);
  m_xml->setReadOnly(true);
  if (xml.contains("<entryFree")) {
    m_type = 0;
    setWindowTitle(tr("View XML"));
  }
  else {
    m_type = 1;
    setWindowTitle(tr("View HTML"));
  }
}
void ShowXmlDialog::onClose() {
  SETTINGS

  settings.beginGroup("Entry");
  settings.setValue(SID_ENTRY_XML_VIEWER_SIZE, size());
  settings.setValue(SID_ENTRY_XML_VIEWER_POS, pos());

  this->reject();
}
void ShowXmlDialog::onSave() {
  QString filter;
  if (m_type == 0) {
    filter = "*.xml";
  }
  else {
    filter = "*.html";
  }
  QString filename = QFileDialog::getSaveFileName(0,tr("Save file as"),".",filter);
  if (! filename.isEmpty()) {
    QFile f(filename);
    if ( ! f.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QLOG_WARN() << QString(tr("Error opening export file: %1 - %2")).arg(filename).arg(f.errorString());
    }
    else {
      QTextStream out(&f);
      out.setCodec("UTF-8");
      out << m_xml->toPlainText();
      f.close();
      QLOG_INFO() << QString(tr("Successfully exported data to %1")).arg(filename);
    }
  }
  qDebug() << "save to" << filename;
  SETTINGS

  settings.beginGroup("Entry");
  settings.setValue(SID_ENTRY_XML_VIEWER_SIZE, size());
  settings.setValue(SID_ENTRY_XML_VIEWER_POS, pos());

  this->accept();
}
