#include "textsearchdialog.h"
#include "QsLog.h"
#include "searchoptionswidget.h"
#include "laneslexicon.h"
#include "externs.h"
#include "definedsettings.h"
extern Lexicon * getLexicon();

TextSearchDialog::TextSearchDialog(SearchOptions::SearchScope_t searchType,QWidget * parent,Qt::WindowFlags f) :
  QDialog(parent,f) {
  setObjectName("arabicsearchdialog");
  m_searchType = searchType;
  QLOG_DEBUG() << Q_FUNC_INFO << searchType;
  switch(searchType) {
  case SearchOptions::Root :
    setWindowTitle(tr("Search for Root"));
    break;
  case SearchOptions::Entry :
    setWindowTitle(tr("Search for Head Word"));
    break;
  case SearchOptions::Word :
    setWindowTitle(tr("Search for Word"));
    break;
  case SearchOptions::Local :
    setWindowTitle(tr("Search current page"));
    break;
  case SearchOptions::Text :
    setWindowTitle(tr("Text Search"));
    break;
  default :
    setWindowTitle("Unknown search type");
    break;
  }


  m_count = 0;
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
  m_edit->enableMapping(false);
  m_prompt->setBuddy(m_edit);

  m_text = new QLabel;
  m_findButton = new QPushButton(tr("&Find"));
  m_findButton->setDefault(true);

  m_keyboardButton  = new QPushButton(tr("Show &keyboard"));
  m_keyboardButton->setAutoDefault(false);

  m_moreButton = new QPushButton(tr("&More"));
  m_moreButton->setCheckable(true);
  m_moreButton->setAutoDefault(false);



  m_buttonBox = new QDialogButtonBox(Qt::Vertical);
  m_buttonBox->addButton(m_findButton, QDialogButtonBox::AcceptRole);
  //  m_buttonBox->addButton(new QPushButton("&Cancel"),QDialogButtonBox::RejectRole);
  QPushButton * button = m_buttonBox->addButton(QDialogButtonBox::Cancel);
  button->setText(tr("&Cancel"));
  m_buttonBox->addButton(m_moreButton, QDialogButtonBox::ActionRole);

  if (searchType == SearchOptions::Word) {
    m_moreButton->setVisible(true);
  }
  else {
    m_moreButton->setVisible(false);
  }
  m_buttonBox->addButton(m_keyboardButton,QDialogButtonBox::ActionRole);
  QPushButton * helpbutton = m_buttonBox->addButton(QDialogButtonBox::Help);
  connect(helpbutton,SIGNAL(clicked()),this,SLOT(onHelp()));
  m_findButton->setFocus();

  connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(m_keyboardButton, SIGNAL(clicked()),this,SLOT(showKeyboard()));
  connect(m_edit,SIGNAL(textChanged(const QString &)),this,SLOT(onTextChanged(const QString &)));
  m_options = new SearchOptionsWidget(searchType,this);

  connect(m_options,SIGNAL(force(bool)),m_edit,SLOT(setForceLTR(bool)));

  connect(m_options,SIGNAL(loadKeymap(const QString &)),this,SLOT(loadKeymap(const QString &)));

  QStringList maps = m_edit->getMaps();
  QString map = m_edit->currentMap();
  /*
  if (map.isEmpty()) {
    map = m_edit->getNullMap();
  }
  maps << m_edit->getNullMap();
  */
  m_options->addKeymaps(map,maps);
  m_options->setOptions(m_searchType);
  m_mapEnabled = m_edit->isMappingEnabled();
  connect(m_moreButton, SIGNAL(toggled(bool)), this, SLOT(showOptions(bool)));

  QVBoxLayout * leftlayout = new QVBoxLayout;

  // QHBoxLayout * inputlayout = new QHBoxLayout;
  QGridLayout * inputlayout = new QGridLayout;
  inputlayout->addWidget(m_prompt,0,0);
  inputlayout->addWidget(m_edit,0,1);
  inputlayout->addWidget(m_text,1,1);
  leftlayout->addLayout(inputlayout);
  //  leftlayout->addWidget(m_text);
  leftlayout->addWidget(m_options);
  leftlayout->addStretch();

  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addLayout(leftlayout);
  hlayout->addStretch();
  hlayout->addWidget(m_buttonBox);

  QVBoxLayout * mainlayout = new QVBoxLayout;
  mainlayout->addLayout(hlayout);
  //  mainlayout->addStretch();
  setLayout(mainlayout);



  m_options->showMore(false);

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
  //  QLOG_DEBUG() << "Search dialog pos" << this->pos() << "mapped to global" <<  this->mapToGlobal(this->pos());
  //  QLOG_DEBUG() << "search dialog frame geometry" << this->frameGeometry();
  m_keyboard->move(p.x(),p.y() + h);
  connect(m_keyboard,SIGNAL(closed()),this,SLOT(keyboardClosed()));
  connect(m_keyboard,SIGNAL(keyboardShortcut(const QString &)),this,SLOT(onKeyboardShortcut(const QString &)));
  m_moreButton->setVisible(false);
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
  if (key == m_moreButton->shortcut().toString()) {
    m_moreButton->animateClick();
  }
}
TextSearchDialog::~TextSearchDialog() {
  this->hideKeyboard();
}
void TextSearchDialog::keyboardClosed() {
  showKeyboard();
}
void TextSearchDialog::hideKeyboard() {
  if (m_attached) {
    this->showKeyboard();
  }
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
void TextSearchDialog::showOptions(bool v) {
  m_options->showMore(v);
  if (!v)
    m_edit->setFocus();

  /// this shrinks the dialog
  this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}
QString TextSearchDialog::getText() const {
  QString t = m_edit->text().trimmed();
  t.remove(QChar(0x202d));
  return t;
}
void TextSearchDialog::setPrompt(const QString & text) {
  m_prompt->setText(text);
}
/*
void TextSearchDialog::setOptions(SearchOptions & opts) {
  opts.setSearchScope(m_searchType);
  m_options->setOptions(m_searchType);
}
*/
void TextSearchDialog::getOptions(SearchOptions & opts) const {
  m_options->getOptions(opts);
  opts.setPattern(m_edit->text());
  opts.setTextSearch(true);
}
void TextSearchDialog::setOptions(SearchOptions & opts)  {
  m_options->setOptions(opts);
  m_edit->setText(opts.pattern());
}
void TextSearchDialog::loadKeymap(const QString & mapname) {
  m_edit->setCurrentMap(mapname);
}
bool TextSearchDialog::getForceLTR() const {
  return m_options->getForceLTR();
}
void TextSearchDialog::setText(const QString & t) {
  m_edit->setText(t);
}
void TextSearchDialog::onHelp() {
  emit(showHelp(this->metaObject()->className()));
}
/*
void TextSearchDialog::languageSwitch(int  index ) {
  QLOG_DEBUG() << Q_FUNC_INFO;
  if (m_options->isTextSearch()) {
    m_edit->enableMapping(m_mapEnabled);
  }
  else {
    m_edit->enableMapping(false);
  }
  QLOG_DEBUG() << "text search" << m_options->isTextSearch();
  QLOG_DEBUG() << "edit map enabled" << m_edit->isMappingEnabled();
}
*/
void TextSearchDialog::onTextChanged(const QString & txt) {
  //  QLOG_DEBUG() << Q_FUNC_INFO << txt;
  bool rtl = false;
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
  }
  else {
    m_text->setText("");
  }

  //  QLOG_DEBUG() << QString("String: %1  RTL %2").arg(txt).arg(rtl);
}
QString TextSearchDialog::showText(const QString & txt) {

  bool rtl = false;
  int rtlCount = 0;
  int ltrCount = 0;
  int currentDir;
  int prevDir;
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
  // TODO set arabic font
  QString html("<body>");
  for(int i=0;i < words.size();i++) {
    html += QString("<span class=\"%1\">%2</span>").arg(css[i]).arg(words[i]);
  }
  html += "</html>";
  qDebug() << html;
  //  qDebug() << QString("%1 : [%2]").arg(txt.size()).arg(txt);
  //  qDebug() << "words" << words;
  return html;
}
