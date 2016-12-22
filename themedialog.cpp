#include "themedialog.h"
#include "QsLog.h"
#include "application.h"
#include "optionstabwidget.h"
#include "externs.h"
ThemeDialog::ThemeDialog(QWidget * parent) : QDialog(parent) {
  QVBoxLayout * vlayout = new QVBoxLayout;
  setWindowTitle(tr("Edit Theme"));
  setObjectName("themedialog");
  m_themes = new QComboBox;
  QStringList themes = getLexicon()->getThemes();
  themes.sort();
  m_themes->addItems(themes);
  m_currentTheme = getLexicon()->currentTheme();
  m_themes->setCurrentText(m_currentTheme);
  m_editThemeButton = new QPushButton(tr("Edit"));
  QString msg = QString(tr("The current theme is: %1")).arg(m_currentTheme);
  vlayout->addWidget(new QLabel(tr("Select theme to edit")));
  vlayout->addWidget(new QLabel(msg));
  vlayout->addWidget(m_themes);

  QDialogButtonBox * btns = new QDialogButtonBox(QDialogButtonBox::Cancel);
  btns->addButton(m_editThemeButton,QDialogButtonBox::AcceptRole);

  connect(btns, SIGNAL(rejected()), this, SLOT(reject()));
  connect(btns, SIGNAL(accepted()), this, SLOT(accept()));

//  connect(m_themes,SIGNAL(currentTextChanged(const QString &)),this,SLOT(onThemeChanged(const QString &)));
  vlayout->addWidget(btns);
  setLayout(vlayout);
}
QString ThemeDialog::theme() const {
  return m_themes->currentText();
}
