#include "optionstabwidget.h"
#include "optionswidget.h"
#include "rootsoptions.h"
#include "printoptions.h"
#include "shortcutoptions.h"
#include "diacriticsoptions.h"
#include "QsLog.h"
#ifndef STANDALONE
#include "application.h"
#include "externs.h"
#endif
OptionsTabWidget::OptionsTabWidget(const QString & theme,QWidget * parent) : QWidget(parent) {
  QVBoxLayout * vlayout = new QVBoxLayout;
  m_tabs = new QTabWidget;
#ifdef STANDALONE
  QSettings settings("default.ini",QSettings::IniFormat);

#else
  QSettings settings(getLexicon()->settingsFileName(theme),QSettings::IniFormat);
#endif
  settings.setIniCodec("UTF-8");
  RootsOptions * tree = new RootsOptions(getLexicon()->currentTheme(),this);
  m_tabs->addTab(tree,tr("Contents"));
  PrintOptions * print = new PrintOptions(getLexicon()->currentTheme(),this);
  m_tabs->addTab(print,tr("Printer"));
  ShortcutOptions * shortcut = new ShortcutOptions(getLexicon()->currentTheme(),this);
  m_tabs->addTab(shortcut,tr("Shortcuts"));

  DiacriticsOptions * diacritics = new DiacriticsOptions(getLexicon()->currentTheme(),this);
  m_tabs->addTab(diacritics,tr("Diacritics"));
  /*
  m_buttons = new QDialogButtonBox(QDialogButtonBox::Save
                                   //                                     | QDialogButtonBox::Cancel
                                   //                                     | QDialogButtonBox::Apply
                                     | QDialogButtonBox::Reset
                                     );
  */
  m_saveButton = new QPushButton("Save all");
  m_resetButton = new QPushButton("Reset all");
  m_buttons = new QDialogButtonBox;
  m_buttons->addButton(m_resetButton,QDialogButtonBox::ResetRole);
  m_buttons->addButton(m_saveButton,QDialogButtonBox::AcceptRole);

  connect(m_tabs,SIGNAL(currentChanged(int)),this,SLOT(currentChanged(int)));
  connect(m_saveButton, SIGNAL(clicked()), this, SLOT(saveChanges()));
  connect(m_resetButton, SIGNAL(clicked()), this, SLOT(resetChanges()));

  connect(tree,SIGNAL(modified(bool)),this,SLOT(valueChanged(bool)));
  connect(print,SIGNAL(modified(bool)),this,SLOT(valueChanged(bool)));
  connect(diacritics,SIGNAL(modified(bool)),this,SLOT(valueChanged(bool)));

  vlayout->addWidget(m_tabs);
  vlayout->addWidget(m_buttons);
  setLayout(vlayout);

  /*
  QPushButton * btn = m_buttons->button(QDialogButtonBox::Apply);
  btn = m_buttons->button(QDialogButtonBox::Reset);
  btn->setEnabled(false);
  connect(btn,SIGNAL(clicked()),this,SLOT(resetChanges()));
  btn = m_buttons->button(QDialogButtonBox::Save);
  btn->setEnabled(false);
  connect(btn,SIGNAL(clicked()),this,SLOT(saveChanges()));
  enableButtons();
  */
  QString group = settings.group();
  while(! group.isEmpty()) {
    settings.endGroup();
    group = settings.group();
  }
  settings.beginGroup("Options");
  this->restoreGeometry(settings.value("Geometry").toByteArray());
  enableButtons();
}
OptionsTabWidget::~OptionsTabWidget() {
  writeSettings();
}
void OptionsTabWidget::writeSettings() {
#ifdef STANDALONE
  QSettings settings("default.ini",QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
#else
  SETTINGS
#endif
  settings.beginGroup("Options");
  settings.setValue("Geometry", saveGeometry());
  settings.endGroup();
}
void OptionsTabWidget::lock(bool enable) {
  for(int i=0;i < m_tabs->count();i++){
    m_tabs->widget(i)->setEnabled(enable);
  }

}
/*
void OptionsTabWidget::applyChanges() {
  OptionsWidget * tab = qobject_cast<OptionsWidget *>(m_tabs->currentWidget());
  if (tab) {
    tab->writeSettings();
    this->enableButtons();
  }
}
*/
bool OptionsTabWidget::isModified() {
  for(int i=0;i < m_tabs->count();i++){
    OptionsWidget * tab = qobject_cast<OptionsWidget *>(m_tabs->widget(i));
    if (tab && tab->isModified()) {
      return true;
    }
  }
  return false;
}
void OptionsTabWidget::enableButtons() {
  bool v = false;
  OptionsWidget * currentTab = qobject_cast<OptionsWidget *>(m_tabs->currentWidget());
  for(int i=0;(i < m_tabs->count()) && !v;i++) {
    OptionsWidget * tab = qobject_cast<OptionsWidget *>(m_tabs->widget(i));
    if (tab) {
      if (tab->isModified()) {
        v = true;
      }
    }
  }
  m_saveButton->setEnabled(v);
  m_resetButton->setEnabled(v);

}
void OptionsTabWidget::valueChanged(bool /* v */) {
  this->enableButtons();
}
void OptionsTabWidget::saveChanges() {
  for(int i=0;i < m_tabs->count();i++) {
    OptionsWidget * tab = qobject_cast<OptionsWidget *>(m_tabs->widget(i));
    if (tab) {
      tab->writeSettings();
    }
  }
  //  this->accept();
}
void OptionsTabWidget::resetChanges() {
  for(int i=0;i < m_tabs->count();i++) {
    OptionsWidget * tab = qobject_cast<OptionsWidget *>(m_tabs->widget(i));
    if (tab) {
      tab->readSettings();
    }
  }
  //  this->accept();
}
void OptionsTabWidget::readTheme(const QString & theme) {
  for(int i=0;i < m_tabs->count();i++) {
    OptionsWidget * tab = qobject_cast<OptionsWidget *>(m_tabs->widget(i));
    if (tab) {
      tab->setFileName(theme);
      tab->blockAllSignals(true);
      tab->readSettings();
      tab->blockAllSignals(false);
    }
  }
  //  this->accept();
}
/*
void OptionsTabWidget::resetChanges() {
  OptionsWidget * w = qobject_cast<OptionsWidget *>(m_tabs->currentWidget());
  if (w) {
    w->readSettings();
    this->enableButtons();
  }
}
*/
void OptionsTabWidget::currentChanged(int /* ix */) {
  enableButtons();
}
  /*
void OptionsTabWidget::setApplyReset(bool v) {
  QPushButton * btn = m_buttons->button(QDialogButtonBox::Apply);
  btn->setEnabled(v);
  btn = m_buttons->button(QDialogButtonBox::Reset);
  btn->setEnabled(v);
}
  */
