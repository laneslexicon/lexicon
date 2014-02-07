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
//CREATE TABLE history(id integer primary key,nodeId text,word text,root text,timewhen text);
HistoryMaster::HistoryMaster(const QString & dbname) {
  m_historyOn = false;
  m_historyEnabled = false;
  m_getQuery = 0;
  m_addQuery = 0;
  m_forQuery = 0;
  m_backQuery = 0;
  m_lastQuery = 0;

  bool ok = openDatabase(dbname);
  if (ok) {
    m_getQuery = new QSqlQuery(m_db);
    m_addQuery = new QSqlQuery(m_db);
    m_forQuery = new QSqlQuery(m_db);
    m_backQuery = new QSqlQuery(m_db);
    m_lastQuery = new QSqlQuery(m_db);
    if (
        (m_addQuery->prepare("insert into history (node,word,root,supplement,page,vol,timewhen,nodeOnly) values (?,?,?,?,?,?,?,?)")) &&
        (m_backQuery->prepare("select * from history where id <= ? order by id desc")) &&
        (m_forQuery->prepare("select * from history where id > ? order by id asc")) &&
        (m_lastQuery->prepare("select * from history where id = (select max(id) from history)")) &&
        (m_getQuery->prepare("select * from history where id = ?"))
          )
        {
        QSqlQuery mq(m_db);
        mq.exec("select max(id) from history");
        if (mq.first()) {
          m_lastId = mq.value(0).toInt();
        }
        m_historyOn = true;
        m_historyEnabled = true;
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
  return m_addQuery->exec();


}
/**
 * The calling function should delete the events
 *
 * @param count
 * @param direciton 0 - back, 1 forward
 * @return
 */
QList<HistoryEvent *> HistoryMaster::getHistory(int count,int direction,int startPos) {
  QList<HistoryEvent *> events;
  if (! m_historyEnabled ) {
    return events;
  }
  if (startPos == -1) {
    startPos = m_lastId;
  }
  if (direction == 0) {
      int ix = 0;
    m_backQuery->bindValue(0,startPos);
    m_backQuery->exec();
    while(m_backQuery->next() && (ix < count)) {
      HistoryEvent * event = new HistoryEvent();
      event->setId(m_backQuery->value(0).toInt());
      Place p;
      p.setNode(m_backQuery->value(1).toString());
      p.setWord(m_backQuery->value(2).toString());
      p.setRoot(m_backQuery->value(3).toString());
      p.setId(m_backQuery->value(0).toInt());
      /// TODO add other place fields
      event->setPlace(p);
      event->setWhen(m_backQuery->value(4).toDateTime());
      if ((ix > 0) && ( events[ix-1]->matches(event))) {
        delete event;
      }
      else {
        events << event;
        ix++;
      }
    }
    return events;
  }
  if (direction == 1) {
    int ix = 0;
    m_forQuery->bindValue(0,startPos);
    m_forQuery->exec();
    while(m_forQuery->next() && (ix < count)) {
      HistoryEvent * event = new HistoryEvent();
      event->setId(m_forQuery->value(0).toInt());
      Place p;
      p.setNode(m_forQuery->value(1).toString());
      p.setWord(m_forQuery->value(2).toString());
      p.setRoot(m_forQuery->value(3).toString());
      p.setId(m_backQuery->value(0).toInt());
      event->setPlace(p);
      event->setWhen(m_forQuery->value(4).toDateTime());
      if ((ix > 0) && ( events[ix-1]->matches(event))) {
        delete event;
      }
      else {
        events << event;
        ix++;
      }
    }
    return events;
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
