#include "textwidthdialog.h"
#include "laneslexicon.h"
#include "graphicsentry.h"
#include "externs.h"
TextwidthDialog::TextwidthDialog(int v,QWidget * parent) : QDialog(parent) {
  setWindowTitle(tr("Adjust default text width"));
  setObjectName("textwidthdialog");
  QVBoxLayout * layout = new QVBoxLayout;
  m_spinner = new QSpinBox;

  m_spinner->setSingleStep(5);
  m_spinner->setMaximum(1000);
  m_spinner->setMinimum(10);
  m_spinner->setValue(v);

  m_applyTextwidth = new QCheckBox(tr("Apply width to all current tabs"));
  m_applyTextwidth->setChecked(true);
  QDialogButtonBox * buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

  QPushButton * button = new QPushButton(tr("Get from current tab"));
  buttons->addButton(button,QDialogButtonBox::ActionRole);
  connect(button,SIGNAL(clicked()),this,SLOT(onFromPage()));

  button = new QPushButton(tr("Set for current tab"));
  buttons->addButton(button,QDialogButtonBox::ActionRole);
  connect(button,SIGNAL(clicked()),this,SLOT(onToPage()));

  layout->addWidget(m_spinner);
  layout->addWidget(m_applyTextwidth);
  layout->addWidget(buttons);

  setLayout(layout);
  connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}
int TextwidthDialog::value() const {
  return m_spinner->value();
}
bool TextwidthDialog::applyAll() const {
  return m_applyTextwidth->isChecked();
}
void TextwidthDialog::applyChanged(int state) {
  if (state == Qt::Checked) {
    connect(m_spinner,SIGNAL(valueChanged(double)),this,SIGNAL(valueChanged(double)));
  }
  else {
    disconnect(m_spinner,SIGNAL(valueChanged(double)),this,SIGNAL(valueChanged(double)));
  }
}
void TextwidthDialog::onFromPage() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(getApp()->currentTab());
  if (entry) {
    int w = entry->getTextWidth();
    m_spinner->setValue(w);
  }
  else {
    QMessageBox::warning(this,tr("The current tab is not showing a entry"),QString(),QMessageBox::Ok);

  }
}
void TextwidthDialog::onToPage() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(getApp()->currentTab());
  if (entry) {
    entry->setTextWidth(m_spinner->value(),true);
  }
  else {
    QMessageBox::warning(this,tr("The current tab is not showing a entry"),QString(),QMessageBox::Ok);

  }
}
