#include "shortcutoptions.h"
#include "definedsettings.h"

ShortcutOptions::ShortcutOptions(QSettings * settings,QWidget * parent) : OptionsWidget(settings,parent) {
  m_settings = settings;
  m_section = "Shortcut";

  m_tabs = new QTabWidget;

  QVBoxLayout * vlayout = new QVBoxLayout;

  //  m_bookmarkAdd = new QKeySequenceEdit(this);

  /// roots
  m_collapseAll = new QKeySequenceEdit;
  m_collapseLetter = new QKeySequenceEdit;

  /// tabs
  m_deleteTab = new QKeySequenceEdit;
  m_goTab = new QKeySequenceEdit;

  /// focus
  m_focusContents = new QKeySequenceEdit;
  m_focusTree = new QKeySequenceEdit;

  /// history
  m_historyBack = new QKeySequenceEdit;
  m_historyNext = new QKeySequenceEdit;

  /// keymaps
  m_keymapsEnable = new QKeySequenceEdit;
  m_keymapsDisable = new QKeySequenceEdit;

  /// local search
  //  m_localSearch = new QKeySequenceEdit;
  m_localSearchClear = new QKeySequenceEdit;
  m_localSearchFind = new QKeySequenceEdit;
  m_localSearchNext = new QKeySequenceEdit;
  m_localSearchShow = new QKeySequenceEdit;

  /// navigation
  m_navFirst = new QKeySequenceEdit;
  m_navNext = new QKeySequenceEdit;
  m_navBack = new QKeySequenceEdit;
  m_navLast = new QKeySequenceEdit;

  /// search
  //  m_searchDelete = new QKeySequenceEdit;  /// TODO what do this do?
  //  m_searchEntry = new QKeySequenceEdit;   /// TODO duplicate of Head
  m_searchHead = new QKeySequenceEdit;
  m_searchNode = new QKeySequenceEdit;
  m_searchRoot = new QKeySequenceEdit;
  m_searchWord = new QKeySequenceEdit;
  m_searchPage = new QKeySequenceEdit;

  /// select
  m_selectAll = new QKeySequenceEdit;
  m_selectEntry = new QKeySequenceEdit;

  m_showNotes = new QKeySequenceEdit;
  m_sync = new QKeySequenceEdit;
  m_interface = new QKeySequenceEdit;
  m_quit = new QKeySequenceEdit;

  QStringList labels;
  QList<QKeySequenceEdit *> edits;

  labels << tr("Collapse all") << tr("Collapse letter");
  edits << m_collapseAll << m_collapseLetter;
  addTab(tr("Roots"),labels,edits);
  labels.clear();
  edits.clear();

  labels << tr("Delete") << tr("Go to");
  edits << m_deleteTab << m_goTab;
  addTab(tr("Tabs"),labels,edits);
  labels.clear();
  edits.clear();


  labels << tr("Roots") << tr("Page");
  edits << m_focusTree << m_focusContents;
  addTab(tr("Focus"),labels,edits);
  labels.clear();
  edits.clear();


  labels << tr("Forward") << tr("Backward");
  edits << m_historyNext << m_historyBack;
  addTab(tr("History"),labels,edits);
  labels.clear();
  edits.clear();



  labels << tr("All") << tr("Entry");
  edits << m_selectAll << m_selectEntry;
  addTab(tr("Select"),labels,edits);
  labels.clear();
  edits.clear();


  labels << tr("Enable") << tr("Disable");
  edits << m_keymapsEnable << m_keymapsDisable;
  addTab(tr("Keymaps"),labels,edits);
  labels.clear();
  edits.clear();


  labels << tr("For Root") << tr("For head word") << tr("For Arabic word");
  labels << tr("For node") << tr("For page");

  edits << m_searchRoot << m_searchHead << m_searchWord << m_searchNode << m_searchPage;
  addTab(tr("Global Search"),labels,edits);
  labels.clear();
  edits.clear();

  labels << tr("Clear") << tr("Find") << tr("Next") << tr("Show");
  edits << m_localSearchClear << m_localSearchFind << m_localSearchNext << m_localSearchShow;
  addTab(tr("Local Search"),labels,edits);
  labels.clear();
  edits.clear();


  labels << tr("First") << tr("Next") << tr("Previous") << tr("Last");
  edits << m_navFirst << m_navNext << m_navBack << m_navLast;
  addTab(tr("Navigation"),labels,edits);
  labels.clear();
  edits.clear();

  labels << tr("Show notes") << tr("Show sync") << tr("Toggle interface") << tr("Exit");
  edits << m_showNotes << m_sync << m_interface << m_quit;
  addTab(tr("Other"),labels,edits);
  labels.clear();
  edits.clear();

  //hlayout->addLayout(formlayout1);
  //hlayout->addLayout(formlayout2);

  vlayout->addWidget(m_tabs);
  m_info = new QLabel;

  QStringList html;


  m_info->setText(html.join(""));
  vlayout->addWidget(m_info);

  setLayout(vlayout);
  readSettings();
  setupConnections();
}

void ShortcutOptions::readSettings() {
  if (m_settings == 0) {
    m_settings = new QSettings("default.ini",QSettings::IniFormat);
  }
  m_settings->beginGroup(m_section);
  m_collapseAll->setKeySequence(QKeySequence(m_settings->value(SID_SHORTCUT_CONTENTS_COLLAPSE_ALL).toString()));

  m_settings->endGroup();
}
void ShortcutOptions::writeSettings() {
  if (m_settings == 0) {
    m_settings = new QSettings("default.ini",QSettings::IniFormat);
  }
  m_settings->beginGroup(m_section);
  m_settings->setValue(SID_SHORTCUT_CONTENTS_COLLAPSE_ALL,m_collapseAll->keySequence().toString());
  m_settings->endGroup();
}
bool ShortcutOptions::isModified()  {
  m_dirty = false;

  m_settings->beginGroup(m_section);

  if (m_settings->value(SID_SHORTCUT_CONTENTS_COLLAPSE_ALL).toString() !=
      m_collapseAll->keySequence().toString()) {
    m_dirty = true;
  }

  m_settings->endGroup();
  return m_dirty;
}
void ShortcutOptions::addTab(const QString & tabtitle,const QStringList & labels,QList<QKeySequenceEdit *> edits) {
  QWidget * widget = new QWidget;
  QFormLayout * formlayout = new QFormLayout;
  for(int i=0;i < labels.size();i++) {
    formlayout->addRow(labels[i],edits[i]);
  }
  widget->setLayout(formlayout);
  m_tabs->addTab(widget,tabtitle);
}
