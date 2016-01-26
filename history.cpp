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
CREATE TABLE history(id integer primary key,timewhen text,place text);
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
  QSqlQuery sql(m_db);
  if (! sql.prepare(SQL_GET_MAX_HISTORY)) {
    QLOG_WARN() << QString(QObject::tr("Unable to prepare SQL_GET_MAX_HISTORY : %1")).arg(sql.lastError().text());
    return;
  }
  sql.exec();
  if (sql.first()) {
    m_lastId = sql.value(0).toInt();
  }
  /// historyOn can be toggled to prevent history writes
  /// historyEnabled
  m_historyOn = true;
  m_historyEnabled = true;
  m_ok = true;
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

int HistoryMaster::getNewestId() {
  Place p;
  QSqlQuery sql(m_db);

  m_lastId = -1;
  if (sql.prepare(SQL_LAST_HISTORY)) {
    sql.exec();
    if (sql.first()) {
      QString str = sql.value("id").toString();
      bool ok;
      int v = str.toInt(&ok);
      if (ok) {
        m_lastId = v;
      }
    }
  }
  return m_lastId;
}

int HistoryMaster::getOldestId() {
  m_firstId = -1;


  QSqlQuery sql(m_db);

  if (sql.prepare(SQL_FIRST_HISTORY)) {
    sql.exec();
    if (sql.first()) {
      QString str = sql.value("id").toString();
      bool ok;
      int v = str.toInt(&ok);
      if (ok) {
        m_firstId = v;
      }
    }
  }
  return m_firstId;
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
  int id = getNewestId();
  int count = 0;
  QSqlQuery sql(m_db);
  if (! sql.prepare(SQL_PREV_HISTORY)) {
    QLOG_WARN() << QString(QObject::tr("Unable to prepare add SQL_PREV_HISTORY : %1")).arg(sql.lastError().text());
    return -1;
  }
  sql.bindValue(0,id);
  sql.exec();

  while(sql.next()) {
    Place hp = Place::fromString(sql.value("place").toString());
    if ((hp.getNode() == p.getNode()) &&
        (hp.getRoot() == p.getRoot()) &&
        (hp.getWord() == p.getWord())) {
      return sql.value("id").toInt();
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
  QSqlQuery sql(m_db);
  if (! sql.prepare(SQL_ADD_HISTORY_PLACE)) {
    QLOG_WARN() << QString(QObject::tr("Unable to prepare add history SQL : %1")).arg(sql.lastError().text());
    QLOG_WARN() << QString(QObject::tr("History disabled"));
    m_historyEnabled = false;
    return false;

  }
  sql.bindValue(0,p.toString());
  sql.bindValue(1,event->getWhen());
  if (! sql.exec()) {
    QLOG_WARN() << QString(QObject::tr("Unable to add history record : %1")).arg(sql.lastError().text());
    return false;
  }
  return true;
  /*
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
  */
  return false;


}
 QList<HistoryEvent *> HistoryMaster::getHistory() {
  QList<HistoryEvent *> events;
  //  if (! m_historyEnabled ) {
  //    return events;
  //  }
  QLOG_DEBUG() << Q_FUNC_INFO;
  QSqlQuery sql(m_db);
  if (! sql.prepare(QString(SQL_LIST_HISTORY).arg(m_size))) {
    QLOG_WARN() << QString(QObject::tr("Unable to prepare gethistory SQL : %1")).arg(sql.lastError().text());
    return events;
  }
  sql.exec();
  while(sql.next()) {
    HistoryEvent * event = new HistoryEvent();
    int id = sql.value("id").toInt();
    event->setId(id);
    Place p  = Place::fromString(sql.value("place").toString());
    event->setPlace(p);
    event->setWhen(sql.value("timewhen").toDateTime());
    events << event;
  }
  return events;
}
HistoryEvent * HistoryMaster::getEvent(int id) {
   HistoryEvent * event = new HistoryEvent();
   //   if ( ! m_historyEnabled ) {
   //     return event;
   //   }
   QSqlQuery sql(m_db);
   if (! sql.prepare(SQL_GET_HISTORY))  {
    QLOG_WARN() << QString(QObject::tr("Unable to prepare SQL_GET_HISTORY : %1")).arg(sql.lastError().text());
    return event;
  }
   sql.bindValue(0,id);
   sql.exec();
   if (sql.first()) {
     event->setId(sql.value("id").toInt());
     Place p = Place::fromString(sql.value("place").toString());
     event->setPlace(p);
     event->setWhen(sql.value("timewhen").toDateTime());
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
   QSqlQuery sql(m_db);
   if (sql.prepare(SQL_DELETE_HISTORY)) {
     return sql.exec();
   }
   else {
    QLOG_WARN() << QString(QObject::tr("Unable to prepare SQL_DELETE_HISTORY : %1")).arg(sql.lastError().text());
   }
   return false;
}
Place HistoryMaster::getPlaceById(int id) {
  QLOG_DEBUG() << Q_FUNC_INFO << id;
  Place p;

  QSqlQuery sql(m_db);
  if (! sql.prepare(SQL_GET_HISTORY)) {
    QLOG_WARN() << QString(QObject::tr("Unable to prepare SQL_GET_HISTORY : %1")).arg(sql.lastError().text());
    return p;
  }
  sql.bindValue(0,id);
  sql.exec();
  if (sql.first()) {
    p = Place::fromString(sql.value("place").toString());
  }
  return p;
}
