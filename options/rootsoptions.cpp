#include "rootsoptions.h"
#include "definedsettings.h"
#include "QsLog.h"
#ifndef STANDALONE
#include "application.h"
#include "externs.h"

#endif
/**
 * Not done:
 *  supplement background colour
 *
 * @param theme
 * @param parent
 */
RootsOptions::RootsOptions(const QString & theme,QWidget * parent) : OptionsWidget(theme,parent) {
  m_section = "Roots";

  QVBoxLayout * vlayout = new QVBoxLayout;
  QFormLayout * formlayout = new QFormLayout;

  m_moveDown = new QLineEdit;
  m_moveDown->setMaxLength(1);
  m_moveDown->setMaximumWidth(SMALL_EDIT);
  m_moveUp = new QLineEdit;
  m_moveUp->setMaxLength(1);
  m_moveUp->setMaximumWidth(SMALL_EDIT);
  m_expand = new QLineEdit;
  m_expand->setMaxLength(1);
  m_expand->setMaximumWidth(SMALL_EDIT);

  m_singleClick = new QCheckBox;
  m_doubleClick = new QCheckBox;
  m_showHeadColumn = new QCheckBox;
  m_showEntryColumn = new QCheckBox;
  m_showNodeColumn = new QCheckBox;
  m_romanItypes = new QCheckBox;
  m_debug = new QCheckBox;


  formlayout->addRow(tr("Show head word"),m_showHeadColumn);
  formlayout->addRow(tr("Show multi-head word"),m_showEntryColumn);
  formlayout->addRow(tr("Show node"),m_showNodeColumn);
  formlayout->addRow(tr("Verb form number in Roman format"),m_romanItypes);
  formlayout->addRow(tr("Move up"),m_moveUp);
  formlayout->addRow(tr("Move down"),m_moveDown);
  formlayout->addRow(tr("Expand/collapse"),m_expand);
  formlayout->addRow(tr("Single click activates item"),m_singleClick);
  formlayout->addRow(tr("Double click activates item"),m_doubleClick);

  QHBoxLayout * hlayout1 = new QHBoxLayout;
  m_standardFont = new QLineEdit;
  m_standardFont->setMaximumWidth(VLARGE_EDIT);
  m_standardButton = new QPushButton(tr("Click to set font"));
  hlayout1->addWidget(m_standardFont);
  hlayout1->addWidget(m_standardButton);
  formlayout->addRow(tr("Non-Arabic font"),hlayout1);

  QHBoxLayout * hlayout2 = new QHBoxLayout;
  m_arabicFont = new QLineEdit;
  m_arabicFont->setMaximumWidth(VLARGE_EDIT);
  m_arabicButton = new QPushButton(tr("Click to set font"));
  hlayout2->addWidget(m_arabicFont);
  hlayout2->addWidget(m_arabicButton);
  formlayout->addRow(tr("Arabic font"),hlayout2);

  connect(m_standardButton,SIGNAL(clicked()),this,SLOT(onSetFont()));
  connect(m_arabicButton,SIGNAL(clicked()),this,SLOT(onSetFont()));


  formlayout->addRow(tr("Enable debug"),m_debug);

  vlayout->addLayout(formlayout);

  m_info = new QLabel;

  QStringList html;
  html << "<h4>";
  html << tr("Notes:");
  html << "</h4>";
  html << "<ul>";
  html << "<li>";
  html << tr("Up/down arrows  can be used to move around the tree");
  html << "</li>";
  html << "<li>";
  html << tr("Space will expand/collapse the current node");
  html << "</li>";
  html << "<li>";
  html << tr("Return on a head word will activate the item");
  html << "</li>";
  html << "</ul>";
  m_info->setText(html.join(""));
  vlayout->addWidget(m_info);
  setLayout(vlayout);
  addButtons();
  readSettings();
  setupConnections();
  //  getLexicon()->setCursorPosition(this);
}
void RootsOptions::readSettings() {
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  m_debug->setChecked(settings.value(SID_CONTENTS_DEBUG).toBool());
  m_moveUp->setText(settings.value(SID_CONTENTS_MOVE_UP).toString());
  m_moveDown->setText(settings.value(SID_CONTENTS_MOVE_DOWN).toString());
  m_expand->setText(settings.value(SID_CONTENTS_EXPAND).toString());

  m_arabicFont->setText(settings.value(SID_CONTENTS_ARABIC_FONT).toString());
  m_standardFont->setText(settings.value(SID_CONTENTS_STANDARD_FONT).toString());

  m_showHeadColumn->setChecked(settings.value(SID_CONTENTS_SHOWHEAD,true).toBool());
  m_showEntryColumn->setChecked(settings.value(SID_CONTENTS_SHOWENTRY,false).toBool());
  m_showNodeColumn->setChecked(settings.value(SID_CONTENTS_SHOWNODE,false).toBool());
  m_romanItypes->setChecked(settings.value(SID_CONTENTS_ROMAN_ITYPES,false).toBool());

  settings.endGroup();
  m_dirty = false;
}
void RootsOptions::writeSettings(const QString & fileName) {
  QString f = m_settingsFileName;
  if (!fileName.isEmpty()) {
    f = fileName;
  }
  QSettings settings(f,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);
  settings.setValue(SID_CONTENTS_SHOWHEAD,m_showHeadColumn->isChecked());
  settings.setValue(SID_CONTENTS_SHOWENTRY,m_showEntryColumn->isChecked());
  settings.setValue(SID_CONTENTS_SHOWNODE,m_showNodeColumn->isChecked());
  settings.setValue(SID_CONTENTS_ROMAN_ITYPES,m_romanItypes->isChecked());
  settings.setValue(SID_CONTENTS_DEBUG,m_debug->isChecked());
  settings.setValue(SID_CONTENTS_MOVE_UP,m_moveUp->text());
  settings.setValue(SID_CONTENTS_MOVE_DOWN,m_moveDown->text());
  settings.setValue(SID_CONTENTS_EXPAND,m_expand->text());
  settings.setValue(SID_CONTENTS_ARABIC_FONT,m_arabicFont->text());
  settings.setValue(SID_CONTENTS_STANDARD_FONT,m_standardFont->text());
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
bool RootsOptions::isModified()  {
  m_dirty = false;
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  if (compare(&settings,SID_CONTENTS_SHOWHEAD,m_showHeadColumn)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_CONTENTS_SHOWENTRY,m_showEntryColumn)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_CONTENTS_SHOWNODE,m_showNodeColumn)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_CONTENTS_ROMAN_ITYPES,m_romanItypes)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_CONTENTS_DEBUG,m_debug)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_CONTENTS_MOVE_UP,m_moveUp)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_CONTENTS_MOVE_DOWN,m_moveDown)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_CONTENTS_EXPAND,m_expand)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_CONTENTS_ARABIC_FONT,m_arabicFont)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_CONTENTS_STANDARD_FONT,m_standardFont)) {
    m_dirty = true;
  }
  settings.endGroup();
  return m_dirty;
}
void RootsOptions::onSetFont() {
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
}
