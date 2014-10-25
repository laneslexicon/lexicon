#include "searchoptionswidget.h"
#include "fullsearch.h"
#include "namespace.h"
#include "scripts.h"
#include "QsLog.h"
#define USE_KEYMAPS 0
/**
 * There are some unused options here that have been left in just in case.
 * In particular the plan to allow for searches of the Buckwalter text
 *
 *
 */
SearchOptionsWidget::SearchOptionsWidget(int searchFor,QWidget * parent) : QWidget(parent) {
  QLOG_DEBUG()  << Q_FUNC_INFO << searchFor;
  m_options.setSearchScope(searchFor);
  m_more = false;
  m_hasMaps = false;
  m_keymapsEnabled = false;
  setup(parent);
}
SearchOptionsWidget::SearchOptionsWidget(SearchOptions & options,QWidget * parent) : QWidget(parent) {
  QLOG_DEBUG() << Q_FUNC_INFO << options.getSearchScope();
  m_more = false;
  m_options = options;
  m_hasMaps = false;
  m_keymapsEnabled = false;
  setup(parent);
}
void SearchOptionsWidget::setup(QWidget * parent) {
  QVBoxLayout * mainlayout = new QVBoxLayout;
  QGridLayout * gridlayout = new QGridLayout;

  m_targetGroup = new QGroupBox(tr("Search target"));
  m_targetGroup->setObjectName("searchtargetgroup");
  m_typeGroup = new QGroupBox(tr("Search type"));
  m_typeGroup->setObjectName("searchtypegroup");
  if (USE_KEYMAPS) {
    m_keymapGroup = new QGroupBox(tr("Keymap"));
    m_keymapGroup->setObjectName("keymapgroup");
  }
  m_ignoreDiacritics = new QCheckBox(tr("Ignore diacritics"));
  m_wholeWordMatch = new QCheckBox(tr("Whole word match"));
  m_headWord = new QRadioButton(tr("Head word"));
  m_fullText = new QRadioButton(tr("Full text"));
  m_normalSearch = new QRadioButton(tr("Normal"),m_typeGroup);
  m_regexSearch = new QRadioButton(tr("Regular expression"),m_typeGroup);

  QHBoxLayout * typelayout = new QHBoxLayout;
  typelayout->addWidget(m_normalSearch);
  typelayout->addWidget(m_regexSearch);
  m_typeGroup->setLayout(typelayout);

  m_arabicTarget = new QRadioButton(tr("Arabic text"),m_targetGroup);
  m_buckwalterTarget = new QRadioButton(tr("Buckwalter transliteration"),m_targetGroup);

  m_includeHeads = new QCheckBox(tr("Include head entries in results"));
  m_showAllSearch = new QCheckBox(tr("Highlight all"));
  m_newTab = new QCheckBox(tr("Show result in new tab"));
  m_makeActive = new QCheckBox(tr("Got to new tab"));
  connect(m_newTab,SIGNAL(stateChanged(int)),this,SLOT(onNewTab(int)));

  m_forceLTR = new QCheckBox(tr("Force Left to Right on input"));
  connect(m_forceLTR,SIGNAL(stateChanged(int)),this,SLOT(onForceLeftToRight(int)));

  m_spacer = new QSpacerItem(0, 20,QSizePolicy::Ignored, QSizePolicy::MinimumExpanding);
  int row = 0;
  gridlayout->addWidget(m_typeGroup,row,0,2,2);
  row += 2;
  gridlayout->addWidget(m_ignoreDiacritics,row,0);
  gridlayout->addWidget(m_wholeWordMatch,row,1);
  row++;
  gridlayout->addWidget(m_forceLTR,row,0);
  row++;
  gridlayout->addWidget(m_includeHeads,row,0);
  row++;
  gridlayout->addWidget(m_showAllSearch,row,0);
  row++;
  gridlayout->addWidget(m_newTab,row,0);
  gridlayout->addWidget(m_makeActive,row,1);
  row++;
  gridlayout->addWidget(m_arabicTarget,row,0);
  gridlayout->addWidget(m_buckwalterTarget,row,1);

  mainlayout->addLayout(gridlayout);
  if ( ! qobject_cast<FullSearchWidget *>(parent))
    mainlayout->addSpacerItem(m_spacer);

  setLayout(mainlayout);

  connect(m_normalSearch,SIGNAL(clicked()),this,SLOT(searchTypeChanged()));
  connect(m_regexSearch,SIGNAL(clicked()),this,SLOT(searchTypeChanged()));
  showMore(false);
}
SearchOptionsWidget::~SearchOptionsWidget() {
  QLOG_DEBUG() << Q_FUNC_INFO;
}
/**
 * This was originally written to have a 'More' button to show/hide
 * additional options. All options are now shown
 *
 *
 */
void SearchOptionsWidget::showMore(bool /* show */) {
  //  m_more = show;
  int type = m_options.getSearchScope();
  m_arabicTarget->hide();
  m_buckwalterTarget->hide();
  m_showAllSearch->hide();
    switch(type) {
  case SearchOptions::Root : {
    m_ignoreDiacritics->hide();
    m_wholeWordMatch->hide();
    m_typeGroup->hide();
    m_includeHeads->hide();
    m_forceLTR->hide();
    m_normalSearch->hide();
    m_regexSearch->hide();
    m_typeGroup->hide();
    if (USE_KEYMAPS) {
      if (m_hasMaps)
        m_keymapGroup->setVisible(false);
    }
    break;
  }
  case SearchOptions::Entry : {
    m_includeHeads->setVisible(false);
    m_normalSearch->show();
    m_regexSearch->show();
    m_typeGroup->show();
      if (USE_KEYMAPS) {
      if (m_hasMaps && m_keymapsEnabled) {
        m_keymapGroup->setVisible(false);
      }
      else {
        m_keymapGroup->setVisible(false);
      }
    }
    if (m_regexSearch->isChecked()) {
      m_forceLTR->show();
      m_ignoreDiacritics->hide();
      m_wholeWordMatch->hide();
    }
    else {
      m_forceLTR->hide();
      m_ignoreDiacritics->show();
      m_wholeWordMatch->show();
    }
    break;
  }
  case SearchOptions::Word : {
    m_typeGroup->show();
    if (USE_KEYMAPS) {
      if (m_hasMaps && m_keymapsEnabled)
        m_keymapGroup->setVisible(false);
      else
        m_keymapGroup->setVisible(false);
    }
    m_includeHeads->show();
    if (m_regexSearch->isChecked()) {
      m_forceLTR->show();
      m_ignoreDiacritics->hide();
      m_wholeWordMatch->hide();
    }
    else {
      m_forceLTR->hide();
      m_ignoreDiacritics->show();
      m_wholeWordMatch->show();
    }
    break;
  }
  case SearchOptions::Local : {
    m_showAllSearch->show();
    m_typeGroup->show();
    if (USE_KEYMAPS) {
      if (m_hasMaps && m_keymapsEnabled)
        m_keymapGroup->setVisible(false);
      else
        m_keymapGroup->setVisible(false);
    }
    m_includeHeads->setVisible(false);

    if (m_regexSearch->isChecked()) {
      m_forceLTR->show();
      m_ignoreDiacritics->hide();
      m_wholeWordMatch->hide();
    }
    else {
      m_forceLTR->hide();
      m_ignoreDiacritics->show();
      m_wholeWordMatch->show();
    }
    break;
  }
  default :
    QLOG_WARN() << Q_FUNC_INFO << tr("Unknown search type") << type;
    break;
  }
}
void SearchOptionsWidget::searchTypeChanged() {
  showMore(m_more);
}
void SearchOptionsWidget::addKeymaps(const QString & activeMap,const QStringList & maps) {
  if (!USE_KEYMAPS)
    return;
  if (maps.size() > 0)
    m_hasMaps = true;

  QVBoxLayout * btnlayout = new QVBoxLayout;
  for(int i=0;i < maps.size();i++) {
    QRadioButton * btn = new QRadioButton(maps[i]);
    if (maps[i].toCaseFolded() == activeMap.toCaseFolded()) {
      btn->setChecked(true);
    }
    btnlayout->addWidget(btn);
    connect(btn,SIGNAL(clicked()),this,SLOT(keymapChanged()));
  }
  m_keymapGroup->setLayout(btnlayout);

  QVBoxLayout * layout = qobject_cast<QVBoxLayout *>(this->layout());
  if (layout) {
    layout->removeItem(m_spacer);
    layout->addWidget(m_keymapGroup);
    layout->addSpacerItem(m_spacer);
  }
}
void SearchOptionsWidget::keymapChanged() {
  if (!USE_KEYMAPS)
    return;
  QRadioButton * btn = qobject_cast<QRadioButton *>(QObject::sender());
  if (btn) {
    QLOG_DEBUG() << "keymap" << btn->text();
    emit(loadKeymap(btn->text()));
  }
}
void SearchOptionsWidget::setKeymapsEnabled(bool v) {
  m_keymapsEnabled = v;
}
void SearchOptionsWidget::onForceLeftToRight(int checked) {
  if (checked == Qt::Checked)
    emit(force(true));
  else
    emit(force(false));
}
bool SearchOptionsWidget::getForceLTR() {
  return m_forceLTR->isChecked();
}
void SearchOptionsWidget::onNewTab(int checked) {
  if (checked == Qt::Checked) {
    m_makeActive->setEnabled(true);
  }
  else {
    m_makeActive->setEnabled(false);
  }

}
QRegExp SearchOptionsWidget::buildRx(const QString & searchtarget,const QString & diacritics,const SearchOptions & options) {
  QRegExp rx;
  QString target = searchtarget;
  /// TODO get from INI
  QRegExp rxclass(diacritics);//"[\\x064b\\x064c\\x064d\\x064e\\x064f\\x0650\\x0651\\x0652\\x0670\\x0671]*");

  QString pattern;

  if (options.getSearchType() == SearchOptions::Normal) {
    /// TODO fix metacharacters and get list from INI
    QStringList metachars;
    metachars << "(" << ")" << "[" << "]" << "?" << ".";
    for(int i=0;i < metachars.size();i++) {
      target.replace(metachars[i],QString("\\%1").arg(metachars[i]));
    }
    if (options.ignoreDiacritics()) {
      /// TODO get from INI
      //      QString ar("[\\x064b\\x064c\\x064d\\x064e\\x064f\\x0650\\x0651\\x0652\\x0670\\x0671]*");
      target = target.replace(rxclass,QString());
      QStringList cc = target.split("");
      for(int i=0;i < cc.size();i++) {
        pattern += cc[i];
        if (UcdScripts::isScript(cc[i],"Arabic")) {
          pattern += diacritics;
        }
      }
    }
    else {
      pattern = target;
    }
    if (options.wholeWordMatch()) {
      pattern = "\\b" + pattern + "\\b";
    }
    else if ( ! options.ignoreDiacritics()) {
      /// TODO add something like [^rxclass]
      //      pattern += "[^\\x064b\\x064c\\x064d\\x064e\\x064f\\x0650\\x0651\\x0652\\x0670\\x0671]";
      //      qDebug() << Q_FUNC_INFO << target << pattern;
    }
    rx.setPattern(pattern);
  }
  else {
    rx.setPattern(target);
  }
  return rx;
}
void SearchOptionsWidget::getOptions(SearchOptions & opts) const {
  QLOG_DEBUG() << Q_FUNC_INFO;
  opts.setIgnoreDiacritics(true);
  opts.setSearchType(SearchOptions::Normal);
  opts.setIgnoreDiacritics(m_ignoreDiacritics->isChecked());
  opts.setWholeWordMatch(m_wholeWordMatch->isChecked());
  if (m_regexSearch->isChecked()) {
    opts.setSearchType(SearchOptions::Regex);
  }

  if (m_normalSearch->isChecked())
    opts.setSearchType(SearchOptions::Normal);

  // if (m_arabicTarget->isChecked())
  //   x |= Lane::Arabic;

  // if (m_buckwalterTarget->isChecked())
  //   x |= Lane::Buckwalter;

  opts.setIncludeHeads(m_includeHeads->isChecked());
  opts.setShowAll(m_showAllSearch->isChecked());

  opts.setKeymaps(m_keymapsEnabled);
  opts.setSearchScope(m_options.getSearchScope());

  opts.setNewTab(m_newTab->isChecked());
  opts.setActivateTab(m_makeActive->isChecked());
}
void SearchOptionsWidget::setOptions(const SearchOptions & options) {
  QLOG_DEBUG() << Q_FUNC_INFO;
  m_options = options;
  m_ignoreDiacritics->setChecked(options.ignoreDiacritics());

  m_wholeWordMatch->setChecked(options.wholeWordMatch());

  m_regexSearch->setChecked((options.getSearchType() == SearchOptions::Regex));
  m_normalSearch->setChecked((options.getSearchType() == SearchOptions::Normal));


  m_newTab->setChecked(options.newTab());
  m_makeActive->setChecked(options.activateTab());
  m_makeActive->setEnabled(options.newTab());
  // m_arabicTarget->setChecked(x & Lane::Arabic);

  // m_buckwalterTarget->setChecked(x & Lane::Buckwalter);

  m_includeHeads->setChecked(options.includeHeads());


  m_keymapsEnabled = options.keymaps();

  m_showAllSearch->setChecked(options.showAll());


}
