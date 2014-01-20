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
  void setSupplement(int i) { m_supplement = i;}
  int getPage() { return m_page;}
  void setPage(int i) { m_page = i;}
  QString getNode() const { return m_node;}
  QString getWord() const { return m_word;}

  void setNodeOnly(bool v) { m_nodeOnly = v;}
  bool getNodeOnly() const { return m_nodeOnly; }
 private:
  int m_vol;
  int m_page;
  QString m_msg;
  QString m_root;
  QString m_node;
  int m_supplement;
  QString m_word;
  QString m_anchor;
  bool m_isRoot;
  bool m_nodeOnly;
};
#endif
