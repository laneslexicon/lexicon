#include "bookmarkoptions.h"
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
BookmarkOptions::BookmarkOptions(const QString & theme,QWidget * parent) : OptionsWidget(theme,parent) {
  m_section = "Bookmark";
  setObjectName("bookmarkoptions");
  m_id = new QLineEdit;
  m_format = new QLineEdit;
  //  m_font = new QLineEdit;
  m_add = new QKeySequenceEdit;
  m_input = new QKeySequenceEdit;
  m_list = new QKeySequenceEdit;
  m_jump = new QKeySequenceEdit;
  m_revert = new QKeySequenceEdit;
  m_clear = new QKeySequenceEdit;
  m_newTab = new QCheckBox;
  m_goTab = new QCheckBox;

  QVBoxLayout * vlayout = new QVBoxLayout;

  QGroupBox * shortcutsbox = new QGroupBox(tr("Shortcuts"));
  QFormLayout * layout1 = new QFormLayout;
  layout1->addRow(tr("Add"),m_add);
  layout1->addRow(tr("Input"),m_input);
  layout1->addRow(tr("List"),m_list);
  layout1->addRow(tr("Jump"),m_jump);
  layout1->addRow(tr("Revert"),m_revert);
  layout1->addRow(tr("Clear"),m_clear);
  shortcutsbox->setLayout(layout1);

  QFormLayout * layout2 = new QFormLayout;
  layout2->addRow(tr("Ids"),m_id);
  layout2->addRow(tr("Menu format"),m_format);
  /*
  QHBoxLayout * hlayout = new QHBoxLayout;
  QPushButton * btn  = new QPushButton(tr("Click to set font"));
  connect(btn,SIGNAL(clicked()),this,SLOT(onSetFont()));

  hlayout->addWidget(m_font);
  hlayout->addWidget(btn);
  hlayout->addStretch();

  layout2->addRow(tr("Arabic font"),hlayout);
*/
  layout2->addRow(tr("Open in new tab"),m_newTab);
  layout2->addRow(tr("Activate new tab"),m_goTab);

  vlayout->addWidget(shortcutsbox);
  vlayout->addLayout(layout2);
  vlayout->addStretch();
  setLayout(vlayout);
  addButtons();
  readSettings();
  setupConnections();
  this->setKeySequenceEditSize(VLARGE_EDIT);

  m_id->setMinimumWidth(VLARGE_EDIT);
  //  m_font->setMinimumWidth(VLARGE_EDIT);
}

void BookmarkOptions::readSettings(bool /* reload */) {
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);
  m_id->setText(settings.value(SID_BOOKMARK_ID,"abcdefghijklmnopqrstuvywxy").toString());
  m_format->setText(settings.value(SID_BOOKMARK_MENU_FORMAT,"Root:%R,Entry:%H, Vol %V/%P (%N)").toString());

  m_newTab->setChecked(settings.value(SID_BOOKMARK_NEW_TAB,false).toBool());
  m_goTab->setChecked(settings.value(SID_BOOKMARK_GO_TAB,false).toBool());

  QString value;
  value = settings.value(SID_BOOKMARK_ADD,QString()).toString();
  //  value.remove(QChar(' '));
  m_add->setKeySequence(QKeySequence(value));

  value = settings.value(SID_BOOKMARK_JUMP,QString()).toString();
  //  value.remove(QChar(' '));
  m_jump->setKeySequence(QKeySequence(value));

  value = settings.value(SID_BOOKMARK_LIST,QString()).toString();
  //  value.remove(QChar(' '));
  m_list->setKeySequence(QKeySequence(value));

  value = settings.value(SID_BOOKMARK_REVERT,QString()).toString();
  //  value.remove(QChar(' '));
  m_revert->setKeySequence(QKeySequence(value));

  value = settings.value(SID_BOOKMARK_CLEAR,QString()).toString();
  //  value.remove(QChar(' '));
  m_clear->setKeySequence(QKeySequence(value));

  settings.endGroup();
  settings.beginGroup("Shortcut");
  value = settings.value(SID_SHORTCUT_BOOKMARK_INPUT,QString()).toString();
  m_input->setKeySequence(QKeySequence(value));

  m_dirty = false;
}
void BookmarkOptions::writeSettings(const QString & fileName) {
  QString f = m_settingsFileName;
  if (!fileName.isEmpty()) {
    f = fileName;
  }

  QSettings settings(f,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  settings.setValue(SID_BOOKMARK_ADD,m_add->keySequence().toString());
  settings.setValue(SID_BOOKMARK_LIST,m_list->keySequence().toString());
  settings.setValue(SID_BOOKMARK_JUMP,m_jump->keySequence().toString());
  settings.setValue(SID_BOOKMARK_REVERT,m_revert->keySequence().toString());
  settings.setValue(SID_BOOKMARK_CLEAR,m_clear->keySequence().toString());
  settings.setValue(SID_BOOKMARK_NEW_TAB,m_newTab->isChecked());
  settings.setValue(SID_BOOKMARK_GO_TAB,m_goTab->isChecked());
  settings.setValue(SID_BOOKMARK_ID,m_id->text());
  settings.setValue(SID_BOOKMARK_MENU_FORMAT,m_format->text());
  settings.endGroup();
  settings.beginGroup("Shortcut");
  settings.setValue(SID_SHORTCUT_BOOKMARK_INPUT,m_input->keySequence().toString());
  //  settings.setValue(SID_BOOKMARK_ARABIC_FONT,m_font->text());
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
bool BookmarkOptions::isModified()  {
  m_dirty = false;
  m_changes.clear();

  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  if (compare(&settings,SID_BOOKMARK_ADD,m_add)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_BOOKMARK_LIST,m_list)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_BOOKMARK_JUMP,m_jump)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_BOOKMARK_REVERT,m_revert)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_BOOKMARK_CLEAR,m_clear)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_BOOKMARK_NEW_TAB,m_newTab)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_BOOKMARK_GO_TAB,m_goTab)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_BOOKMARK_ID,m_id)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_BOOKMARK_MENU_FORMAT,m_format)) {
    m_dirty = true;
  }
  settings.endGroup();
  settings.beginGroup("Shortcut");
  if (compare(&settings,SID_SHORTCUT_BOOKMARK_INPUT,m_input)) {
    m_dirty = true;
  }

  /*
  if (compare(&settings,SID_BOOKMARK_ARABIC_FONT,m_font)) {
    m_dirty = true;
  }
  */
  return m_dirty;
}
void BookmarkOptions::onSetFont() {
  /*
  QFont f;
  f.fromString(m_font->text());

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
    m_font->setText(font.toString());
  }
  delete d;
  */
  return;
}
void BookmarkOptions::onSetColor() {
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
