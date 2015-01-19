#include "zoomdialog.h"
ZoomDialog::ZoomDialog(qreal v,QWidget * parent) : QDialog(parent) {
  setWindowTitle(tr("Adjust default zoom"));
  QVBoxLayout * layout = new QVBoxLayout;
  m_spinner = new QDoubleSpinBox;
  m_spinner->setDecimals(2);
  m_spinner->setValue(v);
  m_spinner->setSingleStep(0.01);
  QDialogButtonBox * buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
  layout->addWidget(m_spinner);
  layout->addWidget(buttons);

  setLayout(layout);
  connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

}
double ZoomDialog::value() {
  return m_spinner->value();
}
