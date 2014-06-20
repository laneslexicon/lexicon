#include "searchoptions.h"
#include "namespace.h"
SearchOptions::SearchOptions(int searchType,QWidget * parent) : QWidget(parent) {
  m_searchType = searchType;
  m_hasMaps = false;
  QVBoxLayout * mainlayout = new QVBoxLayout;

  m_targetGroup = new QGroupBox(tr("Search target"));
  m_targetGroup->setObjectName("searchtargetgroup");
  m_typeGroup = new QGroupBox(tr("Search type"));
  m_typeGroup->setObjectName("searchtypegroup");
  m_contextGroup = new QGroupBox(tr("Search context"));
  m_contextGroup->setObjectName("searchcontextgroup");
  m_keymapGroup = new QGroupBox(tr("Keymap"));
  m_keymapGroup->setObjectName("keymapgroup");

  m_newTab = new QCheckBox(tr("Open in &new tab"));
  m_switchTab = new QCheckBox(tr("Switch to &new tab"));
  m_ignoreDiacritics = new QCheckBox(tr("Ignore diacritics"));
  m_wholeWordMatch = new QCheckBox(tr("Whole word match"));
  m_headWord = new QRadioButton(tr("Head word"));
  m_fullText = new QRadioButton(tr("Full text"));
  m_normalSearch = new QRadioButton(tr("Normal"),m_typeGroup);
  m_regexSearch = new QRadioButton(tr("Regular expression"),m_typeGroup);
  m_arabicTarget = new QRadioButton(tr("Arabic text"),m_targetGroup);
  m_buckwalterTarget = new QRadioButton(tr("Buckwalter transliteration"),m_targetGroup);

  /// Search context
  QHBoxLayout * contextlayout = new QHBoxLayout;
  contextlayout->addWidget(m_headWord);
  contextlayout->addWidget(m_fullText);
  m_contextGroup->setLayout(contextlayout);


  /// diacritics/whole word
  QHBoxLayout * optionslayout = new QHBoxLayout;
  optionslayout->addWidget(m_ignoreDiacritics);
  optionslayout->addWidget(m_wholeWordMatch);
  //  hlayout2->setContentsMargins(5,0,0,0);


  QHBoxLayout * tablayout = new QHBoxLayout;
  tablayout->addWidget(m_newTab);
  tablayout->addWidget(m_switchTab);


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


  mainlayout->addWidget(m_contextGroup);
  mainlayout->addWidget(m_typeGroup);
  mainlayout->addLayout(optionslayout);
  mainlayout->addLayout(forcelayout);
  mainlayout->addWidget(m_targetGroup);
  mainlayout->addLayout(tablayout);
  mainlayout->addSpacerItem(m_spacer);

  setLayout(mainlayout);

  connect(m_arabicTarget,SIGNAL(clicked()),this,SLOT(searchTargetChanged()));
  connect(m_buckwalterTarget,SIGNAL(clicked()),this,SLOT(searchTargetChanged()));
  connect(m_normalSearch,SIGNAL(clicked()),this,SLOT(searchTypeChanged()));
  connect(m_regexSearch,SIGNAL(clicked()),this,SLOT(searchTypeChanged()));
  //  showMore(false);



  m_normalSearch->setChecked(true);
  searchTargetChanged();
  searchTypeChanged();

  //  mainlayout->addWidget(new QCheckBox("test"));
  //  mainlayout->addStretch();
}
SearchOptions::~SearchOptions() {
  qDebug() << Q_FUNC_INFO;
}
void SearchOptions::showMore(bool show) {
  if (m_searchType & Lane::Root) {
    m_contextGroup->setVisible(false);
    m_targetGroup->setVisible(false);
    m_typeGroup->setVisible(false);
    m_ignoreDiacritics->setVisible(false);
    m_wholeWordMatch->setVisible(false);
    m_forceLTR->setVisible(false);
    if (m_hasMaps)
      m_keymapGroup->setVisible(show);

    return;
  }
  m_targetGroup->setVisible(show);
  m_typeGroup->setVisible(show);
  if (m_hasMaps)
    m_keymapGroup->setVisible(show);
  m_switchTab->setVisible(show);
  m_newTab->setVisible(show);
  m_forceLTR->setVisible(show);

}

int SearchOptions::getOptions() {
  int x = 0;
  if (m_ignoreDiacritics->isChecked())
    x |= Lane::Ignore_Diacritics;

  if (m_wholeWordMatch->isChecked())
    x |= Lane::Whole_Word;

  if (m_switchTab->isChecked())
    x |= Lane::Switch_Tab;

  if (m_newTab->isChecked())
    x |= Lane::Create_Tab;

  if (m_regexSearch->isChecked())
    x |= Lane::Regex;

  if (m_normalSearch->isChecked())
    x |= Lane::Normal;

  if (m_arabicTarget->isChecked())
    x |= Lane::Arabic;

  if (m_buckwalterTarget->isChecked())
    x |= Lane::Buckwalter;

  if (m_headWord->isChecked())
    x |= Lane::Head;

  if (m_fullText->isChecked())
    x |= Lane::Full;


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

  if (x & Lane::Switch_Tab)
    v = true;
  else
    v = false;
  m_switchTab->setChecked(v);


  if (x & Lane::Create_Tab)
    v = true;
  else
    v = false;
  m_newTab->setChecked(v);

  if (x & Lane::Regex)
    v = true;
  else
    v = false;
  m_regexSearch->setChecked(v);

  if (x & Lane::Normal)
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

  if (x & Lane::Head)
    v = true;
  else
    v = false;
  m_headWord->setChecked(v);

  if (x & Lane::Full)
    v = true;
  else
    v = false;
  m_fullText->setChecked(v);

  searchTargetChanged();
  searchTypeChanged();
}

void SearchOptions::searchTargetChanged() {
  if (m_arabicTarget->isChecked()) {
    bool v = false;
    if (m_normalSearch->isChecked()) {
      v = true;
    }
  }
}
void SearchOptions::searchTypeChanged() {
  bool v = true;
  if (m_regexSearch->isChecked()) {
    m_ignoreDiacritics->setVisible(false);
    m_wholeWordMatch->setVisible(false);
    m_arabicTarget->setVisible(true);
    m_buckwalterTarget->setVisible(true);
    m_targetGroup->setVisible(true);
    m_forceLTR->setVisible(true);
  }
  else {
    m_ignoreDiacritics->setVisible(true);
    m_wholeWordMatch->setVisible(true);
    m_arabicTarget->setVisible(false);
    m_buckwalterTarget->setVisible(false);
    m_targetGroup->setVisible(false);
    m_forceLTR->setVisible(false);
  }
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
