#include "rootsoptions.h"
#include "definedsettings.h"

RootsOptions::RootsOptions(QSettings * settings,QWidget * parent) : OptionsWidget(settings,parent) {
  m_settings = settings;
  m_section = "Roots";

  QVBoxLayout * vlayout = new QVBoxLayout;
  QFormLayout * formlayout = new QFormLayout;
  /*
  m_collapseAll = new QKeySequenceEdit;
  m_collapseLetter = new QKeySequenceEdit;
  m_syncToContents = new QKeySequenceEdit;
  m_focusToContents = new QKeySequenceEdit;
  */
  m_moveDown = new QLineEdit;
  m_moveDown->setMaxLength(1);
  m_moveUp = new QLineEdit;
  m_moveUp->setMaxLength(1);
  m_expand = new QLineEdit;
  m_expand->setMaxLength(1);

  m_singleClick = new QCheckBox;
  m_doubleClick = new QCheckBox;
  m_debug = new QCheckBox;

  connect(m_singleClick,SIGNAL(stateChanged(int)),this,SLOT(stateChanged(int)));
  connect(m_doubleClick,SIGNAL(stateChanged(int)),this,SLOT(stateChanged(int)));
  connect(m_debug,SIGNAL(stateChanged(int)),this,SLOT(stateChanged(int)));
  connect(m_moveDown,SIGNAL(editingFinished()),this,SLOT(editingFinished()));
  connect(m_moveUp,SIGNAL(editingFinished()),this,SLOT(editingFinished()));
  connect(m_moveUp,SIGNAL(textChanged(const QString &)),this,SLOT(textChanged(const QString &)));
  connect(m_expand,SIGNAL(editingFinished()),this,SLOT(editingFinished()));
  connect(m_expand,SIGNAL(textChanged(const QString &)),this,SLOT(textChanged(const QString &)));

  formlayout->addRow(tr("Move up"),m_moveUp);
  formlayout->addRow(tr("Move down"),m_moveDown);
  formlayout->addRow(tr("Expand/collapse"),m_expand);
  formlayout->addRow(tr("Single click activates item"),m_singleClick);
  formlayout->addRow(tr("Double click activates item"),m_doubleClick);

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
  readSettings();

}
void RootsOptions::readSettings() {
  if (m_settings == 0) {
    m_settings = new QSettings("default.ini",QSettings::IniFormat);
  }
  m_settings->beginGroup(m_section);

  m_debug->setChecked(m_settings->value(SID_ROOTS_DEBUG).toBool());
  m_moveUp->setText(m_settings->value(SID_ROOTS_MOVE_UP).toString());
  m_moveDown->setText(m_settings->value(SID_ROOTS_MOVE_DOWN).toString());
  m_expand->setText(m_settings->value(SID_ROOTS_EXPAND).toString());

  m_settings->endGroup();
}
void RootsOptions::writeSettings() {
  if (m_settings == 0) {
    m_settings = new QSettings("default.ini",QSettings::IniFormat);
  }
  m_settings->beginGroup(m_section);

  m_settings->setValue(SID_ROOTS_DEBUG,m_debug->isChecked());
  m_settings->setValue(SID_ROOTS_MOVE_UP,m_moveUp->text());
  m_settings->setValue(SID_ROOTS_MOVE_DOWN,m_moveDown->text());
  m_settings->setValue(SID_ROOTS_EXPAND,m_expand->text());
  m_settings->sync();

  m_settings->endGroup();
}
bool RootsOptions::isModified()  {
  bool v;
  QString s;
  m_dirty = false;

  m_settings->beginGroup(m_section);
  v = m_settings->value(SID_ROOTS_DEBUG).toBool();
  if (m_debug->isChecked() != v) {
    m_dirty = true;
  }
  s = m_settings->value(SID_ROOTS_MOVE_UP).toString();
  if (m_moveUp->text() != s) {
    m_dirty = true;
  }
  s = m_settings->value(SID_ROOTS_MOVE_DOWN).toString();
  if (m_moveDown->text() != s) {
    m_dirty = true;
  }
  m_settings->endGroup();
  return m_dirty;
}
