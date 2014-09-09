#include "searchoptions.h"
#include "fullsearch.h"
#include "namespace.h"
#include "scripts.h"
#define USE_KEYMAPS 0
bool SearchOptions::ignoreDiacritics() {
  return m_ignoreDiacritics;
}
void SearchOptions::setIgnoreDiacritics(bool v) {
  m_ignoreDiacritics = v;
}
bool SearchOptions::wholeWordMatch() {
  return m_wholeWordMatch;
}
void SearchOptions::setWholeWordMatch(bool v) {
  m_wholeWordMatch = v;
}
void SearchOptions::setSearchType(SearchType_t x) {
  m_type = x;
}
int SearchOptions::getSearchType() {
  return m_type;
}
void SearchOptions::setSearchScope(SearchScope_t x) {
  m_target = x;
}
int SearchOptions::getSearchScope() {
  return m_target;
}
bool SearchOptions::sticky() {
  return m_sticky;
}
void SearchOptions::setSticky(bool v) {
  m_sticky = v;
}

bool SearchOptions::includeHeads() {
  return m_includeHeads;
}
void SearchOptions::setIncludeHeads(bool v) {
  m_includeHeads = v;
}

bool SearchOptions::forceLTR() {
  return m_forceLTR;
}
void SearchOptions::setForceLTR(bool v) {
  m_forceLTR = v;
}
bool SearchOptions::newTab() {
  return m_newTab;
}
void SearchOptions::setNewTab(bool v) {
  m_newTab = v;
}
bool SearchOptions::activateTab() {
  return m_activateTab;
}
void SearchOptions::setActivateTab(bool v) {
  m_activateTab = v;
}

SearchOptionsWidget::SearchOptionsWidget(int searchType,QWidget * parent) : QWidget(parent) {
  m_more = false;
  m_searchType = searchType;
  m_hasMaps = false;
  m_keymapsEnabled = false;
  setup(parent);
}
SearchOptionsWidget::SearchOptionsWidget(SearchOptions & options,QWidget * parent) : QWidget(parent) {
  m_more = false;
  m_options = options;

  m_hasMaps = false;
  m_keymapsEnabled = false;
  m_searchType = m_options.getSearchType();
  setup(parent);
}
void SearchOptionsWidget::setup(QWidget * parent) {
  QVBoxLayout * mainlayout = new QVBoxLayout;

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
  m_arabicTarget = new QRadioButton(tr("Arabic text"),m_targetGroup);
  m_buckwalterTarget = new QRadioButton(tr("Buckwalter transliteration"),m_targetGroup);

  m_includeHeads = new QCheckBox(tr("Include head entries in search results"));
  m_stickySearch = new QCheckBox(tr("Sticky search"));
  /// diacritics/whole word
  QHBoxLayout * optionslayout = new QHBoxLayout;
  optionslayout->addWidget(m_ignoreDiacritics);
  optionslayout->addWidget(m_wholeWordMatch);
  //  hlayout2->setContentsMargins(5,0,0,0);




  /// search type
  QHBoxLayout * typelayout = new QHBoxLayout;
  typelayout->addWidget(m_normalSearch);
  typelayout->addWidget(m_regexSearch);
  //  layout->setContentsMargins(5,0,0,0);
  m_typeGroup->setLayout(typelayout);


/// force LTR
  QHBoxLayout * forcelayout = new QHBoxLayout;
  m_forceLTR = new QCheckBox(tr("Force Left to Right on search input"));
  forcelayout->addWidget(m_forceLTR);
  forcelayout->addStretch();
  connect(m_forceLTR,SIGNAL(stateChanged(int)),this,SLOT(onForceLeftToRight(int)));

  /// search target
  QHBoxLayout * targetlayout = new QHBoxLayout;
  targetlayout->addWidget(m_arabicTarget);
  targetlayout->addWidget(m_buckwalterTarget);
  //  layout->setContentsMargins(5,0,0,0);
  m_targetGroup->setLayout(targetlayout);


  m_spacer = new QSpacerItem(0, 20,QSizePolicy::Ignored, QSizePolicy::MinimumExpanding);


  mainlayout->addWidget(m_typeGroup);
  mainlayout->addLayout(optionslayout);
  mainlayout->addWidget(m_stickySearch);
  mainlayout->addWidget(m_includeHeads);
  mainlayout->addLayout(forcelayout);
  mainlayout->addWidget(m_targetGroup);
  if ( ! qobject_cast<FullSearchWidget *>(parent))
    mainlayout->addSpacerItem(m_spacer);

  setLayout(mainlayout);

  connect(m_normalSearch,SIGNAL(clicked()),this,SLOT(searchTypeChanged()));
  connect(m_regexSearch,SIGNAL(clicked()),this,SLOT(searchTypeChanged()));
}
SearchOptionsWidget::~SearchOptionsWidget() {
  qDebug() << Q_FUNC_INFO;
}
void SearchOptionsWidget::showMore(bool show) {
  m_more = show;

  switch(m_searchType) {
  case Lane::Root : {
    m_stickySearch->setVisible(false);
    m_targetGroup->setVisible(false);
    m_ignoreDiacritics->setVisible(false);
    m_wholeWordMatch->setVisible(false);
    m_includeHeads->setVisible(false);
    m_forceLTR->setVisible(false);
    m_typeGroup->setVisible(false);
    if (USE_KEYMAPS) {
      if (m_hasMaps)
        m_keymapGroup->setVisible(false);
    }
    break;
  }
  case Lane::Entry : {
    m_includeHeads->setVisible(false);
    if (USE_KEYMAPS) {
      if (m_hasMaps && m_keymapsEnabled) {
        m_keymapGroup->setVisible(false);
      }
      else {
        m_keymapGroup->setVisible(false);
      }
    }
    m_stickySearch->setVisible(false);

    if (m_regexSearch->isChecked()) {
      m_targetGroup->setVisible(false);
      m_forceLTR->setVisible(true);
      m_arabicTarget->setVisible(m_more);
      m_buckwalterTarget->setVisible(m_more);
      m_ignoreDiacritics->setVisible(false);
      m_wholeWordMatch->setVisible(false);
    }
    else {
      m_targetGroup->setVisible(false);
      m_forceLTR->setVisible(false);
      m_arabicTarget->setVisible(false);
      m_buckwalterTarget->setVisible(false);
      m_ignoreDiacritics->setVisible(true);
      m_wholeWordMatch->setVisible(true);
    }
    break;
  }
  case Lane::Word : {
    if (USE_KEYMAPS) {
      if (m_hasMaps && m_keymapsEnabled)
        m_keymapGroup->setVisible(false);
      else
        m_keymapGroup->setVisible(false);
    }
    m_stickySearch->setVisible(false);
    m_includeHeads->setVisible(true);

    if (m_regexSearch->isChecked()) {
      m_targetGroup->setVisible(m_more);
      m_forceLTR->setVisible(true);
      m_arabicTarget->setVisible(m_more);
      m_buckwalterTarget->setVisible(m_more);
      m_ignoreDiacritics->setVisible(false);
      m_wholeWordMatch->setVisible(false);
    }
    else {
      m_targetGroup->setVisible(false);
      m_forceLTR->setVisible(false);
      m_arabicTarget->setVisible(false);
      m_buckwalterTarget->setVisible(false);
      m_ignoreDiacritics->setVisible(true);
      m_wholeWordMatch->setVisible(true);
    }
    break;
  }
  case Lane::Local_Search : {
    m_stickySearch->setVisible(true);
    if (USE_KEYMAPS) {
      if (m_hasMaps && m_keymapsEnabled)
        m_keymapGroup->setVisible(false);
      else
        m_keymapGroup->setVisible(false);
    }
    m_includeHeads->setVisible(false);

    if (m_regexSearch->isChecked()) {
      m_targetGroup->setVisible(false);
      m_forceLTR->setVisible(true);
      m_arabicTarget->setVisible(false);
      m_buckwalterTarget->setVisible(false);
      m_ignoreDiacritics->setVisible(false);
      m_wholeWordMatch->setVisible(false);
    }
    else {
      m_targetGroup->setVisible(false);
      m_forceLTR->setVisible(false);
      m_arabicTarget->setVisible(false);
      m_buckwalterTarget->setVisible(false);
      m_ignoreDiacritics->setVisible(true);
      m_wholeWordMatch->setVisible(true);
    }
    break;
  }
  default :
    qDebug() << Q_FUNC_INFO << "unknown search type";
    break;
  }
}

int SearchOptionsWidget::getOptions() {
  int x = 0;
  if (m_ignoreDiacritics->isChecked())
    x |= Lane::Ignore_Diacritics;

  if (m_wholeWordMatch->isChecked())
    x |= Lane::Whole_Word;


  if (m_regexSearch->isChecked())
    x |= Lane::Regex_Search;

  if (m_normalSearch->isChecked())
    x |= Lane::Normal_Search;

  if (m_arabicTarget->isChecked())
    x |= Lane::Arabic;

  if (m_buckwalterTarget->isChecked())
    x |= Lane::Buckwalter;

  if (m_includeHeads->isChecked())
    x |= Lane::Include_Heads;

  if (m_stickySearch->isChecked())
    x |= Lane::Sticky_Search;

  return x;
}
void SearchOptionsWidget::setOptions(int x) {
  bool v;

  m_ignoreDiacritics->setChecked(x & Lane::Ignore_Diacritics);

  m_wholeWordMatch->setChecked(x & Lane::Whole_Word);


  m_regexSearch->setChecked(x & Lane::Regex_Search);

  m_normalSearch->setChecked(x & Lane::Normal_Search);

  m_arabicTarget->setChecked(x & Lane::Arabic);

  m_buckwalterTarget->setChecked(x & Lane::Buckwalter);

  m_includeHeads->setChecked(x & Lane::Include_Heads);

  if (x & Lane::Keymaps_Enabled) {
    m_keymapsEnabled = true;

  }
  else
    m_keymapsEnabled = false;

  m_stickySearch->setChecked(x & Lane::Sticky_Search);

  searchTypeChanged();
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
    qDebug() << "keymap" << btn->text();
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
QRegExp SearchOptionsWidget::buildRx(const QString & searchtarget,int options) {
  QRegExp rx;
  QString target = searchtarget;

  QString pattern;

  if (options & Lane::Normal_Search) {
    /// TODO fix metacharacters and get list from INI
    QStringList metachars;
    metachars << "(" << ")" << "[" << "]" << "?" << ".";
    for(int i=0;i < metachars.size();i++) {
      target.replace(metachars[i],QString("\\%1").arg(metachars[i]));
    }
    if (options & Lane::Ignore_Diacritics) {
      /// TODO get from INI
      QString ar("[\\x064b\\x064c\\x064d\\x064e\\x064f\\x0650\\x0651\\x0652\\x0670\\x0671]*");
      QStringList cc = target.split("");
      for(int i=0;i < cc.size();i++) {
        pattern += cc[i];
        if (UcdScripts::isScript(cc[i],"Arabic")) {
          pattern + ar;
        }
      }
    }
    else {
      pattern = target;
    }
    if (options & Lane::Whole_Word) {
      pattern = "\\b" + pattern + "\\b";
    }
    rx.setPattern(pattern);
  }
  else {
    rx.setPattern(target);
  }
  return rx;
}
void SearchOptionsWidget::getOptions(SearchOptions & opts) const {
  opts.setIgnoreDiacritics(true);
  opts.setSearchType(SearchOptions::Normal);
}
void SearchOptionsWidget::setOptions(const SearchOptions & options) {
  m_options = options;
}
