#include "searchoptions.h"
#include "fullsearch.h"
#include "namespace.h"
SearchOptions::SearchOptions(int searchType,QWidget * parent) : QWidget(parent) {
  m_more = false;
  m_searchType = searchType;
  m_hasMaps = false;
  m_keymapsEnabled = false;

  QVBoxLayout * mainlayout = new QVBoxLayout;

  m_targetGroup = new QGroupBox(tr("Search target"));
  m_targetGroup->setObjectName("searchtargetgroup");
  m_typeGroup = new QGroupBox(tr("Search type"));
  m_typeGroup->setObjectName("searchtypegroup");
  m_keymapGroup = new QGroupBox(tr("Keymap"));
  m_keymapGroup->setObjectName("keymapgroup");

  m_ignoreDiacritics = new QCheckBox(tr("Ignore diacritics"));
  m_wholeWordMatch = new QCheckBox(tr("Whole word match"));
  m_headWord = new QRadioButton(tr("Head word"));
  m_fullText = new QRadioButton(tr("Full text"));
  m_normalSearch = new QRadioButton(tr("Normal"),m_typeGroup);
  m_regexSearch = new QRadioButton(tr("Regular expression"),m_typeGroup);
  m_arabicTarget = new QRadioButton(tr("Arabic text"),m_targetGroup);
  m_buckwalterTarget = new QRadioButton(tr("Buckwalter transliteration"),m_targetGroup);

  m_includeHeads = new QCheckBox(tr("Include head entries in search results"));

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


  /// search target
  QHBoxLayout * targetlayout = new QHBoxLayout;
  targetlayout->addWidget(m_arabicTarget);
  targetlayout->addWidget(m_buckwalterTarget);
  //  layout->setContentsMargins(5,0,0,0);
  m_targetGroup->setLayout(targetlayout);


  m_spacer = new QSpacerItem(0, 20,QSizePolicy::Ignored, QSizePolicy::MinimumExpanding);


  mainlayout->addWidget(m_typeGroup);
  mainlayout->addLayout(optionslayout);
  mainlayout->addWidget(m_includeHeads);
  mainlayout->addLayout(forcelayout);
  mainlayout->addWidget(m_targetGroup);
  if ( ! qobject_cast<FullSearchWidget *>(parent))
    mainlayout->addSpacerItem(m_spacer);

  setLayout(mainlayout);

  connect(m_normalSearch,SIGNAL(clicked()),this,SLOT(searchTypeChanged()));
  connect(m_regexSearch,SIGNAL(clicked()),this,SLOT(searchTypeChanged()));
}
SearchOptions::~SearchOptions() {
  qDebug() << Q_FUNC_INFO;
}
void SearchOptions::showMore(bool show) {
  m_more = show;

  switch(m_searchType) {
  case Lane::Root : {
    m_targetGroup->setVisible(false);
    m_ignoreDiacritics->setVisible(false);
    m_wholeWordMatch->setVisible(false);
    m_includeHeads->setVisible(false);
    m_forceLTR->setVisible(false);
    m_typeGroup->setVisible(false);
    if (m_hasMaps)
      m_keymapGroup->setVisible(false);

    break;
  }
  case Lane::Entry : {
    m_includeHeads->setVisible(false);
    if (m_hasMaps && m_keymapsEnabled) {
      m_keymapGroup->setVisible(false);
    }
    else {
      m_keymapGroup->setVisible(false);
    }

    if (m_regexSearch->isChecked()) {
      m_targetGroup->setVisible(m_more);
      m_forceLTR->setVisible(m_more);
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
    if (m_hasMaps && m_keymapsEnabled)
      m_keymapGroup->setVisible(false);
    else
      m_keymapGroup->setVisible(false);

    m_includeHeads->setVisible(true);

    if (m_regexSearch->isChecked()) {
      m_targetGroup->setVisible(m_more);
      m_forceLTR->setVisible(m_more);
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
  default :
    qDebug() << Q_FUNC_INFO << "unknown search type";
    break;
  }
}

int SearchOptions::getOptions() {
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

  return x;
}
void SearchOptions::setOptions(int x) {
  bool v;

  if (x & Lane::Ignore_Diacritics)
    v = true;
  else
    v = false;
  m_ignoreDiacritics->setChecked(v);

  if (x & Lane::Whole_Word)
    v = true;
  else
    v = false;
  m_wholeWordMatch->setChecked(v);


  if (x & Lane::Regex_Search)
    v = true;
  else
    v = false;
  m_regexSearch->setChecked(v);

  if (x & Lane::Normal_Search)
    v = true;
  else
    v = false;
  m_normalSearch->setChecked(v);

  if (x & Lane::Arabic)
    v = true;
  else
    v = false;
  m_arabicTarget->setChecked(v);

  if (x & Lane::Buckwalter)
    v = true;
  else
    v = false;
  m_buckwalterTarget->setChecked(v);

  if (x & Lane::Include_Heads)
    v = true;
  else
    v = false;
  m_includeHeads->setChecked(v);

  if (x & Lane::Keymaps_Enabled) {
    m_keymapsEnabled = true;

  }
  else
    m_keymapsEnabled = false;

  qDebug() << Q_FUNC_INFO << "Keymaps enabled" << m_keymapsEnabled ;
  searchTypeChanged();
}

void SearchOptions::searchTypeChanged() {
  showMore(m_more);
}
void SearchOptions::addKeymaps(const QString & activeMap,const QStringList & maps) {
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
void SearchOptions::keymapChanged() {
  QRadioButton * btn = qobject_cast<QRadioButton *>(QObject::sender());
  if (btn) {
    qDebug() << "keymap" << btn->text();
    emit(loadKeymap(btn->text()));
  }
}
void SearchOptions::setKeymapsEnabled(bool v) {
  m_keymapsEnabled = v;
}
