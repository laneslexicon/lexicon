#ifndef __NOTES_H__
#define __NOTES_H__
#include <QDebug>
#include <QSettings>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "QsLog.h"
#include "place.h"

class Note  {
 public:
  Note();
  Note(const Note &);
  void setSubject(const QString & text) { m_subject = text;};
  void setPlace(const Place & p) { m_place = p;}
  void setNote(const QString & n) { m_note = n;}
  void setWord(const QString & n) { m_word = n;}
  void setId(int id) { m_id = id;}
  void setWhen(const QString & t) { m_created = t;}

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
  QString m_word;
  int m_id;
};
class NoteMaster {
 public:
  NoteMaster();
  bool openDb();
  void save(Note *);
  void remove(Note *);
  bool autosave() { return m_autosave;}
  bool enabled() { return m_enabled; }
  QList<Note *> find(const QString & word);
 private:
  QSqlDatabase m_db;
  QSqlQuery addQuery;
  QSqlQuery updateQuery;
  QSqlQuery deleteQuery;
  QSqlQuery findQuery;
  QString m_dbName;
  bool m_autosave;
  bool m_enabled;
  void readSettings();
};
#endif
