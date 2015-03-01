#include "popupoptions.h"
DialogOptions::DialogOptions(QWidget * parent) : QDialog(parent) {
  QVBoxLayout * vlayout = new QVBoxLayout;
  QFormLayout * layout = new QFormLayout;

  m_newTab = new QCheckBox;
  m_goTab = new QCheckBox;
  m_wholeWord = new QCheckBox;
  m_diacritics = new QCheckBox;
  m_regex  = new QCheckBox;

  layout->addRow(tr("Whole word"),m_wholeWord);
  layout->addRow(tr("Ignore diacritics"),m_diacritics);
  layout->addRow(tr("Regular exprssion search"),m_regex);
  layout->addRow(tr("Open in new tab"),m_newTab);
  layout->addRow(tr("Go to new tab"),m_goTab);

  QDialogButtonBox * btns = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
  connect(btns,SIGNAL(accepted()),this,SLOT(accept()));
  connect(btns,SIGNAL(rejected()),this,SLOT(reject()));

  vlayout->addLayout(layout);
  vlayout->addWidget(btns);
  setLayout(vlayout);
}
void DialogOptions::setChecked(int which,bool v) {
  switch(which) {
  case DialogOptions::Tab :
    m_newTab->setChecked(v);
    break;
  case DialogOptions::Go :
    m_goTab->setChecked(v);
    break;
  case DialogOptions::Whole :
    m_wholeWord->setChecked(v);
    break;
  case DialogOptions::Diacritics :
    m_diacritics->setChecked(v);
    break;
  case DialogOptions::Regex :
    m_regex->setChecked(v);
    break;
  }
}
bool DialogOptions::isChecked(int which) {
  switch(which) {
  case DialogOptions::Tab :
    return m_newTab->isChecked();
    break;
  case DialogOptions::Go :
    return m_goTab->isChecked();
    break;
  case DialogOptions::Whole :
    return m_wholeWord->isChecked();
    break;
  case DialogOptions::Diacritics :
    return m_diacritics->isChecked();
    break;
  case DialogOptions::Regex :
    return m_regex->isChecked();
    break;
  }
  return false;
}
