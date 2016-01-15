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
#include <QSqlRecord>
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
#include "searchoptionswidget.h"
class ImLineEdit;
class GraphicsEntry;
class SearchOptionsWidget;
class ColumnarTableWidget;
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
  void focusTable();
  int findCount() const;
  QString results() const;
  public slots:
    void itemDoubleClicked(QTableWidgetItem *);
    void onCellDoubleClicked(int,int);
    void hideOptions();
    void findTarget(bool show=false);
    void cancelSearch();
    void showKeyboard();
    void onExport();
    void selectFocus();
    void openNode(const QString &);
 protected:
    void focusInEvent(QFocusEvent *);
    void focusOutEvent(QFocusEvent *);
    //   void eventFilter(QObject *,QEvent *);
 private:
    void showNode(int row);
    void readSettings();
    QString buildText(int,int,int,int);
    bool readCssFromFile(const QString &);
    int getMaxRecords(const QString & tablename);
    bool eventFilter(QObject * target,QEvent * event);
    bool startsWithArabic(const QString & ) const;
    int addRow(const QSqlRecord &,const QString &,int);
    void getTextFragments(QTextDocument * doc,const QString & target,const SearchOptions & options,const QRegExp & rx = QRegExp());
    QTextDocument * fetchDocument(const QString & node);
    QString transform(const QString & xml);

    QColor m_headBackgroundColor;

    KeyboardWidget * m_keyboard;
    bool m_attached;
    bool m_debug;
    bool m_showProgressDialog;
    bool m_cancelSearch;
    bool m_singleRow;
    int m_stepCount;
    QProgressBar * m_progress;
    QTextDocument m_nodeDoc;
    ImLineEdit * m_findTarget;
    QStringList m_fragments;
    QList<int> m_positions;

    SearchOptionsWidget * m_search;
    int m_fragmentSize;
    int m_findCount;
    SearchOptions m_searchOptions;
    QSqlQuery m_query;
    QSqlQuery m_nodeQuery;
    QRegExp m_currentRx;
    QString m_headText;
    QString m_target;
    QString m_xsltSource;
    QString m_currentCSS;
    QString m_diacritics;
    QString m_spanStyle;
    QString m_keyboardConfig;
    QLabel * m_resultsText;
    QPushButton * m_exportButton;
    QSpacerItem * m_spacer;
    QVBoxLayout * m_container;
    ColumnarTableWidget * m_rxlist;
    GraphicsEntry * m_text;
    QPushButton * m_findButton;
    QPushButton * m_hideOptionsButton;
    QPushButton * m_keyboardButton;
    SearchOptions  m_defaultOptions;
    /// for Arabic font from QSettings
    QFont m_resultsFont;
 signals:
    void searchResult(const QString &);
    void showNode(const QString &);
    void setProgressMax(int);
    void setProgressValue(int);
    void printNode(const QString &);
};
#endif
