#include "themedialog.h"
#include "QsLog.h"
#include "application.h"
#include "optionstabwidget.h"
#include "externs.h"
ThemeDialog::ThemeDialog(QWidget * parent) : QDialog(parent) {
  QVBoxLayout * vlayout = new QVBoxLayout;

  m_themes = new QComboBox;
  m_themes->addItems(getLexicon()->getThemes());

  m_editThemeButton = new QPushButton(tr("Edit"));
  m_copyThemeButton = new QPushButton(tr("Copy as"));
  m_deleteThemeButton = new QPushButton(tr("Delete"));
  m_newThemeButton = new QPushButton(tr("Create new theme"));

  QHBoxLayout * editlayout = new QHBoxLayout;
  editlayout->addWidget(new QLabel(tr("Themes")));
  editlayout->addWidget(m_themes);
  editlayout->addWidget(m_editThemeButton);
  editlayout->addWidget(m_copyThemeButton);
  editlayout->addWidget(m_deleteThemeButton);

  m_tabs = new OptionsTabWidget(QString());

  vlayout->addLayout(editlayout);
  vlayout->addWidget(m_tabs);

  //  vlayout->addWidget(m_newThemeButton);
  QDialogButtonBox * btns = new QDialogButtonBox(QDialogButtonBox::Cancel);
  btns->addButton(m_newThemeButton,QDialogButtonBox::AcceptRole);

  connect(btns, SIGNAL(rejected()), this, SLOT(reject()));
  connect(btns, SIGNAL(accepted()), this, SLOT(onNew()));

  connect(m_editThemeButton,SIGNAL(clicked()),this,SLOT(onEdit()));
  vlayout->addWidget(btns);
  setLayout(vlayout);
}
void ThemeDialog::addTabs() {
}
void ThemeDialog::onCopy() {
}
void ThemeDialog::onDelete() {
}
void ThemeDialog::onNew() {
  QLOG_DEBUG() << Q_FUNC_INFO;
}
void ThemeDialog::onEdit() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  //  OptionsDialog d(m_themes->currentText());
  //  d.exec();
}
