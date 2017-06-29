#include "textsearchwidget.h"
#include "textsearch.h"
#include "lanesupport.h"
#include "columnartablewidget.h"
#include "textsearch.h"
#include "centeredcheckbox.h"
#include "place.h"
#include "definedsettings.h"
#include "definedsql.h"
#include "externs.h"
#include "nodeview.h"
#include "exportsearchdialog.h"
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
  /**
   * Other than the mark/select column, the possible columns should come from TextSerch.
   * What the columns headings are is not important as long as they can be correlated with
   * TextSearch fields
   */
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

  m_exportButton = new QPushButton(tr("Export"));
  hlayout->addWidget(m_exportButton);
  layout->addLayout(hlayout);
  m_results->setRowCount(pageSize);
  setLayout(layout);
  m_data = new TextSearch();
  m_data->setXsltFileName(getSupport()->xsltFileName());
  connect(m_results,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
          this,SLOT(itemDoubleClicked(QTableWidgetItem * )));
  connect(m_results,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(onCellDoubleClicked(int,int)));
  connect(m_exportButton,SIGNAL(clicked()),this,SLOT(onExport()));

  connect(m_data,SIGNAL(exportRecord(int,int)),this,SLOT(exportRecord(int,int)));
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
/*
void TextSearchWidget::load(const TextSearch & data) {
  int rows = data.rows(m_summary);
  QList<SearchHit> d = data.getHits(0,rows,m_summary);
  for(int i=0;i < d.size();i++) {
    Place p = Place::fromSearchHit(d[i]);
    addRow(p,d[i].fragment,d[i].ix);
  }
}
*/
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
  // now restore the marks
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

  QString str = "‪" + text;
  QRegularExpression leadingSpaces("^\\s+");
  QRegularExpression lineBreaks("\\r|\\n");
  leadingSpaces.setPatternOptions(QRegularExpression::UseUnicodePropertiesOption);
  lineBreaks.setPatternOptions(QRegularExpression::UseUnicodePropertiesOption);
  // remove the formatting characters surrounding Arabic quoted text
  str.remove(QChar(0xfdd0));
  str.remove(QChar(0xfdd1));
  str = str.remove(leadingSpaces);
  str = str.remove(lineBreaks);

  label = m_results->createLabel(str,"fullsearchlist");

  if (getSupport()->startsWithArabic(text)) {
    label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  }
  else {
    label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  }

  if ( ! m_contextStyle.isEmpty()) {
    str = label->text();
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
  /**
   * SearchParams will be used by NodeView to search the document, highlighting/selecting the search
   * results, so it needs to node search type(regex/normal), pattern and findflags
   */

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
  QSqlQuery nodeQuery;
  if (! nodeQuery.prepare(SQL_FIND_ENTRY_BY_NODE)) {
    QString err = nodeQuery.lastError().text();
    CERR << qPrintable(QString("SQL error %1 , %2").arg(SQL_FIND_ENTRY_BY_NODE).arg(err)) << ENDL;
    return;
  }
  nodeQuery.bindValue(0,node);
  if ( ! nodeQuery.exec() || ! nodeQuery.first()) {
    QString err = nodeQuery.lastError().text();
    CERR << qPrintable(QString("SQL error %1 , %2").arg(SQL_FIND_ENTRY_BY_NODE).arg(err)) << ENDL;
    return;
  }
  Place np = Place::fromEntryRecord(nodeQuery.record());
  QString xml = nodeQuery.value("xml").toString();
  p.html = m_data->toHtml(xml);
  NodeView * v = new NodeView(p,this);
  v->setAttribute(Qt::WA_DeleteOnClose);
  v->setWindowTitle(QString(tr("Showing result %1").arg(row + 1)));
  int page = nodeQuery.value("page").toInt();
  if (Place::volume(page) == 0) {
    page = 0;
  }
  v->setHeader(np.root(),np.word(),node,page);
  v->findFirst();
  v->show();
  v->raise();
  v->activateWindow();
  connect(v,SIGNAL(openNode(const QString &)),this,SLOT(openNode(const QString &)));
  connect(v,SIGNAL(printNode(const QString &)),this,SIGNAL(printNode(const QString &)));
}
void TextSearchWidget::openNode(const QString & node) {
  emit(showNode(node,false));
  m_results->setFocus();
}
void TextSearchWidget::focusTable() {
  m_results->setFocus();
}
/**
 * TODO if exporting summary ? Export all for each or just the count?
 *
 */
void TextSearchWidget::onExport() {
  QStringList columns = m_results->columnHeadings();
  qDebug() << Q_FUNC_INFO << m_marks;
  ExportSearchDialog dlg(columns);
  if (dlg.exec() != QDialog::Accepted) {
    return;
  }
  if (dlg.saveSettings()) {
    dlg.writeSettings();
  }
  QString exportFileName = dlg.exportFileName();
  QFile file(exportFileName);
  if (! file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QString err = QString(tr("Error opening export file to %1 : %2")).arg(exportFileName).arg(file.errorString());
    QLOG_WARN() << err;
    QMessageBox::warning(0, tr("Export Search Results"),err);
    return;
  }
  if (exportFileName.isEmpty()) {
    return;
  }
  file.close();
  QString sep = dlg.separator();
  columns = dlg.columns();

/**
 * If the column names ever get translated, then this needs to match
*
*/
  QMap<QString,QString> cnames;
  cnames.insert(tr("Node"),"N");
  cnames.insert(tr("Root"),"R");
  cnames.insert(tr("Head"),"H");
  cnames.insert(tr("Context"),"T");
  cnames.insert(tr("Page"),"P");
  cnames.insert(tr("Vol"),"V");
  cnames.insert(tr("Pos"),"O");
  QString fields;
  QMapIterator<QString,QString> iter(cnames);
  for(int i=0;i < columns.size();i++) {
    iter.toFront();
    while(iter.hasNext()) {
      iter.next();
      if (columns[i].contains(iter.key(),Qt::CaseInsensitive)) {
        fields.append(iter.value());
      }
    }
  }
  qDebug() << exportFileName <<  sep << columns << fields;
  m_exportAll = dlg.allRows();
  m_data->setFields(fields);
  m_data->setSeparator(sep);
  m_data->toFile(exportFileName);
}
void TextSearchWidget::exportRecord(int entry,int row) {
  qDebug() << Q_FUNC_INFO << entry << row;
  if (m_exportAll) {
    m_data->setExportRecord(true);
    return;
  }

}
