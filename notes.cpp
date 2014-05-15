#include "notes.h"
#include "laneslexicon.h"
extern LanesLexicon * getApp();
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
NoteMaster::NoteMaster() {
  readSettings();
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
  if (!m_enabled)
    return;
  if (n->getId() == -1) {     // add note
    Place p = n->getPlace();
    addQuery.bindValue(":datasource",p.getSource());
    addQuery.bindValue(":word",n->getWord());
    addQuery.bindValue(":place",p.toString());
    addQuery.bindValue(":subject",n->getSubject());
    addQuery.bindValue(":note",n->getNote());
    addQuery.bindValue(":created",QDateTime::currentDateTime().toString());
    if (! addQuery.exec()) {
      QLOG_WARN() << "SQL add note error" << addQuery.lastError().text();
    }
    return;
  }
  updateQuery.bindValue(0,n->getSubject());
  updateQuery.bindValue(1,n->getNote());
  updateQuery.bindValue(2,QDateTime::currentDateTime().toString());
  updateQuery.bindValue(3,n->getId());
  if (! updateQuery.exec()) {
    QLOG_WARN() << "SQL update note error" << updateQuery.lastError().text();
  }
  return;
}
bool NoteMaster::openDb() {
  qDebug() << Q_FUNC_INFO;
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
    QMessageBox::critical(0,QObject::tr("Database Error"),
                          m_db.lastError().text());
    m_enabled = false;
    return false;
  }
  else {
    qDebug() << "tables" << m_db.tables();
  }

  addQuery = QSqlQuery(m_db);
  if (! addQuery.prepare("insert into notes (datasource,word,place,subject,note,created) \
           values (:datasource,:word,:place,:subject,:note,:created)")) {
    QLOG_WARN() << "SQL add prepare error" << addQuery.lastError().text();
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
  qDebug() << "=====================================";
  qDebug() << "Notes system successfully initialised";
  qDebug() << "=====================================";
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
  if (!m_enabled)
    return notes;
  findQuery.bindValue(0,word);
  findQuery.exec();
  while(findQuery.next()) {
    Note * n = new Note();
    n->setId(findQuery.value(0).toInt());
    n->setWord(findQuery.value(1).toString());
    Place p = Place::fromString(findQuery.value(2).toString());
    qDebug() << "Add note" << p.getNode();
    n->setPlace(p);
    n->setSubject(findQuery.value(3).toString());
    n->setNote(findQuery.value(4).toString());
    n->setWhen(findQuery.value(5).toString());
    notes << n;
  }
  if (notes.size() > 0)
    qDebug() << Q_FUNC_INFO << word << "find count" << notes.size();
  return notes;
}
Note * NoteMaster::findOne(int id) {
  Note * note = NULL;
  if (!m_enabled)
    return note;

  findOneQuery.bindValue(0,id);
  if (findOneQuery.exec()) {
    findOneQuery.first();
    note = new Note();
    note->setWord(findOneQuery.value(0).toString());
    Place p = Place::fromString(findOneQuery.value(1).toString());
    qDebug() << "Add note" << p.getNode();
    note->setPlace(p);
    note->setSubject(findOneQuery.value(2).toString());
    note->setNote(findOneQuery.value(3).toString());
    note->setWhen(findOneQuery.value(4).toString());
    note->setAmended(findOneQuery.value(5).toString());
  }
  return note;
}
void NoteMaster::readSettings() {
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  settings->setIniCodec("UTF-8");
  settings->beginGroup("Notes");
  m_dbName = settings->value("Database","notes.sqlite").toString();
  m_autosave = settings->value("Autosave",true).toBool();
  m_enabled = settings->value("Enabled",true).toBool();

}
// "select id,word,subject,create from notes"
QSqlQuery NoteMaster::getNoteList(const QString & sql) {
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
  qDebug() << Q_FUNC_INFO << ids;
  for(int i=0;i < ids.size();i++) {
    deleteQuery.bindValue(0,ids[i]);
    if (deleteQuery.exec()) {
      deletedNotes << ids[i];
    }
  }
  return deletedNotes;
}
