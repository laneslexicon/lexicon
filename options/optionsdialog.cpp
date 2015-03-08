#include "optionsdialog.h"
#include "optionswidget.h"
#include "rootsoptions.h"
#include "printoptions.h"
#include "shortcutoptions.h"
#include "diacriticsoptions.h"
#include "entryoptions.h"
#include "findoptions.h"
#include "bookmarkoptions.h"
#include "systemoptions.h"
#include "QsLog.h"
#ifndef STANDALONE
#include "application.h"
#include "externs.h"
#endif
/**
 * For use in the standalone app:
 *   read config.ini to get the current theme
 *   then use <theme>.ini as the name of the QSettings file
 *
 * @param theme
 * @param parent
 */
OptionsDialog::OptionsDialog(const QString & theme,QWidget * parent) : QDialog(parent) {
  QString useTheme = theme;


  QVBoxLayout * vlayout = new QVBoxLayout;
  m_tabs = new QTabWidget;
#ifdef STANDALONE
  QSettings testSettings("config.ini",QSettings::IniFormat);
  if (useTheme.isEmpty()) {
    useTheme = testSettings.value("Theme").toString();
  }
  QSettings settings(QString("%1.ini").arg(useTheme),QSettings::IniFormat);

#else
  QSettings settings(getLexicon()->settingsFileName(theme),QSettings::IniFormat);
#endif
  m_theme = useTheme;
  QString testFileName("xxx.ini");
  settings.setIniCodec("UTF-8");
  settings.beginGroup("Options");
  resize(settings.value("Size", QSize(500, 700)).toSize());
  move(settings.value("Pos", QPoint(200, 200)).toPoint());

  if (settings.value("Roots",true).toBool()) {
    RootsOptions * tree = new RootsOptions(useTheme,this);
    m_tabs->addTab(tree,tr("Contents"));
    tree->writeSettings(testFileName);
  }
  if (settings.value("Print",true).toBool()) {
    PrintOptions * print = new PrintOptions(useTheme,this);
    m_tabs->addTab(print,tr("Printer"));
    print->writeSettings(testFileName);
  }
  if (settings.value("Shortcuts",true).toBool()) {
    ShortcutOptions * shortcut = new ShortcutOptions(useTheme,this);
    m_tabs->addTab(shortcut,tr("Shortcuts"));
    shortcut->writeSettings(testFileName);
  }
  if (settings.value("Diacritics",true).toBool()) {
    DiacriticsOptions * diacritics = new DiacriticsOptions(useTheme,this);
    m_tabs->addTab(diacritics,tr("Diacritics"));
    diacritics->writeSettings(testFileName);
  }
  if (settings.value("Entry",true).toBool()) {
    EntryOptions * entry = new EntryOptions(useTheme,this);
    m_tabs->addTab(entry,tr("Entry"));
    entry->writeSettings(testFileName);
  }
  if (settings.value("Search",true).toBool()) {
    FindOptions * find = new FindOptions(useTheme,this);
    m_tabs->addTab(find,tr("Search"));
    find->writeSettings(testFileName);
  }
  if (settings.value("Bookmark",true).toBool()) {
    BookmarkOptions * bookmark = new BookmarkOptions(useTheme,this);
    m_tabs->addTab(bookmark,tr("Bookmark"));
    bookmark->writeSettings(testFileName);
  }
  if (settings.value("System",true).toBool()) {
    SystemOptions * systems = new SystemOptions(useTheme,this);
    m_tabs->addTab(systems,tr("System"));
    systems->writeSettings(testFileName);
  }
  /// TODO
  /// maps
  /// keyboards
  /// logging
  /// help
  /// history
  /// icons
  /// themes / resources etc






  m_buttons = new QDialogButtonBox(QDialogButtonBox::Save
                                     | QDialogButtonBox::Cancel
                                     | QDialogButtonBox::Apply
                                     | QDialogButtonBox::Reset
                                     );

  connect(m_tabs,SIGNAL(currentChanged(int)),this,SLOT(currentChanged(int)));
  connect(m_buttons, SIGNAL(accepted()), this, SLOT(saveChanges()));
  connect(m_buttons, SIGNAL(rejected()), this, SLOT(reject()));
  /*
  connect(tree,SIGNAL(modified(bool)),this,SLOT(valueChanged(bool)));
  connect(print,SIGNAL(modified(bool)),this,SLOT(valueChanged(bool)));
  connect(diacritics,SIGNAL(modified(bool)),this,SLOT(valueChanged(bool)));
  connect(entry,SIGNAL(modified(bool)),this,SLOT(valueChanged(bool)));
  connect(find,SIGNAL(modified(bool)),this,SLOT(valueChanged(bool)));
  connect(find,SIGNAL(showHelp(const QString &)),this,SIGNAL(showHelp(const QString &)));
  */
  for(int i=0;i < m_tabs->count();i++) {
    OptionsWidget * w = qobject_cast<OptionsWidget *>(m_tabs->widget(i));
    connect(w,SIGNAL(modified(bool)),this,SLOT(valueChanged(bool)));
    connect(w,SIGNAL(showHelp(const QString &)),this,SIGNAL(showHelp(const QString &)));

  }
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
}
OptionsDialog::~OptionsDialog() {
  writeSettings();
}
void OptionsDialog::writeSettings() {
#ifdef STANDALONE
  QSettings settings(QString("%1.ini").arg(m_theme),QSettings::IniFormat);
#else
  QSettings settings(getLexicon()->settingsFileName(m_theme),QSettings::IniFormat);
#endif
  settings.beginGroup("Options");
  settings.setValue("Size", size());
  settings.setValue("Pos", pos());
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
