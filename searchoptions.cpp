#include "searchoptions.h"
#include "fullsearch.h"
#include "namespace.h"
#include "scripts.h"
#define USE_KEYMAPS 0
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
int SearchOptions::getSearchType() const {
  return m_type;
}
void SearchOptions::setSearchScope(int x) {
  m_target = x;
}
int SearchOptions::getSearchScope() const {
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
