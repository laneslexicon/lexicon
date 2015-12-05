#include "centeredcheckbox.h"
CenteredCheckBox::CenteredCheckBox(QWidget * parent) : QWidget(parent) {
  QHBoxLayout * layout = new QHBoxLayout;
  m_box = new QCheckBox;
  connect(m_box,SIGNAL(stateChanged(int)),this,SIGNAL(stateChanged(int)));
  layout->setAlignment(Qt::AlignCenter);
  layout->addWidget(m_box);
  setLayout(layout);
}
void CenteredCheckBox::setRow(int row) {
  m_row = row;
}
int CenteredCheckBox::row() const {
  return m_row;
}
bool CenteredCheckBox::isChecked() const {
  return m_box->isChecked();
}
void CenteredCheckBox::setChecked(bool v)  {
  return m_box->setChecked(v);
}
