#ifndef __SEARCHWIDGET_H__
#define __SEARCHWIDGET_H__
#include <QDebug>
#include <QSplitter>
#include <QTextEdit>
#include <QSplitter>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFile>
#include <QLabel>
#include <QSettings>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextLine>
#include <QTextLayout>
#include <QTextBlock>
#include <QFont>
#include <QWidget>
#include <QRegExp>
class ImLineEdit;
class GraphicsEntry;
class SearchOptions;
class SearchWidget : public QWidget
{
    Q_OBJECT

 public:
    //    SearchWidget(const QString & str,int options,QWidget * parent = 0);
    SearchWidget(QWidget * parent = 0);
   GraphicsEntry * getEntry() { return m_text;}
   int count();
   void search(const QString &,int options);
   void regexSearch(const QString &,int options);

 public slots:
   void itemChanged(QTableWidgetItem *,QTableWidgetItem *);
   void itemDoubleClicked(QTableWidgetItem *);
   void hideOptions();
   void findTarget();
 private:
   ImLineEdit * m_findTarget;
   bool eventFilter(QObject * target,QEvent * event);
   QString buildSearchSql(int);
   QString buildRxSql(int);
   SearchOptions * m_search;
   int m_searchOptions;
   QSqlQuery m_query;
   QSqlQuery m_nodeQuery;
   QString m_target;
   QLabel * m_resultsText;
   QTableWidget * m_list;
   QTableWidget * m_rxlist;
   GraphicsEntry * m_text;
   QPushButton * m_findButton;
   QPushButton * m_hideOptionsButton;
   QStringList m_nodes;
   QStringList m_rxnodes;
/// for Arabic font from QSettings
   QFont m_resultsFont;
 signals:
   void searchResult(const QString &);
};
#endif
