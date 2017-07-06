#ifndef __TEXTSEARCHWIDGET_H__
#define __TEXTSEARCHWIDGET_H__
#include <QWidget>
#include <QVBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QSet>
#include <QList>
#include <QByteArray>
#include <QPoint>
#include <QVariant>
#include <QTableWidgetItem>
#include <QSqlQuery>
#include <QSqlError>
#include <QPushButton>
#include <QLabel>
class TextSearch;
class ColumnarTableWidget;
class Place;
class TextSearchWidget : public QWidget {
  Q_OBJECT

 public:
  TextSearchWidget(QWidget * parent = 0);
  ~TextSearchWidget();
  //  void load(const TextSearch &);
  void loadPage(int page);
  void setPages(int);
  TextSearch * searcher();
  void readSettings();
  void viewNode(int row);
  void focusTable();
  void showEmpty(const QString & text);
  int search();

 private:
  TextSearch * m_data;
  bool m_summary;
  QString m_contextStyle;
  ColumnarTableWidget * m_results;
  int addRow(const Place & p, const QString & text,int pos);
  int m_pageSize;
  int m_currentPage;
  int m_stepSize;
  QLabel    * m_pagesText;
  QComboBox * m_page;
  QCheckBox * m_summaryTable;
  QPushButton * m_exportButton;
  QPushButton * m_clearMarks;
  QPushButton * m_markAll;
  QCheckBox   * m_thisPage;
  bool m_resizeRows;
  bool m_exportAll;
  int m_rowHeight;
  int m_stepCount;

  QMap<int,QSet<int> > m_marks;     // key is page number (base=1),value is rows marked
  private slots:
    void exportRecord(int page,int row);
    void pageChanged(const QString &);
    void rowMarked(int);
    void summaryChanged(int);
    void onCellDoubleClicked(int row,int /* col */);
    void itemDoubleClicked(QTableWidgetItem * item);
    void openNode(const QString &);
    void onExport();
    void onClear();
    void onMark();
   signals:
      void printNode(const QString &);
      void showNode(const QString &,bool);
      void statusMessage(const QString &);
};
#endif
