#include "searchwidget.h"
#include "QsLog.h"
#include "namespace.h"
SearchDialog::SearchDialog(QWidget * parent,Qt::WindowFlags f) :
  QDialog(parent,f) {
}
void SearchDialog::setup() {
  setWindowTitle(tr("Search"));
  m_prompt = new QLabel(tr("Find"));
  m_edit = new ImLineEdit;
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  m_edit->readSettings(settings);
  m_prompt->setBuddy(m_edit);
  m_newTab = new QCheckBox(tr("Open in &new tab"));
  m_switchFocus = new QCheckBox(tr("Switch to &new tab"));

  m_findButton = new QPushButton(tr("&Find"));
  m_findButton->setDefault(true);

  m_keyboardButton  = new QPushButton(tr("Show &keyboard"));
  m_keyboardButton->setAutoDefault(false);

  m_moreButton = new QPushButton(tr("&More"));
  m_moreButton->setCheckable(true);
  m_moreButton->setAutoDefault(false);



  m_buttonBox = new QDialogButtonBox(Qt::Vertical);
  m_buttonBox->addButton(m_findButton, QDialogButtonBox::AcceptRole);
  m_buttonBox->addButton(new QPushButton("&Cancel"),QDialogButtonBox::RejectRole);
  m_buttonBox->addButton(m_moreButton, QDialogButtonBox::ActionRole);
  m_buttonBox->addButton(m_keyboardButton,QDialogButtonBox::ActionRole);

  m_findButton->setFocus();

  connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(m_keyboardButton, SIGNAL(clicked()),this,SLOT(showKeyboard()));
  m_options = new QWidget;
  QVBoxLayout * optionsLayout = new QVBoxLayout;
  optionsLayout->setMargin(0);


  QGridLayout * glayout = new QGridLayout;

  this->addOptions(glayout);


  QStringList maps = m_edit->getMaps();
  QString map = m_edit->getActiveMap();
  if (map.isEmpty()) {
    map = m_edit->getNullMap();
  }
  maps << m_edit->getNullMap();
  if (maps.size() > 0) {
    m_group = new QGroupBox(tr("Keymap"));
    QVBoxLayout * btnlayout = new QVBoxLayout;
      for(int i=0;i < maps.size();i++) {
        QRadioButton * btn = new QRadioButton(maps[i]);
        if (maps[i].toCaseFolded() == map.toCaseFolded()) {
          btn->setChecked(true);
        }
        btnlayout->addWidget(btn);
        connect(btn,SIGNAL(clicked()),this,SLOT(keymapChanged()));
      }
      m_group->setLayout(btnlayout);
      optionsLayout->addWidget(m_group);
  }
  m_options->setLayout(optionsLayout);

  connect(m_moreButton, SIGNAL(toggled(bool)), this, SLOT(showOptions(bool)));
  QHBoxLayout *topLeftLayout = new QHBoxLayout;
  topLeftLayout->addWidget(m_prompt);
  topLeftLayout->addWidget(m_edit);

  QVBoxLayout *leftLayout = new QVBoxLayout;
  leftLayout->addLayout(topLeftLayout);
  leftLayout->addLayout(glayout);

  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  mainLayout->addLayout(leftLayout, 0, 0);
  mainLayout->addWidget(m_buttonBox, 0, 1);
  mainLayout->addWidget(m_options, 1, 0, 1, 2);
  mainLayout->setRowStretch(2, 1);

  setLayout(mainLayout);
  m_options->hide();
  m_edit->setFocus();
  m_attached = false;
  m_keyboard = new KeyboardWidget(this);
  QPoint p = this->pos();
  QLOG_DEBUG() << "Search dialog pos" << this->pos() << "mapped to global" <<  this->mapToGlobal(this->pos());
  int h = this->frameGeometry().height();
  QLOG_DEBUG() << "search dialog frame geometry" << this->frameGeometry();
  m_keyboard->move(p.x(),p.y() + h);
  //  delete settings;
}
void SearchDialog::addOptions(QGridLayout * glayout) {
  glayout->addWidget(m_switchFocus,0,0);
  glayout->addWidget(m_newTab,0,1);
}
void SearchDialog::showKeyboard() {
  m_keyboard->attach(m_edit);
  m_attached = ! m_attached;
  if (m_attached) {
    m_keyboardButton->setText(tr("Hide keyboard"));
    QPoint p = this->pos();
    int h = this->frameGeometry().height();
    /// TODO adjust this
    m_keyboard->move(p.x() - 50,p.y() + h);
  }
  else
    m_keyboardButton->setText(tr("Show keyboard"));

}
void SearchDialog::showOptions(bool v) {
  m_options->setVisible(v);
  if (!v)
    m_edit->setFocus();
}
void SearchDialog::keymapChanged() {
  QRadioButton * btn = qobject_cast<QRadioButton *>(QObject::sender());
  /// if passed the name of non-existent map
  /// this will deactive the current map as there is no such map
  m_edit->activateMap(btn->text(),true);
  m_edit->setFocus();
}
QString SearchDialog::getText() {
  return m_edit->text().trimmed();
}
bool SearchDialog::getNewTab() {
  if (m_newTab->checkState() == Qt::Checked) {
    return true;
  }
  return false;
}
void SearchDialog::setNewTab(bool v) {
  m_newTab->setChecked(v);
}
bool SearchDialog::getSwitchFocus() {
  if (m_switchFocus->checkState() == Qt::Checked) {
    return true;
  }
  return false;
}
void SearchDialog::setSwitchFocus(bool v) {
  m_switchFocus->setChecked(v);
}
void SearchDialog::setPrompt(const QString & text) {
  m_prompt->setText(text);
}
/**
 *
 *
 * @param parent
 * @param f
 */
WordSearchDialog::WordSearchDialog(QWidget * parent,Qt::WindowFlags f) :
  SearchDialog(parent,f) {

}
void WordSearchDialog::setup() {
  setWindowTitle(tr("Search"));
  m_prompt = new QLabel(tr("Find"));
  m_edit = new ImLineEdit;

  m_prompt->setBuddy(m_edit);
  m_newTab = new QCheckBox(tr("Open in &new tab"));
  m_switchFocus = new QCheckBox(tr("Switch to &new tab"));
  m_ignoreDiacritics = new QCheckBox(tr("Ignore diacritics"));
  m_wholeWordMatch = new QCheckBox(tr("Whole word match"));


  m_findButton = new QPushButton(tr("&Find"));
  m_findButton->setDefault(true);

  m_keyboardButton  = new QPushButton(tr("Show &keyboard"));
  m_keyboardButton->setAutoDefault(false);

  m_moreButton = new QPushButton(tr("&More"));
  m_moreButton->setCheckable(true);
  m_moreButton->setAutoDefault(false);



  m_buttonBox = new QDialogButtonBox(Qt::Vertical);
  m_buttonBox->addButton(m_findButton, QDialogButtonBox::AcceptRole);
  m_buttonBox->addButton(new QPushButton("&Cancel"),QDialogButtonBox::RejectRole);
  m_buttonBox->addButton(m_moreButton, QDialogButtonBox::ActionRole);
  m_buttonBox->addButton(m_keyboardButton,QDialogButtonBox::ActionRole);

  m_findButton->setFocus();

  connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(m_keyboardButton, SIGNAL(clicked()),this,SLOT(showKeyboard()));
  m_options = new QWidget;
  QVBoxLayout * optionsLayout = new QVBoxLayout;
  optionsLayout->setMargin(0);


  QGridLayout * glayout = new QGridLayout;
  this->addOptions(glayout);
  /// they will be shown via the 'more' button
  m_switchFocus->hide();
  m_newTab->hide();

  QStringList maps = m_edit->getMaps();
  QString map = m_edit->getActiveMap();
  if (map.isEmpty()) {
    map = m_edit->getNullMap();
  }
  maps << m_edit->getNullMap();
  if (maps.size() > 0) {
    m_group = new QGroupBox(tr("Keymap"));
    QVBoxLayout * btnlayout = new QVBoxLayout;
      for(int i=0;i < maps.size();i++) {
        QRadioButton * btn = new QRadioButton(maps[i]);
        if (maps[i].toCaseFolded() == map.toCaseFolded()) {
          btn->setChecked(true);
        }
        btnlayout->addWidget(btn);
        connect(btn,SIGNAL(clicked()),this,SLOT(keymapChanged()));
      }
      m_group->setLayout(btnlayout);
      optionsLayout->addWidget(m_group);
  }
  m_options->setLayout(optionsLayout);

  connect(m_moreButton, SIGNAL(toggled(bool)), this, SLOT(showOptions(bool)));
  QHBoxLayout *topLeftLayout = new QHBoxLayout;
  topLeftLayout->addWidget(m_prompt);
  topLeftLayout->addWidget(m_edit);

  QVBoxLayout *leftLayout = new QVBoxLayout;
  leftLayout->addLayout(topLeftLayout);
  leftLayout->addLayout(glayout);

  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  mainLayout->addLayout(leftLayout, 0, 0);
  mainLayout->addWidget(m_buttonBox, 0, 1);
  mainLayout->addWidget(m_options, 1, 0, 1, 2);
  mainLayout->setRowStretch(2, 1);

  setLayout(mainLayout);
  m_options->hide();
  m_edit->setFocus();
  m_attached = false;
  m_keyboard = new KeyboardWidget(this);
  QPoint p = this->pos();
  QLOG_DEBUG() << "Search dialog pos" << this->pos() << "mapped to global" <<  this->mapToGlobal(this->pos());
  int h = this->frameGeometry().height();
  QLOG_DEBUG() << "search dialog frame geometry" << this->frameGeometry();
  m_keyboard->move(p.x(),p.y() + h);
  /// set the defaults
  connect(m_arabicTarget,SIGNAL(clicked()),this,SLOT(searchTargetChanged()));
  connect(m_buckwalterTarget,SIGNAL(clicked()),this,SLOT(searchTargetChanged()));
  connect(m_normalButton,SIGNAL(clicked()),this,SLOT(searchTypeChanged()));
  connect(m_regexButton,SIGNAL(clicked()),this,SLOT(searchTypeChanged()));
  m_arabicTarget->setChecked(true);
  this->readSettings();
}
void WordSearchDialog::addOptions(QGridLayout * glayout) {
  QGroupBox * btngroup = new QGroupBox(tr("Search context"));
  m_headButton = new QCheckBox(tr("Head word"));
  m_fullButton = new QCheckBox(tr("Full text"));
  QHBoxLayout * layout = new QHBoxLayout;
  layout->addWidget(m_headButton);
  layout->addWidget(m_fullButton);
  btngroup->setLayout(layout);

  QGroupBox * targetgroup = new QGroupBox(tr("Search target"));
  QHBoxLayout * layout1= new QHBoxLayout;
  m_arabicTarget = new QRadioButton(tr("Arabic text"),targetgroup);
  m_buckwalterTarget = new QRadioButton(tr("Buckwalter transliteration"),targetgroup);
  layout1->addWidget(m_arabicTarget);
  layout1->addWidget(m_buckwalterTarget);
  targetgroup->setLayout(layout1);

  m_ignoreDiacritics = new QCheckBox(tr("Ignore diacritics"));
  m_wholeWordMatch = new QCheckBox(tr("Whole word match"));

  QGroupBox * typegroup = new QGroupBox(tr("Search type"));
  QHBoxLayout * layout2= new QHBoxLayout;

  m_normalButton = new QRadioButton(tr("Normal"),typegroup);
  m_regexButton = new QRadioButton(tr("Regular expression"),typegroup);
  layout2->addWidget(m_normalButton);
  layout2->addWidget(m_regexButton);
  typegroup->setLayout(layout2);

  int row = 0;
  glayout->addWidget(btngroup,row,0,1,2);
  row++;
  glayout->addWidget(targetgroup,row,0,1,2);
  row++;
  glayout->addWidget(typegroup,row,0,1,2);
  row++;
  glayout->addWidget(m_ignoreDiacritics,row,0);
  glayout->addWidget(m_wholeWordMatch,row,1);
  row++;
  glayout->addWidget(m_switchFocus,row,0);
  glayout->addWidget(m_newTab,row,1);
}
void WordSearchDialog::readSettings() {
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  m_edit->readSettings(settings);
  /// the settings will be deleted in ImLineEdit::readSettings

  settings = app->getSettings();
  settings->beginGroup("Search");
  QString v = settings->value("Context","header").toString();
  if (v == "head") {
    m_headButton->setChecked(true);
    m_fullButton->setChecked(false);
  }
  else {
    m_headButton->setChecked(false);
    m_fullButton->setChecked(true);
  }
  v = settings->value("Type","normal").toString();
  if (v == "normal") {
    m_normalButton->setChecked(true);
    m_regexButton->setChecked(false);
  }
  else {
    m_normalButton->setChecked(false);
    m_regexButton->setChecked(true);
  }
  v = settings->value("Target","arabic").toString();
  if (v == "arabic") {
    m_arabicTarget->setChecked(true);
    m_buckwalterTarget->setChecked(false);
  }
  else {
    m_arabicTarget->setChecked(false);
    m_buckwalterTarget->setChecked(true);
  }
}
void WordSearchDialog::searchTargetChanged() {
  qDebug() << Q_FUNC_INFO;
  if (m_arabicTarget->isChecked()) {
    bool v = false;
    if (m_normalButton->isChecked()) {
      v = true;
    }
    m_ignoreDiacritics->setEnabled(v);
    m_wholeWordMatch->setEnabled(v);
  }
}
void WordSearchDialog::searchTypeChanged() {
  qDebug() << Q_FUNC_INFO;
  bool v = true;
  if (m_regexButton->isChecked()) {
    v = false;
  }
  m_ignoreDiacritics->setEnabled(v);
  m_wholeWordMatch->setEnabled(v);
}
int WordSearchDialog::getOptions() {
  int x = 0;
  if (m_ignoreDiacritics->isChecked())
    x |= Lane::Ignore_Diacritics;

  if (m_wholeWordMatch->isChecked())
    x |= Lane::Whole_Word_Match;

  if (m_switchFocus->isChecked())
    x |= Lane::Switch_Tab;

  if (m_newTab->isChecked())
    x |= Lane::Create_Tab;

  return x;
}
void WordSearchDialog::showOptions(bool v) {
  m_switchFocus->setVisible(v);
  m_newTab->setVisible(v);
  m_options->setVisible(v);
  if (!v)
    m_edit->setFocus();
}
/**
 *
 *
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
}
void NodeSearchDialog::setNewTab(bool v) {
  m_newTab->setChecked(v);
}
QString NodeSearchDialog::getText() const {
  QString t = m_edit->text();
  if (! t.startsWith("n")) {
    t = "n" + t;
  }
  return t;
}
bool NodeSearchDialog::getNewTab() const {
  if (m_newTab->checkState() == Qt::Checked) {
    return true;
  }
  return false;
}
