#include "searchdialogs.h"
#include "QsLog.h"
#include "laneslexicon.h"
#include "externs.h"
#include "definedsettings.h"
extern Lexicon * getLexicon();
/**
 *
 *
 * @param parent
 * @param f
 */
NodeSearchDialog::NodeSearchDialog(QWidget * parent,Qt::WindowFlags f) :
  QDialog(parent,f) {
  setWindowTitle(tr("Search for node"));
  m_searchType = SearchOptions::Node;

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
  QPushButton * helpbutton = m_buttonBox->addButton(QDialogButtonBox::Help);
  connect(helpbutton,SIGNAL(clicked()),this,SLOT(onHelp()));

  connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * optionsLayout = new QVBoxLayout;
  optionsLayout->setMargin(0);
  optionsLayout->addWidget(m_switchFocus);
  m_options->setLayout(optionsLayout);

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
  m_edit->setFocus();
}
void NodeSearchDialog::readSettings() {
  SETTINGS

    settings.beginGroup("Search");
  m_newTab->setChecked(settings.value(SID_NODESEARCH_NEW_TAB,true).toBool());
  m_switchFocus->setChecked(settings.value(SID_NODESEARCH_GO_TAB,true).toBool());
}
QString NodeSearchDialog::getText() const {
  QString t = m_edit->text();
  QRegularExpression rx("^\\d+$");
  QRegularExpressionMatch match = rx.match(t);
  if (match.hasMatch()) {
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
void NodeSearchDialog::onHelp() {
  emit(showHelp(QString("search-%1").arg(m_searchType)));
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
  m_searchType = SearchOptions::Page;

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
  QPushButton * helpbutton = m_buttonBox->addButton(QDialogButtonBox::Help);
  connect(helpbutton,SIGNAL(clicked()),this,SLOT(onHelp()));

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
void PageSearchDialog::onHelp() {
  emit(showHelp(QString("search-%1").arg(m_searchType)));
}
