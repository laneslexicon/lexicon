#ifndef __PLACE_H__
#define __PLACE_H__
#include <QString>
#include <QDebug>
#include "QsLog.h"
class Place {
 public:
  Place() {
    m_supplement = 0;
    m_nodeOnly = false;
    m_page = -1;
    m_vol = -1;
    m_id = -1;
    m_isRoot = false;
  }
  Place(const QString & root) {
    m_root = root;
    m_isRoot = true;
    m_nodeOnly = false;
    m_page = -1;
    m_vol = -1;
    m_id = -1;
  }
  Place(const QString & root,int supp) {
    m_root = root;
    m_supplement = supp;
    m_isRoot = true;
    m_nodeOnly = false;
    m_page = -1;
    m_vol = -1;
    m_id = -1;
  }
  ~Place() {
  }
  inline friend QDebug operator<<(QDebug debug, const Place& p)
{
	debug.nospace() << "Place("
                        << p.getRoot() << ","
                        << p.getWord() << ","
                        << p.getNode() << ","
                        << p.getIsRoot() << ","
                        << p.getSupplement() << ","
                        << p.getPage() << ","
                        << p.getId() << ")";
	return debug.space();
}
  operator QVariant() const
    {
        return QVariant::fromValue(*this);
    }
  bool isEmpty() const {
    return m_root.isEmpty();
  }
  /// we are either looking for a root or a node
  bool isValid() const {
    if (m_root.isEmpty() && ! m_nodeOnly) {
      return false;
    }
    return true;
  }
  void setId(int id) { m_id = id;}
  int getId() const { return m_id;}
  void setRoot(const QString & root) { m_root = root;};
  void setIsRoot(bool v) {
    m_isRoot = v;
  }
  int getIsRoot() const { return m_isRoot;}
  bool isRoot() const { return m_isRoot;}
  void setWord(const QString & word) {
    m_word = word;
  }
  void setNode(const QString & node) { m_node = node;}
  QString getRoot() const { return m_root;}
  int getSupplement() const { return m_supplement;}
  void setSupplement(int i) { m_supplement = i;}
  bool isSupplement() { return (m_supplement == 1);}
  int getPage() const { return m_page;}
  void setPage(int i) { m_page = i;}
  QString getNode() const { return m_node;}
  QString getWord() const { return m_word;}

  void setNodeOnly(bool v) { m_nodeOnly = v;}
  bool getNodeOnly() const { return m_nodeOnly; }
 private:
  int m_vol;
  int m_page;
  int m_id;
  QString m_msg;
  QString m_root;
  QString m_node;
  int m_supplement;
  QString m_word;
  QString m_anchor;
  bool m_isRoot;
  bool m_nodeOnly;
};
Q_DECLARE_METATYPE(Place);
#endif
