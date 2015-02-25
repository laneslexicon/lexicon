#include "optionsdialog.h"
#include "optionswidget.h"
#include "rootsoptions.h"
#include "printoptions.h"
#include "shortcutoptions.h"
#include "diacriticsoptions.h"
#include "entryoptions.h"
#include "findoptions.h"
#include "QsLog.h"
#ifndef STANDALONE
#include "application.h"
#include "externs.h"
#endif
/**
 * For use in the standalone app there should be <theme>.ini files in the
 * same directory as the executable (OSX issues ?)
 *
 * @param theme
 * @param parent
 */
OptionsDialog::OptionsDialog(const QString & theme,QWidget * parent) : QDialog(parent) {
  QString useTheme = theme;
  QVBoxLayout * vlayout = new QVBoxLayout;
  m_tabs = new QTabWidget;
#ifdef STANDALONE
  QSettings settings("config.ini",QSettings::IniFormat);
  if (useTheme.isEmpty()) {
    useTheme = settings.value("Theme").toString();
  }
#else
  QSettings settings(getLexicon()->settingsFileName(theme),QSettings::IniFormat);
#endif
  settings.setIniCodec("UTF-8");
  RootsOptions * tree = new RootsOptions(useTheme,this);
  PrintOptions * print = new PrintOptions(useTheme,this);
  ShortcutOptions * shortcut = new ShortcutOptions(useTheme,this);
  DiacriticsOptions * diacritics = new DiacriticsOptions(useTheme,this);
  EntryOptions * entry = new EntryOptions(useTheme,this);
  FindOptions * find = new FindOptions(useTheme,this);
  m_tabs->addTab(tree,tr("Contents"));
  m_tabs->addTab(entry,tr("Entry"));
  m_tabs->addTab(shortcut,tr("Shortcuts"));
  m_tabs->addTab(print,tr("Printer"));
  m_tabs->addTab(diacritics,tr("Diacritics"));
  m_tabs->addTab(find,tr("Search"));

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
  connect(diacritics,SIGNAL(modified(bool)),this,SLOT(valueChanged(bool)));
  connect(entry,SIGNAL(modified(bool)),this,SLOT(valueChanged(bool)));

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
  QString group = settings.group();
  while(! group.isEmpty()) {
    settings.endGroup();
    group = settings.group();
  }
  settings.beginGroup("Options");
  this->restoreGeometry(settings.value("Geometry").toByteArray());
}
OptionsDialog::~OptionsDialog() {
  writeSettings();
}
void OptionsDialog::writeSettings() {
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
  if (btn) {
    btn->setEnabled(v);
  }
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
  QPushButton * btn = m_buttons->button(QDialogButtonBox::Apply);
  btn->setEnabled(v);
  btn = m_buttons->button(QDialogButtonBox::Reset);
  btn->setEnabled(v);
}
