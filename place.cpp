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
/**
 * Returns in order of precedence: word,root,page
 *
 *
 * @return
 */
QString Place::getShortText() const {
  if (! m_word.isEmpty()) {
    return QString(QObject::tr("%1")).arg(m_word);
  }
  if (! m_root.isEmpty()) {
    return QString(QObject::tr("%1")).arg(m_root);
  }
  if ( m_page != -1) {
    return QString(QObject::tr("Page %1")).arg(m_page);
  }
  return QString();
}
QString Place::getText(bool pageOnly) const {
  QString txt;

  if (pageOnly) {
    if ( m_page != -1) {
      txt += QString(QObject::tr("Vol %1,Page %2")).arg(m_vol).arg(m_page);
    }
  }
  if (! m_root.isEmpty()) {
    txt += QString(QObject::tr("Root:%1")).arg(m_root);
  }
  if (! m_word.isEmpty()) {
    if (! txt.isEmpty()) {
      txt += ",";
    }
    txt += QString(QObject::tr("Entry:%1")).arg(m_word);
  }
  if ((! pageOnly) && ( m_page != -1)) {
    if (! txt.isEmpty()) {
      txt += ",";
    }
    txt += QString(QObject::tr("Vol %1,Page %2")).arg(m_vol).arg(m_page);
  }
  if (! m_node.isEmpty()) {
    bool b = false;
    if (! txt.isEmpty()) {
      txt += " (";
      b = true;
    }
    txt += QString(QObject::tr("Node:%1")).arg(m_node);
    if (b) {
      txt += ")";
    }
  }
  return txt;
}
QString Place::toString() const {
  QString t = QString("%1,%2,%3,%4,%5,%6,%7,%8,%9,%10")
    .arg(m_root)
    .arg(m_node)
    .arg(m_word)
    .arg(m_type)
    .arg(m_page)
    .arg(m_vol)
    .arg(m_id)
    .arg(m_supplement);

  return t;
}
Place Place::fromString(const QString & str) {
  bool ok;
  QStringList x = str.split(",");
  Place p;
  int sz = x.size();
  //  qDebug() << Q_FUNC_INFO << str << sz;
  if (sz > 0)
    p.setRoot(x[0]);
  if (sz > 1)
    p.setNode(x[1]);
  if (sz > 2)
    p.setWord(x[2]);
  if (sz > 3)
    p.setType(x[3].toInt(&ok));
  if (sz > 4)
    p.setPage(x[4].toInt(&ok));
  if (sz > 5)
    p.setVol(x[5].toInt(&ok));
  if (sz > 6)
    p.setId(x[6].toInt(&ok));
  if (sz > 7)
    p.setSupplement(x[7].toInt(&ok));

  return p;
}
