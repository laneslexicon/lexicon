#include "notes.h"

Note::Note(int type) {
  m_id = -1;
  m_type = type;
}
Note::Note(const Note & other) {
  m_note = other.m_note;
  m_subject = other.m_subject;
  m_created = other.m_created;
  m_word = other.m_word;
  m_id = other.m_id;
  m_place = other.m_place;
  m_type = other.m_type;
}
NoteMaster::NoteMaster(QSettings * settings) {
  QLOG_DEBUG() << Q_FUNC_INFO << settings;
  m_settings = settings;
  readSettings();
  if (m_enabled)
    openDb();
}
bool NoteMaster::openDb() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  if ( ! m_enabled )
    return false;
  if ( ! m_db.isOpen()) {
    QFile dbfile(m_dbName);
    if (! dbfile.exists()) {
      /// TODO maybe create database
      m_enabled = false;
      QLOG_WARN() << "Cannot find notes database" << m_dbName;
      return false;
    }
  }
  else {
    return true;
  }
  m_db = QSqlDatabase::addDatabase("QSQLITE","notesdb");
  m_db.setDatabaseName(m_dbName);
  if (! m_db.open()) {
    /*
    QMessageBox::critical(0,QObject::tr("Database Error"),
                          m_db.lastError().text());
    */
    m_enabled = false;
    return false;
  }
  //  else {
  //    QLOG_DEBUG() << "tables" << m_db.tables();
  //  }
  /*
  addQuery = new QSqlQuery(m_db);
  if (! addQuery->prepare("insert into notes (datasource,word,place,subject,note,created) \
           values (:datasource,:word,:place,:subject,:note,:created)")) {
    QLOG_WARN() << Q_FUNC_INFO << "SQL add prepare error" << addQuery->lastError().text();
    m_enabled = false;
    return false;
  }
  updateQuery = new QSqlQuery(m_db);
  if (! updateQuery->prepare("update notes set subject = ?, note = ?, amended = ? where id = ?")) {
    QLOG_WARN() << "SQL update prepare error" << updateQuery->lastError().text();
    m_enabled = false;
    return false;
  }
  deleteQuery = new QSqlQuery(m_db);
  if (! deleteQuery->prepare("delete from  notes  where id = ?")) {
    QLOG_WARN() << "SQL delete error" << deleteQuery->lastError().text();
    m_enabled = false;
    return false;
  }
  findQuery = new QSqlQuery(m_db);
  if (! findQuery->prepare("select id,word,place,subject,note,created,amended from notes where word = ?")) {
    QLOG_WARN() << "SQL find error" << findQuery->lastError().text();
    m_enabled = false;
    return false;
  }
  findOneQuery = new QSqlQuery(m_db);
  if (! findOneQuery->prepare("select word,place,subject,note,created,amended from notes where id = ?")) {
    QLOG_WARN() << "SQL find error" << findOneQuery->lastError().text();
    m_enabled = false;
    return false;
  }
  */
  QLOG_DEBUG() << "=====================================";
  QLOG_DEBUG() << "Notes system successfully initialised";
  QLOG_DEBUG() << "=====================================";
  return true;
}
bool NoteMaster::remove(Note * n) {
  bool ok;
  if (!m_enabled)
    return true;

  QSqlQuery query(m_db);
  ok = query.prepare("delete from  notes  where id = ?");
  if (!ok) {
    QLOG_WARN() << "SQL delete error" << query.lastError().text();
    return false;
  }
  query.bindValue(0,n->getId());
  QLOG_DEBUG() << "Attempting delete of id" << n->getId();
  ok = m_db.transaction();
  if (!ok) {
    QLOG_WARN() << Q_FUNC_INFO << m_db.lastError().text();
    return false;
  }
  ok = query.exec();
  if (!ok) {
    QLOG_WARN() << "SQL delete note error" << query.lastError().text();
    return false;
  }

  ok = m_db.commit();
  if (!ok) {
    QLOG_WARN() << Q_FUNC_INFO << m_db.lastError().text();
  }
  else {
    QLOG_DEBUG() << "Deleted note";
  }
  return ok;
}
bool NoteMaster::deleteById(int id) {
  bool ok;
  if (!m_enabled)
    return true;

  QSqlQuery query(m_db);
  ok = query.prepare("delete from  notes  where id = ?");
  if (!ok) {
    QLOG_WARN() << "SQL delete error" << query.lastError().text();
    return false;
  }
  query.bindValue(0,id);
  ok = m_db.transaction();
  if (!ok) {
    QLOG_WARN() << Q_FUNC_INFO << m_db.lastError().text();
    return false;
  }
  ok = query.exec();
  if (!ok) {
    QLOG_WARN() << "SQL delete note error" << query.lastError().text();
    return false;
  }

  ok = m_db.commit();
  if (!ok) {
    QLOG_WARN() << Q_FUNC_INFO << m_db.lastError().text();
  }
  else {
    QLOG_DEBUG() << "Deleted note" << id;
  }
  return ok;
}
bool NoteMaster::save(Note * n) {
  QLOG_DEBUG() << Q_FUNC_INFO << m_enabled;
  bool ok;
  if (!m_enabled) {
    return false;
  }
  if (n->getId() == -1) {     // add note
    QSqlQuery  query(m_db);
    ok = query.prepare("insert into notes (datasource,word,place,subject,note,created) \
           values (:datasource,:word,:place,:subject,:note,:created)");
    if (!ok) {
      QLOG_WARN() << "Save prepare note error" << query.lastError().text();
      return false;
    }
    Place p = n->getPlace();
    query.bindValue(":datasource",p.getSource());
    query.bindValue(":word",n->getWord());
    query.bindValue(":place",p.toString());
    query.bindValue(":subject",n->getSubject());
    query.bindValue(":note",n->getNote());
    query.bindValue(":created",QDateTime::currentDateTime().toString());
    ok =  m_db.transaction();
    if (!ok) {
      QLOG_WARN() << "begin transaction failed" << m_db.lastError().text();
      return false;
    }
    QLOG_DEBUG() << Q_FUNC_INFO <<  "transaction begin" << ok;
    ok =  query.exec();
    if (! ok ) {
      QLOG_WARN() << Q_FUNC_INFO << "SQL add note error" << query.lastError().text();
    }
    else {
      QLOG_DEBUG() << Q_FUNC_INFO << "Saved note for place" << p.toString();
    }
    ok = m_db.commit();
    if (!ok) {
      QLOG_WARN() << "Commit failed" << m_db.lastError().text();
    }
    return ok;
  }
  else {
    QSqlQuery query(m_db);
    ok =  query.prepare("update notes set subject = ?, note = ?, amended = ? where id = ?");
    if (!ok) {
      QLOG_WARN() << "SQL update prepare error" << query.lastError().text();
      return false;
    }
    query.bindValue(0,n->getSubject());
    query.bindValue(1,n->getNote());
    query.bindValue(2,QDateTime::currentDateTime().toString());
    query.bindValue(3,n->getId());
    ok =  m_db.transaction();
    if (!ok) {
      QLOG_WARN() << "note update begin transaction failed" << m_db.lastError().text();
      return false;
    }
    if (! query.exec()) {
      QLOG_WARN() << "SQL update note error" << query.lastError().text();
    }
    ok = m_db.commit();
    if (!ok) {
      QLOG_WARN() << "Commit failed" << m_db.lastError().text();
    }
    return ok;
  }
  return ok;
}
/**
 * It is the responsibility of the calling routine
 * to delete the notes
 * @param word
 *
 * @return a list of notes for the given word
 */
QList<Note *> NoteMaster::find(const QString & word) {
  bool ok;
  QList<Note *> notes;

  if (!m_enabled) {
    return notes;
  }
  QLOG_DEBUG() << Q_FUNC_INFO;
  QSqlQuery query(m_db);
  ok = query.prepare("select id,word,place,subject,note,created,amended from notes where word = ?");
  if (!ok) {
    QLOG_WARN() << "SQL find error" << query.lastError().text();
    return notes;
  }
  query.bindValue(0,word);
  ok = query.exec();
  if (!ok) {
    QLOG_WARN() << "SQL find error" << query.lastError().text();
    return notes;
  }
  while(query.next()) {
    Note * n = new Note();
    n->setId(query.value(0).toInt());
    n->setWord(query.value(1).toString());
    Place p = Place::fromString(query.value(2).toString());
    n->setPlace(p);
    n->setSubject(query.value(3).toString());
    n->setNote(query.value(4).toString());
    n->setWhen(query.value(5).toString());
    notes << n;
  }
  //  if (notes.size() > 0) {
  //    QLOG_DEBUG() << Q_FUNC_INFO << word << "find count" << notes.size();
  //  }
  return notes;
}
Note * NoteMaster::findOne(int id) {
  Note * note = NULL;
  bool ok;
  if (!m_enabled) {
    return note;
  }
  QLOG_DEBUG() << Q_FUNC_INFO;
  QSqlQuery query(m_db);
  ok = query.prepare("select word,place,subject,note,created,amended from notes where id = ?");
  if (!ok) {
    QLOG_WARN() << "SQL findOne error" << query.lastError().text();
    return note;
  }
  query.bindValue(0,id);
  ok = query.exec();
  if (!ok) {
    QLOG_WARN() << "SQL findOne error" << query.lastError().text();
    return note;
  }
  query.first();
  note = new Note();
  note->setWord(query.value(0).toString());
  Place p = Place::fromString(query.value(1).toString());
  note->setPlace(p);
  note->setSubject(query.value(2).toString());
  note->setNote(query.value(3).toString());
  note->setWhen(query.value(4).toString());
  note->setAmended(query.value(5).toString());
  return note;
}
void NoteMaster::readSettings() {
  if (m_settings == 0) {
    m_settings = new QSettings("default.ini",QSettings::IniFormat);
  }
  m_settings->setIniCodec("UTF-8");
  m_settings->beginGroup("Notes");
  m_dbName = m_settings->value("Database","notes.sqlite").toString();
  m_autosave = m_settings->value("Autosave",true).toBool();
  m_enabled = m_settings->value("Enabled",true).toBool();
  m_settings->endGroup();
}
// "select id,word,subject,create from notes"
/// this is used by notebrowser
QSqlQuery * NoteMaster::getNoteList(const QString & sql) {
  QLOG_DEBUG() << Q_FUNC_INFO;
  QSqlQuery * listQuery = new QSqlQuery(m_db);
  if (!listQuery->prepare(sql)) {
    QLOG_WARN() << "SQL list error" << listQuery->lastError().text();
  }
  else {
    listQuery->exec();
  }
  return listQuery;
}
QList<int> NoteMaster::deleteNotes(QList<int> ids) {
  QList<int> deletedNotes;
  QSqlQuery  query(m_db);
  bool ok = query.prepare("delete from  notes  where id = ?");
  if (!ok) {
    QLOG_WARN() << "SQL delete error" << query.lastError().text();
    return deletedNotes;
  }
  //  query->bindValue(0,n->getId());
  //  ok = query->exec();
  QLOG_DEBUG() << Q_FUNC_INFO << ids;
  for(int i=0;i < ids.size();i++) {
    query.bindValue(0,ids[i]);
    if (query.exec()) {
      deletedNotes << ids[i];
    }
  }
  return deletedNotes;
}
void NoteMaster::close() {
  m_db.close();
}
