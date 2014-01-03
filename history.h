#ifndef __HISTORY_H__
#define __HISTORY_H__
#include <QtWidgets>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "QsLog.h"

class HistoryEvent {
 public:
  HistoryEvent();
  void setWord(const QString & word) { m_word = word;}
  void setRoot(const QString & word) { m_root = word;}
  void setNode(const QString & word) { m_node = word;}
  void setWhen(const QDateTime & word) { m_when = word;}
  void setId(const int i) { m_id = i;}
  QString getWord() { return m_word;}
  QString getRoot() { return m_root;}
  QString getNode() { return m_node;}
  QDateTime getWhen() { return m_when;}
  int getId() { return m_id;}
 private:
  int m_id;
  QString m_word;
  QString m_root;
  QString m_node;
  QDateTime m_when;
};

class HistoryMaster {
 public:
  HistoryMaster(const QString & dbname);
  //  bool add(const QString & root,const QString & word, const QString & node);
  bool add(HistoryEvent *);
  void on() { m_historyOn = true;}
  void off() { m_historyOn = false;}
  bool isOn() { return m_historyOn;}
  QList<HistoryEvent *> getHistory(int howMany);
 private:
  bool openDatabase(const QString & dbname);
  bool m_historyOn;
  QSqlDatabase m_db;
  bool m_historyOk;
  QSqlQuery * m_addQuery;
  QSqlQuery * m_getQuery;
};
extern HistoryMaster * getHistory();
#endif
