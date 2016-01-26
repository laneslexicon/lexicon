#ifndef __HISTORY_H__
#define __HISTORY_H__
#include <QtWidgets>
#include <QSettings>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QDebug>
#include "place.h"
#include "QsLog.h"
#include "application.h"
class HistoryEvent {
 public:
  HistoryEvent();
  void setPlace(const Place & p) { m_place = p;}
  void setWhen(const QDateTime & word) { m_when = word;}
  void setId(const int i) { m_id = i;}
  QString getWord() { return m_place.getWord();}
  QString getRoot() { return m_place.getRoot();}
  QString getNode() { return m_place.getNode();}
  Place getPlace() { return m_place;}
  QDateTime getWhen() { return m_when;}
  int getId() { return m_id;}
  bool isValid() { return m_id == -1;}
  bool matches(HistoryEvent *);
 private:
  int m_id;
  Place m_place;
  QDateTime m_when;
};

class HistoryMaster {
 public:
  HistoryMaster(const QString & dbname);
  ~HistoryMaster();
  void readSettings();
  HistoryEvent * getEvent(int id) ;
  int hasPlace(const Place & p, int historyDepth = -1);
  int getNewestId();
  int getOldestId();
  int count() const;
  int truncate(int max = -1);
  Place getPlaceById(int);
  void setEnabled(bool v);
  bool add(const Place &);
  bool enabled() const { return m_historyEnabled;}
  void on();
  void off();
  bool isOn() { return m_historyOn;}
  bool isOk() { return m_ok;}
  bool clear();
  QList<HistoryEvent *> getHistory();
 private:
  QSqlDatabase m_db;
  int m_maximumHistory;
  int m_maximumBuffer;
  int m_duplicateDepth;
  int m_size;
  int m_lastId;
  int m_firstId;
  bool m_ok;
  bool openDatabase(const QString & dbname);
  bool m_historyOn;
  bool m_historyEnabled;

};
extern HistoryMaster * getHistory();
#endif
