#include "systemoptions.h"
#include "definedsettings.h"
#include "QsLog.h"
#ifdef LANE
#include "application.h"
#include "externs.h"
#endif
#define VERTICAL_SPACING 2

/**
 *
 * @param theme
 * @param parent
 */
SystemOptions::SystemOptions(const QString & theme,QWidget * parent) : OptionsWidget(theme,parent) {
  m_section = "System";
  m_allowNavMode = false;
  QVBoxLayout * vlayout = new QVBoxLayout;
  setObjectName("systemoptions");


  m_contentsLinked = new QCheckBox;
  m_lexicon = new QLineEdit;
  QPushButton * lexiconbutton = new QPushButton(tr("..."));
  QHBoxLayout * lexiconlayout = new QHBoxLayout;
  connect(lexiconbutton,SIGNAL(clicked()),this,SLOT(onSetDatabase()));
  lexiconlayout->addWidget(m_lexicon);
  lexiconlayout->addSpacing(10);
  lexiconlayout->addWidget(lexiconbutton);
  lexiconlayout->addStretch();
  //  this->setControlSize(m_lexicon,VLARGE_EDIT);
  m_interval = new QLineEdit;
  m_interval->setValidator(new QIntValidator);

  m_docked = new QCheckBox;
  m_importCheck = new QCheckBox;
  m_importShow = new QCheckBox;


  //  this->setControlSize(m_focusTab,LARGE_EDIT);
  m_minimalInterface = new QCheckBox;
  m_restoreBookmarks = new QCheckBox;
  m_restoreTabs = new QCheckBox;
  m_saveSettings = new QCheckBox;
  m_saveTabs = new QCheckBox;
  m_rootNavigation = new QCheckBox;
  // Root mod
  m_runDate = new QDateTimeEdit;
  this->setControlSize(m_runDate,VLARGE_EDIT);

  // Save boo
  m_showInterfaceWarning = new QCheckBox;

  m_qtStyle = new QComboBox;
  m_qtStyle->addItems(QStyleFactory::keys());

  m_css = new QLineEdit;
  QHBoxLayout * csslayout = new QHBoxLayout;
  QPushButton * cssbutton = new QPushButton(tr("..."));
  csslayout->addWidget(m_css);
  csslayout->addSpacing(10);
  csslayout->addWidget(cssbutton);
  csslayout->addStretch();
  connect(cssbutton,SIGNAL(clicked()),this,SLOT(onSetCss()));


  m_theme = new QComboBox;

  m_title = new QLineEdit;

  m_toolbarText = new QCheckBox;
  m_useNotes = new QCheckBox;

  m_notesDb = new QLineEdit;
  m_optionsWarning  = new QCheckBox;
QHBoxLayout * noteslayout = new QHBoxLayout;
  QPushButton * notesbutton = new QPushButton(tr("..."));
  noteslayout->addWidget(m_notesDb);
  noteslayout->addSpacing(10);
  noteslayout->addWidget(notesbutton);
  noteslayout->addStretch();
  connect(notesbutton,SIGNAL(clicked()),this,SLOT(onSetNotesDatabase()));

  m_historyDb = new QLineEdit;
  QHBoxLayout * historylayout = new QHBoxLayout;
  QPushButton * historybutton = new QPushButton(tr("..."));
  historylayout->addWidget(m_historyDb);
  historylayout->addSpacing(10);
  historylayout->addWidget(historybutton);
  historylayout->addStretch();
  connect(historybutton,SIGNAL(clicked()),this,SLOT(onSetHistoryDatabase()));

  m_keyboard = new QComboBox;

  m_splashScreen = new QCheckBox;
  m_splashDuration = new QLineEdit;
  m_splashDuration->setValidator(new QIntValidator);
  this->setControlSize(m_splashDuration,SMALL_EDIT);
  m_allowDuplicates = new QCheckBox;
  /// there are no other radiobuttons, so just make this the parent
  QHBoxLayout * tablayout = new QHBoxLayout;
  m_insertNewTab = new QRadioButton(tr("Insert"),this);
  m_appendNewTab = new QRadioButton(tr("Append"),this);
  tablayout->addWidget(m_insertNewTab);
  tablayout->addSpacing(40);
  tablayout->addWidget(m_appendNewTab);
  tablayout->addStretch();

  QGroupBox * dbgroup = new QGroupBox(tr("Databases"));
  QFormLayout * dblayout = new QFormLayout;
  dblayout->addRow(tr("Lexicon"),lexiconlayout);
  dblayout->addRow(tr("Notes"),noteslayout);
  dblayout->addRow(tr("History"),historylayout);
  dblayout->setVerticalSpacing(VERTICAL_SPACING);
  dbgroup->setLayout(dblayout);


  QGroupBox * othergroup = new QGroupBox(tr("Options"));
  QFormLayout * optionlayout = new QFormLayout;



  optionlayout->addRow(tr("Title"),m_title);
  optionlayout->addRow(tr("Application stylesheet"),csslayout);
  optionlayout->addRow(tr("Theme"),m_theme);
  optionlayout->addRow(tr("Qt style"),m_qtStyle);
  optionlayout->addRow(tr("Docked"),m_docked);
  optionlayout->addRow(tr("New tab behaviour"),tablayout);
  optionlayout->addRow(tr("Import links ignore db mismatch"),m_importCheck);
  optionlayout->addRow(tr("Import links show warning"),m_importShow);
  optionlayout->addRow(tr("Contents linked"),m_contentsLinked);
  optionlayout->addRow(tr("Minimal interface"),m_minimalInterface);
  optionlayout->addRow(tr("Restore bookmarks"),m_restoreBookmarks);
  optionlayout->addRow(tr("Restore tabs"),m_restoreTabs);
  optionlayout->addRow(tr("Save settings"),m_saveSettings);
  optionlayout->addRow(tr("Run date"),m_runDate);
  optionlayout->addRow(tr("Show interface warning"),m_showInterfaceWarning);
  optionlayout->addRow(tr("Show preferences close warning"),m_optionsWarning);
  optionlayout->addRow(tr("Message duration (secs)"),m_interval);
  optionlayout->addRow(tr("Toolbar text"),m_toolbarText);
  if (m_allowNavMode) {
    optionlayout->addRow(tr("Nav by root"),m_rootNavigation);
  }
  else {
    m_rootNavigation->setVisible(false);
  }
  optionlayout->addRow(tr("Allow duplicates"),m_allowDuplicates);
  //  optionlayout->addRow(tr("Use notes"),m_useNotes);

  optionlayout->addRow(tr("Keyboard"),m_keyboard);

  QHBoxLayout * splashlayout = new QHBoxLayout;
  splashlayout->addWidget(m_splashScreen);
  splashlayout->addSpacing(10);
  splashlayout->addWidget(new QLabel(tr("Duration (secs)")));
  splashlayout->addWidget(m_splashDuration);
  splashlayout->addSpacing(10);
  QPushButton * splashbutton = new QPushButton(tr("Show location"));
  splashlayout->addWidget(splashbutton);
  connect(splashbutton,SIGNAL(clicked()),this,SLOT(onShowSplash()));
  splashlayout->addStretch();

  optionlayout->addRow(tr("Show splash screen"),splashlayout);
  optionlayout->setVerticalSpacing(VERTICAL_SPACING);
  othergroup->setLayout(optionlayout);


  m_onlineUrl = new QLineEdit;
  m_onlineCurrentPage = new QLineEdit;
  m_offlineLocation = new QLineEdit;
  QHBoxLayout * locationlayout = new QHBoxLayout;
  QPushButton * locationbutton = new QPushButton(tr("..."));
  locationlayout->addWidget(m_offlineLocation);
  locationlayout->addSpacing(10);
  locationlayout->addWidget(locationbutton);
  locationlayout->addStretch();

  connect(locationbutton,SIGNAL(clicked()),this,SLOT(onOfflineLocation()));
  m_offlineCurrentPage = new QLineEdit;
  m_localDocs = new QCheckBox;
  QGroupBox * docgroup = new QGroupBox(tr("Documentation"));
  QFormLayout * doclayout = new QFormLayout;
  doclayout->addRow(tr("Online URL"),m_onlineUrl);
  doclayout->addRow(tr("Online current page"),m_onlineCurrentPage);
  doclayout->addRow(tr("Offline location"),locationlayout);
  doclayout->addRow(tr("Offline current page"),m_offlineCurrentPage);
  doclayout->addRow(tr("Local documentation"),m_localDocs);
  doclayout->setVerticalSpacing(VERTICAL_SPACING);
  docgroup->setLayout(doclayout);


  vlayout->addWidget(dbgroup);
  vlayout->addWidget(othergroup);
  vlayout->addWidget(docgroup);

  vlayout->addStretch();
  setLayout(vlayout);
  addButtons();
  readSettings();
  setupConnections();
  this->setLineEditSize(VLARGE_EDIT);
  this->setComboSize(VLARGE_EDIT);


}

void SystemOptions::readSettings(bool reload) {
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  m_contentsLinked->setChecked(settings.value(SID_SYSTEM_CONTENTS_LINKED,true).toBool());
  m_lexicon->setText(settings.value(SID_SYSTEM_DATABASE,"lexicon.sqlite").toString());

  m_docked->setChecked(settings.value(SID_SYSTEM_DOCKED,true).toBool());
  m_importCheck->setChecked(settings.value(SID_SYSTEM_IMPORTLINKS_IGNORE,false).toBool());
  m_importShow->setChecked(settings.value(SID_SYSTEM_IMPORTLINKS_SHOW_WARNING,false).toBool());
  m_minimalInterface->setChecked(settings.value(SID_SYSTEM_MINIMAL,true).toBool());
  m_restoreBookmarks->setChecked(settings.value(SID_SYSTEM_RESTORE_BOOKMARKS,true).toBool());
  m_restoreTabs->setChecked(settings.value(SID_SYSTEM_RESTORE_TABS,true).toBool());
  m_saveSettings->setChecked(settings.value(SID_SYSTEM_SAVE_SETTINGS,true).toBool());
  m_saveTabs->setChecked(settings.value(SID_SYSTEM_SAVE_TABS,true).toBool());
  m_rootNavigation->setChecked(settings.value(SID_SYSTEM_BY_ROOT,true).toBool());
  m_interval->setText(settings.value(SID_SYSTEM_MESSAGE_TIMEOUT,2).toString());
  m_appendNewTab->setChecked(settings.value(SID_SYSTEM_APPEND_NEW_TABS,true).toBool());
  m_insertNewTab->setChecked(! m_appendNewTab->isChecked());
  QString newStyle = m_qtStyle->currentText();
  m_qtStyle->setCurrentText(settings.value(SID_SYSTEM_QT_STYLE).toString());
  if (reload) {
    QString oldStyle = m_qtStyle->currentText();
    if (oldStyle != newStyle) {
      QApplication::setStyle(QStyleFactory::create(oldStyle));
    }
  }
#ifndef STANDALONE
  m_keyboard->addItems(getLexicon()->getKeyboards());
  m_keyboard->setCurrentText(getLexicon()->getDefaultKeyboard());
  QStringList themes = getLexicon()->getThemes();
  themes.sort();
  m_theme->addItems(themes);
  m_theme->setCurrentText(getLexicon()->currentTheme());
#endif
  QString d = settings.value(SID_SYSTEM_RUN_DATE,QString()).toString();
  m_runDate->setDateTime(QDateTime::fromString(d,Qt::ISODate));
  m_allowDuplicates->setChecked(settings.value(SID_SYSTEM_ALLOW_DUPLICATES,false).toBool());

  m_showInterfaceWarning->setChecked(settings.value(SID_SYSTEM_INTERFACE_WARNING,true).toBool());
  m_css->setText(settings.value(SID_SYSTEM_STYLESHEET,"app.css").toString());

  m_title->setText(settings.value(SID_SYSTEM_TITLE,"Lane's Lexicon").toString());
  m_toolbarText->setChecked(settings.value(SID_SYSTEM_TOOLBAR_TEXT,true).toBool());
  m_optionsWarning->setChecked(settings.value(SID_SYSTEM_OPTIONS_CLOSE,true).toBool());
  settings.endGroup();
  settings.beginGroup("Notes");
  //  m_useNotes->setChecked(settings.value(SID_NOTES_ENABLED,false).toBool());
  m_notesDb->setText(settings.value(SID_NOTES_DATABASE,"notes.sqlite").toString());
  settings.endGroup();
  settings.beginGroup("Splash");
  m_splashScreen->setChecked(settings.value(SID_SPLASH_ENABLED,true).toBool());
  m_splashDuration->setText(settings.value(SID_SPLASH_DELAY,3).toString());
  settings.endGroup();
  settings.beginGroup("History");
  m_historyDb->setText(settings.value(SID_HISTORY_DATABASE,"history.sqlite").toString());
  settings.endGroup();
  settings.beginGroup("Help");
  m_onlineUrl->setText(settings.value(SID_HELP_ONLINE_PREFIX).toString());
  m_onlineCurrentPage->setText(settings.value(SID_HELP_ONLINE_URL,QUrl()).toUrl().toString());
  m_offlineLocation->setText(settings.value(SID_HELP_LOCAL_LOCATION,"site").toString());

  QDir dd(m_offlineLocation->text());
  QUrl u = settings.value(SID_HELP_LOCAL_URL,QUrl()).toUrl();
  m_offlineCurrentPage->setText(dd.relativeFilePath(u.fileName()));
  m_localDocs->setChecked(settings.value(SID_HELP_LOCAL,true).toBool());

  m_dirty = false;
}
void SystemOptions::writeSettings(const QString & fileName) {
  QString f = m_settingsFileName;
  if (!fileName.isEmpty()) {
    f = fileName;
  }
#ifdef LANE
  getLexicon()->setTheme(m_theme->currentText());
  getLexicon()->setDefaultKeyboard(m_keyboard->currentText());
#else

#endif

  QSettings settings(f,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  settings.setValue(SID_SYSTEM_CONTENTS_LINKED,  m_contentsLinked->isChecked());
  settings.setValue(SID_SYSTEM_DATABASE,m_lexicon->text());
  settings.setValue(SID_SYSTEM_MESSAGE_TIMEOUT,m_interval->text());

  settings.setValue(SID_SYSTEM_DOCKED,m_docked->isChecked());
  settings.setValue(SID_SYSTEM_IMPORTLINKS_IGNORE,m_importCheck->isChecked());
  settings.setValue(SID_SYSTEM_IMPORTLINKS_SHOW_WARNING,m_importShow->isChecked());
  settings.setValue(SID_SYSTEM_MINIMAL,m_minimalInterface->isChecked());
  settings.setValue(SID_SYSTEM_RESTORE_BOOKMARKS,m_restoreBookmarks->isChecked());
  settings.setValue(SID_SYSTEM_RESTORE_TABS,m_restoreTabs->isChecked());
  settings.setValue(SID_SYSTEM_SAVE_SETTINGS,m_saveSettings->isChecked());
  settings.setValue(SID_SYSTEM_SAVE_TABS,m_saveTabs->isChecked());
  settings.setValue(SID_SYSTEM_BY_ROOT,m_rootNavigation->isChecked());

  settings.setValue(SID_SYSTEM_RUN_DATE,m_runDate->dateTime().toString(Qt::ISODate));

  settings.setValue(SID_SYSTEM_INTERFACE_WARNING,m_showInterfaceWarning->isChecked());
  settings.setValue(SID_SYSTEM_STYLESHEET,m_css->text());

  settings.setValue(SID_SYSTEM_TITLE,m_title->text());
  settings.setValue(SID_SYSTEM_TOOLBAR_TEXT,m_toolbarText->isChecked());
  settings.setValue(SID_SYSTEM_APPEND_NEW_TABS,m_appendNewTab->isChecked());
  settings.setValue(SID_SYSTEM_ALLOW_DUPLICATES,m_allowDuplicates->isChecked());
  settings.setValue(SID_SYSTEM_OPTIONS_CLOSE,m_optionsWarning->isChecked());
  QString oldStyle = settings.value(SID_SYSTEM_QT_STYLE).toString();
  QString newStyle = m_qtStyle->currentText();
  settings.setValue(SID_SYSTEM_QT_STYLE,m_qtStyle->currentText());
  if (oldStyle != newStyle) {
    QApplication::setStyle(QStyleFactory::create(newStyle));
  }
  settings.endGroup();
  settings.beginGroup("Notes");
  settings.setValue(SID_NOTES_DATABASE,m_notesDb->text());
  /*

    This was originally here and caused corruption to any arabic
    text in the ini file.

  getLexicon()->setDefaultKeyboard(m_keyboard->currentText());
  */
  settings.endGroup();
  settings.beginGroup("Splash");
  settings.setValue(SID_SPLASH_ENABLED,m_splashScreen->isChecked());
  settings.setValue(SID_SPLASH_DELAY,m_splashDuration->text());
  settings.endGroup();
  settings.beginGroup("History");
  settings.setValue(SID_HISTORY_DATABASE,m_historyDb->text());
  settings.endGroup();

  settings.beginGroup("Help");
  settings.setValue(SID_HELP_ONLINE_PREFIX,m_onlineUrl->text());
  settings.setValue(SID_HELP_ONLINE_URL,QUrl(m_onlineCurrentPage->text()));
  settings.setValue(SID_HELP_LOCAL_LOCATION,m_offlineLocation->text());
  QFileInfo fi(QDir(m_offlineLocation->text()),m_offlineCurrentPage->text());
  settings.setValue(SID_HELP_LOCAL_URL,QUrl::fromLocalFile(fi.absoluteFilePath()));
  settings.setValue(SID_HELP_LOCAL,m_localDocs->isChecked());

   settings.sync();
  m_dirty = false;
  emit(modified(false));
}
/**
 *
 *
 *
 * @return
 */
bool SystemOptions::isModified()  {
  m_dirty = false;
  m_changes.clear();
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  if (compare(&settings,SID_SYSTEM_MESSAGE_TIMEOUT,m_interval)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_SYSTEM_CONTENTS_LINKED,m_contentsLinked)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SYSTEM_DATABASE,m_lexicon)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SYSTEM_DOCKED,m_docked)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SYSTEM_APPEND_NEW_TABS,m_appendNewTab)) {
    m_dirty = true;
  }
  //  else {
  //    setButtons(false);
  //  }

  if (compare(&settings,SID_SYSTEM_IMPORTLINKS_IGNORE,m_importCheck)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SYSTEM_IMPORTLINKS_SHOW_WARNING,m_importShow)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SYSTEM_MINIMAL,m_minimalInterface)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SYSTEM_RESTORE_BOOKMARKS,m_restoreBookmarks)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SYSTEM_RESTORE_TABS,m_restoreTabs)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SYSTEM_SAVE_SETTINGS,m_saveSettings)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SYSTEM_SAVE_TABS,m_saveTabs)) {
    m_dirty = true;
  }
  if (m_allowNavMode) {
    if (compare(&settings,SID_SYSTEM_BY_ROOT,m_rootNavigation)) {
      m_dirty = true;
    }
  }
  if (compare(&settings,SID_SYSTEM_RUN_DATE,m_runDate)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SYSTEM_INTERFACE_WARNING,m_showInterfaceWarning)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SYSTEM_ALLOW_DUPLICATES,m_allowDuplicates)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SYSTEM_STYLESHEET,m_css)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_SYSTEM_TITLE,m_title)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SYSTEM_TOOLBAR_TEXT,m_toolbarText)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SYSTEM_OPTIONS_CLOSE,m_optionsWarning)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SYSTEM_QT_STYLE,m_qtStyle)) {
    m_dirty = true;
  }

  settings.endGroup();
  /*
  settings.beginGroup("Notes");
  if (compare(&settings,SID_NOTES_ENABLED,m_useNotes)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_NOTES_DATABASE,m_notesDb)) {
    m_dirty = true;
  }
  settings.endGroup();
  */
  if (m_keyboard->currentText() !=   getLexicon()->getDefaultKeyboard()) {
    m_dirty = true;
  }
  if (m_theme->currentText() != getLexicon()->currentTheme()) {
    m_dirty = true;
  }

  settings.beginGroup("Splash");
  if (compare(&settings,SID_SPLASH_ENABLED,m_splashScreen)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SPLASH_DELAY,m_splashDuration)) {
    m_dirty = true;
  }
  settings.endGroup();
  settings.beginGroup("History");
  if (compare(&settings,SID_HISTORY_DATABASE,m_historyDb)) {
    m_dirty = true;
  }
  settings.endGroup();
  settings.beginGroup("Help");
  if (compare(&settings,SID_HELP_ONLINE_PREFIX,m_onlineUrl)) {
    m_dirty = true;
  }
  /// check this
  if (compare(&settings,SID_HELP_ONLINE_URL,m_onlineCurrentPage)) {
   m_dirty = true;
   }
  if (compare(&settings,SID_HELP_LOCAL_LOCATION,m_offlineLocation)) {
    m_dirty = true;
  }
  QDir dd(m_offlineLocation->text());
  QUrl u = settings.value(SID_HELP_LOCAL_URL,QUrl()).toUrl();
  QString str = dd.relativeFilePath(u.fileName());
  if (str != m_offlineCurrentPage->text()) {
    m_dirty = true;
    if (m_debug) {
      //      QLOG_DEBUG() << "Is Modified" << SID_HELP_LOCAL_URL << str << m_offlineCurrentPage->text();
    }
  }
  if (compare(&settings,SID_HELP_LOCAL,m_localDocs)) {
    m_dirty = true;
  }


  return m_dirty;
}
void SystemOptions::onSetFont() {
  /*
  QFont f;
  f.fromString(m_font->text());

  QFontDialog * d = new QFontDialog(f);
  QList<QComboBox *> boxes = d->findChildren<QComboBox *>();
  if (boxes.size() == 1) {
    int ix = boxes[0]->findText("Arabic");
    if (ix >= 0) {
      boxes[0]->setCurrentText("Arabic");
      boxes[0]->activated(ix);
    }
  }
  d->setCurrentFont(f);
  if (d->exec() == QDialog::Accepted) {
    QFont font = d->currentFont();
    m_font->setText(font.toString());
  }
  delete d;
  return;
  */
}
void SystemOptions::onSetColor() {
}
void SystemOptions::onSetDatabase() {
  QString fileName = QFileDialog::getOpenFileName(this,
    tr("Select database"), ".", tr("SQLite db (*.db *.sqlite)"));

  if (fileName.isEmpty()) {
    return;
  }
  m_lexicon->setText(QDir::current().relativeFilePath(fileName));
}
void SystemOptions::onSetNotesDatabase() {
  QString fileName = QFileDialog::getOpenFileName(this,
    tr("Select notes database"), ".", tr("SQLite db (*.db *.sqlite)"));

  if (fileName.isEmpty()) {
    return;
  }
  m_notesDb->setText(QDir::current().relativeFilePath(fileName));
}
void SystemOptions::onSetHistoryDatabase() {
  QString fileName = QFileDialog::getOpenFileName(this,
    tr("Select history database"), ".", tr("SQLite db (*.db *.sqlite)"));

  if (fileName.isEmpty()) {
    return;
  }
  m_historyDb->setText(QDir::current().relativeFilePath(fileName));
}
void SystemOptions::onSetCss() {
  QString cssDirectory = getLexicon()->getResourceFilePath(Lexicon::Stylesheet);
  QString fileName = QFileDialog::getOpenFileName(this,
    tr("Select application stylesheet"), cssDirectory, tr("CSS (*.css)"));

  if (fileName.isEmpty()) {
    return;
  }

  QDir d(cssDirectory);
  m_css->setText(d.relativeFilePath(fileName));
}
void SystemOptions::onOfflineLocation() {
  QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                  ".",
                                                  QFileDialog::ShowDirsOnly
                                                  | QFileDialog::DontResolveSymlinks);
  if (dir.isEmpty()) {
    return;
  }
  m_offlineLocation->setText(QDir::current().relativeFilePath(dir));
}
void SystemOptions::onShowSplash() {
  getLexicon()->showPath(Lexicon::Splash);
}
