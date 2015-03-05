#include "bookmarkoptions.h"
#include "definedsettings.h"
#include "QsLog.h"
#ifndef STANDALONE
#include "application.h"
#endif
/**
 * Not done:
 *
 * @param theme
 * @param parent
 */
BookmarkOptions::BookmarkOptions(const QString & theme,QWidget * parent) : OptionsWidget(theme,parent) {
  m_section = "Bookmark";

  m_id = new QLineEdit;
  m_font = new QLineEdit;
  m_add = new QKeySequenceEdit;
  m_list = new QKeySequenceEdit;
  m_jump = new QKeySequenceEdit;
  m_revert = new QKeySequenceEdit;
  m_clear = new QKeySequenceEdit;
  m_newTab = new QCheckBox;
  m_goTab = new QCheckBox;

  QVBoxLayout * vlayout = new QVBoxLayout;

  QGroupBox * shortcutsbox = new QGroupBox;
  QFormLayout * layout1 = new QFormLayout;
  layout1->addRow("Add",m_add);
  layout1->addRow("List",m_list);
  layout1->addRow("Jump",m_jump);
  layout1->addRow("Revert",m_revert);
  layout1->addRow("Clear",m_clear);
  shortcutsbox->setLayout(layout1);

  QFormLayout * layout2 = new QFormLayout;
  layout2->addRow("Ids",m_id);

  QHBoxLayout * hlayout = new QHBoxLayout;
  QPushButton * btn  = new QPushButton(tr("Click to set font"));
  connect(btn,SIGNAL(clicked()),this,SLOT(onSetFont()));

  hlayout->addWidget(m_font);
  hlayout->addWidget(btn);
  hlayout->addStretch();

  layout2->addRow(tr("Arabic font"),hlayout);
  layout2->addRow("Open in new tab",m_newTab);
  layout2->addRow("Activate new tab",m_goTab);

  vlayout->addWidget(shortcutsbox);
  vlayout->addLayout(layout2);
  vlayout->addStretch();
  setLayout(vlayout);
  addButtons();
  readSettings();
  setupConnections();

}

void BookmarkOptions::readSettings() {
  qDebug() << Q_FUNC_INFO << m_settingsFileName;
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  m_dirty = false;
}
void BookmarkOptions::writeSettings(const QString & fileName) {
  qDebug() << Q_FUNC_INFO << m_settingsFileName;
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

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
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);


  return m_dirty;
}
void BookmarkOptions::onSetFont() {
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
