#include "searchdialogs.h"
#include "QsLog.h"
#include "namespace.h"
#include "searchoptionswidget.h"
#include "laneslexicon.h"
#include "externs.h"
#include "definedsettings.h"
extern Lexicon * getLexicon();

ArabicSearchDialog::ArabicSearchDialog(int searchType,QWidget * parent,Qt::WindowFlags f) :
  QDialog(parent,f) {
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
  default :
    setWindowTitle("Unknown search type");
    break;
  }


  m_count = 0;
  m_prompt = new QLabel(tr("Find"));
  m_edit = new ImLineEdit;

  //  m_edit->readSettings(getLexicon()->settingsFileName());
  getLexicon()->adjustHeight(m_edit);
  QString mapname = getApp()->getActiveKeymap();
  QString mapfile = getApp()->getKeymapFileName(mapname);

  if (! mapfile.isEmpty()) {
    m_edit->loadMap(mapfile,mapname);
    m_edit->setCurrentMap(mapname);
  }
  else {
    getApp()->enableKeymaps(false);
  }
  m_prompt->setBuddy(m_edit);
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

  m_findButton->setFocus();

  connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(m_keyboardButton, SIGNAL(clicked()),this,SLOT(showKeyboard()));

  m_options = new SearchOptionsWidget(searchType);

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

  connect(m_moreButton, SIGNAL(toggled(bool)), this, SLOT(showOptions(bool)));

  QVBoxLayout * leftlayout = new QVBoxLayout;

  QHBoxLayout * inputlayout = new QHBoxLayout;
  inputlayout->addWidget(m_prompt);
  inputlayout->addWidget(m_edit);

  leftlayout->addLayout(inputlayout);
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
void ArabicSearchDialog::onKeyboardShortcut(const QString & key) {
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
ArabicSearchDialog::~ArabicSearchDialog() {
}
void ArabicSearchDialog::keyboardClosed() {
  showKeyboard();
}
void ArabicSearchDialog::hideKeyboard() {
  if (m_attached) {
    this->showKeyboard();
  }
}
void ArabicSearchDialog::showKeyboard() {
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
}
void ArabicSearchDialog::showOptions(bool v) {
  m_options->showMore(v);
  if (!v)
    m_edit->setFocus();

  /// this shrinks the dialog
  this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}
QString ArabicSearchDialog::getText() const {
  QString t = m_edit->text().trimmed();
  t.remove(QChar(0x202d));
  return t;
}
void ArabicSearchDialog::setPrompt(const QString & text) {
  m_prompt->setText(text);
}
/*
void ArabicSearchDialog::setOptions(SearchOptions & opts) {
  opts.setSearchScope(m_searchType);
  m_options->setOptions(m_searchType);
}
*/
void ArabicSearchDialog::getOptions(SearchOptions & opts) const {
  m_options->getOptions(opts);
}
void ArabicSearchDialog::loadKeymap(const QString & mapname) {
  m_edit->setCurrentMap(mapname);
}
bool ArabicSearchDialog::getForceLTR() const {
  return m_options->getForceLTR();
}
void ArabicSearchDialog::setText(const QString & t) {
  m_edit->setText(t);
}
/**
 *
 *pp
 * @param parent
 * @param f
 */
NodeSearchDialog::NodeSearchDialog(QWidget * parent,Qt::WindowFlags f) :
  QDialog(parent,f) {
  setWindowTitle(tr("Search for node"));

  m_prompt = new QLabel(tr("Find &node"));
  m_edit = new QLineEdit;
  m_prompt->setBuddy(m_edit);

  m_newTab = new QCheckBox(tr("Open in &new tab"));
  m_switchFocus = new QCheckBox(tr("Switch to &new tab"));
  connect(m_newTab,SIGNAL(stateChanged(int)),this,SLOT(checkOptions(int)));

  m_findButton = new QPushButton(tr("&Find"));
  m_findButton->setDefault(true);


  m_options = new QWidget;

  m_buttonBox = new QDialogButtonBox(Qt::Vertical);
  m_buttonBox->addButton(m_findButton, QDialogButtonBox::AcceptRole);
  m_buttonBox->addButton(new QPushButton("&Cancel"),QDialogButtonBox::RejectRole);

  connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * optionsLayout = new QVBoxLayout;
  optionsLayout->setMargin(0);
  optionsLayout->addWidget(m_switchFocus);
  m_options->setLayout(optionsLayout);

  //  connect(m_moreButton, SIGNAL(toggled(bool)), this, SLOT(showOptions(bool)));
  QHBoxLayout *topLeftLayout = new QHBoxLayout;
  topLeftLayout->addWidget(m_prompt);
  topLeftLayout->addWidget(m_edit);

  QVBoxLayout *leftLayout = new QVBoxLayout;
  leftLayout->addLayout(topLeftLayout);
  leftLayout->addWidget(m_newTab);

  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  mainLayout->addLayout(leftLayout, 0, 0);
  mainLayout->addWidget(m_buttonBox, 0, 1);
  mainLayout->addWidget(m_options, 1, 0, 1, 2);
  mainLayout->setRowStretch(2, 1);

  setOptions();
  setLayout(mainLayout);

  readSettings();
}
void NodeSearchDialog::readSettings() {
  SETTINGS

    settings.beginGroup("Search");
  m_newTab->setChecked(settings.value(SID_NODESEARCH_NEW_TAB,true).toBool());
  m_switchFocus->setChecked(settings.value(SID_NODESEARCH_GO_TAB,true).toBool());
}
QString NodeSearchDialog::getText() const {
  QString t = m_edit->text();
  if (! t.startsWith("n")) {
    t = "n" + t;
  }
  return t;
}
void NodeSearchDialog::getOptions(SearchOptions & opts) {
  opts.setNewTab(m_newTab->checkState() == Qt::Checked);
  opts.setActivateTab(m_switchFocus->checkState() == Qt::Checked);
}
void NodeSearchDialog::setOptions() {
  SETTINGS

  settings.setIniCodec("UTF-8");
  settings.beginGroup("Search");
  settings.beginGroup("Node");
  m_newTab->setChecked(settings.value(SID_PAGESEARCH_NEW_TAB,false).toBool());
  m_switchFocus->setChecked(settings.value(SID_PAGESEARCH_GO_TAB,false).toBool());
  checkOptions();
}
void NodeSearchDialog::checkOptions(int /* state */) {
  m_switchFocus->setEnabled(m_newTab->isChecked());
}
void NodeSearchDialog::setText(const QString & t) {
  m_edit->setText(t);
}
/**
 *
 *
 * @param parent
 * @param f
 */
PageSearchDialog::PageSearchDialog(QWidget * parent,Qt::WindowFlags f) :
  QDialog(parent,f) {
  setWindowTitle(tr("Search for page"));

  m_prompt = new QLabel(tr("Find &page"));
  m_edit = new QLineEdit;
  m_edit->setValidator(new QIntValidator(1,3100,this));
  m_prompt->setBuddy(m_edit);

  m_newTab = new QCheckBox(tr("Open in &new tab"));
  m_switchFocus = new QCheckBox(tr("Switch to &new tab"));


  connect(m_newTab,SIGNAL(stateChanged(int)),this,SLOT(checkOptions(int)));
  m_findButton = new QPushButton(tr("&Find"));
  m_findButton->setDefault(true);


  m_options = new QWidget;

  m_buttonBox = new QDialogButtonBox(Qt::Vertical);
  m_buttonBox->addButton(m_findButton, QDialogButtonBox::AcceptRole);
  m_buttonBox->addButton(new QPushButton("&Cancel"),QDialogButtonBox::RejectRole);

  connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * optionsLayout = new QVBoxLayout;
  optionsLayout->setMargin(0);
  optionsLayout->addWidget(m_switchFocus);
  m_options->setLayout(optionsLayout);

  //  connect(m_moreButton, SIGNAL(toggled(bool)), this, SLOT(showOptions(bool)));
  QHBoxLayout *topLeftLayout = new QHBoxLayout;
  topLeftLayout->addWidget(m_prompt);
  topLeftLayout->addWidget(m_edit);

  QVBoxLayout *leftLayout = new QVBoxLayout;
  leftLayout->addLayout(topLeftLayout);
  leftLayout->addWidget(m_newTab);

  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  mainLayout->addLayout(leftLayout, 0, 0);
  mainLayout->addWidget(m_buttonBox, 0, 1);
  mainLayout->addWidget(m_options, 1, 0, 1, 2);
  mainLayout->setRowStretch(2, 1);

  setLayout(mainLayout);
  setOptions();
  readSettings();
}
int PageSearchDialog::getPage() const {
  QString t = m_edit->text();
  bool ok;
  int x = t.toInt(&ok);
  if (!ok) {
    return 0;
  }
  return x;
}
void PageSearchDialog::readSettings() {
  SETTINGS

    settings.beginGroup("Search");
  m_newTab->setChecked(settings.value(SID_PAGESEARCH_NEW_TAB,true).toBool());
  m_switchFocus->setChecked(settings.value(SID_PAGESEARCH_GO_TAB,true).toBool());
}
void PageSearchDialog::getOptions(SearchOptions & opts) {
  opts.setNewTab(m_newTab->checkState() == Qt::Checked);
  opts.setActivateTab(m_switchFocus->checkState() == Qt::Checked);
}
void PageSearchDialog::setOptions() {
  SETTINGS

  settings.setIniCodec("UTF-8");
  settings.beginGroup("Search");
  settings.beginGroup("Page");
  m_newTab->setChecked(settings.value(SID_PAGESEARCH_NEW_TAB,false).toBool());
  m_switchFocus->setChecked(settings.value(SID_PAGESEARCH_GO_TAB,false).toBool());
  checkOptions();
}
void PageSearchDialog::checkOptions(int /* state */) {
  m_switchFocus->setEnabled(m_newTab->isChecked());
}
void PageSearchDialog::setText(const QString & t) {
  m_edit->setText(t);
}
