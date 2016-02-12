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
  m_dbname = dbname;
  m_ok = false;
  readSettings();

  QString fields = "id,node,word,root,supplement,page,vol,timewhen";
  bool ok = openDatabase(dbname);
  if (! ok) {
    m_historyEnabled = false;
    return;
  }
  this->truncate();
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
  if (v && !m_db.isOpen()) {
      openDatabase(m_dbname);
  }
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
    return this->createDatabase();
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
    //    QLOG_DEBUG() << "History rejected, duplicate place";
    return false;
  }
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
}
/**
 * return  m_size history records
 *
 *
 * @return
 */
 QList<HistoryEvent *> HistoryMaster::getHistory(bool ignorelimit) {
  QList<HistoryEvent *> events;
  if (! m_historyEnabled ) {
      return events;
  }
  QLOG_DEBUG() << Q_FUNC_INFO;
  QSqlQuery sql(m_db);
  if (ignorelimit) {
    if (! sql.prepare(SQL_ALL_HISTORY)) {
      QLOG_WARN() << QString(QObject::tr("Prepare error SQL_ALL_HISTORY : %1")).arg(sql.lastError().text());
      return events;
    }
  }
  else {
    if (! sql.prepare(QString(SQL_LIST_HISTORY).arg(m_size))) {
      QLOG_WARN() << QString(QObject::tr("Prepare error SQL_LIST_HISTORY : %1")).arg(sql.lastError().text());
      return events;
    }
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
  m_size = settings.value(SID_HISTORY_SIZE,20).toInt();
  m_duplicateDepth = settings.value(SID_HISTORY_DUPLICATE_DEPTH,10).toInt();
  m_maximumHistory = settings.value(SID_HISTORY_MAXIMUM,50).toInt();
  m_maximumBuffer = settings.value(SID_HISTORY_MAXIMUM_BUFFER,10).toInt();
  m_schemaFile = settings.value("Schema","sql/history_schema.sql").toString();
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
int HistoryMaster::count() const {
  QSqlQuery sql(m_db);
  if (! sql.prepare(SQL_COUNT_HISTORY)) {
    QLOG_WARN() << QString(QObject::tr("Unable to prepare SQL_COUNT_HISTORY : %1")).arg(sql.lastError().text());
    return -1;
  }
  sql.exec();
  if (sql.first()) {
    return sql.value(0).toInt();
  }
  return -1;
}
int HistoryMaster::truncate(int maxrecords) {
  QSqlQuery sql(m_db);
  int x;
  if (maxrecords == -1) {
    maxrecords = m_maximumHistory;
  }
  x = this->count();

  maxrecords += m_maximumBuffer;
  int deleteCount = x - maxrecords;
  QLOG_DEBUG() << Q_FUNC_INFO << QString("Delete count:%1").arg(deleteCount);
  if (deleteCount <= 0) {
    return 0;
  }
  if (!sql.prepare(SQL_GET_FIRST_HISTORY)) {
    QLOG_WARN() << QString(QObject::tr("Unable to prepare SQL_TRUNCATE_HISTORY : %1")).arg(sql.lastError().text());
  }
  sql.bindValue(0,deleteCount);
  sql.exec();
  QList<int> deletes;
  while(sql.next()) {
    deletes << sql.value(0).toInt();
  }
  if (deletes.size() > 0) {
    if (!sql.prepare(SQL_TRUNCATE_HISTORY)) {
      QLOG_WARN() << QString(QObject::tr("Unable to prepare SQL_TRUNCATE_HISTORY : %1")).arg(sql.lastError().text());
    }
    for(int i=0;i < deletes.size();i++) {
      sql.bindValue(0,deletes[i]);
      sql.exec();
    }
    QLOG_DEBUG() << QString("truncated history delete %1, size now %2").arg(deletes.size()).arg(this->count());
  }
  return deletes.size();
}
bool HistoryMaster::createDatabase() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  QFile sqlFile(m_schemaFile);
  if (! sqlFile.exists()) {
    QLOG_WARN() << QString("Cannot create history database, schema file not found : %1").arg(m_schemaFile);
    return false;
  }
  if ( ! sqlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QLOG_WARN() << QString(QObject::tr("Error opening history schema : %1")).arg(m_schemaFile);
    return false;
  }

  m_db = QSqlDatabase::addDatabase("QSQLITE","historyb");
  m_db.setDatabaseName(m_dbname);
  if (! m_db.open()) {
    QLOG_WARN() << QString(QObject::tr("Error opening history db : %1")).arg(m_dbname);
    return false;
  }

  QTextStream in(&sqlFile);
  in.setCodec("UTF-8");
  QString sql;
  bool ok = true;
  while(! in.atEnd() && ok) {
    sql = in.readLine();
    QSqlQuery q = m_db.exec(sql);
    if (q.lastError().isValid()) {
      QLOG_INFO() << QString(QObject::tr("Error executing history schema sql: %1").arg(q.lastError().text()));
      return false;
    }

  }
  sqlFile.close();
  QLOG_INFO() << "=====================================";
  QLOG_INFO() << "History system successfully created";
  QLOG_INFO() << "=====================================";
  return true;

}
