#include "shortcutoptions.h"
#include "definedsettings.h"

ShortcutOptions::ShortcutOptions(QSettings * settings,QWidget * parent) : OptionsWidget(settings,parent) {
  m_settings = settings;
  m_section = "Shortcut";

  QTabWidget * m_tabs = new QTabWidget;

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

  QWidget * widget1 = new QWidget;
  QFormLayout * formlayout1 = new QFormLayout;
  formlayout1->addRow(tr("Collapse all"),m_collapseAll);
  formlayout1->addRow(tr("Collapse letter"),m_collapseLetter);
  widget1->setLayout(formlayout1);
  m_tabs->addTab(widget1,tr("Roots"));

  QWidget * widget2 = new QWidget;
  QFormLayout * formlayout2 = new QFormLayout;
  formlayout2->addRow(tr("Delete"),m_deleteTab);
  formlayout2->addRow(tr("Go to"),m_goTab);
  widget2->setLayout(formlayout2);
  m_tabs->addTab(widget2,tr("Tabs"));


  QWidget * widget3 = new QWidget;
  QFormLayout * formlayout3 = new QFormLayout;
  formlayout3->addRow(tr("Roots"),m_focusTree);
  formlayout3->addRow(tr("Page"),m_focusContents);
  widget3->setLayout(formlayout3);
  m_tabs->addTab(widget3,tr("Focus"));

  QWidget * widget4 = new QWidget;
  QFormLayout * formlayout4 = new QFormLayout;
  formlayout4->addRow(new QLabel(tr("<b>History</b>")));
  formlayout4->addRow(tr("Forward"),m_historyNext);
  formlayout4->addRow(tr("Backward"),m_historyBack);
  widget4->setLayout(formlayout4);
  m_tabs->addTab(widget4,tr("History"));

  QWidget * widget5 = new QWidget;
  QFormLayout * formlayout5 = new QFormLayout;
  formlayout5->addRow(new QLabel(tr("<b>Select</b>")));
  formlayout5->addRow(tr("All"),m_selectAll);
  formlayout5->addRow(tr("Entry"),m_selectEntry);
  widget5->setLayout(formlayout5);
  m_tabs->addTab(widget5,tr("Select"));

  QWidget * widget6 = new QWidget;
  QFormLayout * formlayout6 = new QFormLayout;
  formlayout6->addRow(new QLabel(tr("<b>Keymaps</b>")));
  formlayout6->addRow(tr("Enable"),m_keymapsEnable);
  formlayout6->addRow(tr("Disable"),m_keymapsDisable);
  widget6->setLayout(formlayout6);
  m_tabs->addTab(widget6,tr("Keymaps"));

  QWidget * widget7 = new QWidget;
  QFormLayout * formlayout7 = new QFormLayout;
  formlayout7->addRow(tr("For Root"),m_searchRoot);
  formlayout7->addRow(tr("For head word"),m_searchHead);
  formlayout7->addRow(tr("For Arabic word"),m_searchWord);
  formlayout7->addRow(tr("For node"),m_searchNode);
  formlayout7->addRow(tr("For page"),m_searchPage);
  widget7->setLayout(formlayout7);
  m_tabs->addTab(widget7,tr("Global search"));

  QWidget * widget8 = new QWidget;
  QFormLayout * formlayout8 = new QFormLayout;
  formlayout8->addRow(tr("Clear"),m_localSearchClear);
  formlayout8->addRow(tr("Find"),m_localSearchFind);
  formlayout8->addRow(tr("Next"),m_localSearchNext);
  formlayout8->addRow(tr("Show"),m_localSearchShow);
  widget8->setLayout(formlayout8);
  m_tabs->addTab(widget8,tr("Local search"));


  QWidget * widget9 = new QWidget;
  QFormLayout * formlayout9 = new QFormLayout;
  formlayout9->addRow(tr("First"),m_navFirst);
  formlayout9->addRow(tr("Next"),m_navNext);
  formlayout9->addRow(tr("Previous"),m_navBack);
  formlayout9->addRow(tr("Last"),m_navLast);
  widget9->setLayout(formlayout9);
  m_tabs->addTab(widget9,tr("Navigation"));



  QWidget * widgeta = new QWidget;
  QFormLayout * formlayouta = new QFormLayout;
  formlayouta->addRow(tr("Show notes"),m_showNotes);
  formlayouta->addRow(tr("Show sync"),m_sync);
  formlayouta->addRow(tr("Toggle interface"),m_interface);
  formlayouta->addRow(tr("Exit"),m_quit);
  widgeta->setLayout(formlayouta);
  m_tabs->addTab(widgeta,tr("Navigation"));

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
  m_dirty = true;

  m_settings->beginGroup(m_section);
  /// tests go in here
  m_settings->endGroup();
  return m_dirty;
}
