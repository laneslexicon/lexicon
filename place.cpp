#include "place.h"
/*
   Place p;
   p.setNode("test node");
   p.setId(33);
   qDebug() << p;
   QVariant v;
   v.setValue(p);
   Place x = v.value<Place>();

   qDebug() << v.userType() << x.getNode() << x.getId();
*/
bool Place::isRoot() const {
  if (! m_root.isEmpty() && m_node.isEmpty()) {
    return true;
  }
  return false;
}
/*
  | Volume | First Page | Last Page |
  |--------+------------+-----------|
  |      1 |          1 |       367 |
  |      2 |        369 |       837 |
  |      3 |        839 |      1280 |
  |      4 |       1281 |      1757 |
  |      5 |       1759 |      2219 |
  |      6 |       2221 |      2475 |
  |      7 |       2478 |      2749 |
  |      8 |       2751 |      3064 |
*/
void Place::setPage(int p) {
  m_page = p;
  /*
  if (Place::m_vols.size() == 0) {
    Place::m_vols << 368 << 838 << 1281 << 1758 << 2220 << 2476 << 2749 << 3065;
  }
  for(int i=0; i < Place::m_vols.size();i++) {
    if (p < Place::m_vols[i]) {
      m_vol = i + 1;
      i = Place::m_vols.size();
    }
  }
  */
}
