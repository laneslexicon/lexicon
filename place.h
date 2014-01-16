#ifndef __PLACE_H__
#define __PLACE_H__
#include <QString>
#include "QsLog.h"
class Place {
 public:
  Place() {
    m_supplement = 0;
  }
  Place(const QString & root,int supp) {
    m_root = root;
    m_supplement = supp;
    m_isRoot = true;
  }
  bool isEmpty() {
    return m_root.isEmpty();
  }
  bool isValid() {
    return ! m_root.isEmpty();
  }
  void setRoot(const QString & root) { m_root = root;}
  void setIsRoot(bool v) {
    m_isRoot = v;
  }
  void setWord(const QString & word) {
    m_word = word;
  }
  void setNode(const QString & node) { m_node = node;}
  QString getRoot() const { return m_root;}
  int getSupplement() const { return m_supplement;}
  QString getNode() const { return m_node;}
  QString getWord() const { return m_word;}
 private:
  QString m_msg;
  QString m_root;
  QString m_node;
  int m_supplement;
  QString m_word;
  QString m_anchor;
  bool m_isRoot;
};
#endif
