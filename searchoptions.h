#ifndef __SEARCHOPTIONS_H__
#define __SEARCHOPTIONS_H__

class SearchOptions {
 public:
  enum SearchType_t { Normal, Regex };
  enum  { Root , Entry, Word, Local, Node, Page };
  SearchOptions();
  bool ignoreDiacritics() const;
  void setIgnoreDiacritics(bool);

  void setSearchType(SearchType_t);
  int  getSearchType() const;

  void setSearchScope(int);
  int  getSearchScope() const ;

  bool wholeWordMatch() const;
  void setWholeWordMatch(bool v);

  bool showAll() const;
  void setShowAll(bool);

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
  int  m_target;
  bool m_forceLTR;
  bool m_showAll;
  bool m_newTab;
  bool m_activateTab;
};
#endif
