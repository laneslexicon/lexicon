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
  showMore(false);
}
SearchOptionsWidget::~SearchOptionsWidget() {
  QLOG_DEBUG() << Q_FUNC_INFO;
}
void SearchOptionsWidget::showMore(bool show) {
  m_more = show;
  int type = m_options.getSearchScope();
  switch(type) {
  case SearchOptions::Root : {
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
  case SearchOptions::Entry : {
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
  case SearchOptions::Word : {
    if (USE_KEYMAPS) {
      if (m_hasMaps && m_keymapsEnabled)
        m_keymapGroup->setVisible(false);
      else
        m_keymapGroup->setVisible(false);
    }
    m_stickySearch->setVisible(false);
    m_includeHeads->setVisible(true);

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
  case SearchOptions::Local : {
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
QRegExp SearchOptionsWidget::buildRx(const QString & searchtarget,const QString & diacritics,const SearchOptions & options) {
  QRegExp rx;
  QString target = searchtarget;
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
  opts.setSticky(m_stickySearch->isChecked());

  opts.setKeymaps(m_keymapsEnabled);
  opts.setSearchScope(m_options.getSearchScope());
}
void SearchOptionsWidget::setOptions(const SearchOptions & options) {
  QLOG_DEBUG() << Q_FUNC_INFO;
  m_options = options;
  m_ignoreDiacritics->setChecked(options.ignoreDiacritics());

  m_wholeWordMatch->setChecked(options.wholeWordMatch());

  m_regexSearch->setChecked((options.getSearchType() == SearchOptions::Regex));
  m_normalSearch->setChecked((options.getSearchType() == SearchOptions::Normal));


  // m_arabicTarget->setChecked(x & Lane::Arabic);

  // m_buckwalterTarget->setChecked(x & Lane::Buckwalter);

  m_includeHeads->setChecked(options.includeHeads());


  m_keymapsEnabled = options.keymaps();

  m_stickySearch->setChecked(options.sticky());


}
