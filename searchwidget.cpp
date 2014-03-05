#include "searchwidget.h"
SearchDialog::SearchDialog(QWidget * parent,Qt::WindowFlags f) :
  QDialog(parent,f) {
  setWindowTitle(tr("Search"));
  m_prompt = new QLabel(tr("Find"));
  m_edit = new ImLineEdit;
  m_prompt->setBuddy(m_edit);

  m_newTab = new QCheckBox(tr("Open in &new tab"));
  m_switchFocus = new QCheckBox(tr("Switch to &new tab"));


  m_findButton = new QPushButton(tr("&Find"));
  m_findButton->setDefault(true);

  m_moreButton = new QPushButton(tr("&More"));
  m_moreButton->setCheckable(true);
  m_moreButton->setAutoDefault(false);

  m_options = new QWidget;

  m_buttonBox = new QDialogButtonBox(Qt::Vertical);
  m_buttonBox->addButton(m_findButton, QDialogButtonBox::AcceptRole);
  m_buttonBox->addButton(new QPushButton("&Cancel"),QDialogButtonBox::RejectRole);
  m_buttonBox->addButton(m_moreButton, QDialogButtonBox::ActionRole);

  connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * optionsLayout = new QVBoxLayout;
  optionsLayout->setMargin(0);
  optionsLayout->addWidget(m_switchFocus);

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
  leftLayout->addWidget(m_newTab);

  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  mainLayout->addLayout(leftLayout, 0, 0);
  mainLayout->addWidget(m_buttonBox, 0, 1);
  mainLayout->addWidget(m_options, 1, 0, 1, 2);
  mainLayout->setRowStretch(2, 1);

  setLayout(mainLayout);
  m_options->hide();
  m_edit->setFocus();
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

  connect(m_moreButton, SIGNAL(toggled(bool)), this, SLOT(showOptions(bool)));
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
