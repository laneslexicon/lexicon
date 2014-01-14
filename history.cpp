#include "history.h"
HistoryEvent::HistoryEvent() {
  m_when = QDateTime::currentDateTime();
  m_id = -1;
}
bool HistoryEvent::matches(HistoryEvent * event) {
  return ((m_word == event->getWord()) &&
          (m_root == event->getRoot()) &&
          (m_node == event->getNode()));

}
//CREATE TABLE history(id integer primary key,nodeId text,word text,root text,timewhen text);
HistoryMaster::HistoryMaster(const QString & dbname) {
  bool ok = openDatabase(dbname);
  if (ok) {
    m_getQuery = new QSqlQuery(m_db);
    m_addQuery = new QSqlQuery(m_db);
    m_forQuery = new QSqlQuery(m_db);
    m_backQuery = new QSqlQuery(m_db);
    if (
        (m_addQuery->prepare("insert into history (nodeId,word,root,timewhen) values (?,?,?,?)")) &&
        (m_backQuery->prepare("select * from history where id <= ? order by id desc")) &&
        (m_forQuery->prepare("select * from history where id > ? order by id asc")) &&
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
  m_db.close();
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
bool HistoryMaster::add(HistoryEvent * event) {
  if (! m_historyEnabled ) {
    return false;
  }
  QString root = event->getRoot();
  QString word = event->getWord();
  if (! (root.isEmpty() && word.isEmpty()) ) {
    m_addQuery->bindValue(0,event->getNode());
    m_addQuery->bindValue(1,word);
    m_addQuery->bindValue(2,root);
    m_addQuery->bindValue(3,event->getWhen());
    return m_addQuery->exec();
  }
  QLOG_DEBUG() << "Attempt to add empty history";
  return false;
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
      event->setNode(m_backQuery->value(1).toString());
      event->setWord(m_backQuery->value(2).toString());
      event->setRoot(m_backQuery->value(3).toString());
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
      event->setNode(m_forQuery->value(1).toString());
      event->setWord(m_forQuery->value(2).toString());
      event->setRoot(m_forQuery->value(3).toString());
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
       event->setNode(m_getQuery->value(1).toString());
       event->setWord(m_getQuery->value(2).toString());
       event->setRoot(m_getQuery->value(3).toString());
       event->setWhen(m_getQuery->value(4).toDateTime());
   }
   return event;
}
