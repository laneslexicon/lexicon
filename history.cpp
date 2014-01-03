#include "history.h"
HistoryEvent::HistoryEvent() {
  m_when = QDateTime::currentDateTime();
}
//CREATE TABLE history(id integer primary key,nodeId text,word text,root text,timewhen text);
HistoryMaster::HistoryMaster(const QString & dbname) {
  m_historyOk = openDatabase(dbname);
  if (m_historyOk) {
    m_addQuery = new QSqlQuery(m_db);
    m_getQuery = new QSqlQuery(m_db);
    if (m_addQuery->prepare("insert into history (nodeId,word,root,timewhen) values (?,?,?,?)")) {
      if (m_getQuery->prepare("select * from history order by timewhen desc")) {
        m_historyOn = true;
      }
    }
    else {
      QLOG_WARN() << "History not available" << m_db.lastError().text();
    }
  }
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

  QLOG_INFO() << "Opened database" << dbname;
  return ok;
}
bool HistoryMaster::add(HistoryEvent * event) {
  m_addQuery->bindValue(0,event->getNode());
  m_addQuery->bindValue(1,event->getWord());
  m_addQuery->bindValue(2,event->getRoot());
  m_addQuery->bindValue(3,event->getWhen());
  return m_addQuery->exec();
}
/**
 * The calling function should delete the events
 *
 * @param count
 *
 * @return
 */
QList<HistoryEvent *> HistoryMaster::getHistory(int count) {
  int i = 0;
  QList<HistoryEvent *> events;
  m_getQuery->exec();
  while(m_getQuery->next() && (i < count)) {
    HistoryEvent * event = new HistoryEvent();
    event->setId(m_getQuery->value(0).toInt());
    event->setNode(m_getQuery->value(1).toString());
    event->setWord(m_getQuery->value(2).toString());
    event->setRoot(m_getQuery->value(3).toString());
    event->setWhen(m_getQuery->value(4).toDateTime());
    events << event;
  }
  return events;
}
