#include "themedialog.h"
#include "QsLog.h"
#include "application.h"
#include "optionstabwidget.h"
#include "externs.h"
ThemeDialog::ThemeDialog(QWidget * parent) : QDialog(parent) {
  QVBoxLayout * vlayout = new QVBoxLayout;

  m_themes = new QComboBox;
  QStringList themes = getLexicon()->getThemes();
  themes.sort();
  m_themes->addItems(themes);
  m_currentTheme = getLexicon()->currentTheme();
  m_themes->setCurrentText(m_currentTheme);
  //  m_editThemeButton = new QPushButton(tr("Edit"));
  m_copyThemeButton = new QPushButton(tr("Copy as"));
  m_deleteThemeButton = new QPushButton(tr("Delete"));
  m_newThemeButton = new QPushButton(tr("Create new theme"));

  QHBoxLayout * editlayout = new QHBoxLayout;
  editlayout->addWidget(new QLabel(tr("Themes")));
  editlayout->addWidget(m_themes);
  //  editlayout->addWidget(m_editThemeButton);
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

  connect(m_themes,SIGNAL(currentTextChanged(const QString &)),this,SLOT(onThemeChanged(const QString &)));
  connect(m_copyThemeButton,SIGNAL(clicked()),this,SLOT(onCopy()));
  vlayout->addWidget(btns);
  setLayout(vlayout);
}
void ThemeDialog::onThemeChanged(const QString & theme) {
  qDebug() << Q_FUNC_INFO << m_currentTheme << theme;
  if (theme == m_currentTheme) {
    return;
  }
  if (m_tabs->isModified()) {
        int ret = QMessageBox::warning(this, tr("Edit Theme"),
                                       QString(tr("You have unsaved changes to the theme: %1\nDo you want to save the changes?")).arg(m_currentTheme),
                                       QMessageBox::Yes | QMessageBox::No|QMessageBox::Cancel);
    if (ret == QMessageBox::Cancel) {
      m_themes->setCurrentText(m_currentTheme);
      return;
    }
    if (ret == QMessageBox::Yes) {
      m_tabs->saveChanges();
    }
  }
  /**
   * set the theme settings file name and reload all the tabs
   *
   */
  m_tabs->readTheme(getLexicon()->settingsFileName(theme));
  m_currentTheme = theme;
}
void ThemeDialog::addTabs() {
}
void ThemeDialog::onCopy() {
  bool ok;
  QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                       tr("Theme name:"), QLineEdit::Normal,
                                       QString(), &ok);
  if (! ok || text.isEmpty()){
    return;
  }
  if (m_themes->findText(text,Qt::MatchFixedString) != -1) {
    int ret = QMessageBox::warning(this, tr("Theme copy"),
                               tr("The new theme name is not unique.\n"
                                  "Do you want to overrite the theme?"),
                                   QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Cancel) {
      return;
    }
  }
  QString themeRoot = getLexicon()->getResourcePath(Lexicon::ThemeRoot);

  QString srcFilePath = themeRoot + QDir::separator() + m_themes->currentText();
  QString tgtFilePath = themeRoot + QDir::separator() + text;

  m_copyCount = 0;
  copyRecursively(srcFilePath,tgtFilePath);
  if (m_copyCount > 0) {
    m_themes->addItem(text);
    int ret = QMessageBox::information(this, tr("Theme copy"),
                             QString(tr("Copied %1 files.\nDo you want to activate the new theme.")).arg(m_copyCount),
                             QMessageBox::Yes| QMessageBox::No);
    if (ret == QMessageBox::Yes) {
      if (getLexicon()->setTheme(text) == Lexicon::Ok) {
        m_themes->setCurrentText(text);
      }
      else {
        qDebug() << "Cannot activate new theme" << getLexicon()->takeLastError();
      }
    }
  }
  else {
    /// TODO tell them it failed
  }
}
bool ThemeDialog::copyRecursively(const QString & srcPath,const QString & targetPath) {
  QFileInfo srcFileInfo(srcPath);
  if (srcFileInfo.isDir()) {
    QDir targetDir(targetPath);
    targetDir.cdUp();
    if (!targetDir.mkdir(QFileInfo(targetPath).fileName()))
      return false;
    QDir sourceDir(srcPath);
    QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
    foreach (const QString &fileName, fileNames) {
      const QString newSrcFilePath
        = srcPath + QDir::separator() + fileName;
      const QString newTgtFilePath
        = targetPath + QDir::separator() + fileName;
      if (!copyRecursively(newSrcFilePath, newTgtFilePath))
        return false;
    }
  } else {
    if (!QFile::copy(srcPath, targetPath))
      return false;
    else {
      m_copyCount++;
    }
  }
  return true;
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
