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
#include "logoptions.h"
#include "historyoptions.h"
#include "iconoptions.h"
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
  setWindowTitle(QString("Edit Theme:%1").arg(theme));

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
  m_modified = false;
  QString testFileName("xxx.ini");
  settings.setIniCodec("UTF-8");
  settings.beginGroup("Options");
  //resize(QSize(600, 400));
  resize(settings.value("Size", QSize(600, 400)).toSize());
  move(settings.value("Pos", QPoint(200, 200)).toPoint());
  bool debugChanges = settings.value("Debug",false).toBool();
  if (settings.value("Roots",true).toBool()) {
    RootsOptions * tree = new RootsOptions(useTheme,this);
    m_tabs->addTab(tree,tr("Contents"));
    tree->writeSettings(testFileName);
    tree->setDebug(debugChanges);
  }
  if (settings.value("Print",true).toBool()) {
    PrintOptions * print = new PrintOptions(useTheme,this);
    m_tabs->addTab(print,tr("Printer"));
    print->writeSettings(testFileName);
    print->setDebug(debugChanges);
  }
  if (settings.value("Shortcuts",true).toBool()) {
    ShortcutOptions * shortcut = new ShortcutOptions(useTheme,this);
    m_tabs->addTab(shortcut,tr("Shortcuts"));
    shortcut->writeSettings(testFileName);
    shortcut->setDebug(debugChanges);
  }
  if (settings.value("Diacritics",true).toBool()) {
    DiacriticsOptions * diacritics = new DiacriticsOptions(useTheme,this);
    m_tabs->addTab(diacritics,tr("Diacritics"));
    diacritics->writeSettings(testFileName);
    diacritics->setDebug(debugChanges);
  }
  if (settings.value("Entry",true).toBool()) {
    EntryOptions * entry = new EntryOptions(useTheme,this);
    m_tabs->addTab(entry,tr("Entry"));
    entry->writeSettings(testFileName);
    entry->setDebug(debugChanges);
  }
  if (settings.value("Search",true).toBool()) {
    FindOptions * find = new FindOptions(useTheme,this);
    m_tabs->addTab(find,tr("Search"));
    find->writeSettings(testFileName);
    find->setDebug(debugChanges);
  }
  if (settings.value("Bookmark",true).toBool()) {
    BookmarkOptions * bookmark = new BookmarkOptions(useTheme,this);
    m_tabs->addTab(bookmark,tr("Bookmark"));
    bookmark->writeSettings(testFileName);
    bookmark->setDebug(debugChanges);
  }
  if (settings.value("System",true).toBool()) {
    SystemOptions * systems = new SystemOptions(useTheme,this);
    m_tabs->addTab(systems,tr("System"));
    systems->writeSettings(testFileName);
    systems->setDebug(debugChanges);
  }
  if (settings.value("Logging",true).toBool()) {
    LogOptions * log = new LogOptions(useTheme,this);
    m_tabs->addTab(log,tr("Logging"));
    log->writeSettings(testFileName);
    log->setDebug(debugChanges);
  }
  if (settings.value("History",true).toBool()) {
    HistoryOptions * history = new HistoryOptions(useTheme,this);
    m_tabs->addTab(history,tr("History"));
    history->writeSettings(testFileName);
    history->setDebug(debugChanges);
  }
  if (settings.value("Icons",true).toBool()) {
    IconOptions * icon = new IconOptions(useTheme,this);
    m_tabs->addTab(icon,tr("Icons"));
    icon->writeSettings(testFileName);
    icon->setDebug(debugChanges);
  }
  for(int i=0;i < m_tabs->count();i++) {
    getLexicon()->setCursorPosition(m_tabs->widget(i));
  }
  /// TODO
  /// maps
  /// help
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
  settings.setIniCodec("UTF-8");
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
        QLOG_DEBUG() << Q_FUNC_INFO << QString("Modified tab: %1, %2").arg(i).arg(m_tabs->tabText(i));
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
      m_modified = true;
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
bool OptionsDialog::isModified() const {
  return m_modified;
}
