#include "textsearchdialog.h"
#include "QsLog.h"
#include "laneslexicon.h"
#include "externs.h"
#include "definedsettings.h"
extern Lexicon * getLexicon();

TextSearchDialog::TextSearchDialog(QWidget * parent,Qt::WindowFlags f) :
  QDialog(parent,f) {
  setObjectName("arabicsearchdialog");
  setWindowTitle(tr("Text Search"));

  m_type = SearchOptions::Word;
  m_prompt = new QLabel(tr("Find"));
  m_edit = new ImLineEdit;
  m_edit->setObjectName("arabicedit");
  //  m_edit->readSettings(getLexicon()->settingsFileName());
  getLexicon()->adjustHeight(m_edit);
  QString mapname = getApp()->getActiveKeymap();
  QString mapfile = getApp()->getKeymapFileName(mapname);

  QMapIterator<QString,QString> iter = getApp()->getMapIterator();
  while(iter.hasNext()) {
    iter.next();
    m_edit->loadMap(iter.value(),iter.key());
  }
  m_edit->setCurrentMap(mapname);
  //  m_edit->enableMapping(false);
  m_prompt->setBuddy(m_edit);
  m_mapEnabled = m_edit->isMappingEnabled();
  m_text = new QLabel;

  m_findButton = new QPushButton(tr("&Find"));

  m_findButton->setEnabled(false);
  m_keyboardButton  = new QPushButton(tr("Show &keyboard"));
  m_keyboardButton->setAutoDefault(false);
  m_keymapButton  = new QPushButton(tr(""));
  m_keymapButton->setCheckable(true);


  /*
  m_moreButton = new QPushButton(tr("&More"));
  m_moreButton->setCheckable(true);
  m_moreButton->setAutoDefault(false);
  */
  //
  // the order of the buttons in the QDialogButtonBox depends
  // on the platform and the role of each button
  //
  m_buttonBox = new QDialogButtonBox(Qt::Vertical);
  m_buttonBox->addButton(m_findButton, QDialogButtonBox::AcceptRole);

  QPushButton * button = m_buttonBox->addButton(QDialogButtonBox::Cancel);
  button->setText(tr("&Cancel"));
  m_buttonBox->addButton(m_keyboardButton, QDialogButtonBox::ActionRole);
  m_buttonBox->addButton(m_keymapButton, QDialogButtonBox::ActionRole);
  QPushButton * helpbutton = m_buttonBox->addButton(QDialogButtonBox::Help);
  connect(helpbutton,SIGNAL(clicked()),this,SLOT(onHelp()));
  m_findButton->setFocus();

  connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(m_keyboardButton, SIGNAL(clicked()),this,SLOT(showKeyboard()));
  connect(m_keymapButton, SIGNAL(clicked()),this,SLOT(keymapChanged()));
  connect(m_edit,SIGNAL(textChanged(const QString &)),this,SLOT(onTextChanged(const QString &)));
  //  m_mapEnabled = m_edit->isMappingEnabled();
  m_keymapButton->setChecked(m_mapEnabled);

  QVBoxLayout * leftlayout = new QVBoxLayout;

  QGridLayout * inputlayout = new QGridLayout;
  inputlayout->addWidget(m_prompt,0,0);
  inputlayout->addWidget(m_edit,0,1);
  inputlayout->addWidget(m_text,1,1);
  leftlayout->addLayout(inputlayout);



  m_regexSearch = new QRadioButton(tr("Regular expression"));
  m_normalSearch = new QRadioButton(tr("Normal"));
  m_wholeWord = new QCheckBox(tr("Whole word match"));
  m_ignoreCase = new QCheckBox(tr("Ignore case"));
  m_ignoreDiacritics = new QCheckBox(tr("Ignore diacritics"));
  m_goTab = new QCheckBox(tr("Activate tab"));
  m_newTab = new QCheckBox(tr("Open new tab"));
  m_headPhrase = new QCheckBox(tr("Search head phrase"));
  m_highlightAll = new QCheckBox(tr("Highlight all"));

  m_typeGroup = new QGroupBox(tr("Search type"));
  QHBoxLayout * typeLayout = new QHBoxLayout;
  typeLayout->addWidget(m_normalSearch);
  typeLayout->addWidget(m_regexSearch);
  m_typeGroup->setLayout(typeLayout);

  m_form = new QGridLayout;
  m_form->addWidget(m_typeGroup,0,0,1,2);
  m_form->addWidget(m_wholeWord,1,0);
  m_form->addWidget(m_ignoreCase,1,1);
  m_form->addWidget(m_ignoreDiacritics,2,0);
  m_form->addWidget(m_headPhrase,3,0);
  m_form->addWidget(m_highlightAll,4,0);
  m_form->addWidget(m_newTab,5,0);
  m_form->addWidget(m_goTab,5,1);

  leftlayout->addLayout(m_form);

  m_headPhrase->setVisible(false);
  m_highlightAll->setVisible(false);

  connect(m_regexSearch,SIGNAL(toggled(bool)),this,SLOT(searchTypeChanged(bool)));
  connect(m_newTab,SIGNAL(stateChanged(int)),this,SLOT(newTabChanged(int)));

  leftlayout->addStretch();

  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addLayout(leftlayout);
  hlayout->addWidget(m_buttonBox);

  QVBoxLayout * mainlayout = new QVBoxLayout;
  mainlayout->addLayout(hlayout);
  setLayout(mainlayout);

  m_edit->setFocus();

  m_attached = false;
  /**
   * initialise the KeyboardWidget telling where the keyboard definitions are and the
   * name of the config file (which tells it what the default one is).
   */

  SETTINGS
  settings.beginGroup("Keyboards");
  QString keyboardConfig = settings.value(SID_KEYBOARDS_CONFIG,"keyboard.ini").toString();
  m_keyboard = new KeyboardWidget(getLexicon()->getResourceFilePath(Lexicon::Keyboard),keyboardConfig,this);

  QList<QAbstractButton *> buttons = m_buttonBox->buttons();
  for(int i=0;i < buttons.size();i++) {
    QString k = buttons[i]->shortcut().toString();
    if (! k.isEmpty()) {
      m_keyboard->addShortcut(k);
    }
  }
  settings.endGroup();
  settings.beginGroup("Maps");
  m_edit->enableMapping(settings.value(SID_MAPS_ENABLED,false).toBool());
   QPoint p = this->pos();
  int h = this->frameGeometry().height();
  m_keyboard->move(p.x(),p.y() + h);
  connect(m_keyboard,SIGNAL(closed()),this,SLOT(keyboardClosed()));
  connect(m_keyboard,SIGNAL(keyboardShortcut(const QString &)),this,SLOT(onKeyboardShortcut(const QString &)));

  m_type = SearchOptions::Text;
  readSettings();

  keymapChanged();
  m_findButton->setDefault(true);
}
void TextSearchDialog::onKeyboardShortcut(const QString & key) {
  if (key == m_keyboardButton->shortcut().toString()) {
      showKeyboard();
      return;
    }
  QPushButton * button = m_buttonBox->button(QDialogButtonBox::Cancel);
  if (button) {
    if (button->shortcut().toString() == key) {
      button->animateClick();
      return;
    }
  }
  if (key == m_findButton->shortcut().toString()) {
    m_findButton->animateClick();
    return;
  }
  //  if (key == m_moreButton->shortcut().toString()) {
  //    m_moreButton->animateClick();
  //  }
}
TextSearchDialog::~TextSearchDialog() {
  this->hideKeyboard();
  if (m_type != SearchOptions::Root) {
    SETTINGS
    //TODO make this dependent on type root/head/local/text
      settings.beginGroup("TextSearch");
    settings.setValue(SID_TEXTSEARCH_DIALOG_SIZE, size());
    settings.setValue(SID_TEXTSEARCH_DIALOG_POS, pos());
  }
}
void TextSearchDialog::keyboardClosed() {
  showKeyboard();
}
void TextSearchDialog::hideKeyboard() {
  if (m_attached) {
    this->showKeyboard();
  }
}
void TextSearchDialog::newTabChanged(int /* state */) {
  m_goTab->setEnabled(m_newTab->isChecked());
}
void TextSearchDialog::showKeyboard() {
  if (! m_attached) {
    m_keyboard->attach(m_edit);
    m_keyboard->autoScale();
    m_keyboardButton->setText(tr("Hide &keyboard"));
    QPoint p;
    p = m_keyboard->currentPosition();
    if (p.isNull()) {
      p = this->pos();
      int h = this->frameGeometry().height();
      p.setX(p.x() - 50);
      p.setY(p.y() + h);
    }
    m_keyboard->move(p);
    m_attached = true;
  }
  else {
    m_keyboard->detach();
    m_keyboardButton->setText(tr("Show &keyboard"));
    m_attached = false;
  }
  m_edit->setFocus();
}
QString TextSearchDialog::getText() const {
  QString t = m_edit->text().trimmed();
  t.remove(QChar(0x202d));
  return t;
}
void TextSearchDialog::setPrompt(const QString & text) {
  m_prompt->setText(text);
}
void TextSearchDialog::loadKeymap(const QString & mapname) {
  m_edit->setCurrentMap(mapname);
}

void TextSearchDialog::setText(const QString & t) {
  m_edit->setText(t);
}
void TextSearchDialog::onHelp() {
  QLOG_DEBUG() << Q_FUNC_INFO << this->metaObject()->className();
  emit(showHelp(this->metaObject()->className()));
}
void TextSearchDialog::onTextChanged(const QString & txt) {
  //  QLOG_DEBUG() << Q_FUNC_INFO << txt;

  int rtlCount = 0;
  int ltrCount = 0;
  for(int i=0;i < txt.length();i++) {
    switch(txt.at(i).direction()) {
    case QChar::DirAL:
    case QChar::DirR:
    case QChar::DirAN:
      rtlCount++;
      break;
    case QChar::DirL:
    case QChar::DirEN:
      ltrCount++;
      break;
    default:
      break;
    }
  }
  QString str(txt);
  if ((rtlCount > 0) && (ltrCount > 0)) {
    m_edit->blockSignals(true);
    str.remove(QChar(0x202d));
    str = QString("%1%2").arg(QChar(0x202d)).arg(str);
    m_edit->setText(str);
    m_edit->blockSignals(false);
    m_text->setText(showText(txt));
    //    m_text->setText(getSupport()->scanAndStyle(txt,"pageset"));
  }
  else {
    m_text->setText("");
  }
  m_findButton->setEnabled(m_edit->text().length() > 0);
  //  QLOG_DEBUG() << QString("String: %1  RTL %2").arg(txt).arg(rtl);
}
QString TextSearchDialog::showText(const QString & txt) {


  int rtlCount = 0;
  int ltrCount = 0;
  QString p;
  QString cp;
  for(int i=0;i < txt.length();i++) {
    switch(txt.at(i).direction()) {
    case QChar::DirAL:
    case QChar::DirR:
    case QChar::DirAN:
      rtlCount++;
      p += "r";
      cp = "r";
      break;
    case QChar::DirL:
    case QChar::DirEN:
      ltrCount++;
      p += "l";
      cp = "l";
      break;
    default:
      p += cp;
      break;
    }
  }
  //  qDebug() << QString("%1 : [%2]").arg(p.size()).arg(p);
  QStringList words;
  QStringList css;
  int pos = 0;
  for(int i=1;i < p.length();i++) {
    if (p.at(i) != p.at(i-1)) {
      words << txt.mid(pos, i - pos);
      if (p.at(i-1) == 'r') {
        css << "ar";
      }
      else {
        css << "en";
      }
      pos = i;
    }
  }
  if (pos < p.length()) {
    words << txt.mid(pos);
    if (p.at(pos) == 'r') {
      css << "ar";
    }
    else {
      css << "en";
    }
  }
  // embed the Arabic font info in the style attribute as QLabel does not
  // support CSS classes
  QString style = getSupport()->getSpanStyle("searchsummary");
  QString html("<html><body>");
  for(int i=0;i < words.size();i++) {
    if (css[i] == "ar") {
      html += QString("<span style=\"%1\">%2</span>").arg(style).arg(words[i]);
    }
    else {
      html += QString("<span style=\"%1\">%2</span>").arg(css[i]).arg(words[i]);
    }
  }
  html += "</body></html>";
  return html;
}
/**
 * This will read the settings from textsearch
 * when used for local search these options are overriden by using the fromOption(options)
 *
 */
void TextSearchDialog::readSettings() {
  SETTINGS

  settings.beginGroup("Maps");

  bool enabled = settings.value(SID_MAPS_ENABLED,false).toBool();

  m_keymapButton->setChecked(enabled);
  settings.endGroup();
  settings.beginGroup("TextSearch");
  m_wholeWord->setChecked(settings.value(SID_TEXTSEARCH_WHOLE_WORD,false).toBool());
  m_ignoreCase->setChecked(settings.value(SID_TEXTSEARCH_IGNORE_CASE,true).toBool());
  m_ignoreDiacritics->setChecked(settings.value(SID_TEXTSEARCH_DIACRITICS,true).toBool());
  if (settings.value(SID_TEXTSEARCH_TYPE_REGEX,false).toBool()) {
    m_regexSearch->setChecked(true);
  }
  else {
    m_normalSearch->setChecked(true);
  }
  m_showKeymap.insert(SearchOptions::Word,settings.value(SID_TEXTSEARCH_KEYMAP_BUTTON,false).toBool());

  m_keymapButton->setVisible(settings.value(SID_TEXTSEARCH_KEYMAP_BUTTON,false).toBool());
  m_goTab->setChecked(settings.value(SID_TEXTSEARCH_GO_TAB,true).toBool());
  m_newTab->setChecked(settings.value(SID_TEXTSEARCH_NEW_TAB,true).toBool());

  resize(settings.value(SID_TEXTSEARCH_DIALOG_SIZE,QSize(580,230)).toSize());
  QPoint p = settings.value(SID_TEXTSEARCH_DIALOG_POS,QPoint()).toPoint();
  if (p.isNull()) {
  int width = this->frameGeometry().width();
  int height = this->frameGeometry().height();

  QDesktopWidget wid;

  int screenWidth = wid.screen()->width();
  int screenHeight = wid.screen()->height();

  this->setGeometry((screenWidth/2)-(width/2),(screenHeight/2)-(height/2),width,height);
  }
  else {
    move(p);
  }
  settings.endGroup();

  settings.beginGroup("HeadSearch");
  m_showKeymap.insert(SearchOptions::Entry,settings.value(SID_HEADSEARCH_KEYMAP_BUTTON,false).toBool());
  settings.endGroup();

  settings.beginGroup("LocalSearch");
  m_showKeymap.insert(SearchOptions::Local,settings.value(SID_LOCALSEARCH_KEYMAP_BUTTON,false).toBool());
  settings.endGroup();

  settings.beginGroup("Search");
  m_showKeymap.insert(SearchOptions::Root,settings.value(SID_ROOTSEARCH_KEYMAP_BUTTON,false).toBool());
  settings.endGroup();



}
void TextSearchDialog::searchTypeChanged(bool /* v */) {
  m_wholeWord->setEnabled(! m_regexSearch->isChecked());
  m_ignoreDiacritics->setEnabled(! m_regexSearch->isChecked());
}
TextOption TextSearchDialog::options() const {
  TextOption o;

  o.wholeWord =  m_wholeWord->isChecked();
  o.caseSensitive =  ! m_ignoreCase->isChecked();
  o.ignoreDiacritics = m_ignoreDiacritics->isChecked();
  o.regex =  m_regexSearch->isChecked();
  o.target = m_edit->text();
  return o;
}
QPair<bool,bool> TextSearchDialog::tabOptions() const {
  return qMakePair(m_newTab->isChecked(),m_goTab->isChecked());
}
bool TextSearchDialog::headPhrase() const {
  return m_headPhrase->isChecked();
}
void TextSearchDialog::showTabOptions(bool v) {
  m_goTab->setVisible(v);
  m_newTab->setVisible(v);
}
SearchOptions TextSearchDialog::searchOptions() const {
  SearchOptions o;

  o.setIgnoreDiacritics(m_ignoreDiacritics->isChecked());
  o.setWholeWordMatch(m_wholeWord->isChecked());
  o.setShowAll(m_highlightAll->isChecked());      // local search
  o.setIgnoreCase(m_ignoreCase->isChecked());
  o.setHeadPhrase(m_headPhrase->isChecked());     // headword search
  o.setPattern(m_edit->text());
  o.setNewTab(m_newTab->isChecked());
  o.setActivateTab(m_goTab->isChecked());
  if (m_regexSearch->isChecked()) {
    o.setSearchType(SearchOptions::Regex);
  }
  else {
    o.setSearchType(SearchOptions::Normal);
  }

  return o;
}
void TextSearchDialog::fromOptions(SearchOptions & o) {
  m_ignoreDiacritics->setChecked(o.ignoreDiacritics());
  m_wholeWord->setChecked(o.wholeWordMatch());
  m_highlightAll->setChecked(o.showAll());     // local search
  m_headPhrase->setChecked(o.headPhrase());    // head search
  m_ignoreCase->setChecked(o.ignoreCase());
  m_regexSearch->setChecked(o.regex());
  m_goTab->setChecked(o.activateTab());
  m_newTab->setChecked(o.newTab());


}
void TextSearchDialog::setForRoot() {
  m_type = SearchOptions::Root;
    m_wholeWord->setVisible(false);
    m_ignoreCase->setVisible(false);
    m_ignoreDiacritics->setVisible(false);
    m_headPhrase->setVisible(false);
    m_highlightAll->setVisible(false);
    m_typeGroup->setVisible(false);
    m_newTab->setVisible(true);
    m_goTab->setVisible(true);
    setWindowTitle(tr("Search for root"));
    m_keymapButton->setVisible(m_showKeymap.value(m_type));
    this->layout()->setSizeConstraint(QLayout::SetFixedSize); // shrink the dialog

}
void TextSearchDialog::setForHead() {
  m_type = SearchOptions::Entry;
    m_wholeWord->setVisible(true);
    m_ignoreCase->setVisible(true);
    m_ignoreDiacritics->setVisible(true);
    m_headPhrase->setVisible(true);
    m_highlightAll->setVisible(false);
    m_newTab->setVisible(true);
    m_goTab->setVisible(true);
    m_keymapButton->setVisible(m_showKeymap.value(m_type));
    setWindowTitle(tr("Search for headword"));
}
void TextSearchDialog::setForLocal() {
  m_type = SearchOptions::Local;
    m_wholeWord->setVisible(true);
    m_ignoreCase->setVisible(true);
    m_ignoreDiacritics->setVisible(true);
    m_headPhrase->setVisible(false);
    m_highlightAll->setVisible(true);
    m_newTab->setVisible(false);
    m_goTab->setVisible(false);
    m_keymapButton->setVisible(m_showKeymap.value(m_type));
    setWindowTitle(tr("Local search"));
}
void TextSearchDialog::keymapChanged() {
  if (m_keymapButton->isChecked()) {
  m_edit->enableMapping(true);
  m_keymapButton->setText(tr("Keymap enabled"));
  }
  else {
    m_edit->enableMapping(false);
    m_keymapButton->setText(tr("Keymap disabled"));
  }
  m_edit->setFocus();
}
/**
 * Have to be in the same order as the NotFound_t enum
 *
 *
 * @return
 */
QStringList TextSearchDialog::failureActions() {
  QStringList s;
  s << tr("Search again dialog, default Yes");
  s << tr("Search again dialog, default No");
  s << tr("Search dialog and status message");
  s << tr("No dialog, status message only");
  return s;
}
