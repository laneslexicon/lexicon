#include "findoptions.h"
#include "definedsettings.h"
#include "QsLog.h"
#include "popupoptions.h"
#ifndef STANDALONE
#include "application.h"
#endif
/**
 * Not done:
 *
 * @param theme
 * @param parent
 */
FindOptions::FindOptions(const QString & theme,QWidget * parent) : OptionsWidget(theme,parent) {
  m_section = "Entry";
  QVBoxLayout * layout = new QVBoxLayout;
  QGroupBox * fullbox = new QGroupBox(tr("Full text search"));

  m_fullDebug = new QCheckBox;
  m_fullFragmentSize = new QSpinBox;
  m_fullIncludeHeads = new QCheckBox;
  m_fullOneRow = new QCheckBox;
  m_fullStep = new QSpinBox;
  //  m_fullViewerWidth = new QLineEdit;
  //  m_fullViewerHeight = new QLineEdit;
  //  m_fullXslt = new QLineEdit;
  m_fullHeadColor = new QLineEdit;
  m_fullHeadText = new QLineEdit;

  /// these are for the SearchDialog box
  /*
  m_fullNewTab = new QCheckBox;
  m_fullGoTab = new QCheckBox;
  m_fullWholeWord = new QCheckBox;
  m_fullDiacritics = new QCheckBox;
  m_fullRegex  = new QCheckBox;
  */
  QFormLayout * fulllayout = new QFormLayout;
  QHBoxLayout * colorlayout = new QHBoxLayout;
  colorlayout->addWidget(m_fullHeadColor);
  QPushButton * colorbutton = new QPushButton(tr("Set colour"));
  connect(colorbutton,SIGNAL(clicked()),this,SLOT(onSetColor()));
  colorlayout->addWidget(colorbutton);
  colorlayout->addStretch();


  fulllayout->addRow(tr("Include head words in\nsearch results"),m_fullIncludeHeads);
  fulllayout->addRow(tr("Fragment size"),m_fullFragmentSize);
  fulllayout->addRow(tr("Head word background colour"),colorlayout);
  fulllayout->addRow(tr("Step size"),m_fullStep);
  fulllayout->addRow(tr("Text for head word results"),m_fullHeadText);
  fulllayout->addRow(tr("One row for each entry"),m_fullOneRow);
  /*
  fulllayout->addRow(tr("Whole word"),m_fullWholeWord);
  fulllayout->addRow(tr("Ignore diacritics"),m_fullDiacritics);
  fulllayout->addRow(tr("Regular exprssion search"),m_fullRegex);
  fulllayout->addRow(tr("Open in new tab"),m_fullNewTab);
  fulllayout->addRow(tr("Go to new tab"),m_fullGoTab);
  */
  fulllayout->addRow(tr("Progress interval"),m_fullStep);
  QPushButton * fullbtn = new QPushButton(tr("Set"));
  fulllayout->addRow(tr("Search dialog options"),fullbtn);
  fulllayout->addRow(tr("Debug"),m_fullDebug);

  connect(fullbtn,SIGNAL(clicked()),this,SLOT(onHeadDialog()));

  fullbox->setLayout(fulllayout);



  /// Head word search
  QGroupBox * headbox = new QGroupBox(tr("Head word search"));
  m_headDebug = new QCheckBox;
  m_headStep = new QSpinBox;
  m_headVertical = new QCheckBox;
  m_headFocusTable = new QCheckBox;
  /*
  m_headNewTab = new QCheckBox;
  m_headGoTab = new QCheckBox;
  m_headWholeWord = new QCheckBox;
  m_headDiacritics = new QCheckBox;
  m_headRegex  = new QCheckBox;
  */
  QFormLayout * headlayout = new QFormLayout;
  headlayout->addRow(tr("Vertical layout"),m_headVertical);
  headlayout->addRow(tr("Step interval"),m_headStep);
  headlayout->addRow(tr("Initial focus on results"),m_headFocusTable);
  //  headlayout->addRow(tr("Open in new tab"),m_headNewTab);
  //  headlayout->addRow(tr("Go to new tab"),m_headGoTab);
  headlayout->addRow(tr("Debug"),m_headDebug);
  headbox->setLayout(headlayout);

  /// Local search
  QGroupBox * localbox = new QGroupBox(tr("Local search"));
  m_localForce      = new QCheckBox;
  m_localIgnore     = new QCheckBox;
  m_localWholeWord  = new QCheckBox;
  m_localRegex      = new QCheckBox;
  m_localShowAll    = new QCheckBox;
  QFormLayout * locallayout = new QFormLayout;
  locallayout->addRow(tr("Regular expression search"),m_localRegex);
  locallayout->addRow(tr("Ignore diacritics"),m_localIgnore);
  locallayout->addRow(tr("Whole word match"),m_localWholeWord);
  locallayout->addRow(tr("Show all results"),m_localShowAll);
  locallayout->addRow(tr("Force Left-to-Right input\nfor regular expression search"),m_localForce);

  localbox->setLayout(locallayout);


  layout->addWidget(fullbox);
  layout->addWidget(headbox);
  layout->addWidget(localbox);

  setLayout(layout);
  addButtons();
  readSettings();
  setupConnections();

}

void FindOptions::readSettings() {
  qDebug() << Q_FUNC_INFO << m_settingsFileName;
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  m_fullDebug->setChecked(settings.value(SID_FULLSEARCH_DEBUG,false).toBool());
  m_fullFragmentSize->setValue(settings.value(SID_FULLSEARCH_FRAGMENT_SIZE,50).toInt());
  m_fullIncludeHeads->setChecked(settings.value(SID_FULLSEARCH_INCLUDE_HEADS,true).toBool());
  m_fullOneRow->setChecked(settings.value(SID_FULLSEARCH_ONE_ROW,true).toBool());
  m_fullStep->setValue(settings.value(SID_FULLSEARCH_STEP,50).toInt());
  m_fullHeadColor->setText(settings.value(SID_FULLSEARCH_HEAD_BACKGROUND).toString());


  m_fullNewTab     = settings.value(SID_FULLSEARCH_NEW_TAB,true).toBool();
  m_fullGoTab      = settings.value(SID_FULLSEARCH_GO_TAB,true).toBool();
  m_fullWholeWord  = settings.value(SID_FULLSEARCH_WHOLE_WORD,true).toBool();
  m_fullDiacritics = settings.value(SID_FULLSEARCH_DIACRITICS,true).toBool();
  m_fullRegex      = settings.value(SID_FULLSEARCH_TYPE_REGEX,true).toBool();


  //  m_viewerSize = settings.value(SID_FULLSEARCH_VIEWER_SIZE,QSize(600,400)).toSize();

  /*

  m_fullWholeWord->setChecked(settings.value(SID_FULLSEARCH_WHOLE_WORD,true).toBool());
 m_fullXslt;
 m_fullHeadColor;
 m_fullHeadText;

 m_headDebug->setChecked(
m_headStep;
 m_headVertical->setChecked(
 m_headFocusTable->setChecked(
  m_headNewTab->setChecked(settings.value(SID_HEADSEARCH_NEW_TAB,true).toBool());
  m_headGoTab->setChecked(settings.value(SID_HEADSEARCH_GO_TAB,true).toBool());

 m_localForce->setChecked(
 m_localIgnore->setChecked(
 m_localWholeWord->setChecked(
 m_localRegex->setChecked(
 m_localShowAll->setChecked(
  */
  m_dirty = false;
}
void FindOptions::writeSettings() {
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);



  settings.sync();
  settings.endGroup();
  emit(modified(false));
}
/**
 * TODO not complete
 *
 *
 * @return
 */
bool FindOptions::isModified()  {
  m_dirty = false;
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);
  /*
 m_fullDebug;
 m_fullFragmentSize;
 m_fullIncludeHeads;
 m_fullOneRow;
m_fullStep;
 m_fullViewerWidth;
 m_fullViewerHeight;
 m_fullWholeWord;
 m_fullXslt;
 m_fullHeadColor;
 m_fullHeadText;
rd search
 m_headDebug;
m_headStep;
 m_headVertical;
 m_headFocusTable;
earch
 m_localForce;
 m_localIgnore;
 m_localWholeWord;
 m_localRegex;
 m_localShowAll;
  */

  return m_dirty;
}
void FindOptions::onSetFont() {
  /*
  bool arabic = false;
  QPushButton * button = qobject_cast<QPushButton *>(QObject::sender());
  QFont f;
  if (button == m_standardButton) {
    f.fromString(m_standardFont->text());
  }
  else {
    arabic = true;
    f.fromString(m_arabicFont->text());
  }
  if (arabic) {
    QFontDialog * d = new QFontDialog(f);
    QList<QComboBox *> boxes = d->findChildren<QComboBox *>();
    if (boxes.size() == 1) {
      int ix = boxes[0]->findText("Arabic");
      if (ix >= 0) {
        boxes[0]->setCurrentText("Arabic");
        boxes[0]->activated(ix);
      }
    }
    d->setCurrentFont(f);
    if (d->exec() == QDialog::Accepted) {
      QFont font = d->currentFont();
      m_arabicFont->setText(font.toString());
    }
    delete d;
    return;
  }
  bool ok;
  QFont font = QFontDialog::getFont(&ok, f,  this);
  if (!ok) {
    return;
  }
  m_standardFont->setText(font.toString());
  */
}
void FindOptions::onSetColor() {
  QColor color;
  color.setNamedColor(m_fullHeadColor->text());
  QColorDialog d(color);
  if (d.exec() != QDialog::Accepted) {
    return;
  }
  int r,g,b;
  color = d.currentColor();
  color.getRgb(&r,&g,&b);
  QString str = QString("%1,%2,%3").arg(r).arg(g).arg(b);
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup("Colors");
  QStringList keys = settings.allKeys();
  QStringList v;
  for(int i=0;i < keys.size();i++) {
    v = settings.value(keys[i]).toStringList();
    if (v.join(",") == str) {
      m_fullHeadColor->setText(keys[i]);
      return;
    }
  }
  m_fullHeadColor->setText(d.currentColor().name());
}
void FindOptions::onFullDialog() {
  DialogOptions d;
  d.setChecked(DialogOptions::Tab,m_fullNewTab);
  d.setChecked(DialogOptions::Go,m_fullGoTab);
  d.setChecked(DialogOptions::Whole,m_fullWholeWord);
  d.setChecked(DialogOptions::Diacritics,m_fullDiacritics);
  d.setChecked(DialogOptions::Regex,m_fullRegex);
  if (d.exec() == QDialog::Accepted) {
    m_fullNewTab =  d.isChecked(DialogOptions::Tab);
    m_fullGoTab = d.isChecked(DialogOptions::Go);
    m_fullWholeWord = d.isChecked(DialogOptions::Whole);
    m_fullDiacritics = d.isChecked(DialogOptions::Diacritics);
    m_fullRegex = d.isChecked(DialogOptions::Regex);
  }
}
void FindOptions::onHeadDialog() {
  DialogOptions d;
  d.setChecked(DialogOptions::Tab,m_headNewTab);
  d.setChecked(DialogOptions::Go,m_headGoTab);
  d.setChecked(DialogOptions::Whole,m_headWholeWord);
  d.setChecked(DialogOptions::Diacritics,m_headDiacritics);
  d.setChecked(DialogOptions::Regex,m_headRegex);
  if (d.exec() == QDialog::Accepted) {
    m_headNewTab =  d.isChecked(DialogOptions::Tab);
    m_headGoTab = d.isChecked(DialogOptions::Go);
    m_headWholeWord = d.isChecked(DialogOptions::Whole);
    m_headDiacritics = d.isChecked(DialogOptions::Diacritics);
    m_headRegex = d.isChecked(DialogOptions::Regex);
  }
}
