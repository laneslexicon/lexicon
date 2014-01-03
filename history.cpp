#include "history.h"
HistoryEvent::HistoryEvent() {
  m_when = QDateTime::currentDateTime();
}
//CREATE TABLE history(id integer primary key,nodeId text,word text,root text,timewhen text);
HistoryMaster::HistoryMaster(const QString & dbname) {
  m_historyOk = openDatabase(dbname);
  if (m_historyOk) {
    m_addQuery = new QSqlQuery(m_db);
    m_forQuery = new QSqlQuery(m_db);
    m_backQuery = new QSqlQuery(m_db);
    if (m_addQuery->prepare("insert into history (nodeId,word,root,timewhen) values (?,?,?,?)")) {
      if ((m_backQuery->prepare("select * from history where id <= ? order by id desc")) &&
        (m_forQuery->prepare("select * from history where id > ? order by id asc")))
        {
        QSqlQuery mq(m_db);
        mq.exec("select max(id) from history");
        if (mq.first()) {
          m_lastId = mq.value(0).toInt();
        }
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
 * @param direciton 0 - back, 1 forward
 * @return
 */
QList<HistoryEvent *> HistoryMaster::getHistory(int count,int direction,int startPos) {
  int i = 0;
  if (startPos == -1) {
    startPos = m_lastId;
  }
  if (direction == 0) {
    QList<HistoryEvent *> events;
    m_backQuery->bindValue(0,startPos);
    m_backQuery->exec();
    while(m_backQuery->next() && (i < count)) {
      HistoryEvent * event = new HistoryEvent();
      event->setId(m_backQuery->value(0).toInt());
      event->setNode(m_backQuery->value(1).toString());
      event->setWord(m_backQuery->value(2).toString());
      event->setRoot(m_backQuery->value(3).toString());
      event->setWhen(m_backQuery->value(4).toDateTime());
      events << event;
    }
    return events;
  }
  if (direction == 1) {
    QList<HistoryEvent *> events;
    m_forQuery->bindValue(0,startPos);
    m_forQuery->exec();
    while(m_forQuery->next() && (i < count)) {
      HistoryEvent * event = new HistoryEvent();
      event->setId(m_forQuery->value(0).toInt());
      event->setNode(m_forQuery->value(1).toString());
      event->setWord(m_forQuery->value(2).toString());
      event->setRoot(m_forQuery->value(3).toString());
      event->setWhen(m_forQuery->value(4).toDateTime());
      events << event;
    }
    return events;
  }
}
