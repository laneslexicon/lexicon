#ifndef __SEARCHRESULTS_H__
#define __SEARCHRESULTS_H__
#include <QTableWidget>
#include <QGridLayout>
#include <QFormLayout>
#include <QDebug>
#include <QSplitter>
#include <QLineEdit>
#include <QTextEdit>
#include <QSplitter>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFile>
#include <QLabel>
#include <QDialog>
#include <QKeySequenceEdit>
#include <QSettings>
#include <QDateTime>
#include <QShortcut>
#include <QSignalMapper>
#include <QMessageBox>
#include <QListWidget>
#include <QListWidgetItem>
#include "graphicsentry.h"
class SearchResultsWidget : public QWidget {
  Q_OBJECT

 public:
  SearchResultsWidget(const QString & str, QWidget * parent = 0);
 public slots:
 void itemChanged(QListWidgetItem *,QListWidgetItem *);

 private:
  QSqlQuery m_query;
QSqlQuery m_nodeQuery;
  QString m_target;
  QListWidget * m_list;
  GraphicsEntry * m_text;
};
#endif
