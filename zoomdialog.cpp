#include "zoomdialog.h"
#include "graphicsentry.h"
ZoomDialog::ZoomDialog(qreal v,QWidget * parent) : QDialog(parent) {
  setWindowTitle(tr("Adjust default zoom"));
  QVBoxLayout * layout = new QVBoxLayout;
  m_spinner = new QDoubleSpinBox;
  m_spinner->setDecimals(2);
  m_spinner->setValue(v);
  m_spinner->setSingleStep(0.01);

  m_applyZoom = new QCheckBox(tr("Apply zoom to current page"));
  m_applyZoom->setChecked(true);
  QDialogButtonBox * buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
  layout->addWidget(m_spinner);
  layout->addWidget(m_applyZoom);
  layout->addWidget(buttons);

  setLayout(layout);
  connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
  connect(m_applyZoom,SIGNAL(stateChanged(int)),this,SLOT(applyChanged(int)));
  if (m_applyZoom->isChecked()) {
    connect(m_spinner,SIGNAL(valueChanged(double)),this,SIGNAL(valueChanged(double)));
  }
}
double ZoomDialog::value() {
  return m_spinner->value();
}
void ZoomDialog::applyChanged(int state) {
  if (state == Qt::Checked) {
    connect(m_spinner,SIGNAL(valueChanged(double)),this,SIGNAL(valueChanged(double)));
    qDebug() << "checked";
  }
  else {
    disconnect(m_spinner,SIGNAL(valueChanged(double)),this,SIGNAL(valueChanged(double)));
    qDebug() << "unchecked";
  }
}
