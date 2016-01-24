#include "shortcutoptions.h"
#include "definedsettings.h"
#include "QsLog.h"
#ifndef STANDALONE
#include "application.h"
#include "externs.h"
#endif
ShortcutOptions::ShortcutOptions(const QString & theme,QWidget * parent) : OptionsWidget(theme,parent) {
  setObjectName("shortcutoptions");
  m_section = "Shortcut";

  m_tabs = new QTabWidget;

  QVBoxLayout * vlayout = new QVBoxLayout;

  QStringList keys;


  keys << SID_SHORTCUT_CONTENTS_COLLAPSE_ALL << SID_SHORTCUT_CONTENTS_COLLAPSE_LETTER << SID_SHORTCUT_CONTENTS_SHOW;
  addTab("Contents",keys);
  keys.clear();

  keys << SID_SHORTCUT_DELETE_TAB << SID_SHORTCUT_DELETE_OTHER_TABS << SID_SHORTCUT_GO_TAB << SID_SHORTCUT_LIST_TABS;
  addTab("Tabs",keys);
  keys.clear();

  keys << SID_SHORTCUT_HISTORY_BACK << SID_SHORTCUT_HISTORY_NEXT << SID_SHORTCUT_HISTORY_CLEAR;
  addTab("History",keys);
  keys.clear();

  keys << SID_SHORTCUT_NAV_FIRST << SID_SHORTCUT_NAV_NEXT << SID_SHORTCUT_NAV_PREV << SID_SHORTCUT_NAV_LAST;// << SID_SHORTCUT_NAV_ROOT_MODE << SID_SHORTCUT_NAV_PAGE_MODE;
  addTab("Navigation",keys);
  keys.clear();

  keys << SID_SHORTCUT_KEYMAPS_ENABLE << SID_SHORTCUT_KEYMAPS_DISABLE;
  addTab(tr("Keymaps"),keys);
  keys.clear();

  keys << SID_SHORTCUT_SEARCH_ROOT << SID_SHORTCUT_SEARCH_HEAD << SID_SHORTCUT_SEARCH_WORD << SID_SHORTCUT_SEARCH_PAGE << SID_SHORTCUT_SEARCH_NODE;
  addTab(tr("Global Search"),keys);
  keys.clear();

  keys << SID_SHORTCUT_LOCAL_SEARCH_FIND << SID_SHORTCUT_LOCAL_SEARCH_NEXT << SID_SHORTCUT_LOCAL_SEARCH_SHOW << SID_SHORTCUT_LOCAL_SEARCH_CLEAR;
  addTab(tr("Local Search"),keys);
  keys.clear();

  keys << SID_SHORTCUT_TOGGLE_INTERFACE << SID_SHORTCUT_SHOW_LOGS << SID_SHORTCUT_QUIT << SID_SHORTCUT_SYNC_PAGE_WITH_CONTENTS << SID_SHORTCUT_SYNC_CONTENTS_WITH_PAGE << SID_SHORTCUT_SHOW_NOTES << SID_SHORTCUT_MENU_SHOW << SID_SHORTCUT_KEEP_SYNCED << SID_SHORTCUT_OPTIONS << SID_SHORTCUT_FOCUS_TREE << SID_SHORTCUT_FOCUS_CONTENT;
  addTab("Other",keys);
  keys.clear();



  //hlayout->addLayout(formlayout1);
  //hlayout->addLayout(formlayout2);

  vlayout->addWidget(m_tabs);
  m_info = new QLabel;

  QStringList html;


  m_info->setText(html.join(""));
  vlayout->addWidget(m_info);


  setLayout(vlayout);
  addButtons();
  readSettings();
  setupConnections();
  this->setKeySequenceEditSize(VLARGE_EDIT);
}

void ShortcutOptions::readSettings() {
  QString key;
  QString value;
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  for(int i=0;i < m_tabs->count();i++) {
    QWidget * w = m_tabs->widget(i);
    QList<QKeySequenceEdit *> edits = w->findChildren<QKeySequenceEdit *>();
    foreach(QKeySequenceEdit *  widget,edits) {
      key = widget->objectName();
      value = settings.value(key,QString()).toString();
      value.remove(QChar(' '));
      m_values.insert(key,value);
      if (! value.isEmpty()) {
        widget->setKeySequence(QKeySequence(value));
      }
    }
  }
  settings.endGroup();
}
/**
 *
 *
 */
void ShortcutOptions::writeSettings(const QString & fileName) {
  QString f = m_settingsFileName;
  if (!fileName.isEmpty()) {
    f = fileName;
  }
  QSettings settings(f,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);
  for(int i=0;i < m_tabs->count();i++) {
    QWidget * w = m_tabs->widget(i);
    QList<QKeySequenceEdit *> edits = w->findChildren<QKeySequenceEdit *>();
    foreach(QKeySequenceEdit *  widget,edits) {
      QString key = widget->objectName();
      QString value = widget->keySequence().toString();
      value.remove(QChar(' '));
      settings.setValue(key,value);
    }
  }
  settings.endGroup();
  settings.sync();
  m_dirty = false;
  emit(modified(false));

}
/**
 * compare the key sequences with what's in the QSettings
 *
 */
bool ShortcutOptions::isModified()  {
  QString key;
  QString value;
  QString oldValue;
  m_dirty = false;
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  for(int i=0;i < m_tabs->count();i++) {
    QWidget * w = m_tabs->widget(i);
    QList<QKeySequenceEdit *> edits = w->findChildren<QKeySequenceEdit *>();
    foreach(QKeySequenceEdit *  widget,edits) {
      key = widget->objectName();
      oldValue = settings.value(key,QString()).toString();
      oldValue.remove(QChar(' '));
      value = widget->keySequence().toString();
      value.remove(QChar(' '));
      if (value != oldValue) {
        m_dirty = true;
      }
    }
  }
  return m_dirty;
}
void ShortcutOptions::addTab(const QString & title,const QStringList & keys) {
  QWidget * widget = new QWidget;
  QFormLayout * formlayout = new QFormLayout;
  for(int i=0; i < keys.size();i++) {
    QKeySequenceEdit * edit = new QKeySequenceEdit;
    edit->setObjectName(keys[i]);
    formlayout->addRow(new QLabel(keys[i]),edit);
  }
  widget->setLayout(formlayout);
  m_tabs->addTab(widget,title);
}
