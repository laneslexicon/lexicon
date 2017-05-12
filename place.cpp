#include "place.h"
/*
   Place p;
   p.setNode("test node");
   p.setId(33);
   QLOG_DEBUG() << p;
   QVariant v;
   v.setValue(p);
   Place x = v.value<Place>();

   QLOG_DEBUG() << v.userType() << x.getNode() << x.getId();
*/
/// TODO can we move this somewhere
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
int Place::volume(int p) {
  for(int i=0; i < 8;i++) {
    if (p <= Place::m_vols[i]) {
      return (i + 1);
    }
  }
  return 0;
}
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
 * Returns in order of precedence: head,word,root,page
 *
 *
 * @return
 */
QString Place::getShortText() const {
  if (! m_head.isEmpty()) {
    return QString(QObject::tr("%1")).arg(m_head);
  }
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
QString Place::location() const {
  return QString(QObject::tr("Vol %1,Page %2")).arg(m_vol).arg(m_page);
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
    txt += QString(QObject::tr("Head word:%1")).arg(m_word);
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
  QString t = QString("%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12")
    .arg(m_datasource)
    .arg(m_root)
    .arg(m_node)
    .arg(m_word)
    .arg(m_action)
    .arg(m_page)
    .arg(m_vol)
    .arg(m_id)
    .arg(m_supplement)
    .arg(m_options)
    .arg(m_type)
    .arg(m_head);


  return t;
}
Place Place::fromString(const QString & str) {
  bool ok;
  QStringList x = str.split(",");
  Place p;
  int sz = x.size();
  //  QLOG_DEBUG() << Q_FUNC_INFO << str << sz;
  if (sz > 0)
    p.setSource(x[0].toInt(&ok));
  if (sz > 1)
    p.setRoot(x[1]);
  if (sz > 2)
    p.setNode(x[2]);
  if (sz > 3)
    p.setWord(x[3]);
  if (sz > 4)
    p.setAction(x[4].toInt(&ok));
  if (sz > 5)
    p.setPage(x[5].toInt(&ok));
  if (sz > 6)
    p.setVol(x[6].toInt(&ok));
  if (sz > 7)
    p.setId(x[7].toInt(&ok));
  if (sz > 8)
    p.setSupplement(x[8].toInt(&ok));
  if (sz > 9)
    p.setOptions(x[9].toInt(&ok));
  if (sz > 10)
    p.setType(x[10].toInt(&ok));
  if (sz > 11)
    p.setHead(x[11]);
  return p;
}
Place Place::fromRoot(const QString & root) {
  Place p;
  p.setType(Place::Root);
  p.setRoot(root);
  return p;
}
Place Place::fromNode(const QString & node) {
  Place p;
  p.setType(Place::Node);
  p.setNode(node);
  return p;
}
Place Place::fromPage(int page) {
  Place p;
  p.setType(Place::Page);
  p.setPage(page);
  return p;
}
Place Place::fromEntryRecord(const QSqlRecord & rec) {
  Place p;
  p.m_root = rec.value("root").toString();
  p.m_node = rec.value("nodeid").toString();
  p.m_supplement = rec.value("supplement").toInt();
  p.m_word = rec.value("word").toString();
  p.m_page = rec.value("page").toInt();
  p.m_vol = volume(p.m_page);
  p.m_action = Place::User;
  p.m_head = rec.value("headword").toString();
  if (p.m_head.size() == 0) {
      p.m_head = rec.value("word").toString();
  }
  return p;
}
void Place::setHead(const QString & x) {
  m_head = x;
}
QString Place::format(const QString & fmt) const {
  QString str = fmt;
  str.replace("%R",m_root);
  str.replace("%H",m_word);
  str.replace("%V",QString("%1").arg(m_vol));
  str.replace("%P",QString("%1").arg(m_page));
  str.replace("%N",m_node);
  return str;
}
QString Place::formatc(const QString & fmt) const {
  QMap<QString,QString> v;
  v.insert("R",m_root);
  v.insert("H",m_word);
  v.insert("N",m_node);
  v.insert("V",QString("%1").arg(m_vol));
  v.insert("P",QString("%1").arg(m_page));
  QString pattern = fmt;
  QStringList codes = v.keys();

  for(int i=0;i < codes.size();i++) {
    QRegularExpression ex(QString("\\?%1(.+)\\?%1").arg(codes[i]));
    QRegularExpressionMatch m =  ex.match(pattern);
    if (m.hasMatch()) {
      QString m1 = m.captured(1);
      if (v.value(codes[i]).isEmpty())  {
        pattern.replace(ex,"");
      }
      else {
      	QString oldvalue = m1;
      	m1.replace(QString("?%1").arg(codes[i]),"");
	QString x = this->format(m1);
	pattern.replace(QString("?%1%2?%3").arg(codes[i]).arg(oldvalue).arg(codes[i]),x);
      }
    }
    else {
      //      pattern.replace(QString("%%1").arg(codes[i]),v.value(codes[i]));
    }
  }
  return this->format(pattern);

}
