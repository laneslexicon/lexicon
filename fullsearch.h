#ifndef __FULLSEARCH_H__
#define __FULLSEARCH_H__
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
#include <QProgressDialog>
#include <QEventLoop>
#include <QHeaderView>
#ifdef __APPLE__
#include <QStyleFactory>
#endif
#include "searchoptions.h"
class ImLineEdit;
class GraphicsEntry;
class SearchOptionsWidget;
class FocusTable;
class KeyboardWidget;
class FullSearchWidget : public QWidget
{
    Q_OBJECT

 public:
    //    FullSearchWidget(const QString & str,int options,QWidget * parent = 0);
    FullSearchWidget(QWidget * parent = 0);
    ~FullSearchWidget();
   GraphicsEntry * getEntry() { return m_text;}
   void setForceLTR(bool v);
   void search(const QString &,const SearchOptions &);
   void regexSearch(const QString &,const SearchOptions &);
   void textSearch(const QString &,const SearchOptions &);
   void setSearch(const QString & searchFor,const SearchOptions &);
   void setOptionsHidden(bool);
   void focusTable();
 public slots:
   void itemChanged(QTableWidgetItem *,QTableWidgetItem *);
   void itemDoubleClicked(QTableWidgetItem *);
   void hideOptions();
   void findTarget(bool show=false);
   void cancelSearch();
   void showKeyboard();
 protected:
   void focusInEvent(QFocusEvent *);
   void focusOutEvent(QFocusEvent *);
   //   void eventFilter(QObject *,QEvent *);
 private:
   QColor m_headBackgroundColor;
   QString m_headText;
    KeyboardWidget * m_keyboard;
    bool m_attached;
   void readSettings();
   QString buildText(int,int,int,int);
   bool readCssFromFile(const QString &);
   int getMaxRecords(const QString & tablename);
   QProgressBar * m_progress;
   ImLineEdit * m_findTarget;
   bool eventFilter(QObject * target,QEvent * event);
   QStringList m_fragments;
   QList<int> m_positions;
   int addRow(const QString &,const QString &,const QString &,const QString &,int);
   void getTextFragments(QTextDocument * doc,const QString & target,const SearchOptions & options,const QRegExp & rx = QRegExp());
   QString buildSearchSql(const SearchOptions &);
   QString buildRxSql(const SearchOptions &);
   QTextDocument * fetchDocument(const QString & node);
   QString transform(const QString & xml);
   QTextDocument m_nodeDoc;
   SearchOptionsWidget * m_search;
   bool m_debug;
   int m_fragmentSize;
   SearchOptions m_searchOptions;
   QSqlQuery m_query;
   QSqlQuery m_nodeQuery;
   QString m_target;
   QRegExp m_currentRx;
   QString m_xsltSource;
   QLabel * m_resultsText;
   QSpacerItem * m_spacer;
   QVBoxLayout * m_container;
   //   QTableWidget * m_list;
   FocusTable * m_rxlist;
   GraphicsEntry * m_text;
   QPushButton * m_findButton;
   QPushButton * m_hideOptionsButton;
   QPushButton * m_keyboardButton;
   QStringList m_nodes;
   QString m_currentCSS;
   SearchOptions  m_defaultOptions;
   bool m_showProgressDialog;
   bool m_cancelSearch;
/// for Arabic font from QSettings
   QFont m_resultsFont;
 signals:
   void searchResult(const QString &);
   void showNode(const QString &);
   void setProgressMax(int);
   void setProgressValue(int);
};
#endif
