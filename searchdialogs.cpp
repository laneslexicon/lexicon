#include "searchdialogs.h"
#include "QsLog.h"
#include "namespace.h"
#include "searchoptions.h"
RootSearchDialog::RootSearchDialog(QWidget * parent,Qt::WindowFlags f) :
  QDialog(parent,f) {

  setWindowTitle(tr("Search for Root"));
  m_prompt = new QLabel(tr("Find"));
  m_edit = new ImLineEdit;
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  m_edit->readSettings(settings);
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
  m_buttonBox->addButton(new QPushButton("&Cancel"),QDialogButtonBox::RejectRole);
  m_buttonBox->addButton(m_moreButton, QDialogButtonBox::ActionRole);
  m_buttonBox->addButton(m_keyboardButton,QDialogButtonBox::ActionRole);

  m_findButton->setFocus();

  connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(m_keyboardButton, SIGNAL(clicked()),this,SLOT(showKeyboard()));
  m_options = new SearchOptions(Lane::Root);

  connect(m_options,SIGNAL(loadKeymap(const QString &)),this,SLOT(loadKeymap(const QString &)));

  QStringList maps = m_edit->getMaps();
  QString map = m_edit->getActiveMap();
  if (map.isEmpty()) {
    map = m_edit->getNullMap();
  }
  maps << m_edit->getNullMap();
  m_options->addKeymaps(map,maps);

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
  m_keyboard = new KeyboardWidget(this);
  QPoint p = this->pos();
  //  QLOG_DEBUG() << "Search dialog pos" << this->pos() << "mapped to global" <<  this->mapToGlobal(this->pos());
  int h = this->frameGeometry().height();
  //  QLOG_DEBUG() << "search dialog frame geometry" << this->frameGeometry();
  m_keyboard->move(p.x(),p.y() + h);
  //  delete settings;
}
void RootSearchDialog::showKeyboard() {
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
void RootSearchDialog::showOptions(bool v) {
  m_options->showMore(v);
  if (!v)
    m_edit->setFocus();

  /// this shrinks the dialog
  if ( ! v )
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}
/*
void SearchDialog::keymapChanged() {
  QRadioButton * btn = qobject_cast<QRadioButton *>(QObject::sender());
  /// if passed the name of non-existent map
  /// this will deactive the current map as there is no such map
  m_edit->activateMap(btn->text(),true);
  m_edit->setFocus();
}
*/
QString RootSearchDialog::getText() {
  return m_edit->text().trimmed();
}
void RootSearchDialog::setPrompt(const QString & text) {
  m_prompt->setText(text);
}
void RootSearchDialog::setOptions(int opts) {
  m_options->setOptions(opts);
}
void RootSearchDialog::loadKeymap(const QString & mapname) {
  m_edit->activateMap(mapname,true);
}
