#ifndef __TEXTSEARCHWIDGET_H__
#define __TEXTSEARCHWIDGET_H__
#include <QWidget>
#include <QVBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QSet>
#include <QList>
#include <QByteArray>
#include <QPoint>
#include <QVariant>
#include <QTableWidgetItem>
#include <QSqlQuery>
#include <QSqlError>
#include <QPushButton>
class TextSearch;
class ColumnarTableWidget;
class Place;
class TextSearchWidget : public QWidget {
  Q_OBJECT

 public:
  TextSearchWidget(int pageSize = 10,bool summary = false,QWidget * parent = 0);
  //  void load(const TextSearch &);
  void loadPage(int page);
  void setPages(int);
  TextSearch * searcher();
  void readSettings();
  void viewNode(int row);
  void focusTable();


 private:
  TextSearch * m_data;
  bool m_summary;
  QString m_contextStyle;
  ColumnarTableWidget * m_results;
  int addRow(const Place & p, const QString & text,int pos);
  int m_pageSize;
  int m_currentPage;
  QComboBox * m_page;
  QCheckBox * m_summaryTable;
  QPushButton * m_exportButton;
  bool m_resizeRows;
  bool m_exportAll;
  int m_rowHeight;
  int m_stepCount;

  QMap<int,QSet<int>> m_marks;     // key is page number (base=1),value is rows marked
  private slots:
    void exportRecord(int page,int row);
    void pageChanged(const QString &);
    void rowMarked(int);
    void summaryChanged(int);
    void onCellDoubleClicked(int row,int /* col */);
    void itemDoubleClicked(QTableWidgetItem * item);
    void openNode(const QString &);
    void onExport();
   signals:
      void printNode(const QString &);
      void showNode(const QString &,bool);
};
#endif
