#ifndef __SEARCHOPTIONS_H__
#define __SEARCHOPTIONS_H__
#include <QString>
class SearchOptions {
 public:
  enum SearchType_t { Normal, Regex };
  /// Word == HeadSearch , Entry == FullSearch
  enum SearchScope_t { Unknown, Root , Entry, Word, Local, Node, Page, Text };
  SearchOptions();
  bool ignoreDiacritics() const;
  void setIgnoreDiacritics(bool);

  void setSearchType(SearchType_t);
  SearchType_t  getSearchType() const;

  void setSearchScope(SearchScope_t);
  SearchScope_t  getSearchScope() const ;

  bool wholeWordMatch() const;
  void setWholeWordMatch(bool v);

  bool enWholeWordMatch() const;
  void setEnWholeWordMatch(bool v);

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

  bool ignoreCase() const;
  void setIgnoreCase(bool);

  bool textSearch() const;
  void setTextSearch(bool);

  bool headPhrase() const;
  void setHeadPhrase(bool);

  QString pattern() const;
  void setPattern(const QString &);

  bool isValid() const;
  bool isWholeWord() const;   // returns value according to whether isArabic is set

  bool regex() const;
 private:
  bool m_textSearch;
  bool m_ignoreDiacritics;
  bool m_wholeWordMatch;
  bool m_enWholeWordMatch;
  bool m_keymaps;
  SearchType_t  m_type;
  bool m_includeHeads;
  SearchScope_t  m_target;
  bool m_forceLTR;
  bool m_showAll;
  bool m_newTab;
  bool m_activateTab;
  bool m_ignoreCase;
  QString m_pattern;
  bool m_headPhrase;
};
#endif
