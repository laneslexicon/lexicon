#include "systemoptions.h"
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
SystemOptions::SystemOptions(const QString & theme,QWidget * parent) : OptionsWidget(theme,parent) {
  m_section = "System";
  QVBoxLayout * vlayout = new QVBoxLayout;


  m_arabicFont = new QLineEdit;
  m_color = new QLineEdit;
  m_contentsLinked = new QCheckBox;
  m_currentMap = new QLineEdit;
  m_lexicon = new QLineEdit;
  m_debug = new QCheckBox;
  m_docked = new QCheckBox;
  m_focusTab = new QCheckBox;
  // Icon siz
  m_mapsEnabled = new QCheckBox;
  m_minimalInterface = new QCheckBox;
  // Navigati
  m_nullMapName = new QLineEdit;

  m_linksInCurrentTab = new QCheckBox;
  m_linksActivate = new QCheckBox;

  m_restoreBookmarks = new QCheckBox;
  m_restoreTabs = new QCheckBox;
  m_saveSettings = new QCheckBox;
  m_saveTabs = new QCheckBox;


  // Root mod
  m_runData = new QLineEdit;
  // Save boo
  m_showInterfaceWarning = new QCheckBox;
  m_css = new QLineEdit;
  m_theme = new QLineEdit;
  m_title = new QLineEdit;
  m_toolbarText = new QCheckBox;
  m_useNotes = new QCheckBox;

  vlayout->addStretch();
  setLayout(vlayout);
  addButtons();
  readSettings();
  setupConnections();

}

void SystemOptions::readSettings() {
  qDebug() << Q_FUNC_INFO << m_settingsFileName;
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  m_dirty = false;
}
void SystemOptions::writeSettings(const QString & fileName) {
  QString f = m_settingsFileName;
  if (!fileName.isEmpty()) {
    f = fileName;
  }
  qDebug() << Q_FUNC_INFO << f;

  QSettings settings(f,QSettings::IniFormat);
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
bool SystemOptions::isModified()  {
  m_dirty = false;
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  return m_dirty;
}
void SystemOptions::onSetFont() {
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
  return;
  */
}
void SystemOptions::onSetColor() {
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
