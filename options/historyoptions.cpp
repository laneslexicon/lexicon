#include "historyoptions.h"
#include "definedsettings.h"
#include "QsLog.h"
#ifndef STANDALONE
#include "application.h"
#endif
/**
 *
 * @param theme
 * @param parent
 */
HistoryOptions::HistoryOptions(const QString & theme,QWidget * parent) : OptionsWidget(theme,parent) {
  m_section = "History";
  setObjectName("historyoptions");

  QVBoxLayout * vlayout = new QVBoxLayout;

  QFormLayout * layout = new QFormLayout;

  m_enabled = new QCheckBox ;
  //  m_database = new QLineEdit ;
  m_menuFont = new QLineEdit;
  m_menuFormat = new QLineEdit;
  m_newTab = new QCheckBox ;
  m_goTab = new QCheckBox ;
  m_duplicateDepth = new QLineEdit;
  m_duplicateDepth->setValidator(new QIntValidator);;
  m_size = new QLineEdit ;
  m_size->setValidator(new QIntValidator);
  m_maximum = new QLineEdit ;
  m_maximum->setValidator(new QIntValidator);
  //  m_listFont = new QLineEdit;

 layout->addRow(tr("Enabled"),m_enabled);
 // layout->addRow(tr("Database"),m_database);

 QHBoxLayout * hlayout1 = new QHBoxLayout;
 m_menuButton = new QPushButton(tr("Click to set font"));
 connect(m_menuButton,SIGNAL(clicked()),this,SLOT(onSetFont()));
 hlayout1->addWidget(m_menuFont);
 hlayout1->addWidget(m_menuButton);
 hlayout1->addStretch();

 layout->addRow(tr("Menu font"),hlayout1);
 layout->addRow(tr("Menu format"),m_menuFormat);
 /*
 QHBoxLayout * hlayout2 = new QHBoxLayout;
 m_listButton = new QPushButton(tr("Click to set font"));
 connect(m_listButton,SIGNAL(clicked()),this,SLOT(onSetFont()));
 hlayout2->addWidget(m_listFont);
 hlayout2->addWidget(m_listButton);
 hlayout2->addStretch();

 layout->addRow(tr("List font"),hlayout2);
 */

 layout->addRow(tr("Open in new tab"),m_newTab);
 layout->addRow(tr("Got to new tab"),m_goTab);
 layout->addRow(tr("Duplicate search depth"),m_duplicateDepth);
 layout->addRow(tr("Menu entries limit"),m_size);
 layout->addRow(tr("Maximum database records"),m_maximum);
 vlayout->addLayout(layout);
 vlayout->addStretch();
  setLayout(vlayout);
  addButtons();
  readSettings();
  setupConnections();
  this->setLineEditSize(VLARGE_EDIT);

}

void HistoryOptions::readSettings(bool /* reload */) {
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  m_enabled->setChecked(settings.value(SID_HISTORY_ENABLED,true).toBool());

  m_menuFormat->setText(settings.value(SID_HISTORY_MENU_FORMAT,QString()).toString());
  m_menuFont->setText(settings.value(SID_HISTORY_MENU_ARABIC_FONT,QString()).toString());
  m_newTab->setChecked(settings.value(SID_HISTORY_NEW_TAB,true).toBool());
  m_goTab->setChecked(settings.value(SID_HISTORY_GO_TAB,true).toBool());
  m_duplicateDepth->setText(settings.value(SID_HISTORY_DUPLICATE_DEPTH,"20").toString());
  m_size->setText(settings.value(SID_HISTORY_SIZE,"20").toString());
  m_maximum->setText(settings.value(SID_HISTORY_MAXIMUM,50).toString());
  //  m_listFont->setText(settings.value(SID_HISTORY_LIST_ARABIC_FONT,QString()).toString());

  m_dirty = false;
}
void HistoryOptions::writeSettings(const QString & fileName) {
  QString f = m_settingsFileName;
  if (!fileName.isEmpty()) {
    f = fileName;
  }

  QSettings settings(f,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  settings.setValue(SID_HISTORY_ENABLED,m_enabled->isChecked());
  settings.setValue(SID_HISTORY_MENU_FORMAT,m_menuFormat->text());
  settings.setValue(SID_HISTORY_MENU_ARABIC_FONT,m_menuFont->text());
  settings.setValue(SID_HISTORY_NEW_TAB,m_newTab->isChecked());
  settings.setValue(SID_HISTORY_GO_TAB,m_goTab->isChecked());
  settings.setValue(SID_HISTORY_DUPLICATE_DEPTH,m_duplicateDepth->text());
  settings.setValue(SID_HISTORY_SIZE,m_size->text());
  settings.setValue(SID_HISTORY_MAXIMUM,m_maximum->text());
  //  settings.setValue(SID_HISTORY_LIST_ARABIC_FONT,m_listFont->text());
  settings.sync();
  m_dirty = false;
  emit(modified(false));
}
/**
 * TODO not complete
 *
 *
 * @return
 */
bool HistoryOptions::isModified()  {
  m_dirty = false;
  m_changes.clear();

  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  if (compare(&settings,SID_HISTORY_ENABLED,m_enabled)){
    m_dirty = true;
  }
  if (compare(&settings,SID_HISTORY_MENU_FORMAT,m_menuFormat)) {
      m_dirty = true;
  }
  if (compare(&settings,SID_HISTORY_MENU_ARABIC_FONT,m_menuFont)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_HISTORY_NEW_TAB,m_newTab)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_HISTORY_GO_TAB,m_goTab)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_HISTORY_DUPLICATE_DEPTH,m_duplicateDepth)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_HISTORY_SIZE,m_size)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_HISTORY_MAXIMUM,m_maximum)) {
    m_dirty = true;
  }
  /*
  if (compare(&settings,SID_HISTORY_LIST_ARABIC_FONT,m_listFont)) {
    m_dirty = true;
  }
  */
  return m_dirty;
}
void HistoryOptions::onSetFont() {
  QFont f;

  f.fromString(m_menuFont->text());
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
    m_menuFont->setText(font.toString());
  }
  delete d;
  return;
}
void HistoryOptions::onSetColor() {
  /*
  QColor color;
  color.setNamedColor(m_highlightColor->text());
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

      m_highlightColor->setText(keys[i]);
      return;
    }
  }
  m_highlightColor->setText(d.currentColor().name());
  */
}
