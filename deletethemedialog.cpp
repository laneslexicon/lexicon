#include "deletethemedialog.h"
#include "QsLog.h"
#include "application.h"
#include "externs.h"
DeleteThemeDialog::DeleteThemeDialog(QWidget * parent) : QDialog(parent) {
  QVBoxLayout * layout = new QVBoxLayout;

  setWindowTitle(tr("Delete Theme"));
  m_themes = new QComboBox;
  QStringList themes = getLexicon()->getThemes();
  themes.sort();

  m_themes->addItems(themes);
  m_currentTheme = getLexicon()->currentTheme();
  m_themes->setCurrentText(m_currentTheme);


  m_switch = new QComboBox;
  m_switch->addItems(themes);
  m_currentTheme = getLexicon()->currentTheme();


  connect(m_themes,SIGNAL(currentIndexChanged(int)),this,SLOT(onChangeTheme(int)));
  m_themes->setCurrentText(m_currentTheme);

  m_themes->removeItem(m_themes->findText(m_currentTheme));
  //  m_switch->removeItem(m_themes->findText(m_currentTheme));
  QFormLayout * form = new QFormLayout;
  form->addRow(tr("Delete theme"),m_themes);
  form->addRow(tr("New theme"),m_switch);

  m_deleteButton = new QPushButton(tr("Delete"));
  QDialogButtonBox * btns = new QDialogButtonBox(QDialogButtonBox::Cancel);
  btns->addButton(m_deleteButton,QDialogButtonBox::AcceptRole);
  connect(btns,SIGNAL(accepted()),this,SLOT(accept()));
  connect(btns,SIGNAL(rejected()),this,SLOT(reject()));

  layout->addWidget(new QLabel(QString(tr("The current theme is \"%1\" and cannot be deleted.")).arg(m_currentTheme)));
  layout->addLayout(form);
  layout->addWidget(btns);
  //  layout->addStretch();
  setLayout(layout);

  if (themes.size() == 1) {
    m_deleteButton->setEnabled(false);
  }
};
void DeleteThemeDialog::onChangeTheme(int index) {
  m_switch->clear();
  for(int i=0;i < m_themes->count();i++) {
    if (i != index) {
      m_switch->addItem(m_themes->itemText(i));
    }
  }
  m_switch->addItem(m_currentTheme);
  m_switch->setCurrentText(m_currentTheme);
}
QPair<QString,QString> DeleteThemeDialog::getThemes() {
  return qMakePair(m_themes->currentText(),m_switch->currentText());
}
