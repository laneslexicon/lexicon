#ifndef __TASK_H__
#define __TASK_H__
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <QSettings>
#include <QStringList>
#include <QJsonDocument>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <iostream>
#include <string>
#include <QDir>
#include <QDirIterator>
#include <sstream>
#include <QMap>
class Task : public QObject
{
  Q_OBJECT
public:
  Task(QCoreApplication * parent = 0) : QObject(parent) {

  }
  void set(QString m,QStringList v) {
    m_opts.insert(m,v);
  }
  QMap<QString,QStringList> m_opts;
public slots:
  void run();
signals:
  void finished();
};
#endif
