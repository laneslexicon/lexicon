#ifndef __HISTORY_H__
#define __HISTORY_H__
#include <QtWidgets>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "place.h"
#include "QsLog.h"

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
  //  bool add(const QString & root,const QString & word, const QString & node);
  HistoryEvent * getEvent(int id);
  Place getLastPlace();
  void setEnabled(bool v) { m_historyEnabled = v;}
  bool add(const Place &);
  void on() { m_historyOn = true;}
  void off() { m_historyOn = false;}
  bool isOn() { return m_historyOn;}
  QList<HistoryEvent *> getHistory(int howMany,int direction,int startPos = -1);
 private:
  QSqlDatabase m_db;

  int m_lastId;
  bool openDatabase(const QString & dbname);
  /// this is turned on/off depending on user actions
  bool m_historyOn;
    /// this disables history altogether
  bool m_historyEnabled;
  QSqlQuery * m_getQuery;
  QSqlQuery * m_addQuery;
  QSqlQuery * m_backQuery;
  QSqlQuery * m_forQuery;
  QSqlQuery * m_lastQuery;
};
extern HistoryMaster * getHistory();
#endif
