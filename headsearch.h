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
#include <QHeaderView>
#include "searchoptionswidget.h"
class GraphicsEntry;
class FocusTable;
class Place;
class HeadSearchWidget : public QWidget
{
    Q_OBJECT

 public:
    //    HeadSearchWidget(const QString & str,const SearchOptions &,QWidget * parent = 0);
    HeadSearchWidget(QWidget * parent = 0);
    Place getPlace();
    GraphicsEntry * getEntry();
    int count();
   void search(const QString &,const SearchOptions &);
   void showFirst();
   void focusTable();
 public slots:
   void cancelSearch();
   void viewedItemChanged(QTableWidgetItem *,QTableWidgetItem *);
   void itemChanged(QTableWidgetItem *);
   void itemDoubleClicked(QTableWidgetItem *);
   void onRemoveResults();
   void onExport();
   void selectFocus();
 protected:
   void focusInEvent(QFocusEvent *);
   void focusOutEvent(QFocusEvent *);

 private:
   QStringList m_columns;
   void readSettings();
   bool eventFilter(QObject * target,QEvent * event);
   bool m_focusTable;
   QString buildText(const SearchOptions &);
   QRegExp m_currentRx;
   QString m_diacritics;
   SearchOptions m_searchOptions;
   QLabel * m_searchTitle;
   int m_stepCount;
   QSqlQuery m_query;
   QSqlQuery m_nodeQuery;
   QString m_target;
   QLabel * m_resultsText;
   FocusTable * m_list;
   GraphicsEntry * m_entry;
/// for Arabic font from QSettings
   QFont m_resultsFont;
   bool m_singleClick;
   bool m_cancelSearch;
   bool m_debug;
   bool m_verticalLayout;
   QPushButton * m_convertButton;
   QPushButton * m_exportButton;
 signals:
   void searchResult(const QString &);
   void deleteSearch();
};
#endif
