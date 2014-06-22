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
#include <QProgressBar>
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
   void readSettings();
   void setMaxRecords();
   int m_maxRecordCount;
   QProgressBar * m_progress;
   ImLineEdit * m_findTarget;
   bool eventFilter(QObject * target,QEvent * event);
   QStringList m_fragments;
   QList<int> m_positions;
   void addRow(const QString &,const QString &,const QString &,const QString &,int);
   void getTextFragments(QTextDocument * doc,const QString & target,int options);
   QString buildSearchSql(int);
   QString buildRxSql(int);
   QTextDocument * fetchDocument(const QString & node);
   QString transform(const QString & xml);
   QTextDocument m_nodeDoc;
   SearchOptions * m_search;
   bool m_debug;
   int m_fragmentSize;
   int m_searchOptions;
   QSqlQuery m_query;
   QSqlQuery m_nodeQuery;
   QString m_target;
   QRegExp m_currentRx;
   QString m_xsltSource;
   QLabel * m_resultsText;
   QTableWidget * m_list;
   QTableWidget * m_rxlist;
   GraphicsEntry * m_text;
   QPushButton * m_findButton;
   QPushButton * m_hideOptionsButton;
   QStringList m_nodes;
   int m_defaultOptions;
/// for Arabic font from QSettings
   QFont m_resultsFont;
 signals:
   void searchResult(const QString &);
};
#endif
