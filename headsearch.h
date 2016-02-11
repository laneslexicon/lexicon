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
class ColumnarTableWidget;
class HeadSearchWidget : public QWidget
{
    Q_OBJECT

 public:
    HeadSearchWidget(QWidget * parent = 0);
    int count();
   void search(const QString &,const SearchOptions &);

   void focusTable();
   void showNode(int row);
 public slots:
   void cancelSearch();
   void onItemDoubleClicked(QTableWidgetItem *);
   void onCellDoubleClicked(int,int);
   void onExport();
   void selectFocus();
   void openNode(const QString &,bool);
 protected:
   void focusInEvent(QFocusEvent *);
   void focusOutEvent(QFocusEvent *);

 private:
   bool readCssFromFile(const QString &);
   QString m_xsltSource;
   QString m_css;
   QString transform(const QString & xml) const;
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
   ColumnarTableWidget * m_heads;
   GraphicsEntry * m_entry;
   bool m_resizeRows;
   int m_rowHeight;
   bool m_headPhrase;
   bool m_cancelSearch;
   bool m_nodeinfoClose;
   bool m_nodeinfoForce;
   QPushButton * m_exportButton;
 signals:
   void searchResult(const QString &);
   void deleteSearch();
   void showNode(const QString &,bool forceNewTab = false);
};
#endif
