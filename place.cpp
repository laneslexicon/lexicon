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
int Place::m_vols[8] = {367,837,1280,1757,2219,2475,2749,3064};

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
  for(int i=0; i < 8;i++) {
    if (p <= Place::m_vols[i]) {
      m_vol = i + 1;
      return;
    }
  }
}
