#include "notes.h"

Note::Note() {
  m_id = -1;
}
Note::Note(const Note & other) {
  m_note = other.m_note;
  m_subject = other.m_subject;
  m_created = other.m_created;
  m_word = other.m_word;
  m_id = other.m_id;
  m_place = other.m_place;
}
NoteMaster::NoteMaster(QSettings * settings) {
  m_settings = settings;
  readSettings();
  delete m_settings;
  if (m_enabled)
    openDb();
}
void NoteMaster::remove(Note * n) {
  if (!m_enabled)
    return;
  deleteQuery.bindValue(0,n->getId());
  if (! deleteQuery.exec()) {
    QLOG_WARN() << "SQL delete note error" << deleteQuery.lastError().text();
  }
}
void NoteMaster::save(Note * n) {
  bool ok;
  if (!m_enabled) {
    return;
  }
  QLOG_DEBUG() << Q_FUNC_INFO <<  m_db.transaction();
  if (n->getId() == -1) {     // add note
    QSqlQuery  query(m_db);
    query.prepare("insert into notes (datasource,word,place,subject,note,created) \
           values (:datasource,:word,:place,:subject,:note,:created)");

    Place p = n->getPlace();
    qDebug() << "Add query" <<      query.isActive();
    qDebug() << "Update query" <<    updateQuery.isActive();
    qDebug() << "Delete query" <<    deleteQuery.isActive();
    qDebug() << "Find query" <<    findQuery.isActive();
    qDebug() << "FindOne query" <<    findOneQuery.isActive();
    query.bindValue(":datasource",p.getSource());
    query.bindValue(":word",n->getWord());
    query.bindValue(":place",p.toString());
    query.bindValue(":subject",n->getSubject());
    query.bindValue(":note",n->getNote());
    query.bindValue(":created",QDateTime::currentDateTime().toString());
    qint64 ms = QDateTime::currentMSecsSinceEpoch();
    ok =  query.exec();
    qDebug() << "Exec msecs" << (QDateTime::currentMSecsSinceEpoch() - ms);
    if (! ok ) {
      QLOG_WARN() << Q_FUNC_INFO << "SQL add note error" << query.lastError().text();
    }
    else {
      QLOG_DEBUG() << Q_FUNC_INFO << "Save note for place" << p.toString();
    }
  }
  else {
    updateQuery.bindValue(0,n->getSubject());
    updateQuery.bindValue(1,n->getNote());
    updateQuery.bindValue(2,QDateTime::currentDateTime().toString());
    updateQuery.bindValue(3,n->getId());
    if (! updateQuery.exec()) {
      QLOG_WARN() << "SQL update note error" << updateQuery.lastError().text();
    }
  }
  QLOG_DEBUG() << "Commit" << m_db.commit();
  return;
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
  else {
    QLOG_DEBUG() << "tables" << m_db.tables();
  }

  addQuery = QSqlQuery(m_db);
  if (! addQuery.prepare("insert into notes (datasource,word,place,subject,note,created) \
           values (:datasource,:word,:place,:subject,:note,:created)")) {
    QLOG_WARN() << Q_FUNC_INFO << "SQL add prepare error" << addQuery.lastError().text();
    m_enabled = false;
    return false;
  }
  updateQuery = QSqlQuery(m_db);
  if (! updateQuery.prepare("update notes set subject = ?, note = ?, amended = ? where id = ?")) {
    QLOG_WARN() << "SQL update prepare error" << updateQuery.lastError().text();
    m_enabled = false;
    return false;
  }
  deleteQuery = QSqlQuery(m_db);
  if (! deleteQuery.prepare("delete from  notes  where id = ?")) {
    QLOG_WARN() << "SQL delete error" << deleteQuery.lastError().text();
    m_enabled = false;
    return false;
  }
  findQuery = QSqlQuery(m_db);
  if (! findQuery.prepare("select id,word,place,subject,note,created,amended from notes where word = ?")) {
    QLOG_WARN() << "SQL find error" << findQuery.lastError().text();
    m_enabled = false;
    return false;
  }
  findOneQuery = QSqlQuery(m_db);
  if (! findOneQuery.prepare("select word,place,subject,note,created,amended from notes where id = ?")) {
    QLOG_WARN() << "SQL find error" << findOneQuery.lastError().text();
    m_enabled = false;
    return false;
  }
  QLOG_DEBUG() << "=====================================";
  QLOG_DEBUG() << "Notes system successfully initialised";
  QLOG_DEBUG() << "=====================================";
  return true;
}
/**
 * It is the responsibility of the calling routine
 * to delete the notes
 * @param word
 *
 * @return a list of notes for the given word
 */
QList<Note *> NoteMaster::find(const QString & word) {
  QList<Note *> notes;

  if (!m_enabled) {
    return notes;
  }
  QLOG_DEBUG() << Q_FUNC_INFO;
  findQuery.bindValue(0,word);
  findQuery.exec();
  while(findQuery.next()) {
    Note * n = new Note();
    n->setId(findQuery.value(0).toInt());
    n->setWord(findQuery.value(1).toString());
    Place p = Place::fromString(findQuery.value(2).toString());
    n->setPlace(p);
    n->setSubject(findQuery.value(3).toString());
    n->setNote(findQuery.value(4).toString());
    n->setWhen(findQuery.value(5).toString());
    notes << n;
  }
  findQuery.finish();
  if (notes.size() > 0) {
    QLOG_DEBUG() << Q_FUNC_INFO << word << "find count" << notes.size();
  }
  QLOG_DEBUG() << "find query active" << findQuery.isActive();
  return notes;
}
Note * NoteMaster::findOne(int id) {
  Note * note = NULL;
  if (!m_enabled) {
    return note;
  }
  QLOG_DEBUG() << Q_FUNC_INFO;

  findOneQuery.bindValue(0,id);
  if (findOneQuery.exec()) {
    findOneQuery.first();
    note = new Note();
    note->setWord(findOneQuery.value(0).toString());
    Place p = Place::fromString(findOneQuery.value(1).toString());
    note->setPlace(p);
    note->setSubject(findOneQuery.value(2).toString());
    note->setNote(findOneQuery.value(3).toString());
    note->setWhen(findOneQuery.value(4).toString());
    note->setAmended(findOneQuery.value(5).toString());
  }
  findOneQuery.finish();
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

}
// "select id,word,subject,create from notes"
QSqlQuery NoteMaster::getNoteList(const QString & sql) {
  QLOG_DEBUG() << Q_FUNC_INFO;
  QSqlQuery listQuery = QSqlQuery(m_db);
  if (!listQuery.prepare(sql)) {
    QLOG_WARN() << "SQL list error" << listQuery.lastError().text();
  }
  else {
    listQuery.exec();
  }
  return listQuery;
}
QList<int> NoteMaster::deleteNotes(QList<int> ids) {
  QList<int> deletedNotes;
  QLOG_DEBUG() << Q_FUNC_INFO << ids;
  for(int i=0;i < ids.size();i++) {
    deleteQuery.bindValue(0,ids[i]);
    if (deleteQuery.exec()) {
      deletedNotes << ids[i];
    }
  }
  return deletedNotes;
}
void NoteMaster::close() {
  m_db.close();
}
