#ifndef __NOTES_H__
#define __NOTES_H__
#include <QDebug>
#include <QSettings>
//#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include "QsLog.h"
#include "place.h"

class Note  {
  enum Type { User, Error };
 public:
  Note(int type = Note::User);
  Note(const Note &);


  void setSubject(const QString & text) { m_subject = text;};
  void setPlace(const Place & p) { m_place = p;}
  void setNote(const QString & n) { m_note = n;}
  void setWord(const QString & n) { m_word = n;}
  void setId(int id) { m_id = id;}
  void setWhen(const QString & t) { m_created = t;}
  void setAmended(const QString & t) { m_amended = t;}

  QString getSubject() { return m_subject; }
  QString getNote() { return m_note; }
  QString getWord() { return m_word;}
  Place getPlace() { return m_place; }
  int getId() { return m_id;}
 private:
  QString m_note;
  QString m_subject;
  Place m_place;
  QString m_created;
  QString m_amended;
  QString m_word;
  int m_id;
  int m_type;
};
class NoteMaster {
 public:
  NoteMaster(QSettings * settings = 0);
  bool openDb();
  bool save(Note *);
  void close();
  QList<int> deleteNotes(QList<int>);
  bool remove(Note *);
  bool deleteById(int);
  bool autosave() { return m_autosave;}
  bool enabled() { return m_enabled; }
  QList<Note *> find(const QString & word);
  Note * findOne(int);
  QSqlQuery * getNoteList(const QString & sql);
 private:
  QSettings * m_settings;
  QSqlDatabase m_db;
  QSqlQuery * addQuery;
  QSqlQuery * updateQuery;
  QSqlQuery * deleteQuery;
  QSqlQuery * findQuery;
  QSqlQuery * findOneQuery;
  QString m_dbName;
  bool m_autosave;
  bool m_enabled;
  void readSettings();
};
#endif
