#include "fontchangedialog.h"
#include "arabicfontdialog.h"
#include "application.h"
#include "definedsettings.h"
#include "externs.h"
#define FONT_SIZE_WIDTH 40
#define LEAD_SPACING 50
#define SPACING 20
FontChangeDialog::FontChangeDialog(QWidget * parent) : QDialog(parent) {
  readSettings();
  m_modified = false;
  setWindowTitle(tr("Change Arabic font"));
  setObjectName("changearabicfont");
  QVBoxLayout * layout = new QVBoxLayout;

  QFormLayout * formlayout = new QFormLayout;

  m_arabicFont = new QComboBox;
  m_allFonts = new QCheckBox;
  m_changeIni = new QCheckBox;
  m_changeApplicationCss = new QCheckBox;
  m_changeEntryCss = new QCheckBox;
  m_changeIni->setChecked(true);
  m_changeApplicationCss->setChecked(true);
  m_changeEntryCss->setChecked(true);
  m_currentFont = new QLabel("");
  m_changes = new QListWidget;
  m_changes->setEnabled(false);
  m_applyButton = new QPushButton(tr("Change font"));
  QFontDatabase fdb;
  m_arabicFont->addItems(fdb.families(QFontDatabase::Arabic));
  m_fontLabel = new QLabel(tr("Available Arabic fonts"));
  formlayout->addRow(m_fontLabel,m_arabicFont);
  formlayout->addRow(tr("Show all fonts"),m_allFonts);
  formlayout->addRow(new QLabel(tr("Apply changes to:")));
  formlayout->addRow(tr("\tINI settings"),m_changeIni);
  formlayout->addRow(tr("\tApplication style"),m_changeApplicationCss);
  formlayout->addRow(tr("\tPage style"),m_changeEntryCss);
  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addWidget(m_applyButton);
  hlayout->addStretch();
  formlayout->addRow(tr(""),hlayout);

  connect(m_arabicFont,SIGNAL(currentTextChanged(const QString &)),this,SLOT(onFontChanged(const QString &)));

  connect(m_allFonts,SIGNAL(stateChanged(int)),this,SLOT(onShowAllChanged(int)));
  connect(m_applyButton,SIGNAL(clicked()),this,SLOT(onApply()));
  QDialogButtonBox *   buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);

  connect(buttonBox,SIGNAL(rejected()),this,SLOT(reject()));
  layout->addWidget(m_currentFont);
  layout->addLayout(formlayout);
  layout->addWidget(m_changes);
  layout->addWidget(buttonBox);
  layout->addStretch();
  setLayout(layout);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  m_changes->addItem(tr("Changes will be detailed here"));
  setCurrentFontText();
}
void FontChangeDialog::setCurrentFontText() {
  QString t = "The current Arabic ";
  QStringList fonts = getLexicon()->getUsedFont();
  if (fonts.size() == 1) {
    t += tr("font is");
    m_arabicFont->setCurrentText(fonts[0]);
  }
  else {
    t += tr("fonts are");
  }
  t += " " + fonts.join(",") + ".";
  m_currentFont->setText(t);
}

QSize FontChangeDialog::sizeHint() const {
  return QSize(600,400);
}

void FontChangeDialog::readSettings() {
}
void FontChangeDialog::writeSettings() {
}
void FontChangeDialog::onFontChanged(const QString & family) {
}
void FontChangeDialog::onShowAllChanged(int state) {
  QFontDatabase fdb;
  if (state == Qt::Checked) {
    m_arabicFont->clear();
    m_arabicFont->addItems(fdb.families());
    m_fontLabel->setText(tr("All fonts"));
  }
  else {
    m_arabicFont->clear();
    m_arabicFont->addItems(fdb.families(QFontDatabase::Arabic));
    m_fontLabel->setText(tr("Available Arabic fonts"));
  }
}
void FontChangeDialog::onApply() {
  QString arabicFont = m_arabicFont->currentText();
  QStringList changes;

  m_changes->setEnabled(true);

  m_changes->clear();
  QString t;
  t = getLexicon()->currentTheme();
  t = QString(tr("Changing font for the current theme : %1")).arg(t);
  QListWidgetItem * item;
  QFont f;
  if (m_changeIni->isChecked()) {
    item = new QListWidgetItem(t);
    f = item->font();
    f.setItalic(true);
    item->setFont(f);
    m_changes->addItem(item);
    m_changes->addItem("");


    t = getLexicon()->settingsFileName();
    t = QString("The following settings in %1 have been changed:").arg(t);
    item = new QListWidgetItem(t);
    item->setFont(f);
    m_changes->addItem(item);
    m_changes->addItem("");
    changes = getLexicon()->changeFontInSettings(arabicFont);
    m_changes->addItems(changes);
    m_changes->addItem("");
    m_modified = true;
  }
  QDir cssDirectory(getLexicon()->getResourceFilePath(Lexicon::Stylesheet));
  QStringList filters;
  filters << "*.css";
  QFileInfoList files =  cssDirectory.entryInfoList(filters);
  QString fileName;
  bool ok = true;
  for(int i=0;i < files.size();i++) {
    fileName = files[i].absoluteFilePath();
    if (fileName == getLexicon()->getStylesheetFilePath(Lexicon::Application)) {
      ok = m_changeApplicationCss->isChecked();
    }
    if (fileName == getLexicon()->getStylesheetFilePath(Lexicon::Entry)) {
      ok = m_changeEntryCss->isChecked();
    }
    if (fileName == getLexicon()->getStylesheetFilePath(Lexicon::Print)) {
      ok = m_changeEntryCss->isChecked();
    }
    if (ok) {
      t = QString("CSS changes in %1").arg(fileName);
      item = new QListWidgetItem(t);
      item->setFont(f);
      m_changes->addItem(item);
      m_changes->addItem("");

      changes = getLexicon()->changeFontInStylesheet(fileName,arabicFont);
      m_changes->addItems(changes);
      m_changes->addItem("");
      m_modified = true;
    }
  }
  setCurrentFontText();
}
bool FontChangeDialog::isModified() const {
  return m_modified;
}
