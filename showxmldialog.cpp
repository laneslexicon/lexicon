#include "showxmldialog.h"
#include "application.h"
#include "externs.h"
#include "definedsettings.h"
#include "definedsql.h"
ShowXmlDialog::ShowXmlDialog(const QString & node,QWidget * parent) :QDialog(parent) {
  QVBoxLayout * layout = new QVBoxLayout;
  m_xml = new QPlainTextEdit;
  m_node = node;
  m_unlock = new QCheckBox(tr("Unchecked to enable editing"));
  connect(m_unlock,SIGNAL(stateChanged(int)),this,SLOT(onUnlock(int)));

  m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Save|QDialogButtonBox::Close);
  connect(m_buttonBox,SIGNAL(accepted()),this,SLOT(onSave()));
  connect(m_buttonBox,SIGNAL(rejected()),this,SLOT(onClose()));

  QPushButton * btn = new QPushButton(tr("Export"));
  m_buttonBox->addButton(btn,QDialogButtonBox::ActionRole);
  connect(btn,SIGNAL(clicked()),this,SLOT(onExport()));

  QHBoxLayout * hlayout = new QHBoxLayout ;
  hlayout->addWidget(m_unlock);
  hlayout->addWidget(m_buttonBox);
  layout->addWidget(m_xml);
  layout->addLayout(hlayout);
  setLayout(layout);

  SETTINGS

  settings.beginGroup("Entry");
  resize(settings.value(SID_ENTRY_XML_VIEWER_SIZE,QSize(600,400)).toSize());
  move(settings.value(SID_ENTRY_XML_VIEWER_POS,QPoint(450,20)).toPoint());
  m_locked = settings.value(SID_ENTRY_XML_VIEWER_LOCKED,true).toBool();
}
void ShowXmlDialog::setXml(const QString & xml) {
  m_xml->setPlainText(xml);
  m_xml->setReadOnly(true);
  if (xml.contains("<entryFree")) {
    m_type = ShowXmlDialog::Xml;
    setWindowTitle(tr("View XML"));
    m_unlock->setVisible(true);
    m_unlock->setChecked(m_locked);
    m_originalXml = xml;
  }
  else {
    m_type = ShowXmlDialog::Html;
    setWindowTitle(tr("View HTML"));
    m_unlock->setVisible(false);
    m_buttonBox->button(QDialogButtonBox::Save)->setVisible(false);
  }
}
void ShowXmlDialog::onClose() {
  SETTINGS

  settings.beginGroup("Entry");
  settings.setValue(SID_ENTRY_XML_VIEWER_SIZE, size());
  settings.setValue(SID_ENTRY_XML_VIEWER_POS, pos());

  this->reject();
}
void ShowXmlDialog::onExport() {
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
      if ((m_type == ShowXmlDialog::Xml) && (m_unlock->isChecked())) {
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
void ShowXmlDialog::onSave() {
  QLOG_DEBUG()  << Q_FUNC_INFO;
  QSqlQuery q;
  if (m_node.isEmpty()) {
    QLOG_DEBUG() << Q_FUNC_INFO << "No node id";
    return;
  }
  if (!q.prepare(SQL_UPDATE_ENTRY_XML)) {
    QLOG_WARN() << QString(tr("SQL prepare failed for update entry xml:%1")).arg(q.lastError().text());
    return;
  }
  QString xml = m_xml->toPlainText();// + "<---" + m_originalXml + "--->";

  q.bindValue(0,xml);
  q.bindValue(1,m_node);
  if (! q.exec()) {
    QLOG_WARN() << QString(tr("Exec failed for SQL_LINK_UPDATE_STATSU query:%1")).arg(q.lastError().text());
  }
  else {
    QSqlDatabase::database().commit();
    statusMessage(QString("Saved Xml"));
  }
}
void ShowXmlDialog::onUnlock(int state) {
  m_xml->setReadOnly(state == Qt::Checked);
  if (state == Qt::Checked) {
    m_unlock->setText(tr("Uncheck to enable editing"));
  }
  else {
    m_unlock->setText(tr("Check to disable editing"));
  }
}
