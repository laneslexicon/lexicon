#ifndef __HEADSEARCH_H__
#define __HEADSEARCH_H__
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
#include <QProgressDialog>
class GraphicsEntry;
class FocusTable;
class HeadSearchWidget : public QWidget
{
    Q_OBJECT

 public:
    HeadSearchWidget(const QString & str,int options,QWidget * parent = 0);
    HeadSearchWidget(QWidget * parent = 0);
   GraphicsEntry * getEntry() { return m_text;}
   int count();
   void search(const QString &,int options);
   void showFirst();
   void focusTable();
 public slots:
   void cancelSearch();
   void itemChanged(QTableWidgetItem *,QTableWidgetItem *);
   void itemDoubleClicked(QTableWidgetItem *);
 protected:
   void focusInEvent(QFocusEvent *);
   void focusOutEvent(QFocusEvent *);

 private:
   void readSettings();
   bool eventFilter(QObject * target,QEvent * event);
   QString buildText(int);
   QRegExp m_currentRx;
   int m_searchOptions;
   int m_stepCount;
   QSqlQuery m_query;
   QSqlQuery m_nodeQuery;
   QString m_target;
   QLabel * m_resultsText;
   FocusTable * m_list;
   GraphicsEntry * m_text;
/// for Arabic font from QSettings
   QFont m_resultsFont;
   bool m_cancelSearch;
   bool m_debug;
 signals:
   void searchResult(const QString &);
};
#endif
