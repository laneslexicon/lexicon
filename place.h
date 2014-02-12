#ifndef __PLACE_H__
#define __PLACE_H__
#include <QString>
#include <QDebug>
#include <QList>
#include "QsLog.h"
/*
  Using the QVariant;
    Place p;
    p.setNode("nxxxx");
    QVariant v(p);

    Place np = v.value<Place>();

*/

class Place {
 public:
  Place() {
    m_supplement = 0;
    m_page = -1;
    m_vol = -1;
    m_id = -1;
    m_showOnlyNodes = false;
    m_type = Place::User;
  }
  Place(const QString & root) {
    m_root = root;
    m_page = -1;
    m_vol = -1;
    m_id = -1;
    m_showOnlyNodes = false;
    m_type = Place::User;
  }
  Place(const QString & root,int supp) {
    m_root = root;
    m_supplement = supp;
    m_page = -1;
    m_vol = -1;
    m_id = -1;
    m_showOnlyNodes = false;
    m_type = Place::User;
  }
  ~Place() {
  }
  bool operator == (const Place & p) {
    return ((m_root == p.m_root) && (m_word == p.m_word) && (m_node == p.m_node));
  }
  bool operator != (const Place & p) {
    return ! ((m_root == p.m_root) && (m_word == p.m_word) && (m_node == p.m_node));
  }
  enum Type { User, History, Bookmark, RestoreTab };

  void setType(int t) { m_type = t;}
  int getType() const { return m_type;}
 inline friend QDebug operator<<(QDebug debug, const Place& p)
{
	debug.nospace() << "Place("
                        << p.getNode() << ","
                        << p.getSupplement() << ","
                        << p.getVol() << ","
                        << p.getPage() << ","
                        << p.getNodeOnly() << ","
                        << p.getType() << ","
                        << p.getId() << "," << "\n"
                        << p.getRoot() << ","
                        << p.getWord()
                        << ")";
	return debug.space();
}
  operator QVariant() const
    {
        return QVariant::fromValue(*this);
    }
  void setData(const QVariant & v) {
    m_data = v;
  }
  QVariant getData() { return m_data;}

  bool isEmpty() const {
    return m_root.isEmpty();
  }
  /// we are either looking for a root or a node
  bool isValid() const {
    if (m_root.isEmpty() &&  m_node.isEmpty()) {
      return false;
    }
    return true;
  }
  bool isSamePlace  (const Place & p) const {
    return ((m_root == p.m_root) && (m_node == p.m_node));
  }
  void setId(int id) { m_id = id;}
  int getId() const { return m_id;}
  void setRoot(const QString & root) { m_root = root;};
  bool isRoot() const;
  void setWord(const QString & word) {
    m_word = word;
  }
  void setNode(const QString & node) { m_node = node;}
  QString getRoot() const { return m_root;}
  int getSupplement() const { return m_supplement;}
  void setSupplement(int i) { m_supplement = i;}
  bool isSupplement() { return (m_supplement == 1);}
  int getPage() const { return m_page;}
  void setPage(int i);
  int getVol() const { return m_vol;}
  QString getNode() const { return m_node;}
  QString getWord() const { return m_word;}
  bool getNodeOnly() const { return m_showOnlyNodes;}

  void setNodeOnly(bool v) { m_showOnlyNodes = v;}
 private:
  int m_vol;
  int m_page;
  int m_id;
  int m_type;
  QString m_root;
  QString m_node;
  int m_supplement;
  QString m_word;
  bool m_showOnlyNodes;
  QVariant m_data;
};

Q_DECLARE_METATYPE(Place);


#endif
