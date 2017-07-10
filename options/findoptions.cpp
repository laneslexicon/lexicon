#include "findoptions.h"
#include "definedsettings.h"
#include "QsLog.h"
#include "popupoptions.h"
#include "textsearchdialog.h"
#ifndef STANDALONE
#include "application.h"
#include "externs.h"
#endif
/**
 * Not done:
 *
 * @param theme
 * @param parent
 */
FindOptions::FindOptions(const QString & theme,QWidget * parent) : OptionsWidget(theme,parent) {
  setObjectName("findoptions");
  m_section = "TextSearch";
  QVBoxLayout * layout = new QVBoxLayout;
  QGroupBox * fullbox = new QGroupBox(tr("Text search"));


  m_textFragmentSize = new QSpinBox;
  this->setControlSize(m_textFragmentSize,LARGE_EDIT);
  m_textSummary = new QCheckBox;
  m_textCurrentPage = new QCheckBox;
  m_textStep = new QSpinBox;
  m_textPageSize = new QSpinBox;
  m_contextStyle = new QLineEdit;

  m_textRepeat = new QComboBox;
  m_textRepeat->addItems(TextSearchDialog::failureActions());

  m_textStep->setSingleStep(25);
  m_textStep->setRange(100,1000);
  this->setControlSize(m_textStep,MEDIUM_EDIT);

  this->setControlSize(m_textPageSize,MEDIUM_EDIT);
  m_textPageSize->setRange(0,3000);
  m_textPageSize->setSingleStep(5);
  this->setControlSize(m_textPageSize,MEDIUM_EDIT);



  QFormLayout * fulllayout = new QFormLayout;

  fulllayout->addRow(tr("Rows per page"),m_textPageSize);
  fulllayout->addRow(tr("Fragment size"),m_textFragmentSize);
  fulllayout->addRow(tr("Context style"),m_contextStyle);
  fulllayout->addRow(tr("One row for each entry"),m_textSummary);
  fulllayout->addRow(tr("Mark/clear selections for current page"),m_textCurrentPage);
  fulllayout->addRow(tr("Progress update interval"),m_textStep);
  QHBoxLayout * textrepeatlayout = new QHBoxLayout;
  textrepeatlayout->addWidget(m_textRepeat);
  textrepeatlayout->addStretch();
  fulllayout->addRow(tr("Search failure action"),textrepeatlayout);//m_textRepeat);
  QPushButton * fullbtn = new QPushButton(tr("Set"));
  this->setControlSize(fullbtn,MEDIUM_EDIT);
  QHBoxLayout * setlayout1 = new QHBoxLayout;
  setlayout1->addWidget(fullbtn);
  setlayout1->addStretch();
  fulllayout->addRow(tr("Search dialog options"),setlayout1);
  //  fulllayout->addRow(tr("Debug"),m_fullDebug);

  connect(fullbtn,SIGNAL(clicked()),this,SLOT(onFullDialog()));

  fullbox->setLayout(fulllayout);



  /// Head word search
  QGroupBox * headbox = new QGroupBox(tr("Head word search"));
  m_headPhrase = new QCheckBox;
  m_headStep = new QSpinBox;
  this->setControlSize(m_headStep,MEDIUM_EDIT);
  this->setControlSize(m_headStep,MEDIUM_EDIT);
  m_headStep->setSingleStep(25);

  m_headRepeat = new QComboBox;
  m_headRepeat->addItems(TextSearchDialog::failureActions());

  m_nodeinfoClose = new QCheckBox;
  m_nodeinfoForce = new QCheckBox;
  QFormLayout * headlayout = new QFormLayout;
  headlayout->addRow(tr("Progress udpate interval"),m_headStep);
  headlayout->addRow(tr("Close entry info after load"),m_nodeinfoClose);
  headlayout->addRow(tr("Show entry info forces new tab"),m_nodeinfoForce);
  QHBoxLayout * headrepeatlayout = new QHBoxLayout;
  headrepeatlayout->addWidget(m_headRepeat);
  headrepeatlayout->addStretch();
  headlayout->addRow(tr("Search failure action"),headrepeatlayout);//m_textRepeat);
  QPushButton * headbtn = new QPushButton(tr("Set"));
  QHBoxLayout * setlayout2 = new QHBoxLayout;
  setlayout2->addWidget(headbtn);
  setlayout2->addStretch();

  headlayout->addRow(tr("Search dialog options"),setlayout2);
  connect(headbtn,SIGNAL(clicked()),this,SLOT(onHeadDialog()));

  headlayout->addRow(tr("Use head phrase"),m_headPhrase);
  headbox->setLayout(headlayout);

  /// Local search
  QGroupBox * localbox = new QGroupBox(tr("Local search"));

  m_localShowAll    = new QCheckBox;
  m_localRepeat = new QComboBox;
  m_localRepeat->addItems(TextSearchDialog::failureActions());

  QFormLayout * locallayout = new QFormLayout;
  locallayout->addRow(tr("Highlight all results"),m_localShowAll);
  QHBoxLayout * localrepeatlayout = new QHBoxLayout;
  localrepeatlayout->addWidget(m_localRepeat);
  localrepeatlayout->addStretch();
  locallayout->addRow(tr("Search failure action"),localrepeatlayout);//m_textRepeat);

  QPushButton * localbtn = new QPushButton(tr("Set"));
  QHBoxLayout * setlayout3 = new QHBoxLayout;
  setlayout3->addWidget(localbtn);
  setlayout3->addStretch();
  locallayout->addRow(tr("Search dialog options"),setlayout3);

  connect(localbtn,SIGNAL(clicked()),this,SLOT(onLocalDialog()));

  localbox->setLayout(locallayout);

  ///
  QGroupBox * tabbox = new QGroupBox(tr("Tab options for root,page and node search"));
  QGridLayout * gridlayout = new QGridLayout;

  m_nodeNew = new  QCheckBox;
  m_nodeGo  = new  QCheckBox;
  m_pageNew = new  QCheckBox;
  m_pageGo  = new  QCheckBox;
  m_rootNew = new  QCheckBox;
  m_rootGo  = new  QCheckBox;
  m_rootKeymap = new  QCheckBox;
  m_rootRepeat = new QComboBox;
  m_rootRepeat->addItems(TextSearchDialog::failureActions());

  gridlayout->addWidget(new QLabel("Root"),0,1);
  gridlayout->addWidget(new QLabel("Page"),0,2);
  gridlayout->addWidget(new QLabel("Node"),0,3);

  gridlayout->addWidget(new QLabel("New tab"),1,0);
  gridlayout->addWidget(new QLabel("Activate tab"),2,0);

  gridlayout->addWidget(m_rootNew,1,1);
  gridlayout->addWidget(m_pageNew,1,2);
  gridlayout->addWidget(m_nodeNew,1,3);
  gridlayout->addWidget(m_rootGo,2,1);
  gridlayout->addWidget(m_pageGo,2,2);
  gridlayout->addWidget(m_nodeGo,2,3);
  tabbox->setLayout(gridlayout);

  QGroupBox * otherbox = new QGroupBox(tr("Root search"));
  QFormLayout * otherlayout = new QFormLayout;
  otherlayout->addRow(tr("Root search show keymap button"),m_rootKeymap);
  QHBoxLayout * rootrepeatlayout = new QHBoxLayout;
  rootrepeatlayout->addWidget(m_rootRepeat);
  rootrepeatlayout->addStretch();
  otherlayout->addRow(tr("Search failure action"),rootrepeatlayout);//m_textRepeat);
  otherbox->setLayout(otherlayout);

  layout->addWidget(fullbox);
  layout->addWidget(headbox);
  layout->addWidget(localbox);
  layout->addWidget(otherbox);
  layout->addWidget(tabbox);
  layout->addStretch();

  setLayout(layout);
  addButtons();
  readSettings();
  setupConnections();
  //  getLexicon()->setCursorPosition(this);
}

void FindOptions::readSettings(bool /* reload */) {
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup("TextSearch");

  //  m_fullDebug->setChecked(settings.value(SID_FULLSEARCH_DEBUG,false).toBool());
  m_textFragmentSize->setValue(settings.value(SID_TEXTSEARCH_FRAGMENT_SIZE,50).toInt());
  //  m_fullIncludeHeads->setChecked(settings.value(SID_FULLSEARCH_INCLUDE_HEADS,true).toBool());
  m_textSummary->setChecked(settings.value(SID_TEXTSEARCH_SUMMARY_FORMAT,false).toBool());
  m_textStep->setValue(settings.value(SID_TEXTSEARCH_STEP,50).toInt());
  m_contextStyle->setText(settings.value(SID_FULLSEARCH_CONTEXT_STYLE,QString()).toString());
  m_textNewTab     = settings.value(SID_TEXTSEARCH_NEW_TAB,true).toBool();
  m_textGoTab      = settings.value(SID_TEXTSEARCH_GO_TAB,true).toBool();
  m_textWholeWord  = settings.value(SID_TEXTSEARCH_WHOLE_WORD,false).toBool();
  m_textDiacritics = settings.value(SID_TEXTSEARCH_DIACRITICS,true).toBool();
  m_textRegex      = settings.value(SID_TEXTSEARCH_TYPE_REGEX,false).toBool();
  m_textCase = settings.value(SID_TEXTSEARCH_IGNORE_CASE,true).toBool();
  m_textRepeat->setCurrentIndex(settings.value(SID_TEXTSEARCH_FAILURE_ACTION,TextSearchDialog::SearchAgainYes).toInt());
  m_textCurrentPage->setChecked(settings.value(SID_TEXTSEARCH_CURRENTPAGE_ONLY,false).toBool());
  m_textPageSize->setValue(settings.value(SID_TEXTSEARCH_PAGE_SIZE,100).toInt());
  m_textKeymap = settings.value(SID_TEXTSEARCH_KEYMAP_BUTTON,false).toBool();
  settings.endGroup();
  settings.beginGroup("HeadSearch");
  // head word search

  m_headPhrase->setChecked(settings.value(SID_HEADSEARCH_USE_PHRASE,false).toBool());
  m_nodeinfoClose->setChecked(settings.value(SID_HEADSEARCH_NODEINFO_CLOSE,true).toBool());
  m_nodeinfoForce->setChecked(settings.value(SID_HEADSEARCH_NODEINFO_FORCE,true).toBool());
  m_headStep->setValue(settings.value(SID_HEADSEARCH_STEP,50).toInt());
  m_headRepeat->setCurrentIndex(settings.value(SID_HEADSEARCH_FAILURE_ACTION,TextSearchDialog::SearchAgainYes).toInt());

  m_headNewTab     = settings.value(SID_HEADSEARCH_NEW_TAB,true).toBool();
  m_headGoTab      = settings.value(SID_HEADSEARCH_GO_TAB,true).toBool();
  m_headWholeWord  = settings.value(SID_HEADSEARCH_WHOLE_WORD,true).toBool();
  m_headDiacritics = settings.value(SID_HEADSEARCH_DIACRITICS,true).toBool();
  m_headRegex      = settings.value(SID_HEADSEARCH_TYPE_REGEX,true).toBool();
  m_headKeymap = settings.value(SID_HEADSEARCH_KEYMAP_BUTTON,false).toBool();

  settings.endGroup();
  settings.beginGroup("LocalSearch");
  m_localWholeWord  = settings.value(SID_LOCALSEARCH_WHOLE_WORD,true).toBool();
  m_localDiacritics = settings.value(SID_LOCALSEARCH_DIACRITICS,true).toBool();
  m_localRegex      = settings.value(SID_LOCALSEARCH_TYPE_REGEX,true).toBool();
  m_ignoreCase      = settings.value(SID_LOCALSEARCH_IGNORE_CASE,true).toBool();
  m_localKeymap = settings.value(SID_LOCALSEARCH_KEYMAP_BUTTON,false).toBool();
  m_localShowAll->setChecked(settings.value(SID_LOCALSEARCH_SHOW_ALL,true).toBool());
  m_localRepeat->setCurrentIndex(settings.value(SID_LOCALSEARCH_FAILURE_ACTION,TextSearchDialog::SearchAgainYes).toInt());

  settings.endGroup();
  settings.beginGroup("Search");

  m_nodeNew->setChecked(settings.value(SID_NODESEARCH_NEW_TAB,true).toBool());
  m_nodeGo->setChecked(settings.value(SID_NODESEARCH_GO_TAB,true).toBool());
  m_pageNew->setChecked(settings.value(SID_PAGESEARCH_NEW_TAB,true).toBool());
  m_pageGo->setChecked(settings.value(SID_PAGESEARCH_GO_TAB,true).toBool());
  m_rootNew->setChecked(settings.value(SID_ROOTSEARCH_NEW_TAB,true).toBool());
  m_rootGo->setChecked(settings.value(SID_ROOTSEARCH_GO_TAB,true).toBool());
  m_rootKeymap->setChecked(settings.value(SID_ROOTSEARCH_KEYMAP_BUTTON,false).toBool());
  m_rootRepeat->setCurrentIndex(settings.value(SID_ROOTSEARCH_FAILURE_ACTION,TextSearchDialog::SearchAgainYes).toInt());

  m_dirty = false;
}
void FindOptions::writeSettings(const QString & fileName) {
  QString f = m_settingsFileName;
  if (!fileName.isEmpty()) {
    f = fileName;
  }

  QSettings settings(f,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup("TextSearch");

  //  settings.setValue(SID_FULLSEARCH_DEBUG,m_fullDebug->isChecked());
  settings.setValue(SID_TEXTSEARCH_PAGE_SIZE,m_textPageSize->value());
  settings.setValue(SID_TEXTSEARCH_FRAGMENT_SIZE,m_textFragmentSize->value());
  settings.setValue(SID_TEXTSEARCH_SUMMARY_FORMAT,m_textSummary->isChecked());
  settings.setValue(SID_TEXTSEARCH_CURRENTPAGE_ONLY,m_textCurrentPage->isChecked());
  settings.setValue(SID_TEXTSEARCH_STEP,m_textStep->value());
  settings.setValue(SID_TEXTSEARCH_NEW_TAB,m_textNewTab);
  settings.setValue(SID_TEXTSEARCH_GO_TAB,m_textGoTab);
  settings.setValue(SID_TEXTSEARCH_WHOLE_WORD,m_textWholeWord);
  settings.setValue(SID_TEXTSEARCH_DIACRITICS,m_textDiacritics);
  settings.setValue(SID_TEXTSEARCH_TYPE_REGEX,m_textRegex);
  settings.setValue(SID_TEXTSEARCH_IGNORE_CASE,m_textCase);
  settings.setValue(SID_TEXTSEARCH_KEYMAP_BUTTON,m_textKeymap);
  settings.setValue(SID_TEXTSEARCH_CONTEXT_STYLE,m_contextStyle->text());
  settings.setValue(SID_TEXTSEARCH_FAILURE_ACTION,m_textRepeat->currentIndex());
  settings.endGroup();
  settings.beginGroup("HeadSearch");

  settings.setValue(SID_HEADSEARCH_USE_PHRASE,m_headPhrase->isChecked());
  /*
  settings.setValue(SID_HEADSEARCH_VERTICAL_LAYOUT,m_headVertical->isChecked());
  settings.setValue(SID_HEADSEARCH_FOCUS_TABLE,m_headFocusTable->isChecked());

  */
  settings.setValue(SID_HEADSEARCH_NODEINFO_CLOSE,m_nodeinfoClose->isChecked());
  settings.setValue(SID_HEADSEARCH_NODEINFO_FORCE,m_nodeinfoForce->isChecked());
  settings.setValue(SID_HEADSEARCH_STEP,m_headStep->value());
  settings.setValue(SID_HEADSEARCH_NEW_TAB,m_headNewTab);
  settings.setValue(SID_HEADSEARCH_GO_TAB,m_headGoTab);
  settings.setValue(SID_HEADSEARCH_WHOLE_WORD,m_headWholeWord );
  settings.setValue(SID_HEADSEARCH_DIACRITICS,m_headDiacritics);
  settings.setValue(SID_HEADSEARCH_TYPE_REGEX,m_headRegex);
  settings.setValue(SID_HEADSEARCH_KEYMAP_BUTTON,m_headKeymap);
  settings.setValue(SID_HEADSEARCH_FAILURE_ACTION,m_headRepeat->currentIndex());


  settings.endGroup();
  settings.beginGroup("LocalSearch");

   settings.setValue(SID_LOCALSEARCH_WHOLE_WORD,m_localWholeWord);
   settings.setValue(SID_LOCALSEARCH_DIACRITICS,m_localDiacritics);
   settings.setValue(SID_LOCALSEARCH_TYPE_REGEX,m_localRegex);
   settings.setValue(SID_LOCALSEARCH_KEYMAP_BUTTON,m_localKeymap);
   settings.setValue(SID_LOCALSEARCH_IGNORE_CASE,m_ignoreCase);
   settings.setValue(SID_LOCALSEARCH_FAILURE_ACTION,m_localRepeat->currentIndex());


  settings.value(SID_LOCALSEARCH_SHOW_ALL,m_localShowAll->isChecked());

  settings.endGroup();
  settings.beginGroup("Search");

  settings.setValue(SID_NODESEARCH_NEW_TAB,m_nodeNew->isChecked());
  settings.setValue(SID_NODESEARCH_GO_TAB,m_nodeGo->isChecked());
  settings.setValue(SID_PAGESEARCH_NEW_TAB,m_pageNew->isChecked());
  settings.setValue(SID_PAGESEARCH_GO_TAB,m_pageGo->isChecked());
  settings.setValue(SID_ROOTSEARCH_NEW_TAB,m_rootNew->isChecked());
  settings.setValue(SID_ROOTSEARCH_GO_TAB,m_rootGo->isChecked());
  settings.setValue(SID_ROOTSEARCH_KEYMAP_BUTTON,m_rootKeymap->isChecked());
  settings.setValue(SID_ROOTSEARCH_FAILURE_ACTION,m_rootRepeat->currentIndex());

  settings.sync();
  settings.endGroup();
  emit(modified(false));
}
/**
 * TODO not complete
 *
 *
 * @return
 */
bool FindOptions::isModified()  {
  m_dirty = false;
  m_changes.clear();

  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup("TextSearch");

  /*
  if (compare(&settings,SID_FULLSEARCH_DEBUG,m_fullDebug))  {
    m_dirty = true;
  }
  */
  if (compare(&settings,SID_TEXTSEARCH_CONTEXT_STYLE,m_contextStyle)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_TEXTSEARCH_FRAGMENT_SIZE,m_textFragmentSize)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_TEXTSEARCH_PAGE_SIZE,m_textPageSize)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_TEXTSEARCH_SUMMARY_FORMAT,m_textSummary)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_TEXTSEARCH_STEP,m_textStep)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_TEXTSEARCH_CURRENTPAGE_ONLY,m_textCurrentPage)) {
    m_dirty = true;
  }
  if (m_textNewTab     != settings.value(SID_TEXTSEARCH_NEW_TAB,true).toBool()) {
    m_dirty = true;
  }

  if (m_textGoTab      != settings.value(SID_TEXTSEARCH_GO_TAB,true).toBool()) {
    m_dirty = true;
  }

  if (m_textWholeWord  != settings.value(SID_TEXTSEARCH_WHOLE_WORD,true).toBool()) {
    m_dirty = true;
  }

  if (m_textDiacritics != settings.value(SID_TEXTSEARCH_DIACRITICS,true).toBool()) {
    m_dirty = true;
  }

  if (m_textRegex      != settings.value(SID_TEXTSEARCH_TYPE_REGEX,true).toBool()) {
    m_dirty = true;
  }

  if (m_textCase      != settings.value(SID_TEXTSEARCH_IGNORE_CASE,true).toBool()) {
    m_dirty = true;
  }
  if (m_textKeymap     != settings.value(SID_TEXTSEARCH_KEYMAP_BUTTON,true).toBool()) {
    m_dirty = true;
  }
  if (compare(&settings,SID_TEXTSEARCH_FAILURE_ACTION,m_textRepeat)) {
    m_dirty = true;
  }


  settings.endGroup();
  settings.beginGroup("HeadSearch");

  if (compare(&settings,SID_HEADSEARCH_USE_PHRASE,m_headPhrase)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_HEADSEARCH_NODEINFO_CLOSE,m_nodeinfoClose)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_HEADSEARCH_NODEINFO_FORCE,m_nodeinfoForce)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_HEADSEARCH_STEP,m_headStep)) {
    m_dirty = true;
  }

  if (m_headNewTab     != settings.value(SID_HEADSEARCH_NEW_TAB,true).toBool()) {
    m_dirty = true;
  }

  if (m_headGoTab      != settings.value(SID_HEADSEARCH_GO_TAB,true).toBool()) {
    m_dirty = true;
  }

  if (m_headWholeWord  != settings.value(SID_HEADSEARCH_WHOLE_WORD,true).toBool()) {
    m_dirty = true;
  }

  if (m_headDiacritics != settings.value(SID_HEADSEARCH_DIACRITICS,true).toBool()) {
    m_dirty = true;
  }

  if (m_headRegex      != settings.value(SID_HEADSEARCH_TYPE_REGEX,true).toBool()) {
    m_dirty = true;
  }

  if (m_headKeymap      != settings.value(SID_HEADSEARCH_KEYMAP_BUTTON,true).toBool()) {
     m_dirty = true;
  }
  if (compare(&settings,SID_HEADSEARCH_FAILURE_ACTION,m_headRepeat)) {
    m_dirty = true;
  }


  settings.endGroup();
  settings.beginGroup("LocalSearch");

  if (compare(&settings,SID_LOCALSEARCH_SHOW_ALL,m_localShowAll)) {
    m_dirty = true;
  }

  if (m_localWholeWord  != settings.value(SID_LOCALSEARCH_WHOLE_WORD,true).toBool()) {
    m_dirty = true;
  }

  if (m_localDiacritics != settings.value(SID_LOCALSEARCH_DIACRITICS,true).toBool()) {
    m_dirty = true;
  }

  if (m_localRegex      != settings.value(SID_LOCALSEARCH_TYPE_REGEX,true).toBool()) {
    m_dirty = true;
  }

  if (m_localKeymap      != settings.value(SID_LOCALSEARCH_KEYMAP_BUTTON,true).toBool()) {
      m_dirty = true;
  }

  if (m_ignoreCase      != settings.value(SID_LOCALSEARCH_IGNORE_CASE,true).toBool()) {
    m_dirty = true;
  }
 if (compare(&settings,SID_LOCALSEARCH_FAILURE_ACTION,m_localRepeat)) {
    m_dirty = true;
  }

  settings.endGroup();
  settings.beginGroup("Search");


  if (compare(&settings,SID_NODESEARCH_NEW_TAB,m_nodeNew)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_NODESEARCH_GO_TAB,m_nodeGo)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_PAGESEARCH_NEW_TAB,m_pageNew)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_PAGESEARCH_GO_TAB,m_pageGo)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ROOTSEARCH_NEW_TAB,m_rootNew)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ROOTSEARCH_GO_TAB,m_rootGo)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ROOTSEARCH_KEYMAP_BUTTON,m_rootKeymap)) {
    m_dirty = true;
  }
 if (compare(&settings,SID_ROOTSEARCH_FAILURE_ACTION,m_rootRepeat)) {
    m_dirty = true;
  }

  return m_dirty;
}
void FindOptions::onSetFont() {
  /*
  bool arabic = false;
  QPushButton * button = qobject_cast<QPushButton *>(QObject::sender());
  QFont f;
  if (button == m_standardButton) {
    f.fromString(m_standardFont->text());
  }
  else {
    arabic = true;
    f.fromString(m_arabicFont->text());
  }
  if (arabic) {
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
      m_arabicFont->setText(font.toString());
    }
    delete d;
    return;
  }
  bool ok;
  QFont font = QFontDialog::getFont(&ok, f,  this);
  if (!ok) {
    return;
  }
  m_standardFont->setText(font.toString());
  */
}
void FindOptions::onSetColor() {
}
void FindOptions::onFullDialog() {
  DialogOptions d;
  d.setWindowTitle(tr("Text Search Options"));
  d.setChecked(DialogOptions::Tab,m_textNewTab);
  d.setChecked(DialogOptions::Go,m_textGoTab);
  d.setChecked(DialogOptions::Whole,m_textWholeWord);
  d.setChecked(DialogOptions::Diacritics,m_textDiacritics);
  d.setChecked(DialogOptions::Regex,m_textRegex);
  d.setChecked(DialogOptions::IgnoreCase,m_textCase);
  d.setChecked(DialogOptions::Keymap,m_textKeymap);
  if (d.exec() == QDialog::Accepted) {
    m_textNewTab =  d.isChecked(DialogOptions::Tab);
    m_textGoTab = d.isChecked(DialogOptions::Go);
    m_textWholeWord = d.isChecked(DialogOptions::Whole);
    m_textDiacritics = d.isChecked(DialogOptions::Diacritics);
    m_textRegex = d.isChecked(DialogOptions::Regex);
    m_textCase = d.isChecked(DialogOptions::IgnoreCase);
    m_textKeymap = d.isChecked(DialogOptions::Keymap);

    bool v = isModified();
    setButtons(v);
    emit(modified(v));
  }
}
void FindOptions::onHeadDialog() {
  DialogOptions d;
  d.setWindowTitle(tr("Head Search Options"));
  d.setChecked(DialogOptions::Tab,m_headNewTab);
  d.setChecked(DialogOptions::Go,m_headGoTab);
  d.setChecked(DialogOptions::Whole,m_headWholeWord);
  d.setChecked(DialogOptions::Diacritics,m_headDiacritics);
  d.setChecked(DialogOptions::Regex,m_headRegex);
  d.setChecked(DialogOptions::Keymap,m_headKeymap);
  if (d.exec() == QDialog::Accepted) {
    m_headNewTab =  d.isChecked(DialogOptions::Tab);
    m_headGoTab = d.isChecked(DialogOptions::Go);
    m_headWholeWord = d.isChecked(DialogOptions::Whole);
    m_headDiacritics = d.isChecked(DialogOptions::Diacritics);
    m_headRegex = d.isChecked(DialogOptions::Regex);
    m_headKeymap = d.isChecked(DialogOptions::Keymap);
    bool v = isModified();
    setButtons(v);
    emit(modified(v));
  }
}
void FindOptions::onLocalDialog() {
  DialogOptions d;
  d.setWindowTitle(tr("Local Search Options"));
  d.enableOption(DialogOptions::Tab,false);
  d.enableOption(DialogOptions::Go,false);
  d.setChecked(DialogOptions::Whole,m_localWholeWord);
  d.setChecked(DialogOptions::Diacritics,m_localDiacritics);
  d.setChecked(DialogOptions::Regex,m_localRegex);
  d.setChecked(DialogOptions::IgnoreCase,m_ignoreCase);
  d.setChecked(DialogOptions::Keymap,m_localKeymap);
  if (d.exec() == QDialog::Accepted) {
    m_localWholeWord = d.isChecked(DialogOptions::Whole);
    m_localDiacritics = d.isChecked(DialogOptions::Diacritics);
    m_localRegex = d.isChecked(DialogOptions::Regex);
    m_ignoreCase = d.isChecked(DialogOptions::IgnoreCase);
    m_localKeymap = d.isChecked(DialogOptions::Keymap);
    emit(modified(true));
    bool v = isModified();
    setButtons(v);
    emit(modified(v));
  }
}
