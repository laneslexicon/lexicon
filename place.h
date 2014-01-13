#ifndef __PLACE_H__
#define __PLACE_H__
#include <QString>
#include "QsLog.h"
class Place {
 public:
  Place() {}
  Place(const QString & root,int supp) {
    m_root = root;
    m_supplement = supp;
    m_isRoot = true;
  }
  void setIsRoot(bool v) {
    m_isRoot = true;
  }
  void setWord(const QString & word) {
    m_word = word;
  }
  QString getRoot() const { return m_root;}
  int getSupplement() const { return m_supplement;}
 private:
  QString m_root;
  QString m_node;
  int m_supplement;
  QString m_word;
  QString m_anchor;
  bool m_isRoot;
};
#endif
