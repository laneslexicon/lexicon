#include "history.h"
#include "externs.h"
#include "definedsettings.h"
#include "definedsql.h"
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

  QString fields = "id,node,word,root,supplement,page,vol,timewhen";
  bool ok = openDatabase(dbname);
  if (! ok) {
    m_historyEnabled = false;
    return;
  }

  m_getQuery = QSqlQuery(m_db);
  m_listQuery = QSqlQuery(m_db);
  m_addQuery =  QSqlQuery(m_db);
  m_forQuery =  QSqlQuery(m_db);
  m_backQuery = QSqlQuery(m_db);
  m_lastQuery = QSqlQuery(m_db);
  m_firstQuery = QSqlQuery(m_db);
  if (
      m_addQuery.prepare(QString(SQL_ADD_HISTORY)) &&
      m_backQuery.prepare(QString(SQL_PREV_HISTORY).arg(fields)) &&
      m_forQuery.prepare(QString(SQL_NEXT_HISTORY).arg(fields)) &&
      m_lastQuery.prepare(QString(SQL_LAST_HISTORY).arg(fields)) &&
      m_firstQuery.prepare(QString(SQL_FIRST_HISTORY).arg(fields)) &&
      m_listQuery.prepare(QString(SQL_LIST_HISTORY).arg(fields).arg(m_size)) &&
      m_getQuery.prepare(QString(SQL_GET_HISTORY).arg(fields)))
    {
      QSqlQuery mq(m_db);
      mq.exec(SQL_GET_MAX_HISTORY);
      if (mq.first()) {
        m_lastId = mq.value(0).toInt();
      }
      m_historyOn = true;
      m_historyEnabled = true;
      m_ok = true;
    }
  else {
    if (m_getQuery.lastError().isValid()) {
      QLOG_WARN() << "Error on getQuery" << m_getQuery.lastError().text();
    }
    if (m_listQuery.lastError().isValid()) {
      QLOG_WARN() << "Error on listQuery" << m_listQuery.lastError().text();
    }
    if (m_addQuery.lastError().isValid()) {
      QLOG_WARN() << "Error on addQuery" << m_addQuery.lastError().text();
    }
    if (m_forQuery.lastError().isValid()) {
      QLOG_WARN() << "Error on forQuery" << m_forQuery.lastError().text();
    }
    if (m_backQuery.lastError().isValid()) {
      QLOG_WARN() << "Error on backQuery" << m_backQuery.lastError().text();
    }
    if (m_lastQuery.lastError().isValid()) {
      QLOG_WARN() << "Error on lastQuery" << m_lastQuery.lastError().text();
    }
    if (m_firstQuery.lastError().isValid()) {
      QLOG_WARN() << "Error on lastQuery" << m_firstQuery.lastError().text();
    }
    QLOG_WARN() << "History not available" << m_db.lastError().text();
  }
}
HistoryMaster::~HistoryMaster() {
  if (m_db.isOpen()) {
    m_db.close();
  }
}
/**
 * Only disabled/enable if the db etc is ok
 *
 * @param v
 */
void HistoryMaster::setEnabled(bool v) {
  if (m_ok) {
    m_historyEnabled = v;
  }
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

  m_lastQuery.exec();
  if (m_lastQuery.first()) {
    p = this->toPlace(m_lastQuery);
  }
  return p;
}
int HistoryMaster::getLastId() {
  m_lastId = -1;
  m_lastQuery.exec();
  if (m_lastQuery.first()) {
    m_lastId = m_lastQuery.value(0).toInt();
  }
  return m_lastId;
}
Place HistoryMaster::getFirstPlace() {
  Place p;

  m_firstQuery.exec();
  if (m_firstQuery.first()) {
    p = this->toPlace(m_firstQuery);
  }
  return p;
}
int HistoryMaster::getFirstId() {
  m_firstId = -1;
  m_firstQuery.exec();
  if (m_firstQuery.first()) {
    m_firstId = m_firstQuery.value(0).toInt();
  }
  return m_firstId;
}
//  QString fields = "id,node,word,root,supplement,page,vol,timewhen,nodeOnly,pagemode";
Place HistoryMaster::toPlace(QSqlQuery & sql) {
  Place p;
  p.setId(sql.value(0).toInt());
  p.setNode(sql.value(1).toString());
  p.setWord(sql.value(2).toString());
  p.setRoot(sql.value(3).toString());
  p.setSupplement(sql.value(4).toInt());
  p.setPage(sql.value(5).toInt());
  p.setVol(sql.value(6).toInt());

  p.setWhen(sql.value(7).toString());
  return p;

}
/**
 * Read backwards in the history for <depth> number of records
 * search for the supplied Place
 *
 * @param p
 * @param depth
 *
 * @return
 */
int HistoryMaster::hasPlace(const Place & p,int depth) {
  if (depth == -1) {
    depth = m_duplicateDepth;
  }
  int id = getLastId();
  m_backQuery.bindValue(0,id);
  m_backQuery.exec();
  int count = 0;
  while(m_backQuery.next()) {
    Place hp = this->toPlace(m_backQuery);
    if ((hp.getNode() == p.getNode()) &&
      (hp.getRoot() == p.getRoot()) &&
      (hp.getWord() == p.getWord())) {
      return m_backQuery.value(0).toInt();
    }
    count++;
    if (count > depth) {
      return -1;
    }
  }
  return -1;
}
//create table history(id integer primary key,nodeid text,word text,root text,timewhen text);
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
  QLOG_DEBUG() << Q_FUNC_INFO << m_historyEnabled << p.toString();
  if (! m_historyEnabled ) {
    return false;
  }
  if (! p.isValid()) {
    return false;
  }
  /// don't add these events
  if (
      (p.getAction() == Place::History) ||
      (p.getAction() == Place::Bookmark) ||
      (p.getAction() == Place::RestoreTab) ||
      (p.getAction() == Place::SwitchTab)
      ) {
    return false;
  }
  QLOG_DEBUG() << Q_FUNC_INFO << p.toString();
  /// don't add two adjacent identical places
  if (this->hasPlace(p) != -1) {
    QLOG_DEBUG() << "History rejected, duplicate place";
    return false;
  }
  /// TODO get last record and exit if sample place
  /// TODO add other fields
  HistoryEvent * event = new HistoryEvent;
  event->setPlace(p);

  m_addQuery.bindValue(0,p.getNode());
  m_addQuery.bindValue(1,p.getWord());
  m_addQuery.bindValue(2,p.getRoot());
  m_addQuery.bindValue(3,p.getSupplement());
  m_addQuery.bindValue(4,p.getPage());
  m_addQuery.bindValue(5,p.getVol());
  m_addQuery.bindValue(6,event->getWhen());
  if (m_addQuery.exec()) {
    QLOG_DEBUG() << "Successfully updated history table";
    return true;
  }
  else {
    QLOG_WARN() << QString(QObject::tr("Unable to add history record : %1")).arg(m_addQuery.lastError().text());
  }
  return false;


}
 QList<HistoryEvent *> HistoryMaster::getHistory() {
  QList<HistoryEvent *> events;
  //  if (! m_historyEnabled ) {
  //    return events;
  //  }
  QLOG_DEBUG() << Q_FUNC_INFO;
  m_listQuery.exec();
  while(m_listQuery.next()) {
    HistoryEvent * event = new HistoryEvent();
    int fno = m_listQuery.record().indexOf("id");
    event->setId(m_listQuery.value(fno).toInt());
    Place p = this->toPlace(m_listQuery);
    event->setPlace(p);
    event->setWhen(m_listQuery.value(7).toDateTime());
    events << event;
  }
  return events;
}
HistoryEvent * HistoryMaster::getEvent(int id) {
   HistoryEvent * event = new HistoryEvent();
   //   if ( ! m_historyEnabled ) {
   //     return event;
   //   }
   m_getQuery.bindValue(0,id);
   m_getQuery.exec();
   if (m_getQuery.first()) {
       event->setId(m_getQuery.value(0).toInt());
       Place p = this->toPlace(m_getQuery);
       event->setPlace(p);
       event->setWhen(m_getQuery.value(7).toDateTime());
   }
   return event;
}
void HistoryMaster::readSettings() {
  SETTINGS
  settings.beginGroup("History");
  m_historyEnabled = settings.value(SID_HISTORY_ENABLED,true).toBool();
  m_size = settings.value(SID_HISTORY_SIZE,10).toInt();
  m_duplicateDepth = settings.value(SID_HISTORY_DUPLICATE_DEPTH,5).toInt();
}
bool HistoryMaster::clear() {
   if ( ! m_historyEnabled ) {
     return true;
   }
   QSqlQuery qry(m_db);
   if (qry.prepare("delete from history")) {
     return qry.exec();
   }
   else {
     QLOG_DEBUG() << "Error preparing delete history sql";
   }
   return false;
}
Place HistoryMaster::getPlaceById(int id) {
  QLOG_DEBUG() << Q_FUNC_INFO << id;
  Place p;
  m_getQuery.bindValue(0,id);
  m_getQuery.exec();
  if (m_getQuery.first()) {
    p = this->toPlace(m_getQuery);
  }
  return p;
}
