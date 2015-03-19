#include "createthemedialog.h"
#include "QsLog.h"
#include "application.h"
#include "externs.h"
CreateThemeDialog::CreateThemeDialog(QWidget * parent) : QDialog(parent) {
  QVBoxLayout * layout = new QVBoxLayout;

  setWindowTitle(tr("Create Theme"));
  m_themes = new QComboBox;
  QStringList themes = getLexicon()->getThemes();
  themes.sort();
  m_themes->addItems(themes);
  m_currentTheme = getLexicon()->currentTheme();
  m_themes->setCurrentText(m_currentTheme);


  m_newTheme = new QLineEdit;
  m_activate = new QCheckBox;

  m_themes->setCurrentText(m_currentTheme);

  QFormLayout * form = new QFormLayout;
  form->addRow(tr("Copy theme"),m_themes);
  form->addRow(tr("New theme name"),m_newTheme);
  form->addRow(tr("Make new theme current"),m_activate);
  m_createButton = new QPushButton(tr("Create"));
  QDialogButtonBox * btns = new QDialogButtonBox(QDialogButtonBox::Cancel);
  btns->addButton(m_createButton,QDialogButtonBox::AcceptRole);

  connect(m_createButton,SIGNAL(clicked()),this,SLOT(onCreate()));
  connect(btns,SIGNAL(rejected()),this,SLOT(reject()));
  connect(m_newTheme,SIGNAL(textChanged(const QString &)),this,SLOT(onTextChanged(const QString &)));

  layout->addWidget(new QLabel(QString(tr("The current theme is:%1")).arg(m_currentTheme)));
  layout->addLayout(form);
  layout->addWidget(btns);
  //  layout->addStretch();

  m_createButton->setEnabled(false);
  setLayout(layout);
  m_newTheme->setFocus();
};
void CreateThemeDialog::onCreate() {
  QString t = m_newTheme->text();
  if (m_themes->findText(t,Qt::MatchFixedString) != -1) {
    int ret = QMessageBox::warning(this, tr("Theme copy"),
                                   tr("The new theme name is not unique.\n"
                                      "Do you want to overrite the existing theme?"),
                                   QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Cancel) {
      return;
    }
  }
  this->accept();
}
void CreateThemeDialog::onTextChanged(const QString & text) {
  if (text.isEmpty()) {
    m_createButton->setEnabled(false);
    return;
  }
  m_createButton->setEnabled(true);
}
QPair<QString,QString> CreateThemeDialog::getThemes() {
  return qMakePair(m_themes->currentText(),m_newTheme->text());
}
bool CreateThemeDialog::activate() {
  return m_activate->isChecked();
}
