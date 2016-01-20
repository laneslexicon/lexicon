#include "searchoptions.h"
#include "fullsearch.h"
#include "scripts.h"
#define USE_KEYMAPS 0
SearchOptions::SearchOptions() {
  m_ignoreDiacritics = true;
  m_wholeWordMatch = false;
  m_keymaps = false;
  m_type = SearchOptions::Normal;
  m_includeHeads = false;
  m_target = SearchOptions::Unknown;
  m_forceLTR = false;
  m_showAll = false;
  m_newTab = false;
  m_activateTab = false;
  m_ignoreCase = false;
}
bool SearchOptions::ignoreDiacritics() const {
  return m_ignoreDiacritics;
}
void SearchOptions::setIgnoreDiacritics(bool v) {
  m_ignoreDiacritics = v;
}
bool SearchOptions::wholeWordMatch() const {
  return m_wholeWordMatch;
}
void SearchOptions::setWholeWordMatch(bool v) {
  m_wholeWordMatch = v;
}
void SearchOptions::setSearchType(SearchType_t x) {
  m_type = x;
}
SearchOptions::SearchType_t SearchOptions::getSearchType() const {
  return m_type;
}
void SearchOptions::setSearchScope(SearchScope_t x) {

  m_target = x;
}
SearchOptions::SearchScope_t SearchOptions::getSearchScope() const {
  return m_target;
}
bool SearchOptions::showAll() const {
  return m_showAll;
}
void SearchOptions::setShowAll(bool v) {
  m_showAll = v;
}

bool SearchOptions::includeHeads() const {
  return m_includeHeads;
}
bool SearchOptions::ignoreCase() const {
  return m_ignoreCase;
}

void SearchOptions::setIncludeHeads(bool v) {
  m_includeHeads = v;
}

bool SearchOptions::forceLTR() const {
  return m_forceLTR;
}
void SearchOptions::setForceLTR(bool v) {
  m_forceLTR = v;
}
bool SearchOptions::newTab() const {
  return m_newTab;
}
void SearchOptions::setNewTab(bool v) {
  m_newTab = v;
}
bool SearchOptions::activateTab() const {
  return m_activateTab;
}
void SearchOptions::setActivateTab(bool v) {
  m_activateTab = v;
}
bool SearchOptions::keymaps() const {
  return m_keymaps;
}
void SearchOptions::setKeymaps(bool v) {
  m_keymaps = v;
}
void SearchOptions::setIgnoreCase(bool v) {
  m_ignoreCase = v;
}
bool SearchOptions::isValid() const {
  return (m_target != SearchOptions::Unknown);
}
QString SearchOptions::pattern() const {
  return m_pattern;
}
void SearchOptions::setPattern(const QString & str) {
  m_pattern = str;
}
void SearchOptions::setHeadPhrase(bool v) {
  m_headPhrase = v;
}
bool SearchOptions::headPhrase() const {
  return m_headPhrase;
}
