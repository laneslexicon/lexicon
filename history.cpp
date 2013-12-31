#include "history.h"
HistoryEvent::HistoryEvent() {
  m_when = QDateTime::currentDateTime();
}

HistoryMaster::HistoryMaster(const QString & dbname) {
  m_historyOk = openDatabase(dbname);
  if (m_historyOk) {
    m_addQuery = new QSqlQuery(m_db);
    if (m_addQuery->prepare("insert into history (nodeId,word,root,timewhen) values (?,?,?,?)")) {
      m_historyOn = true;
    }
    else {
      qDebug() << "whoops";
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
