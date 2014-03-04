#include "searchwidget.h"
SearchWidget::SearchWidget(QWidget * parent) : QWidget(parent) {
  QVBoxLayout * layout = new QVBoxLayout;
  m_edit = new WrappedEdit;

  m_cancelBtn = new QPushButton(tr("Cancel"));
  layout->addWidget(m_edit);
  layout->addWidget(m_cancelBtn);

  connect(m_cancelBtn,SIGNAL(clicked()),this,SLOT(onCancel()));
  setLayout(layout);
}
void SearchWidget::onCancel() {
  close();
}

RootSearchDialog::RootSearchDialog(QWidget * parent,Qt::WindowFlags f) :
  QDialog(parent,f) {
  setWindowTitle(tr("Search for root"));

  m_prompt = new QLabel(tr("Find &root"));
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
void RootSearchDialog::showOptions(bool v) {
  qDebug() << Q_FUNC_INFO << v;
  m_options->setVisible(v);
  //  this->adjustSize();
  if (!v)
    m_edit->setFocus();
}
void RootSearchDialog::keymapChanged() {
  QRadioButton * btn = qobject_cast<QRadioButton *>(QObject::sender());
  /// if passed the name of non-existent map
  /// this will deactive the current map as there is no such map
  m_edit->activateMap(btn->text(),true);
  m_edit->setFocus();
}
void RootSearchDialog::onCancel() {
  //  reject();
}
QString RootSearchDialog::getText() {
  return m_edit->text().trimmed();
}
bool RootSearchDialog::getNewTab() {
  if (m_newTab->checkState() == Qt::Checked) {
    return true;
  }
  return false;
}
void RootSearchDialog::setNewTab(bool v) {
  m_newTab->setChecked(v);
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
/**
 *
 *
 * @param parent
 * @param f
 */
WordSearchDialog::WordSearchDialog(QWidget * parent,Qt::WindowFlags f) :
  QDialog(parent,f) {
  setWindowTitle(tr("Search for word"));
  QVBoxLayout * layout = new QVBoxLayout;
  m_edit = new ImLineEdit(this);
  m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

  connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));



  QStringList maps = m_edit->getMaps();
  QString map = m_edit->getActiveMap();
  if (map.isEmpty()) {
    map = m_edit->getNullMap();
  }
  maps << m_edit->getNullMap();
  if (maps.size() > 0) {
    QGroupBox * group = new QGroupBox(tr("Keymap"));
    /// TODO this should be Grid to use less space ?
    QVBoxLayout * btnlayout = new QVBoxLayout;
      for(int i=0;i < maps.size();i++) {
        QRadioButton * btn = new QRadioButton(maps[i]);
        if (maps[i].toCaseFolded() == map.toCaseFolded()) {
          btn->setChecked(true);
        }
        btnlayout->addWidget(btn);
        connect(btn,SIGNAL(clicked()),this,SLOT(keymapChanged()));
      }
      group->setLayout(btnlayout);
      layout->addWidget(group);
  }
  layout->addWidget(m_edit,0);
  layout->addStretch(1);
  layout->addWidget(m_buttonBox);

  setModal(true);
  setLayout(layout);
  m_edit->setFocus();
}
QString WordSearchDialog::getText() {
  /// getText() converts buckwalter to arabic
  return m_edit->text().trimmed();
}
void WordSearchDialog::keymapChanged() {
  QRadioButton * btn = qobject_cast<QRadioButton *>(QObject::sender());
  /// if passed the name of non-existent map
  /// this will deactive the current map as there is no such map
  m_edit->activateMap(btn->text(),true);
  m_edit->setFocus();
}
