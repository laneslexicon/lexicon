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
#include "spanningoptions.h"
#include "QsLog.h"
#include "definedsettings.h"
#include "changesdialog.h"
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
  setAttribute(Qt::WA_DeleteOnClose);

  setWindowTitle(QString("Edit Theme:%1").arg(theme));
  setObjectName("optionsdialog");
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
  m_hasChanges = false;
  QString testFileName(".vanilla.ini");
  settings.setIniCodec("UTF-8");
  settings.beginGroup("System");
  m_debug = settings.value(SID_SYSTEM_DEBUG,false).toBool();
  settings.endGroup();
  settings.beginGroup("Options");

  //  resize(QSize(600, 400));
  resize(settings.value("Size", QSize(600, 400)).toSize());
  move(settings.value("Pos", QPoint(200, 200)).toPoint());
  bool writeTest = settings.value("Create",false).toBool();
  //  m_showWarning = settings.value(SID_OPTIONS_CLOSE,true).toBool();
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
  if (settings.value("Spanning",true).toBool()) {
    SpanningOptions * spanning = new SpanningOptions(useTheme,this);
    m_tabs->addTab(spanning,tr("Embedded Arabic CSS"));
    if (writeTest) {
      spanning->writeSettings(testFileName);
    }
  }
  for(int i=0;i < m_tabs->count();i++) {
    getLexicon()->setCursorPosition(m_tabs->widget(i));
  }
  /// TODO
  /// maps ?

  m_buttons = new QDialogButtonBox(QDialogButtonBox::Save
                                     | QDialogButtonBox::Close
                                   // | QDialogButtonBox::Apply
                                     | QDialogButtonBox::Reset
                                     );

  connect(m_tabs,SIGNAL(currentChanged(int)),this,SLOT(currentChanged(int)));
  connect(m_buttons, SIGNAL(accepted()), this, SLOT(saveChanges()));
  connect(m_buttons, SIGNAL(rejected()), this, SLOT(onClose()));

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
  settings.beginGroup("Options");

  QSize sz = settings.value("Size", QSize(600, 950)).toSize();
  resize(sz);
  move(settings.value("Pos", QPoint(200, 200)).toPoint());

  m_changed = false;
}
OptionsDialog::~OptionsDialog() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  writeSettings();
  if (! m_tempFileName.isEmpty()) {
    QFile::remove(m_tempFileName);
  }
}
/**
 * This can be called by the applicaton as part of the the shutdown procedure
 * In that case don't offer the cancel button
 */
void OptionsDialog::onClose() {
  QLOG_DEBUG() << Q_FUNC_INFO << m_hasChanges;

  SETTINGS
  settings.beginGroup("Options");
  QString action = settings.value(SID_OPTIONS_ALWAYS,"ask").toString();
  QDialogButtonBox * btn = qobject_cast<QDialogButtonBox *>(sender());
  if (m_hasChanges && (action == "ask")) {
    bool v = false;
    if (btn) {
      v = true;
    }
    ChangesDialog * d = new ChangesDialog(v);
    d->setChanges(this->getChanges());

    d->exec();
    int ret = d->choice();
    v = d->always();
    delete d;
    switch(ret) {
    case QDialogButtonBox::Save :{
      // Save was clicked
      this->saveChanges();
      break;
    }
    case QDialogButtonBox::Discard :{
      break;
    }
    case QDialogButtonBox::Cancel : {
      return;
      break;
    }
    }
    ///
    SETTINGS
    settings.beginGroup("Options");
    if (v) {
        if (ret == QDialogButtonBox::Save) {
          settings.setValue(SID_OPTIONS_ALWAYS,"save");
        }
        if (ret == QDialogButtonBox::Discard) {
          settings.setValue(SID_OPTIONS_ALWAYS,"discard");
        }
    }
    else {
          settings.setValue(SID_OPTIONS_ALWAYS,"ask");
    }
    /*
    QCheckBox * noshow = new QCheckBox(tr("Check this box to make your choice the default and not show this dialog again"));
    QMessageBox msgBox;
    msgBox.setCheckBox(noshow);
    msgBox.setWindowTitle("Preferences");
    msgBox.setText("Some settings have been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    if (btn) {
      msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    }
    else {
      msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
    }
    msgBox.setDefaultButton(QMessageBox::Save);
    msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
    ret = msgBox.exec();


     m_showWarning = ! noshow->isChecked();

    switch(ret) {
    case QMessageBox::Save :{
      // Save was clicked
      this->saveChanges();
      break;
    }
    case QMessageBox::Discard :{
      break;
    }
    case QMessageBox::Cancel : {
      return;
    }
    }
    */
  }

  emit(hasChanges(m_changed));
  m_hasChanges = false;
  m_changed = false;
  this->reject();
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
  //  settings.setValue(SID_OPTIONS_CLOSE,m_showWarning);
  settings.endGroup();
}
void OptionsDialog::enableButtons() {
  bool v = false;
  OptionsWidget * currentTab = qobject_cast<OptionsWidget *>(m_tabs->currentWidget());
  for(int i=0;i < m_tabs->count();i++) {
    OptionsWidget * tab = qobject_cast<OptionsWidget *>(m_tabs->widget(i));
    if (tab) {
      //      tab->setDebug(m_debug);
      if (tab->isModified()) {
        QLOG_INFO() << Q_FUNC_INFO << QString("Modified tab: %1, %2").arg(i).arg(m_tabs->tabText(i));
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
  m_hasChanges = v;
}
void OptionsDialog::valueChanged(bool /* v */) {
  this->enableButtons();
}
void OptionsDialog::saveChanges() {
  for(int i=0;i < m_tabs->count();i++) {
    OptionsWidget * tab = qobject_cast<OptionsWidget *>(m_tabs->widget(i));
    if (tab && tab->isModified()) {
      tab->blockSignals(true);
      QLOG_DEBUG() << "Tab has changes" << i << tab->metaObject()->className();
      m_changed = true;
      tab->writeSettings();
      tab->blockSignals(false);
    }
  }
  m_hasChanges = false;
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
      QLOG_DEBUG() << QString(tr("Restored settings file from [%1] to [%2]"))
        .arg(m_tempFileName)
        .arg(fileName);
    }
    else {
      QLOG_DEBUG() << Q_FUNC_INFO << "Restore failed";
    }
  }
  else {
    QLOG_DEBUG() << Q_FUNC_INFO << "Remove failed";
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

      tab->blockSignals(false);
    }
  }
  m_hasChanges = false;
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
  return m_hasChanges;
}
QStringList OptionsDialog::getChanges() const {
  QStringList changes;
  for(int i=0;i < m_tabs->count();i++) {
    OptionsWidget * tab = qobject_cast<OptionsWidget *>(m_tabs->widget(i));
    if (tab) {
      //      tab->setDebug(m_debug);
      if (tab->isModified()) {
        changes << m_tabs->tabText(i);
        changes << tab->getChanges();
      }
    }
  }
  return changes;
}
