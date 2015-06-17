#include "exportsearchdialog.h"
#include "application.h"
#include "externs.h"
#include "definedsettings.h"
ExportSearchDialog::ExportSearchDialog(const QStringList & columns,QWidget * parent) : QDialog(parent) {
  setWindowTitle(tr("Export Options"));
  m_columnLabels = columns;
  QVBoxLayout * layout = new QVBoxLayout;
  QGroupBox * columnbox = new QGroupBox(tr("Columns"));
  QFormLayout * columnlayout = new QFormLayout;
  columnlayout->setHorizontalSpacing(100);
  for(int i=0;i < columns.size();i++) {
    QCheckBox * col = new QCheckBox;
    m_columns << col;
    columnlayout->addRow(columns[i],col);
  }
  columnbox->setLayout(columnlayout);


  QHBoxLayout * separatorlayout  = new QHBoxLayout;
  QGroupBox * separatorbox = new QGroupBox(tr("Field separator"));
  m_tabSeparator = new QRadioButton(tr("Tab"),separatorbox);
  m_commaSeparator = new QRadioButton(tr("Comma"),separatorbox);
  m_otherSeparator = new QRadioButton(tr("Other"),separatorbox);
  m_charSeparator = new QLineEdit;
  m_tabSeparator->setChecked(true);

#ifdef Q_OS_OSX
    m_charSeparator->setMinimumWidth(50);
#else
    m_charSeparator->setMaximumWidth(50);
#endif

  separatorlayout->addWidget(m_tabSeparator);
  separatorlayout->addWidget(m_commaSeparator);
  separatorlayout->addWidget(m_otherSeparator);
  separatorlayout->addWidget(m_charSeparator);

  separatorbox->setLayout(separatorlayout);



  QHBoxLayout * rowselectlayout  = new QHBoxLayout;
  QGroupBox * rowselectbox = new QGroupBox(tr("Rows to export"));
  m_allRows = new QRadioButton(tr("All"),rowselectbox);
  m_markedRows = new QRadioButton(tr("Marked"),rowselectbox);


  rowselectlayout->addWidget(m_allRows);
  rowselectlayout->addWidget(m_markedRows);
  rowselectbox->setLayout(rowselectlayout);

  QFormLayout * otherlayout = new QFormLayout;

  QHBoxLayout * filelayout = new QHBoxLayout;
  m_fileName = new QLineEdit;
  /*
#ifdef Q_OS_OSX
    m_fileName->setMinimumWidth(50);
#else
    m_fileName->setMaximumWidth(50);
#endif
  */
  QPushButton * fileButton = new QPushButton(tr("..."));
  filelayout->addWidget(m_fileName);
  filelayout->addWidget(fileButton);

  connect(fileButton,SIGNAL(clicked()),this,SLOT(onFileSelect()));
  otherlayout->addRow(tr("Export to file"),filelayout);

  m_headers = new QCheckBox;
  otherlayout->addRow(tr("Including column headers"),m_headers);

  m_saveSettings = new QCheckBox;
  otherlayout->addRow(tr("Save these settings"),m_saveSettings);



  layout->addWidget(columnbox);
  layout->addWidget(separatorbox);
  layout->addWidget(rowselectbox);
  layout->addLayout(otherlayout);

  QDialogButtonBox * buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttons,SIGNAL(accepted()),this,SLOT(accept()));
  connect(buttons,SIGNAL(rejected()),this,SLOT(reject()));
  layout->addWidget(buttons);
  setLayout(layout);

  readSettings();
}
QString ExportSearchDialog::separator() const {
  if (m_tabSeparator->isChecked()) {
    return QString("\t");
  }
  if (m_commaSeparator->isChecked()) {
    return QString(",");
  }
  if (m_otherSeparator->isChecked()) {
    return m_charSeparator->text();
  }
  return QString();
}
QString ExportSearchDialog::exportFileName() const {

  return m_fileName->text();
}
void ExportSearchDialog::onFileSelect() {
  QString fileName = QFileDialog::getSaveFileName(this,
    tr("Save as"), ".", tr("All files (*.*)"));
  if (! fileName.isEmpty()) {
    m_fileName->setText(QDir::current().relativeFilePath(fileName));
  }
}
QSize ExportSearchDialog::sizeHint() const {
  return QSize(600,400);
}
void ExportSearchDialog::readSettings() {

  SETTINGS

  settings.beginGroup("Export Search");
  QStringList columns = settings.value(SID_EXPORT_SEARCH_COLUMNS,QStringList()).toStringList();
  for (int i=0;i < columns.size();i++) {
    int ix = m_columnLabels.indexOf(columns[i]);
    if ((ix >= 0) && (ix < m_columns.size())) {
      m_columns[i]->setChecked(true);
    }
  }
  QString s = settings.value(SID_EXPORT_SEARCH_SEPARATOR,"\t").toString();
  qDebug() << QString("One%1Two").arg(s);
  if (s == QString("\t")) {
    m_tabSeparator->setChecked(true);
  }
  else if (s == QString(",")) {
    m_commaSeparator->setChecked(true);
  }
  else {
    m_otherSeparator->setChecked(true);
    m_charSeparator->setText(s);
  }
  if (settings.contains(SID_EXPORT_SEARCH_SIZE)) {
    resize(settings.value(SID_EXPORT_SEARCH_SIZE,QSize(600,400)).toSize());
    move(settings.value(SID_EXPORT_SEARCH_POS,QPoint(400,200)).toPoint());
  }
  m_saveSettings->setChecked(settings.value(SID_EXPORT_SEARCH_SAVE,true).toBool());

  m_allRows->setChecked(settings.value(SID_EXPORT_SEARCH_ROWS,true).toBool());
  m_markedRows->setChecked(! m_allRows->isChecked());
  m_headers->setChecked(settings.value(SID_EXPORT_SEARCH_HEADERS,true).toBool());

  m_fileName->setText(settings.value(SID_EXPORT_SEARCH_FILENAME).toString());
}
void ExportSearchDialog::writeSettings() {
  SETTINGS

  settings.beginGroup("Export Search");
  QStringList columns;
  for (int i=0;i < m_columns.size();i++) {
    if (m_columns[i]->isChecked()) {
      columns << m_columnLabels[i];
    }
  }
  settings.setValue(SID_EXPORT_SEARCH_COLUMNS,columns);
  settings.setValue(SID_EXPORT_SEARCH_ROWS,m_allRows->isChecked());
  settings.setValue(SID_EXPORT_SEARCH_FILENAME,m_fileName->text());
  settings.setValue(SID_EXPORT_SEARCH_SEPARATOR,this->separator());
  settings.setValue(SID_EXPORT_SEARCH_SIZE, size());
  settings.setValue(SID_EXPORT_SEARCH_POS, pos());
  settings.setValue(SID_EXPORT_SEARCH_SAVE,m_saveSettings->isChecked());
  settings.setValue(SID_EXPORT_SEARCH_HEADERS,m_headers->isChecked());
}
bool ExportSearchDialog::saveSettings() const {
  return m_saveSettings->isChecked();
}
bool ExportSearchDialog::markedRows() const {
  return m_markedRows->isChecked();
}
QStringList ExportSearchDialog::columns() const {
  QStringList columns;
  for (int i=0;i < m_columns.size();i++) {
    if (m_columns[i]->isChecked()) {
      columns << m_columnLabels[i];
    }
  }
  return columns;
}
