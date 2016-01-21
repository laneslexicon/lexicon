#include "zoomdialog.h"
#include "laneslexicon.h"
#include "graphicsentry.h"
#include "externs.h"
ZoomDialog::ZoomDialog(qreal v,QWidget * parent) : QDialog(parent) {
  setWindowTitle(tr("Adjust default zoom"));
  setObjectName("zoomdialog");
  QVBoxLayout * layout = new QVBoxLayout;
  m_spinner = new QDoubleSpinBox;
  m_spinner->setDecimals(2);
  m_spinner->setValue(v);
  m_spinner->setSingleStep(0.01);

  m_applyZoom = new QCheckBox(tr("Apply zoom to all current tabs"));
  m_applyZoom->setChecked(true);
  QDialogButtonBox * buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

  QPushButton * button = new QPushButton(tr("Get from current tab"));
  buttons->addButton(button,QDialogButtonBox::ActionRole);
  connect(button,SIGNAL(clicked()),this,SLOT(onFromPage()));

  button = new QPushButton(tr("Set for current tab"));
  buttons->addButton(button,QDialogButtonBox::ActionRole);
  connect(button,SIGNAL(clicked()),this,SLOT(onToPage()));

  layout->addWidget(m_spinner);
  layout->addWidget(m_applyZoom);
  layout->addWidget(buttons);

  setLayout(layout);
  connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}
double ZoomDialog::value() const {
  return m_spinner->value();
}
bool ZoomDialog::applyAll() const {
  return m_applyZoom->isChecked();
}
void ZoomDialog::applyChanged(int state) {
  if (state == Qt::Checked) {
    connect(m_spinner,SIGNAL(valueChanged(double)),this,SIGNAL(valueChanged(double)));
  }
  else {
    disconnect(m_spinner,SIGNAL(valueChanged(double)),this,SIGNAL(valueChanged(double)));
  }
}
void ZoomDialog::onFromPage() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(getApp()->currentTab());
  if (entry) {
    qreal r = entry->getScale();
    m_spinner->setValue(r);
  }
  else {
    QMessageBox::warning(this,tr("The current tab is not showing a entry"),QString(),QMessageBox::Ok);

  }
}
void ZoomDialog::onToPage() {
  GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(getApp()->currentTab());
  if (entry) {
    entry->setScale(m_spinner->value());
  }
  else {
    QMessageBox::warning(this,tr("The current tab is not showing a entry"),QString(),QMessageBox::Ok);

  }
}
