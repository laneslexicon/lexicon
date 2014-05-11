#include "notes.h"
#include "laneslexicon.h"
extern LanesLexicon * getApp();
Note::Note() {
  m_id = -1;
}
NoteMaster::NoteMaster() {
  readSettings();
  openDb();
}
void NoteMaster::remove(Note * n) {
  deleteQuery.bindValue(0,n->getId());
  if (! deleteQuery.exec()) {
    QLOG_WARN() << "SQL delete note error" << deleteQuery.lastError().text();
  }
}
void NoteMaster::save(Note * n) {
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
  updateQuery.bindValue(2,n->getId());
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
  if (! updateQuery.prepare("update notes set subject = ?, note = ? where id = ?")) {
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
  if (! findQuery.prepare("select id,word,place,subject,note,created from notes where word = ?")) {
    QLOG_WARN() << "SQL find error" << findQuery.lastError().text();
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
  findQuery.bindValue(0,word);
  findQuery.exec();
  QList<Note *> notes;
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
void NoteMaster::readSettings() {
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  settings->setIniCodec("UTF-8");
  settings->beginGroup("Notes");
  m_dbName = settings->value("Database","notes.sqlite").toString();
  m_autosave = settings->value("Autosave",true).toBool();
  m_enabled = settings->value("Enabled",true).toBool();

}
