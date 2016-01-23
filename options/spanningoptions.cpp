#include "spanningoptions.h"
#include "definedsettings.h"
#include "externs.h"
#include "QsLog.h"
SpanningOptions::SpanningOptions(const QString & theme,QWidget * parent) : OptionsWidget(theme,parent) {
  setObjectName("spanningoptions");
  m_section = "SpannedText";


  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);
  settings.beginGroup("Arabic");
  m_keys = settings.childKeys();
  for(int i=0;i < m_keys.size();i++) {
    m_names.insert(m_keys[i],QString());
  }

  m_names.insert("bookmarklist",tr("Bookmark list"));

  m_names.insert("fullsearch",tr("Lexicon search results list"));
  m_names.insert("fullsearchresults",tr("Lexicon search results summary"));

  m_names.insert("headsearchlist",tr("Headword search results list"));
  m_names.insert("headsearchresults",tr("Headword search results summary"));

  m_names.insert("linktext",tr("Cross reference info box"));
  m_names.insert("nodeview",tr("Entry viewer heading"));
  m_names.insert("notebrowser",tr("Note browser"));
  m_names.insert("pageset",tr("Tab Set lists"));
  m_names.insert("rootnotfound",tr("Root not found"));
  m_names.insert("wordnotfound",tr("Word not found"));
  m_names.insert("tablist",tr("Tab List entries"));
  m_names.insert("messagebox",tr("Message box"));
  QVBoxLayout * vlayout = new QVBoxLayout;

  QFormLayout * formlayout = new QFormLayout;
  QLineEdit * edit;
  QString str;

  for(int i=0;i < m_keys.size();i++) {
    edit = new QLineEdit;
    m_edits.insert(m_keys[i],edit);
    str = m_names.value(m_keys[i]);
    if (str.isEmpty()) {
      str = m_keys[i];
    }
    formlayout->addRow(str,edit);
  }
  vlayout->addLayout(formlayout);

  /*
  m_info = new QLabel;

  QStringList html;
  html << "<br/><p>";
  html << tr("Please see the documentation for information about these CSS fragments.");
  html << "</p>";
  m_info->setText(html.join(""));
  vlayout->addWidget(m_info);
  */
  vlayout->addStretch();
  setLayout(vlayout);
  addButtons();
  readSettings();
  setupConnections();
}
void SpanningOptions::readSettings() {
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);
  settings.beginGroup("Arabic");

  for(int i=0;i < m_keys.size();i++) {
    QLineEdit * edit = m_edits.value(m_keys[i]);
    edit->setText(settings.value(m_keys[i],QString()).toString());
  }

}
void SpanningOptions::writeSettings(const QString & fileName) {
  QString f = m_settingsFileName;
  if (!fileName.isEmpty()) {
    f = fileName;
  }
  QSettings settings(f,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);
  settings.beginGroup("Arabic");

  for(int i=0;i < m_keys.size();i++) {
    QLineEdit * edit = m_edits.value(m_keys[i]);
    settings.setValue(m_keys[i],edit->text());
  }
}
bool SpanningOptions::isModified()  {
  m_dirty = false;
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  settings.beginGroup("Arabic");

  for(int i=0;i < m_keys.size();i++) {
    if (compare(&settings,m_keys[i],m_edits.value(m_keys[i]))) {
      m_dirty = true;
    }
  }
  return m_dirty;
}
void SpanningOptions::onSetFont() {
}
