#include "fontchangedialog.h"
#include "application.h"
#include "definedsettings.h"
#include "externs.h"
#define FONT_SIZE_WIDTH 40
#define LEAD_SPACING 50
#define SPACING 20
FontChangeWidget::FontChangeWidget(QLocale::Script script,QWidget * parent) : QWidget(parent) {
  readSettings();
  m_script = script;
  m_modified = false;
  setObjectName("fontchangewidget");
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
  if (m_script == QLocale::ArabicScript) {
    m_arabicFont->addItems(fdb.families(QFontDatabase::Arabic));
    m_fontLabel = new QLabel(tr("Fonts"));
  }
  else {
    m_arabicFont->addItems(fdb.families(QFontDatabase::Latin));
    m_fontLabel = new QLabel(tr("Fonts"));
  }
  formlayout->addRow(tr("Most used font"),m_currentFont);
  QHBoxLayout * fontlayout = new QHBoxLayout;
  fontlayout->addWidget(m_arabicFont);
  fontlayout->addStretch();
  formlayout->addRow(m_fontLabel,fontlayout);
  QHBoxLayout * sizelayout = new QHBoxLayout;
  sizelayout->addWidget(m_fontSize);
  sizelayout->addSpacing(10);
  sizelayout->addWidget(new QLabel(tr("(-1 to leave unchanged)")));
  sizelayout->addStretch();
  formlayout->addRow(tr("Font size"),sizelayout);
  formlayout->addRow(tr("Show all fonts"),m_allFonts);
  formlayout->addRow(new QLabel(tr("Apply changes to:")));
  formlayout->addRow(tr("  Configuration settings"),m_changeIni);
  formlayout->addRow(tr("  Application stylesheet"),m_changeApplicationCss);
  formlayout->addRow(tr("  Lexicon stylesheet"),m_changeEntryCss);
  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addWidget(m_applyButton);
  hlayout->addStretch();
  formlayout->addRow(tr(""),hlayout);


  layout->addLayout(formlayout);
  layout->addWidget(m_changes);
  layout->addStretch();
  setLayout(layout);

  m_changes->addItem(tr("Changes will be detailed here"));
  setCurrentFontText();
  connect(m_arabicFont,SIGNAL(currentTextChanged(const QString &)),this,SLOT(onFontChanged(const QString &)));
  connect(m_allFonts,SIGNAL(stateChanged(int)),this,SLOT(onShowAllChanged(int)));
  connect(m_applyButton,SIGNAL(clicked()),this,SLOT(onApply()));
}
void FontChangeWidget::setCurrentFontText() {
  QMap<QString,int>  fonts;
  QString script;

  if (m_script == QLocale::ArabicScript) {
    fonts = getLexicon()->getUsedFont("arabic",false);
    script = "Arabic";
  }
  else {
    fonts = getLexicon()->getUsedFont("arabic",true);
    script = "Latin";
  }
  if (fonts.size() <= 0) {
    m_currentFont->setText(QString(tr("Unable determine current %1 font")).arg(script));
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
  m_currentFont->setText(mainFont); //str.join(","));
}
/*
QSize FontChangeWidget::sizeHint() const {
  return QSize(600,400);
}
*/
void FontChangeWidget::setSelector(const QString & selector,bool match) {
  m_selector = selector;
  m_matching = match;
}
void FontChangeWidget::readSettings() {
}
void FontChangeWidget::writeSettings() {
}
void FontChangeWidget::onFontChanged(const QString & family) {
}
void FontChangeWidget::onShowAllChanged(int state) {
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
void FontChangeWidget::onApply() {
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
    t = QString(tr("Setting changes in %1 :")).arg(QDir::current().relativeFilePath(t));
    item = new QListWidgetItem(t);
    item->setFont(f);
    m_changes->addItem(item);
    m_changes->addItem("");
    changes = getLexicon()->changeFontInSettings(m_selector,m_matching,arabicFont,fontSize);
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
      t = QString(tr("CSS changes in %1")).arg(QDir::current().relativeFilePath(fileName));
      item = new QListWidgetItem(t);
      item->setFont(f);
      m_changes->addItem(item);
      m_changes->addItem("");
      if (! m_selector.isEmpty()) {
        changes = getLexicon()->changeFontInStylesheet(fileName,m_selector,m_matching,arabicFont,fontSize);
      }
      m_changes->addItems(changes);
      m_changes->addItem("");
      m_modified = true;
    }
  }
  setCurrentFontText();
}
bool FontChangeWidget::isModified() const {
  return m_modified;
}
FontChangeDialog::FontChangeDialog(QWidget * parent) : QDialog(parent) {
  setWindowTitle(tr("Change Font"));
  QVBoxLayout * layout = new QVBoxLayout;
  QHBoxLayout * fontlayout = new QHBoxLayout;

  QGroupBox * arabicBox = new QGroupBox(tr("Arabic Writing System"));
  QVBoxLayout * arabiclayout = new QVBoxLayout;
  m_arabicFont = new FontChangeWidget(QLocale::ArabicScript,this);
  m_arabicFont->setSelector("arabic",1);
  arabiclayout->addWidget(m_arabicFont);
  arabicBox->setLayout(arabiclayout);

  QGroupBox * latinBox = new QGroupBox(tr("Latin Writing System"));
  QVBoxLayout * latinlayout = new QVBoxLayout;
  m_latinFont = new FontChangeWidget(QLocale::LatinScript,this);
  m_latinFont->setSelector("arabic",0);
  latinlayout->addWidget(m_latinFont);
  latinBox->setLayout(latinlayout);

  fontlayout->addWidget(arabicBox);
  fontlayout->addWidget(latinBox);

  QDialogButtonBox *   buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);

  QPushButton * helpbutton = buttonBox->addButton(QDialogButtonBox::Help);
  connect(helpbutton,SIGNAL(clicked()),this,SLOT(onHelp()));

  connect(buttonBox,SIGNAL(rejected()),this,SLOT(reject()));
  layout->addLayout(fontlayout);
  layout->addWidget(buttonBox);
  setLayout(layout);
  SETTINGS

  settings.beginGroup("Dialog State");
  QSize sz = settings.value(QString("%1 size").arg(this->metaObject()->className())).toSize();
  QPoint p  =settings.value(QString("%1 position").arg(this->metaObject()->className())).toPoint();
  if (! sz.isNull()) {
    this->resize(sz);
  }
  if (! p.isNull()) {
    this->move(p);
  }

}
FontChangeDialog::~FontChangeDialog() {
  SETTINGS
  settings.beginGroup("Dialog State");
  settings.setValue(QString("%1 position").arg(this->metaObject()->className()),this->pos());
  settings.setValue(QString("%1 size").arg(this->metaObject()->className()),this->size());
}
void FontChangeDialog::onHelp() {
  emit(showHelp(this->metaObject()->className()));
}
bool FontChangeDialog::isModified() const {
  return m_modified;
}
