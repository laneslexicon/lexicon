#include "popupoptions.h"
DialogOptions::DialogOptions(QWidget * parent) : QDialog(parent) {
  setObjectName("dialogoptions");
  QVBoxLayout * vlayout = new QVBoxLayout;
  QFormLayout * layout = new QFormLayout;

  m_newTab = new QCheckBox;
  m_goTab = new QCheckBox;
  m_wholeWord = new QCheckBox;
  m_diacritics = new QCheckBox;
  m_regex  = new QCheckBox;
  m_force  = new QCheckBox;
  m_ignoreCase = new QCheckBox;

  layout->addRow(tr("Whole word"),m_wholeWord);
  layout->addRow(tr("Ignore diacritics"),m_diacritics);
  layout->addRow(tr("Regular expression search"),m_regex);
  layout->addRow(tr("Force Left-to-Right input\n(for regular exprssion search)"),m_force);
  layout->addRow(tr("Ignore case"),m_ignoreCase);
  layout->addRow(tr("Open in new tab"),m_newTab);
  layout->addRow(tr("Go to new tab"),m_goTab);


  QDialogButtonBox * btns = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
  connect(btns,SIGNAL(accepted()),this,SLOT(accept()));
  connect(btns,SIGNAL(rejected()),this,SLOT(reject()));

  vlayout->addLayout(layout);
  vlayout->addWidget(btns);
  setLayout(vlayout);
}
void DialogOptions::enableOption(int which,bool v) {
  switch(which) {
  case DialogOptions::Tab :
    m_newTab->setEnabled(v);
    break;
  case DialogOptions::Go :
    m_goTab->setEnabled(v);
    break;
  case DialogOptions::Whole :
    m_wholeWord->setEnabled(v);
    break;
  case DialogOptions::Diacritics :
    m_diacritics->setEnabled(v);
    break;
  case DialogOptions::Regex :
    m_regex->setEnabled(v);
    break;
  case DialogOptions::Force :
    m_force->setEnabled(v);
    break;
  case DialogOptions::IgnoreCase :
    m_ignoreCase->setEnabled(v);
    break;
  }
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
  case DialogOptions::Force :
    m_force->setChecked(v);
    break;
  case DialogOptions::IgnoreCase :
    m_ignoreCase->setChecked(v);
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
  case DialogOptions::Force :
    return m_force->isChecked();
    break;
  case DialogOptions::IgnoreCase :
    return m_ignoreCase->isChecked();
    break;
  }
  return false;
}
