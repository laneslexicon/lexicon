#include "history.h"
HistoryEvent::HistoryEvent() {
  m_when = QDateTime::currentDateTime();
  m_id = -1;
}
bool HistoryEvent::matches(HistoryEvent * event) {
  /// TODO replace with place == place
  Place p = event->getPlace();
  return ((m_place.getWord() == p.getWord()) &&
          (m_place.getRoot() == p.getRoot()) &&
          (m_place.getNode() == p.getNode()));

}
/*
CREATE TABLE history(id integer primary key,node text,word text,root text,supplement int,page int,vol int,timewhen text,nodeonly int, pagemode int);
*/
HistoryMaster::HistoryMaster(const QString & dbname) {
  m_historyOn = false;
  m_ok = false;
  readSettings();
  m_getQuery = 0;
  m_listQuery = 0;
  m_addQuery = 0;
  m_forQuery = 0;
  m_backQuery = 0;
  m_lastQuery = 0;
  QString fields = "id,node,word,root,supplement,page,vol,timewhen,nodeOnly,pagemode";
  bool ok = openDatabase(dbname);
  if (ok) {
    m_getQuery = new QSqlQuery(m_db);
    m_listQuery = new QSqlQuery(m_db);
    m_addQuery = new QSqlQuery(m_db);
    m_forQuery = new QSqlQuery(m_db);
    m_backQuery = new QSqlQuery(m_db);
    m_lastQuery = new QSqlQuery(m_db);
    if (
        m_addQuery->prepare(QString("insert into history (%1) values (?,?,?,?,?,?,?,?,?)").arg(fields)) &&
        m_backQuery->prepare(QString("select %1 from history where id <= ? order by id desc").arg(fields)) &&
        m_forQuery->prepare(QString("select %1 from history where id > ? order by id asc").arg(fields)) &&
        m_lastQuery->prepare(QString("select %1 from history where id = (select max(id) from history)").arg(fields)) &&
        m_listQuery->prepare(QString("select %1 from history order by id desc limit %2").arg(fields).arg(m_size)) &&
        m_getQuery->prepare(QString("select %1 from history where id = ?").arg(fields)))
        {
        QSqlQuery mq(m_db);
        mq.exec("select max(id) from history");
        if (mq.first()) {
          m_lastId = mq.value(0).toInt();
        }
        m_historyOn = true;
        m_historyEnabled = true;
        m_ok = true;
        }
    else {
      QLOG_WARN() << "History not available" << m_db.lastError().text();
    }
  }
}
HistoryMaster::~HistoryMaster() {
  if (m_addQuery) {
    delete m_addQuery;
  }
  if (m_backQuery) {
    delete m_backQuery;
  }
  if (m_forQuery) {
    delete m_forQuery;
  }
  if (m_getQuery) {
    delete m_getQuery;
  }
  if (m_lastQuery) {
    delete m_lastQuery;
  }
  if (m_db.isOpen()) {
    m_db.close();
  }
}
void HistoryMaster::setEnabled(bool v) {
  if (m_ok)
    m_historyEnabled = v;
}
void HistoryMaster::on() {
  if (m_ok)
    m_historyOn = true;
}
void HistoryMaster::off() {
  m_historyOn = false;
}

Place HistoryMaster::getLastPlace() {
  Place p;

  m_lastQuery->exec();
  if (m_lastQuery->first()) {
    p.setId(m_lastQuery->value(0).toInt());
    p.setNode(m_lastQuery->value(1).toString());
    p.setWord(m_lastQuery->value(2).toString());
    p.setRoot(m_lastQuery->value(3).toString());
  }
  return p;
}
//  QString fields = "id,node,word,root,supplement,page,vol,timewhen,nodeOnly,pagemode";
Place HistoryMaster::toPlace(QSqlQuery * sql) {
  Place p;
  p.setId(sql->value(0).toInt());
  p.setNode(sql->value(1).toString());
  p.setWord(sql->value(2).toString());
  p.setRoot(sql->value(3).toString());
  p.setSupplement(sql->value(4).toInt());
  p.setPage(sql->value(5).toInt());
  p.setVol(sql->value(6).toInt());

  p.setWhen(sql->value(7).toString());
  p.setNodeOnly(sql->value(8).toBool());
  p.setPageMode(sql->value(9).toBool());
  return p;

}
//create table history(id integer primary key,nodeId text,word text,root text,timewhen text);
bool HistoryMaster::openDatabase(const QString & dbname) {
  QFile dbfile(dbname);
  if (! dbfile.exists()) {
    return false;
  }
  if (m_db.isOpen()) {
    m_db.close();
  }
  bool ok;
  m_db = QSqlDatabase::addDatabase("QSQLITE","historydb");
  m_db.setDatabaseName(dbname);
  ok = m_db.open();

  QLOG_INFO() << "Opened database for history" << dbname;
  return ok;
}
bool HistoryMaster::add(const Place & p) {
  if (! m_historyEnabled ) {
    return false;
  }
  if (! p.isValid()) {
    return false;
  }
  /// don't add history event
  if (p.getType() == Place::History) {
    return false;
  }
  /// don't add two adjecent identical places
  Place o = getLastPlace();
  if ((o.getNode() == p.getNode()) &&
      (o.getRoot() == p.getRoot()) &&
      (o.getWord() == p.getWord())) {
    return false;
  }
  /// TODO get last record and exit if sample place
  /// TODO add other fields
  HistoryEvent * event = new HistoryEvent;
  event->setPlace(p);

  m_addQuery->bindValue(0,p.getNode());
  m_addQuery->bindValue(1,p.getWord());
  m_addQuery->bindValue(2,p.getRoot());
  m_addQuery->bindValue(3,p.getSupplement());
  m_addQuery->bindValue(4,p.getPage());
  m_addQuery->bindValue(5,p.getVol());
  m_addQuery->bindValue(6,event->getWhen());
  m_addQuery->bindValue(7,p.getNodeOnly());
  m_addQuery->bindValue(8,p.getPageMode());
  return m_addQuery->exec();


}
 QList<HistoryEvent *> HistoryMaster::getHistory() {
  QList<HistoryEvent *> events;
  if (! m_historyEnabled ) {
    return events;
  }
  QLOG_DEBUG() << Q_FUNC_INFO;
  m_listQuery->exec();
  while(m_listQuery->next()) {
    HistoryEvent * event = new HistoryEvent();
    int fno = m_listQuery->record().indexOf("id");
    event->setId(m_listQuery->value(fno).toInt());
    Place p;
    /// TODO add other place fields
    p.setNode(m_listQuery->value(1).toString());
    p.setWord(m_listQuery->value(2).toString());
    p.setRoot(m_listQuery->value(3).toString());
    p.setId(m_listQuery->value(0).toInt());
    event->setPlace(p);
    event->setWhen(m_listQuery->value(4).toDateTime());
    events << event;
  }
  return events;
}
HistoryEvent * HistoryMaster::getEvent(int id) {
   HistoryEvent * event = new HistoryEvent();
   if ( ! m_historyEnabled ) {
     return event;
   }
   m_getQuery->bindValue(0,id);
   m_getQuery->exec();
   if (m_getQuery->first()) {
       event->setId(m_getQuery->value(0).toInt());
       Place p;
       p.setNode(m_getQuery->value(1).toString());
       p.setWord(m_getQuery->value(2).toString());
       p.setRoot(m_getQuery->value(3).toString());
       p.setId(m_backQuery->value(0).toInt());
       event->setPlace(p);
       event->setWhen(m_getQuery->value(4).toDateTime());
   }
   return event;
}
void HistoryMaster::readSettings() {
  QSettings settings;
  settings.setIniCodec("UTF-8");
  settings.beginGroup("History");
  m_historyEnabled = settings.value("Enabled",true).toBool();
  m_size = settings.value("Size",10).toInt();

}
