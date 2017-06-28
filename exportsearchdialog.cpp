#include "exportsearchdialog.h"
#include "application.h"
#include "lanesupport.h"
#include "externs.h"
#include "definedsettings.h"
ExportSearchDialog::ExportSearchDialog(const QStringList & columns,const QString & columnKey,QWidget * parent) : QDialog(parent) {
  setWindowTitle(tr("Export Options"));
  m_columnLabels = columns;
  /// settings key to get list of required columns
  if (columnKey.isEmpty()) {
    m_columnKey = SID_EXPORT_SEARCH_COLUMNS;
  }
  else {
    m_columnKey = columnKey;
  }
  QVBoxLayout * layout = new QVBoxLayout;
  QGroupBox * columnbox = new QGroupBox(tr("Select columns to export, use up/down to change the order"));
  QHBoxLayout * columnhbox = new QHBoxLayout;
  QFormLayout * columnlayout = new QFormLayout;
  m_columnOrder = new QListWidget;

  columnlayout->setHorizontalSpacing(100);
  for(int i=0;i < columns.size();i++) {
    QCheckBox * col = new QCheckBox;
    m_columns << col;
    columnlayout->addRow(columns[i],col);


  }
  columnhbox->addLayout(columnlayout);


  columnhbox->addWidget(m_columnOrder);

  QVBoxLayout * updownlayout = new QVBoxLayout;
  updownlayout->addStretch();
  m_up = new QPushButton("Up");
  m_down = new QPushButton("Down");
  m_up->setEnabled(false);
  m_down->setEnabled(false);
  updownlayout->addWidget(m_up);
  updownlayout->addWidget(m_down);
  updownlayout->addStretch();
  columnhbox->addLayout(updownlayout);
  columnbox->setLayout(columnhbox);
  connect(m_up,SIGNAL(clicked()),this,SLOT(columnUp()));
  connect(m_down,SIGNAL(clicked()),this,SLOT(columnDown()));
  connect(m_columnOrder,SIGNAL(itemSelectionChanged()),this,SLOT(columnSelectionChanged()));

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
  otherlayout->addRow(tr("Include column headings"),m_headers);

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
  for(int i=0;i < m_columns.size();i++) {
    connect(m_columns[i],SIGNAL(stateChanged(int)),this,SLOT(columnChanged(int)));
    if (m_columns[i]->isChecked()) {
      m_columnOrder->addItem(new QListWidgetItem(m_columnLabels[i]));
    }
  }
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
  QStringList columns = settings.value(m_columnKey,QStringList()).toStringList();
  for (int i=0;i < columns.size();i++) {
    int ix = m_columnLabels.indexOf(columns[i]);
    if ((ix >= 0) && (ix < m_columns.size())) {
      m_columns[ix]->setChecked(true);
    }
  }
  QString s = settings.value(SID_EXPORT_SEARCH_SEPARATOR,"\t").toString();

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
  settings.setValue(m_columnKey,columns);
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
bool ExportSearchDialog::allRows() const {
  return m_allRows->isChecked();
}
QStringList ExportSearchDialog::columns() const {
  QStringList columns;
  for(int i=0;i < m_columnOrder->count();i++) {
    columns << m_columnOrder->item(i)->text();
  }
  return columns;
}
void ExportSearchDialog::setColumnKey(const QString & str) {
  m_columnKey = str;
}
void ExportSearchDialog::columnUp() {
  int currentRow = m_columnOrder->currentRow();
  if (currentRow == 0) {
    return;
  }
  QListWidgetItem * item = m_columnOrder->takeItem(currentRow);
  m_columnOrder->insertItem(currentRow - 1, item);
  m_columnOrder->setCurrentRow(currentRow - 1);
}
void ExportSearchDialog::columnDown() {
  int currentRow = m_columnOrder->currentRow();
  if (currentRow == (m_columnOrder->count() - 1)) {
    return;
  }
  QListWidgetItem * item = m_columnOrder->takeItem(currentRow);
  m_columnOrder->insertItem(currentRow + 1, item);
  m_columnOrder->setCurrentRow(currentRow + 1);
}
void ExportSearchDialog::columnSelectionChanged() {
  m_up->setEnabled(m_columnOrder->count() > 1);
  m_down->setEnabled(m_columnOrder->count() > 1);
}
void ExportSearchDialog::columnChanged(int state) {
  int row = -1;
  QCheckBox * box = qobject_cast<QCheckBox *>(sender());
  if (box) {
    for(int i=0;i < m_columns.size();i++) {
      if (m_columns[i] == box) {
        row = i;
      }
    }
    if (row == -1) {
      return;
    }
    QString str = m_columnLabels[row];
    if (state == Qt::Checked) {
      m_columnOrder->addItem(new QListWidgetItem(str));
    }
    else {
      QList<QListWidgetItem *> r = m_columnOrder->findItems(str,Qt::MatchFixedString);
      for(int i=0;i < r.size();i++) {
        delete r[i];
      }
    }
  }
  bool ok = false;
  if (m_columnOrder->selectedItems().size() > 0) {
    if (m_columnOrder->count() > 1) {
      ok = true;
    }
  }
  m_up->setEnabled(ok);
  m_down->setEnabled(ok);
}
