#include "shortcutoptions.h"
#include "definedsettings.h"

ShortcutOptions::ShortcutOptions(QSettings * settings,QWidget * parent) : OptionsWidget(settings,parent) {
  m_settings = settings;
  m_section = "Shortcut";

  QVBoxLayout * vlayout = new QVBoxLayout;
  QFormLayout * formlayout1 = new QFormLayout;
  QFormLayout * formlayout2 = new QFormLayout;
  QHBoxLayout * hlayout = new QHBoxLayout;

  //  m_bookmarkAdd = new QKeySequenceEdit(this);

  /// roots
  m_collapseAll = new QKeySequenceEdit(this);
  m_collapseLetter = new QKeySequenceEdit(this);

  /// tabs
  m_deleteTab = new QKeySequenceEdit(this);
  m_goTab = new QKeySequenceEdit(this);

  /// focus
  m_focusContents = new QKeySequenceEdit(this);
  m_focusTree = new QKeySequenceEdit(this);

  /// history
  m_historyBack = new QKeySequenceEdit(this);
  m_historyNext = new QKeySequenceEdit(this);

  /// keymaps
  m_keymapsEnable = new QKeySequenceEdit(this);
  m_keymapsDisable = new QKeySequenceEdit(this);

  /// local search
  //  m_localSearch = new QKeySequenceEdit(this);
  m_localSearchClear = new QKeySequenceEdit(this);
  m_localSearchFind = new QKeySequenceEdit(this);
  m_localSearchNext = new QKeySequenceEdit(this);
  m_localSearchShow = new QKeySequenceEdit(this);

  /// navigation
  m_navFirst = new QKeySequenceEdit(this);
  m_navNext = new QKeySequenceEdit(this);
  m_navBack = new QKeySequenceEdit(this);
  m_navLast = new QKeySequenceEdit(this);

  ///  m_pageSearch = new QKeySequenceEdit(this);


  /// search
  //  m_searchDelete = new QKeySequenceEdit(this);  /// TODO what do this do?
  //  m_searchEntry = new QKeySequenceEdit(this);   /// TODO duplicate of Head
  m_searchHead = new QKeySequenceEdit(this);
  m_searchNode = new QKeySequenceEdit(this);
  m_searchRoot = new QKeySequenceEdit(this);
  m_searchWord = new QKeySequenceEdit(this);
  m_searchPage = new QKeySequenceEdit(this);

  /// select
  m_selectAll = new QKeySequenceEdit(this);
  m_selectEntry = new QKeySequenceEdit(this);

  m_showNotes = new QKeySequenceEdit(this);
  m_sync = new QKeySequenceEdit(this);
  m_interface = new QKeySequenceEdit(this);
  m_quit = new QKeySequenceEdit(this);


  formlayout1->addRow(new QLabel(tr("<b>Roots</b>")));
  formlayout1->addRow(tr("Collapse all"),m_collapseAll);
  formlayout1->addRow(tr("Collapse letter"),m_collapseLetter);


  formlayout2->addRow(new QLabel(tr("<b>Tabs</b>")));
  formlayout2->addRow(tr("Delete"),m_deleteTab);
  formlayout2->addRow(tr("Go to"),m_goTab);

  formlayout1->addRow(new QLabel(tr("<b>Focus</b>")));
  formlayout1->addRow(tr("Roots"),m_focusTree);
  formlayout1->addRow(tr("Page"),m_focusContents);

  formlayout2->addRow(new QLabel(tr("<b>History</b>")));
  formlayout2->addRow(tr("Forward"),m_historyNext);
  formlayout2->addRow(tr("Backward"),m_historyBack);

  formlayout1->addRow(new QLabel(tr("<b>Select</b>")));
  formlayout1->addRow(tr("All"),m_selectAll);
  formlayout1->addRow(tr("Entry"),m_selectEntry);

  formlayout2->addRow(new QLabel(tr("<b>Keymaps</b>")));
  formlayout2->addRow(tr("Enable"),m_keymapsEnable);
  formlayout2->addRow(tr("Disable"),m_keymapsDisable);

  formlayout1->addRow(new QLabel(tr("<b>Local Search</b>")));
  formlayout1->addRow(tr("Clear"),m_localSearchClear);
  formlayout1->addRow(tr("Find"),m_localSearchFind);
  formlayout1->addRow(tr("Next"),m_localSearchNext);
  formlayout1->addRow(tr("Show"),m_localSearchShow);

  formlayout2->addRow(new QLabel(tr("<b>Navigation</b>")));
  formlayout2->addRow(tr("First"),m_navFirst);
  formlayout2->addRow(tr("Next"),m_navNext);
  formlayout2->addRow(tr("Previous"),m_navBack);
  formlayout2->addRow(tr("Last"),m_navLast);

  formlayout1->addRow(new QLabel(tr("<b>Global Search</b>")));
  formlayout1->addRow(tr("For Root"),m_searchRoot);
  formlayout1->addRow(tr("For head word"),m_searchHead);
  formlayout1->addRow(tr("For Arabic word"),m_searchWord);
  formlayout1->addRow(tr("For node"),m_searchNode);
  formlayout1->addRow(tr("For page"),m_searchPage);


  formlayout2->addRow(new QLabel(tr("<b>Other</b>")));
  formlayout2->addRow(tr("Show notes"),m_showNotes);
  formlayout2->addRow(tr("Show sync"),m_sync);
  formlayout2->addRow(tr("Toggle interface"),m_interface);
  formlayout2->addRow(tr("Exit"),m_quit);

  hlayout->addLayout(formlayout1);
  hlayout->addLayout(formlayout2);

  vlayout->addLayout(hlayout);
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

  m_settings->endGroup();
}
void ShortcutOptions::writeSettings() {
  if (m_settings == 0) {
    m_settings = new QSettings("default.ini",QSettings::IniFormat);
  }
  m_settings->beginGroup(m_section);

  m_settings->endGroup();
}
bool ShortcutOptions::isModified()  {
  bool v;
  QString s;
  m_dirty = false;

  m_settings->beginGroup(m_section);
  /// tests go in here
  m_settings->endGroup();
  return m_dirty;
}
