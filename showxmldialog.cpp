#include "showxmldialog.h"
#include "application.h"
#include "externs.h"
#include "definedsettings.h"
ShowXmlDialog::ShowXmlDialog(QWidget * parent) :QDialog(parent) {
  QVBoxLayout * layout = new QVBoxLayout;
  m_xml = new QPlainTextEdit;

  m_stripWrapper = new QCheckBox(tr("Perseus format"));
  QDialogButtonBox * btns = new QDialogButtonBox(QDialogButtonBox::Save|QDialogButtonBox::Close);
  QPushButton * btn = btns->button(QDialogButtonBox::Save);
  btn->setText(tr("Export"));
  connect(btns,SIGNAL(accepted()),this,SLOT(onSave()));
  connect(btns,SIGNAL(rejected()),this,SLOT(onClose()));

  QHBoxLayout * hlayout = new QHBoxLayout ;
  hlayout->addWidget(m_stripWrapper);
  hlayout->addWidget(btns);
  layout->addWidget(m_xml);
  layout->addLayout(hlayout);
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
    m_type = ShowXmlDialog::Xml;
    setWindowTitle(tr("View XML"));
    m_stripWrapper->setVisible(true);
    m_stripWrapper->setChecked(true);
  }
  else {
    m_type = ShowXmlDialog::Html;
    setWindowTitle(tr("View HTML"));
    m_stripWrapper->setVisible(false);
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
  if (m_type == ShowXmlDialog::Xml) {
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
      QString t = m_xml->toPlainText();
      /// strip the surround <word> tags if
      if ((m_type == ShowXmlDialog::Xml) && (m_stripWrapper->isChecked())) {
        int ix = t.indexOf("<entryFree",0,Qt::CaseInsensitive);
        if (ix != -1) {
          t.remove(0,ix);
        }
        ix = t.indexOf("</entryFree>",0,Qt::CaseInsensitive);
        if (ix != -1) {
          t.remove(ix + 12,t.length() + 1);
        }
      }
      out << t;
      f.close();
      QLOG_INFO() << QString(tr("Successfully exported data to %1")).arg(filename);
      statusMessage(QString(tr("Successfully exported data to %1")).arg(filename));
    }
  }
  SETTINGS

  settings.beginGroup("Entry");
  settings.setValue(SID_ENTRY_XML_VIEWER_SIZE, size());
  settings.setValue(SID_ENTRY_XML_VIEWER_POS, pos());

  this->accept();
}
