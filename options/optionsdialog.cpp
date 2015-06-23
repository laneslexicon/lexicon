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
#include "definedsettings.h"
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
  m_tabs->setObjectName("optionstab");
#ifndef LANE
  QSettings testSettings("config.ini",QSettings::IniFormat);
  if (useTheme.isEmpty()) {
    useTheme = testSettings.value("Theme").toString();
  }
  QSettings settings(QString("%1.ini").arg(useTheme),QSettings::IniFormat);

#else
  QSettings settings(getLexicon()->settingsFileName(theme),QSettings::IniFormat);
#endif

  m_tempFileName = getLexicon()->copyToTemp(settings.fileName());
  m_theme = useTheme;
  m_modified = false;
  m_hasChanges = false;
  settings.setIniCodec("UTF-8");
  settings.beginGroup("System");
  m_debug = settings.value(SID_SYSTEM_DEBUG,false).toBool();
  settings.beginGroup("Options");
  QString testFileName(".vanilla.ini");
  //resize(QSize(600, 400));
  resize(settings.value("Size", QSize(600, 400)).toSize());
  move(settings.value("Pos", QPoint(200, 200)).toPoint());
  bool writeTest = settings.value("Create",false).toBool();
  if (settings.value("System",true).toBool()) {
    SystemOptions * systems = new SystemOptions(useTheme,this);
    m_tabs->addTab(systems,tr("System"));
    if (writeTest) {
      systems->writeSettings(testFileName);
    }
  }
  if (settings.value("Roots",true).toBool()) {
    RootsOptions * tree = new RootsOptions(useTheme,this);
    m_tabs->addTab(tree,tr("Contents"));
    if (writeTest) {
      tree->writeSettings(testFileName);
    }
  }
  if (settings.value("Print",true).toBool()) {
    PrintOptions * print = new PrintOptions(useTheme,this);
    m_tabs->addTab(print,tr("Printer"));
    if (writeTest) {
      print->writeSettings(testFileName);
    }
  }
  if (settings.value("Shortcuts",true).toBool()) {
    ShortcutOptions * shortcut = new ShortcutOptions(useTheme,this);
    m_tabs->addTab(shortcut,tr("Shortcuts"));
    if (writeTest) {
      shortcut->writeSettings(testFileName);
    }
  }
  if (settings.value("Diacritics",true).toBool()) {
    DiacriticsOptions * diacritics = new DiacriticsOptions(useTheme,this);
    m_tabs->addTab(diacritics,tr("Diacritics"));
    if (writeTest) {
      diacritics->writeSettings(testFileName);
    }
  }
  if (settings.value("Entry",true).toBool()) {
    EntryOptions * entry = new EntryOptions(useTheme,this);
    m_tabs->addTab(entry,tr("Entry"));
    if (writeTest) {
      entry->writeSettings(testFileName);
    }
  }
  if (settings.value("Search",true).toBool()) {
    FindOptions * find = new FindOptions(useTheme,this);
    m_tabs->addTab(find,tr("Search"));
    if (writeTest) {
      find->writeSettings(testFileName);
    }
  }
  if (settings.value("Bookmark",true).toBool()) {
    BookmarkOptions * bookmark = new BookmarkOptions(useTheme,this);
    m_tabs->addTab(bookmark,tr("Bookmark"));
    if (writeTest) {
      bookmark->writeSettings(testFileName);
    }
  }
  if (settings.value("Logging",true).toBool()) {
    LogOptions * log = new LogOptions(useTheme,this);
    m_tabs->addTab(log,tr("Logging"));
    if (writeTest) {
      log->writeSettings(testFileName);
    }
  }
  if (settings.value("History",true).toBool()) {
    HistoryOptions * history = new HistoryOptions(useTheme,this);
    m_tabs->addTab(history,tr("History"));
    if (writeTest) {
      history->writeSettings(testFileName);
    }
  }
  if (settings.value("Icons",true).toBool()) {
    IconOptions * icon = new IconOptions(useTheme,this);
    m_tabs->addTab(icon,tr("Icons"));
    if (writeTest) {
      icon->writeSettings(testFileName);
    }
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
                                     | QDialogButtonBox::Close
                                   // | QDialogButtonBox::Apply
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
  QPushButton * btn;

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
  if (! m_tempFileName.isEmpty()) {
    QFile::remove(m_tempFileName);
  }
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
void OptionsDialog::enableButtons() {
  bool v = false;
  OptionsWidget * currentTab = qobject_cast<OptionsWidget *>(m_tabs->currentWidget());
  for(int i=0;i < m_tabs->count();i++) {
    OptionsWidget * tab = qobject_cast<OptionsWidget *>(m_tabs->widget(i));
    if (tab) {
      tab->setDebug(m_debug);
      if (tab->isModified()) {
        QLOG_DEBUG() << Q_FUNC_INFO << QString("Modified tab: %1, %2").arg(i).arg(m_tabs->tabText(i));
        m_hasChanges = v = true;
      }
    }
  }
  QPushButton * btn = m_buttons->button(QDialogButtonBox::Save);
  if (btn) {
    btn->setEnabled(v);
  }
  if (v && ! m_tempFileName.isEmpty()) {
    btn = m_buttons->button(QDialogButtonBox::Reset);
    if (btn) {
      btn->setEnabled(m_hasChanges);
    }
  }
}
void OptionsDialog::valueChanged(bool /* v */) {
  this->enableButtons();
}
void OptionsDialog::saveChanges() {
  for(int i=0;i < m_tabs->count();i++) {
    OptionsWidget * tab = qobject_cast<OptionsWidget *>(m_tabs->widget(i));
    if (tab && tab->isModified()) {
      tab->blockSignals(true);
      tab->writeSettings();
      m_modified = false;
      tab->blockSignals(false);
    }
  }
  this->accept();
}
void OptionsDialog::resetChanges() {
  bool ok = false;
  QLOG_DEBUG() << Q_FUNC_INFO << m_tempFileName;
  if (m_tempFileName.isEmpty()) {
    return;
  }
  if (! QFile::exists(m_tempFileName)) {
    return;
  }
#ifndef LANE
  QString fileName = QString("%1.ini").arg(m_theme);
#else
  QString fileName = getLexicon()->settingsFileName(m_theme);
#endif
  QString tempFile = getLexicon()->copyToTemp(fileName);
  if (tempFile.isEmpty()) {
    QLOG_WARN() << QString(tr("Unable to create temporary file for reset: %1")).arg(tempFile);
    return;
  }
  if (QFile::remove(fileName)) {
    if  (QFile::copy(m_tempFileName,fileName)) {
      ok = true;
      qDebug() << QString(tr("Restored settings file from [%1] to [%2]"))
        .arg(m_tempFileName)
        .arg(fileName);
    }
    else {
      qDebug() << Q_FUNC_INFO << "Restore failed";
    }
  }
  else {
    qDebug() << Q_FUNC_INFO << "Remove failed";
  }

  QFile::remove(tempFile);
  if (! ok ) {
    return;
  }
  for(int i=0;i < m_tabs->count();i++) {
    OptionsWidget * tab = qobject_cast<OptionsWidget *>(m_tabs->widget(i));
    if (tab) {
      tab->blockSignals(true);
      tab->readSettings();
      m_modified = false;
      tab->blockSignals(false);
    }
  }
  enableButtons();
}
void OptionsDialog::currentChanged(int /* ix */) {
  enableButtons();
}
/*
void OptionsDialog::setApplyReset(bool v) {
  QPushButton * btn;
  btn = m_buttons->button(QDialogButtonBox::Reset);
  if (m_tempFile
  btn->setEnabled(v);
}
*/
bool OptionsDialog::isModified() const {
  return m_modified;
}
