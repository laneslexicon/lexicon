#include "systemoptions.h"
#include "definedsettings.h"
#include "QsLog.h"
#ifndef STANDALONE
#include "application.h"
#include "externs.h"
#endif

/**
 *
 * @param theme
 * @param parent
 */
SystemOptions::SystemOptions(const QString & theme,QWidget * parent) : OptionsWidget(theme,parent) {
  m_section = "System";
  QVBoxLayout * vlayout = new QVBoxLayout;



  m_contentsLinked = new QCheckBox;
  m_lexicon = new QLineEdit;
  QPushButton * lexiconbutton = new QPushButton(tr("..."));
  QHBoxLayout * lexiconlayout = new QHBoxLayout;
  connect(lexiconbutton,SIGNAL(clicked()),this,SLOT(onSetDatabase()));
  lexiconlayout->addWidget(m_lexicon);
  lexiconlayout->addWidget(lexiconbutton);
  lexiconlayout->addStretch();
  //  this->setControlSize(m_lexicon,VLARGE_EDIT);
  m_debug = new QCheckBox;
  m_docked = new QCheckBox;
  m_focusTab = new QLineEdit;
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

  m_css = new QLineEdit;
  QHBoxLayout * csslayout = new QHBoxLayout;
  QPushButton * cssbutton = new QPushButton(tr("..."));
  csslayout->addWidget(m_css);
  csslayout->addWidget(cssbutton);
  csslayout->addStretch();
  connect(cssbutton,SIGNAL(clicked()),this,SLOT(onSetCss()));


  m_theme = new QComboBox;

  m_title = new QLineEdit;

  m_toolbarText = new QCheckBox;
  m_useNotes = new QCheckBox;

  m_notesDb = new QLineEdit;
  QHBoxLayout * noteslayout = new QHBoxLayout;
  QPushButton * notesbutton = new QPushButton(tr("..."));
  noteslayout->addWidget(m_notesDb);
  noteslayout->addWidget(notesbutton);
  noteslayout->addStretch();
  connect(notesbutton,SIGNAL(clicked()),this,SLOT(onSetNotesDatabase()));

  m_historyDb = new QLineEdit;
  QHBoxLayout * historylayout = new QHBoxLayout;
  QPushButton * historybutton = new QPushButton(tr("..."));
  historylayout->addWidget(m_historyDb);
  historylayout->addWidget(historybutton);
  historylayout->addStretch();
  connect(historybutton,SIGNAL(clicked()),this,SLOT(onSetHistoryDatabase()));

  m_keyboard = new QComboBox;

  m_splashScreen = new QCheckBox;

  QGroupBox * dbgroup = new QGroupBox(tr("Databases"));
  QFormLayout * dblayout = new QFormLayout;
  dblayout->addRow(tr("Lexicon"),lexiconlayout);
  dblayout->addRow(tr("Notes"),noteslayout);
  dblayout->addRow(tr("History"),historylayout);
  dbgroup->setLayout(dblayout);


  QGroupBox * othergroup = new QGroupBox(tr("Options"));
  QFormLayout * optionlayout = new QFormLayout;



  optionlayout->addRow(tr("Debug"),m_debug);
  optionlayout->addRow(tr("Docked"),m_docked);
  optionlayout->addRow(tr("Current tab"),m_focusTab);
  optionlayout->addRow(tr("Contents linked"),m_contentsLinked);
  optionlayout->addRow(tr("Minimal interface"),m_minimalInterface);
  optionlayout->addRow(tr("Restore bookmarks"),m_restoreBookmarks);
  optionlayout->addRow(tr("Restore tab"),m_restoreTabs);
  optionlayout->addRow(tr("Save settings"),m_saveSettings);
  optionlayout->addRow(tr("Run date"),m_runDate);
  optionlayout->addRow(tr("Show interface warning"),m_showInterfaceWarning);
  optionlayout->addRow(tr("Application stylesheet"),csslayout);
  optionlayout->addRow(tr("Theme"),m_theme);
  optionlayout->addRow(tr("Title"),m_title);
  optionlayout->addRow(tr("Toolbar text"),m_toolbarText);
  optionlayout->addRow(tr("Nav by root"),m_rootNavigation);
  optionlayout->addRow(tr("Use notes"),m_useNotes);

  optionlayout->addRow(tr("Keyboard"),m_keyboard);
  optionlayout->addRow(tr("Show splash screen"),m_splashScreen);
  othergroup->setLayout(optionlayout);

  vlayout->addWidget(dbgroup);
  vlayout->addWidget(othergroup);

  vlayout->addStretch();
  setLayout(vlayout);
  addButtons();
  readSettings();
  setupConnections();
  this->setLineEditSize(VLARGE_EDIT);
  this->setComboSize(VLARGE_EDIT);
}

void SystemOptions::readSettings() {
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  m_contentsLinked->setChecked(settings.value(SID_SYSTEM_CONTENTS_LINKED,true).toBool());
  m_lexicon->setText(settings.value(SID_SYSTEM_DATABASE,"lexicon.sqlite").toString());
  m_debug->setChecked(settings.value(SID_SYSTEM_DEBUG,true).toBool());
  m_docked->setChecked(settings.value(SID_SYSTEM_DOCKED,true).toBool());
  m_focusTab->setText(settings.value(SID_SYSTEM_CURRENT_TAB,"0").toString());
  m_minimalInterface->setChecked(settings.value(SID_SYSTEM_MINIMAL,true).toBool());
  m_restoreBookmarks->setChecked(settings.value(SID_SYSTEM_RESTORE_BOOKMARKS,true).toBool());
  m_restoreTabs->setChecked(settings.value(SID_SYSTEM_RESTORE_TABS,true).toBool());
  m_saveSettings->setChecked(settings.value(SID_SYSTEM_SAVE_SETTINGS,true).toBool());
  m_saveTabs->setChecked(settings.value(SID_SYSTEM_SAVE_TABS,true).toBool());
  m_rootNavigation->setChecked(settings.value(SID_SYSTEM_BY_ROOT,true).toBool());


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

  m_showInterfaceWarning->setChecked(settings.value(SID_SYSTEM_INTERFACE_WARNING,true).toBool());
  m_css->setText(settings.value(SID_SYSTEM_STYLESHEET,"app.css").toString());

  m_title->setText(settings.value(SID_SYSTEM_TITLE,"Lane's Lexicon").toString());
  m_toolbarText->setChecked(settings.value(SID_SYSTEM_TOOLBAR_TEXT,true).toBool());

  settings.endGroup();
  settings.beginGroup("Notes");
  m_useNotes->setChecked(settings.value(SID_NOTES_ENABLED,false).toBool());
  m_notesDb->setText(settings.value(SID_NOTES_DATABASE,"notes.sqlite").toString());
  settings.endGroup();
  settings.beginGroup("Splash");
  m_splashScreen->setChecked(settings.value(SID_SPLASH_ENABLED,true).toBool());
  settings.endGroup();
  settings.beginGroup("History");
  m_historyDb->setText(settings.value(SID_HISTORY_DATABASE,"history.sqlite").toString());


  m_dirty = false;
}
void SystemOptions::writeSettings(const QString & fileName) {
  QString f = m_settingsFileName;
  if (!fileName.isEmpty()) {
    f = fileName;
  }
  qDebug() << Q_FUNC_INFO << f;
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
  settings.setValue(SID_SYSTEM_DEBUG,m_debug->isChecked());
  settings.setValue(SID_SYSTEM_DOCKED,m_docked->isChecked());
  settings.setValue(SID_SYSTEM_CURRENT_TAB,m_focusTab->text());
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

  settings.endGroup();
  settings.beginGroup("Notes");
  settings.setValue(SID_NOTES_ENABLED,m_useNotes->isChecked());
  settings.setValue(SID_NOTES_DATABASE,m_notesDb->text());
  /*

    This was originally here and caused corruption to any arabic
    text in the ini file.

  getLexicon()->setDefaultKeyboard(m_keyboard->currentText());
  */
  settings.endGroup();
  settings.beginGroup("Splash");
  settings.setValue(SID_SPLASH_ENABLED,m_splashScreen->isChecked());
  settings.endGroup();
  settings.beginGroup("History");
  settings.setValue(SID_HISTORY_DATABASE,m_historyDb->text());
  qDebug() << Q_FUNC_INFO << settings.format() << settings.iniCodec()->name();
  settings.sync();
  m_dirty = false;
  emit(modified(false));
}
/**
 * TODO not complete
 *
 *
 * @return
 */
bool SystemOptions::isModified()  {
  m_dirty = false;
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  if (compare(&settings,SID_SYSTEM_CONTENTS_LINKED,m_contentsLinked)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SYSTEM_DATABASE,m_lexicon)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SYSTEM_DEBUG,m_debug)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SYSTEM_DOCKED,m_docked)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SYSTEM_CURRENT_TAB,m_focusTab)) {
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
  if (compare(&settings,SID_SYSTEM_BY_ROOT,m_rootNavigation)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SYSTEM_RUN_DATE,m_runDate)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_SYSTEM_INTERFACE_WARNING,m_showInterfaceWarning)) {
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

  settings.endGroup();
  settings.beginGroup("Notes");
  if (compare(&settings,SID_NOTES_ENABLED,m_useNotes)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_NOTES_DATABASE,m_notesDb)) {
    m_dirty = true;
  }
#ifdef LANE
  if (m_keyboard->currentText() !=   getLexicon()->getDefaultKeyboard()) {
    m_dirty = true;
  }
  if (m_theme->currentText() != getLexicon()->currentTheme()) {
    m_dirty = true;
  }
#endif
  settings.endGroup();
  settings.beginGroup("Splash");
  if (compare(&settings,SID_SPLASH_ENABLED,m_splashScreen)) {
    m_dirty = true;
  }
  settings.endGroup();
  settings.beginGroup("History");
  if (compare(&settings,SID_HISTORY_DATABASE,m_historyDb)) {
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
  /*
  QColor color;
  color.setNamedColor(m_highlightColor->text());
  QColorDialog d(color);
  if (d.exec() != QDialog::Accepted) {
    return;
  }
  int r,g,b;
  color = d.currentColor();
  color.getRgb(&r,&g,&b);
  QString str = QString("%1,%2,%3").arg(r).arg(g).arg(b);
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup("Colors");
  QStringList keys = settings.allKeys();
  QStringList v;
  for(int i=0;i < keys.size();i++) {
    v = settings.value(keys[i]).toStringList();
    if (v.join(",") == str) {

      m_highlightColor->setText(keys[i]);
      return;
    }
  }
  m_highlightColor->setText(d.currentColor().name());
  */
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
