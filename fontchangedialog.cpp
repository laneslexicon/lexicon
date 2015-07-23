#include "fontchangedialog.h"
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
  setObjectName("fontchangedialog");
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
  m_fontSize = new QLineEdit;
  m_fontSize->setText("-1");
  QValidator * sizeValidator = new QIntValidator(-1,100,this);
  m_fontSize->setValidator(sizeValidator);
  m_changes = new QListWidget;
  m_changes->setEnabled(false);
  m_applyButton = new QPushButton(tr("Change font"));
  QFontDatabase fdb;
  m_arabicFont->addItems(fdb.families(QFontDatabase::Arabic));
  m_fontLabel = new QLabel(tr("Available Arabic fonts"));
  formlayout->addRow(tr("Current fonts used"),m_currentFont);
  formlayout->addRow(m_fontLabel,m_arabicFont);
  QHBoxLayout * sizelayout = new QHBoxLayout;
  sizelayout->addWidget(m_fontSize);
  sizelayout->addSpacing(10);
  sizelayout->addWidget(new QLabel(tr("(-1 to leave unchanged)")));
  sizelayout->addStretch();
  formlayout->addRow(tr("Font size"),sizelayout);
  formlayout->addRow(tr("Show all fonts"),m_allFonts);
  formlayout->addRow(new QLabel(tr("Apply changes to:")));
  formlayout->addRow(tr("\tINI settings"),m_changeIni);
  formlayout->addRow(tr("\tApplication style"),m_changeApplicationCss);
  formlayout->addRow(tr("\tPage style"),m_changeEntryCss);
  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addWidget(m_applyButton);
  hlayout->addStretch();
  formlayout->addRow(tr(""),hlayout);

  QDialogButtonBox *   buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);

  QPushButton * helpbutton = buttonBox->addButton(QDialogButtonBox::Help);
  connect(helpbutton,SIGNAL(clicked()),this,SLOT(onHelp()));

  connect(buttonBox,SIGNAL(rejected()),this,SLOT(reject()));

  layout->addLayout(formlayout);
  layout->addWidget(m_changes);
  layout->addWidget(buttonBox);
  layout->addStretch();
  setLayout(layout);

  m_changes->addItem(tr("Changes will be detailed here"));
  setCurrentFontText();
  connect(m_arabicFont,SIGNAL(currentTextChanged(const QString &)),this,SLOT(onFontChanged(const QString &)));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(m_allFonts,SIGNAL(stateChanged(int)),this,SLOT(onShowAllChanged(int)));
  connect(m_applyButton,SIGNAL(clicked()),this,SLOT(onApply()));
}
void FontChangeDialog::setCurrentFontText() {
  QString t = "In the current Arabic ";
  QMap<QString,int>  fonts = getLexicon()->getUsedFont("arabic");
  if (fonts.size() <= 0) {
    m_currentFont->setText(tr("Unable determine current Arabic font"));
    return;
  }
  QMultiMap<int,QString> mm;
  QMapIterator<QString,int> iter(fonts);
  /**
   * insert into the multimap which will be ascending usage count order
   * so we can build the font usage string by taking the last
   *
   */
  while(iter.hasNext()) {
    iter.next();
    mm.insert(iter.value(),iter.key());
  }
  QStringList str;
  QList<int> keys = mm.keys();
  QString mainFont = mm.value(keys.last());
  while(keys.size() > 0) {
    int ix = keys.takeLast();
    QString f = mm.value(ix);
    str <<  QString("%1 (%2)").arg(f).arg(ix);
    mm.remove(ix,f);
  }
  /// set the combo to the most used font
  m_arabicFont->setCurrentText(mainFont);
  /// set the font usage text
  m_currentFont->setText(str.join(","));
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
  bool ok = false;
  QString arabicFont = m_arabicFont->currentText();
  int fontSize = m_fontSize->text().toInt(&ok);
  if (! ok ) {
    fontSize = -1;
  }
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
    changes = getLexicon()->changeFontInSettings(arabicFont,fontSize);
    m_changes->addItems(changes);
    m_changes->addItem("");
    m_modified = true;
  }
  QDir cssDirectory(getLexicon()->getResourceFilePath(Lexicon::Stylesheet));
  QStringList filters;
  filters << "*.css";
  QFileInfoList files =  cssDirectory.entryInfoList(filters);
  QString fileName;
  ok = true;
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

      changes = getLexicon()->changeFontInStylesheet(fileName,arabicFont,fontSize);
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
void FontChangeDialog::onHelp() {
  emit(showHelp(this->objectName()));
}
