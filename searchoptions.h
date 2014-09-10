#ifndef __SEARCHOPTIONS_H__
#define __SEARCHOPTIONS_H__
#include <QtCore>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>

class FullSearchWidget;
class SearchOptions {
 public:
  enum SearchType_t { Normal, Regex };
  enum SearchScope_t { Root , Entry, Word, Local };

  bool ignoreDiacritics() const;
  void setIgnoreDiacritics(bool);

  void setSearchType(SearchType_t);
  int  getSearchType() const;

  void setSearchScope(SearchScope_t);
  int  getSearchScope() const ;

  bool wholeWordMatch() const;
  void setWholeWordMatch(bool v);

  bool sticky() const;
  void setSticky(bool);

  bool includeHeads() const;
  void setIncludeHeads(bool);

  bool forceLTR() const;
  void setForceLTR(bool);

  bool newTab() const;
  void setNewTab(bool);

  bool activateTab() const;
  void setActivateTab(bool);

  bool keymaps() const;
  void setKeymaps(bool);

 private:
  bool m_ignoreDiacritics;
  bool m_wholeWordMatch;
  bool m_keymaps;
  SearchType_t  m_type;
  bool m_includeHeads;
  SearchScope_t  m_target;
  bool m_forceLTR;
  bool m_sticky;
  bool m_newTab;
  bool m_activateTab;
};
class SearchOptionsWidget : public QWidget {
  Q_OBJECT

 public:
  SearchOptionsWidget(int searchType,QWidget * parent = 0);
  SearchOptionsWidget(SearchOptions & opts,QWidget * parent = 0);
  ~SearchOptionsWidget();

  void getOptions(SearchOptions &) const;
  void setOptions(const SearchOptions &);

  int getOptions();
  void setOptions(int);
  void showMore(bool);
  void addKeymaps(const QString &activeMap,const QStringList & maps);
  void setKeymapsEnabled(bool v);
  bool getForceLTR();
  static QRegExp buildRx(const QString &,int);
 public slots:
  void searchTypeChanged();
  void keymapChanged();
  void onForceLeftToRight(int);
 private:
  void setup(QWidget * parent);
  bool m_hasMaps;
  bool m_more;                   // false basic, 1 show more
  bool m_keymapsEnabled;
  SearchOptions m_options;
  int m_searchType;
  QSpacerItem * m_spacer;
  QGroupBox *  m_targetGroup;
  QGroupBox * m_typeGroup;
  QGroupBox * m_contextGroup;
  QGroupBox * m_keymapGroup;
  QCheckBox * m_ignoreDiacritics;
  QCheckBox * m_wholeWordMatch;
  QCheckBox * m_includeHeads;
  QCheckBox * m_stickySearch;
  QRadioButton * m_headWord;
  QRadioButton * m_fullText;
  QCheckBox * m_forceLTR;
  QRadioButton * m_normalSearch;
  QRadioButton * m_regexSearch;
  QRadioButton * m_arabicTarget;
  QRadioButton * m_buckwalterTarget;

 signals:
  void loadKeymap(const QString &);
  void force(bool);
};
#endif
