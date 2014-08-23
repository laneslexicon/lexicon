#include "optionsdialog.h"
#include "optionswidget.h"
#include "rootsoptions.h"
#include "printoptions.h"
OptionsDialog::OptionsDialog(QWidget * parent) : QDialog(parent) {
  QVBoxLayout * vlayout = new QVBoxLayout;
  m_tabs = new QTabWidget;
  QSettings * settings = new QSettings("default.ini",QSettings::IniFormat);
  settings->setIniCodec("UTF-8");
  RootsOptions * tree = new RootsOptions(settings);
  m_tabs->addTab(tree,tr("Contents"));
  PrintOptions * print = new PrintOptions(settings);
  m_tabs->addTab(print,tr("Printer"));
  m_buttons = new QDialogButtonBox(QDialogButtonBox::Save
                                     | QDialogButtonBox::Cancel
                                     | QDialogButtonBox::Apply
                                     | QDialogButtonBox::Reset
                                     );

  connect(m_tabs,SIGNAL(currentChanged(int)),this,SLOT(currentChanged(int)));
  connect(m_buttons, SIGNAL(accepted()), this, SLOT(saveChanges()));
  connect(m_buttons, SIGNAL(rejected()), this, SLOT(reject()));

  connect(tree,SIGNAL(modified(bool)),this,SLOT(valueChanged(bool)));
  connect(print,SIGNAL(modified(bool)),this,SLOT(valueChanged(bool)));

  vlayout->addWidget(m_tabs);
  vlayout->addWidget(m_buttons);
  setLayout(vlayout);
  QPushButton * btn = m_buttons->button(QDialogButtonBox::Apply);
  btn->setEnabled(false);
  connect(btn,SIGNAL(clicked()),this,SLOT(applyChanges()));
  btn = m_buttons->button(QDialogButtonBox::Reset);
  btn->setEnabled(false);
  connect(btn,SIGNAL(clicked()),this,SLOT(resetChanges()));
  btn = m_buttons->button(QDialogButtonBox::Save);
  btn->setEnabled(false);
  connect(btn,SIGNAL(clicked()),this,SLOT(saveChanges()));
  enableButtons();
}
void OptionsDialog::applyChanges() {
  OptionsWidget * tab = qobject_cast<OptionsWidget *>(m_tabs->currentWidget());
  if (tab) {
    tab->writeSettings();
    this->enableButtons();
  }
}
void OptionsDialog::enableButtons() {
  bool v = false;
  OptionsWidget * currentTab = qobject_cast<OptionsWidget *>(m_tabs->currentWidget());
  for(int i=0;i < m_tabs->count();i++) {
    OptionsWidget * tab = qobject_cast<OptionsWidget *>(m_tabs->widget(i));
    if (tab) {
      if (tab->isModified()) {
        v = true;
      }
      if (tab == currentTab) {
        this->setApplyReset(tab->isModified());
      }
    }
  }
  QPushButton * btn = m_buttons->button(QDialogButtonBox::Save);
  btn->setEnabled(v);
}
void OptionsDialog::valueChanged(bool /* v */) {
  this->enableButtons();
}
void OptionsDialog::saveChanges() {
  for(int i=0;i < m_tabs->count();i++) {
    OptionsWidget * tab = qobject_cast<OptionsWidget *>(m_tabs->currentWidget());
    if (tab) {
      tab->writeSettings();
    }
  }
  this->accept();
}
void OptionsDialog::resetChanges() {
  OptionsWidget * w = qobject_cast<OptionsWidget *>(m_tabs->currentWidget());
  if (w) {
    w->readSettings();
    this->enableButtons();
  }
}
void OptionsDialog::currentChanged(int /* ix */) {
  enableButtons();
}
void OptionsDialog::setApplyReset(bool v) {
  qDebug() << Q_FUNC_INFO;
  QPushButton * btn = m_buttons->button(QDialogButtonBox::Apply);
  btn->setEnabled(v);
  btn = m_buttons->button(QDialogButtonBox::Reset);
  btn->setEnabled(v);
}
