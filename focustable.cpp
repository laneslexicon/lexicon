#include "focustable.h"
#include "exportsearchdialog.h"
#include "QsLog.h"
#include "externs.h"
#define SELECT_COLUMN 0
CheckBoxTableItem::CheckBoxTableItem(QWidget * parent) : QWidget(parent) {
  m_checkbox = new QCheckBox;
  QHBoxLayout * boxlayout = new QHBoxLayout;
  boxlayout->addWidget(m_checkbox);
  boxlayout->setAlignment(Qt::AlignCenter);
  boxlayout->setContentsMargins(0,0,0,0);
  setLayout(boxlayout);
}
bool CheckBoxTableItem::isChecked() const {
  return m_checkbox->isChecked();
}
/**
 *
 *
 * @param parent
 */
FocusTable::FocusTable(QWidget * parent) : QTableWidget(parent) {
  setFocusPolicy(Qt::StrongFocus);
}
void FocusTable::focusInEvent(QFocusEvent * event) {
  this->setStyleSheet("");
  QTableWidget::focusInEvent(event);
}
void FocusTable::focusOutEvent(QFocusEvent * event) {
  /// TODO get from INI
  this->setStyleSheet("QTableView { selection-background-color : lightgray}");
  QTableWidget::focusOutEvent(event);
}
void FocusTable::exportResults() {
  QStringList columns;

  for(int i=0;i < this->columnCount();i++) {
    QString s = this->horizontalHeaderItem(i)->text();
    if (! s.isEmpty()) {
      columns << s;
    }
  }
  ExportSearchDialog dlg(columns);
  if (dlg.exec() != QDialog::Accepted) {
    return;
  }
  if (dlg.saveSettings()) {
    dlg.writeSettings();
  }
  QString exportFileName = dlg.exportFileName();
  QFile file(exportFileName);
  if (! file.open(QIODevice::WriteOnly)) {
    QString err = QString(tr("Error opening export file to %1 : %2")).arg(exportFileName).arg(file.errorString());
    QLOG_WARN() << err;
    QMessageBox::warning(this, tr("Export Search Results"),err);
    return;
  }
  if (exportFileName.isEmpty()) {
    return;
  }
  QString sep = dlg.separator();
  columns = dlg.columns();
  QTextStream out(&file);
  out.setCodec("UTF-8");

  out << columns.join(sep) << endl;

  QList<int> cols;
  for(int i=0;i < this->columnCount();i++) {
    QString s = this->horizontalHeaderItem(i)->text();
    if (! s.isEmpty() && columns.contains(s)) {
        cols << i;
    }
  }
  bool markedRowsOnly = dlg.markedRows();
  int rowCount = this->rowCount();
  bool ok;
  int writeCount = 0;
  for(int i=0;i < rowCount;i++) {
    ok = false;
    if (! markedRowsOnly) {
      ok = true;
    }
    else {
      CheckBoxTableItem * m = qobject_cast<CheckBoxTableItem *>(this->cellWidget(i,SELECT_COLUMN));
      if (m && m->isChecked()) {
        ok = true;
      }
    }
    if (ok) {
      for(int j=0;j < cols.size();j++) {
        QTableWidgetItem * item = this->item(i,cols[j]);
        if (item) {
          out << item->text() << sep;
        }
        else {
          //          qDebug() << "No item at" << i << j;
        }
      }
      out << endl;
      writeCount++;
    }
  }
  file.close();
  statusMessage(QString(tr("Exported search results to file: %1 (%2 lines)")).arg(exportFileName).arg(writeCount));
}
