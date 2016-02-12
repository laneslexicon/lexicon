#include "notes.h"
#include "definedsql.h"
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
void Note::setType(int n) {
  m_type = n;
}
int Note::getType() const {
  return m_type;
}
QString Note::getSubject() const {
  return m_subject;
}
QString Note::getWord() const {
  return m_word;
}
QString Note::getNote() const {
  return m_note;
}
Place Note::getPlace() const {
  return m_place;
}
int Note::getId() const {
  return m_id;
}

NoteMaster::NoteMaster(QSettings * settings) {
  QLOG_DEBUG() << Q_FUNC_INFO;
  m_settings = settings;
  readSettings();
  if (m_enabled)
    openDb();
}
NoteMaster::~NoteMaster() {
  delete m_settings;
}
bool NoteMaster::openDb() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  if ( ! m_enabled )
    return false;
  if ( ! m_db.isOpen()) {
    QFile dbfile(m_dbName);
    if (! dbfile.exists()) {
      m_enabled = this->createDatabase();
      return m_enabled;
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
    ok = query.prepare(SQL_INSERT_NOTE);
    if (!ok) {
      QLOG_WARN() << QString(QObject::tr("SQL prepare error on save note:%1")).arg(query.lastError().text());
      return false;
    }
    Place p = n->getPlace();
    query.bindValue(":datasource",p.getSource());
    query.bindValue(":word",n->getWord());
    query.bindValue(":place",p.toString());
    query.bindValue(":subject",n->getSubject());
    query.bindValue(":note",n->getNote());
    query.bindValue(":type",n->getType());
    query.bindValue(":created",QDateTime::currentDateTime().toString());
    query.bindValue(":node",p.getNode());
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
    ok =  query.prepare(SQL_UPDATE_NOTE);
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
  QSqlQuery query(m_db);
  ok = query.prepare(SQL_GET_NOTES_FOR_WORD);
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
    n->setType(query.value(6).toInt());
    notes << n;
  }
  //  if (notes.size() > 0) {
  //    QLOG_DEBUG() << Q_FUNC_INFO << word << "find count" << notes.size();
  //  }
  return notes;
}
QList<Note *> NoteMaster::findByNode(const QString & node) {
  bool ok;
  QList<Note *> notes;

  if (!m_enabled) {
    return notes;
  }
  QSqlQuery query(m_db);
  ok = query.prepare(SQL_GET_NOTES_FOR_NODE);
  if (!ok) {
    QLOG_WARN() << QString(QObject::tr("SQL prepare error for notes by node:%1")).arg(query.lastError().text());

    return notes;
  }
  query.bindValue(0,node);
  ok = query.exec();
  if (!ok) {
    QLOG_WARN() << QString(QObject::tr("SQL find error for notes by node:%1")).arg(query.lastError().text());
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
    n->setType(query.value(6).toInt());
    notes << n;
  }
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
  ok = query.prepare(SQL_GET_NOTE_BY_ID);
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
  note->setId(id);
  note->setWord(query.value(0).toString());
  Place p = Place::fromString(query.value(1).toString());
  note->setPlace(p);
  note->setSubject(query.value(2).toString());
  note->setNote(query.value(3).toString());
  note->setWhen(query.value(4).toString());
  note->setAmended(query.value(5).toString());
  note->setType(query.value(6).toInt());
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
  m_schemaFile = m_settings->value("Schema","sql/notes_schema.sql").toString();
  m_settings->endGroup();
}
/**
 * select id,word,subject,create from notes
 * this is used by notebrowser which deletes the query
 *
 *
 * @param sql
 *
 * @return
 */
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
bool NoteMaster::createDatabase() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  QFile sqlFile(m_schemaFile);
  if (! sqlFile.exists()) {
    QLOG_WARN() << QString("Cannot create notes database, schema file not found : %1").arg(m_schemaFile);
    return false;
  }
  if ( ! sqlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QLOG_WARN() << QString(QObject::tr("Error opening notes schema : %1")).arg(m_schemaFile);
    return false;
  }

  m_db = QSqlDatabase::addDatabase("QSQLITE","notesdb");
  m_db.setDatabaseName(m_dbName);
  if (! m_db.open()) {
    QLOG_WARN() << QString(QObject::tr("Error opening notes db : %1")).arg(m_dbName);
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
      QLOG_INFO() << QString(QObject::tr("Error executing notes schema sql: %1").arg(q.lastError().text()));
      return false;
    }

  }
  sqlFile.close();
  QLOG_INFO() << "=====================================";
  QLOG_INFO() << "Notes system successfully created";
  QLOG_INFO() << "=====================================";
  return true;

}
