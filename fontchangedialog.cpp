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
  QVBoxLayout * layout = new QVBoxLayout;

  QFormLayout * formlayout = new QFormLayout;

  m_arabicFont = new QComboBox;
  m_allFonts = new QCheckBox;
  m_changes = new QListWidget;
  m_changes->setEnabled(false);
  m_applyButton = new QPushButton(tr("Change font"));
  QFontDatabase fdb;
  m_arabicFont->addItems(fdb.families(QFontDatabase::Arabic));
  m_fontLabel = new QLabel(tr("Available Arabic fonts"));
  formlayout->addRow(m_fontLabel,m_arabicFont);
  formlayout->addRow(tr("Show all fonts"),m_allFonts);

  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addWidget(m_applyButton);
  hlayout->addStretch();
  formlayout->addRow(tr(""),hlayout);

  connect(m_arabicFont,SIGNAL(currentTextChanged(const QString &)),this,SLOT(onFontChanged(const QString &)));

  connect(m_allFonts,SIGNAL(stateChanged(int)),this,SLOT(onShowAllChanged(int)));
  connect(m_applyButton,SIGNAL(clicked()),this,SLOT(onApply()));
  QDialogButtonBox *   buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);

  connect(buttonBox,SIGNAL(rejected()),this,SLOT(reject()));
  layout->addLayout(formlayout);
  layout->addWidget(m_changes);
  layout->addWidget(buttonBox);
  setLayout(layout);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
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
  QListWidgetItem * item = new QListWidgetItem(t);
  QFont f = item->font();
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
  QDir cssDirectory(getLexicon()->getResourceFilePath(Lexicon::Stylesheet));
  QStringList filters;
  filters << "*.css";
  QFileInfoList files =  cssDirectory.entryInfoList(filters);
  for(int i=0;i < files.size();i++) {

    t = QString("CSS changes in %1").arg(files[i].absoluteFilePath());
    item = new QListWidgetItem(t);
    item->setFont(f);
    m_changes->addItem(item);
    m_changes->addItem("");

    changes = getLexicon()->changeFontInStylesheet(files[i].absoluteFilePath(),arabicFont);
    m_changes->addItems(changes);
    m_changes->addItem("");
  }

}
