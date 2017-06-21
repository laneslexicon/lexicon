#include "textsearchwidget.h"
#include "textsearch.h"
#include "lanesupport.h"
#include "columnartablewidget.h"
#include "textsearch.h"
#include "centeredcheckbox.h"
#include "place.h"
#include "definedsettings.h"
#include "externs.h"
#define SELECT_COLUMN 0
#define ROOT_COLUMN 1
#define HEAD_COLUMN 2
#define NODE_COLUMN 3
#define POSITION_COLUMN 4
#define VOL_COLUMN 5
#define CONTEXT_COLUMN 6
extern LaneSupport * getSupport();
TextSearchWidget::TextSearchWidget(int pageSize,bool summary,QWidget * parent) : QWidget(parent) {
  QStringList headings;
  headings << tr("Mark") << tr("Root") << tr("Headword") << tr(" Node ") << tr("Occurs")  << tr("Vol/Page") << tr("Context");
  if (! summary) {
    headings[POSITION_COLUMN] = tr("Position");
  }
  readSettings();
  m_summary = summary;
  m_pageSize = pageSize;
  m_results = new ColumnarTableWidget(headings,this);
  m_results->setFixedRowHeight(m_rowHeight);
  m_results->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_results->setSelectionMode(QAbstractItemView::SingleSelection);
  m_results->setMarkColumn(SELECT_COLUMN);
  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(m_results);
  m_page = new QComboBox(this);
  connect(m_page,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(pageChanged(const QString &)));

  m_summaryTable = new QCheckBox(tr("One row per entry"));
  connect(m_summaryTable,SIGNAL(stateChanged(int)),this,SLOT(summaryChanged(int)));

  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addWidget(new QLabel(tr("Page")));
  hlayout->addWidget(m_page);
  hlayout->addWidget(m_summaryTable);
  hlayout->addStretch();
  layout->addLayout(hlayout);
  m_results->setRowCount(pageSize);
  setLayout(layout);
  m_data = new TextSearch();
  m_data->setXsltFileName(getSupport()->xsltFileName());
  connect(m_results,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
          this,SLOT(itemDoubleClicked(QTableWidgetItem * )));
  connect(m_results,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(onCellDoubleClicked(int,int)));
}
TextSearch * TextSearchWidget::searcher() {
  return m_data;
}
/*
void TextSearchWidget::search() {
  m_data->search();
}
*/
void TextSearchWidget::setPages(int pages) {
  m_page->blockSignals(true);
  m_page->clear();
  for(int i=0;i < pages;i++) {
    m_page->addItem(QString("%1").arg(i+1));
  }
  if (pages > 0) {
    m_page->setCurrentIndex(0);
    m_page->setEnabled(true);
    m_summaryTable->setEnabled(true);
  }
  else {
    m_page->setEnabled(false);
    m_summaryTable->setEnabled(false);
  }
  if (pages == 1) {
    m_page->setEnabled(false);
  }
  m_page->blockSignals(false);
}
void TextSearchWidget::load(const TextSearch & data) {
  int rows = data.rows(m_summary);
  QList<SearchHit> d = data.getHits(0,rows,m_summary);
  for(int i=0;i < d.size();i++) {
    Place p = Place::fromSearchHit(d[i]);
    addRow(p,d[i].fragment,d[i].ix);
  }
}
void TextSearchWidget::loadPage(int page) {
  m_results->setRowCount(0);
  //  int rows = data.rows(m_summary);
  QList<SearchHit> d = m_data->getPage(page,m_summary);
  for(int i=0;i < d.size();i++) {
    Place p = Place::fromSearchHit(d[i]);
    addRow(p,d[i].fragment,d[i].ix);
  }
  m_results->resizeColumnToContents(SELECT_COLUMN);
  m_results->resizeColumnToContents(NODE_COLUMN);
  m_results->resizeColumnToContents(POSITION_COLUMN);
  m_results->resizeColumnToContents(CONTEXT_COLUMN);
  if (m_resizeRows) {
    m_results->resizeRowsToContents();
  }
  m_currentPage = page;
  // new restore the marks
  if (m_marks.contains(page)) {
    QSet<int> s = m_marks.value(page);
    QSetIterator<int> iter(m_marks.value(page));
    int row;
    while(iter.hasNext()) {
      row = iter.next();
      CenteredCheckBox * cb = qobject_cast<CenteredCheckBox *>(m_results->cellWidget(row,SELECT_COLUMN));
      if (cb) {
        cb->setChecked(true);
      }
    }
  }
}
void TextSearchWidget::pageChanged(const QString & page) {
  bool ok;
  int p = page.toInt(&ok,10);
  if (ok) {
    this->loadPage(p);
  }
}
int TextSearchWidget::addRow(const Place & p, const QString & text,int pos) {

  QTableWidgetItem * item;
  QLabel * label;

  int row = m_results->rowCount();
  m_results->insertRow(row);
  CenteredCheckBox * cb = new CenteredCheckBox;
  cb->setRow(row);
  m_results->setCellWidget(row,SELECT_COLUMN,cb);
  connect(cb,SIGNAL(stateChanged(int)),this,SLOT(rowMarked(int)));

  label = new QLabel(getSupport()->scanAndStyle(p.root(),"fullsearchlist"));
  label->setAlignment(Qt::AlignCenter);
  m_results->setCellWidget(row,ROOT_COLUMN,label);

  label = m_results->createLabel(p.head(),"fullsearchlist");
  label->setAlignment(Qt::AlignCenter);
  m_results->setCellWidget(row,HEAD_COLUMN,label);

  label = m_results->createLabel(p.node());
  label->setAlignment(Qt::AlignCenter);
  m_results->setCellWidget(row,NODE_COLUMN,label);

  label = m_results->createLabel((QString("%1").arg(pos)));
  label->setAlignment(Qt::AlignCenter);
  m_results->setCellWidget(row,POSITION_COLUMN,label);

  label = m_results->createLabel(text,"fullsearchlist");

  if (getSupport()->startsWithArabic(text)) {
    label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  }
  else {
    label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  }

  if ( ! m_contextStyle.isEmpty()) {
    QString str = label->text();
    str = QString("<span style=\"%1\">%2</span>").arg(m_contextStyle).arg(str);
    label->setText(str);
  }

  m_results->setCellWidget(row,CONTEXT_COLUMN,label);
  label = m_results->createLabel(p.format("%V/%P"));
  label->setAlignment(Qt::AlignCenter);
  m_results->setCellWidget(row,VOL_COLUMN,label);

  return row;
}
void TextSearchWidget::rowMarked(int state) {
   CenteredCheckBox * cb = qobject_cast<CenteredCheckBox *>(sender());
   QSet<int> s;
   if (cb) {
     int row = cb->row();
     if (state == Qt::Checked) {
       if (m_marks.contains(m_currentPage)) {
         s  = m_marks.value(m_currentPage);
       }
       s.insert(row);
       m_marks.insert(m_currentPage,s);
     }
     else {
       if (m_marks.contains(m_currentPage)) {
         s  = m_marks.value(m_currentPage);
       }
       s.remove(row);
       m_marks.insert(m_currentPage,s);
     }
   }
}
void TextSearchWidget::readSettings() {
  SETTINGS

  settings.beginGroup("FullSearch");
  m_contextStyle = settings.value(SID_FULLSEARCH_CONTEXT_STYLE,QString()).toString();
  //  m_singleRow = settings.value(SID_FULLSEARCH_ONE_ROW,true).toBool();
  QString f = settings.value(SID_FULLSEARCH_RESULTS_FONT,QString()).toString();
  if (! f.isEmpty()) {
    //    m_resultsFont.fromString(f);
  }
  m_resizeRows = settings.value(SID_FULLSEARCH_RESIZE_ROWS,true).toBool();
  m_rowHeight  = settings.value(SID_FULLSEARCH_ROW_HEIGHT,40).toInt();;

}
void TextSearchWidget::summaryChanged(int state) {
  bool summary = (state == Qt::Checked);
  if (summary != m_summary) {
    m_summary = summary;
    m_marks.clear();
    QPair<int,int> pages = m_data->getPageCounts();
    if (m_summary) {
      m_results->setHorizontalHeaderItem(POSITION_COLUMN,new QTableWidgetItem("Count"));
      setPages(pages.first);
    }
    else {
      m_results->setHorizontalHeaderItem(POSITION_COLUMN,new QTableWidgetItem("Position"));
      setPages(pages.second);
    }
    this->loadPage(1);
  }
}
/**
 * To view the node we need:
 *     node number
 *     QTextDocument::FindFlags
 *     QRegularExpression or QString pattern
 *
 * @param row
 */
void TextSearchWidget::onCellDoubleClicked(int row,int /* col */) {
   this->viewNode(row);
}
void TextSearchWidget::itemDoubleClicked(QTableWidgetItem * item) {
  if (item) {
    this->viewNode(item->row());
  }
}
void TextSearchWidget::viewNode(int row) {
  QString node;
  QLabel * label = qobject_cast<QLabel *>(m_results->cellWidget(row,NODE_COLUMN));
  if (label) {
    node = label->text();
  }
  else {
    CERR << qPrintable(QString("Cannot find node cell at %1/%2").arg(row).arg(NODE_COLUMN)) << ENDL;
    return;
  }
  SearchParams p = m_data->params();
  p.node = node;
  p.pos = 0;
  if (! m_summary) {
    QLabel * label = qobject_cast<QLabel *>(m_results->cellWidget(row,POSITION_COLUMN));
    if (label) {
      bool ok;
      int i = label->text().toInt(&ok);
      if (ok) {
        p.pos = i;
      }
    }
  }
  qDebug() << p;
}
