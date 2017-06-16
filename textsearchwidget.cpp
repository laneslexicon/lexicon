#include "textsearchwidget.h"
#include "textsearch.h"
#include "lanesupport.h"
#include "columnartablewidget.h"
#include "textsearch.h"
#include "centeredcheckbox.h"
#include "place.h"
#define SELECT_COLUMN 0
#define ROOT_COLUMN 1
#define HEAD_COLUMN 2
#define NODE_COLUMN 3
#define POSITION_COLUMN 4
#define VOL_COLUMN 5
#define CONTEXT_COLUMN 6
extern LaneSupport * getSupport();
TextSearchWidget::TextSearchWidget(bool summary,QWidget * parent) : QWidget(parent) {
  QStringList headings;
  headings << tr("Mark") << tr("Root") << tr("Headword") << tr("Node") << tr("Occurs")  << tr("Vol/Page") << tr("Context");
  if (! summary) {
    headings[POSITION_COLUMN] = tr("Position");
  }
  m_summary = summary;
  m_results = new ColumnarTableWidget(headings,this);
  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(m_results);
  setLayout(layout);
}
void TextSearchWidget::load(const TextSearch & data) {
  int rows = data.rows(m_summary);
  QList<SearchHit> d = data.getHits(0,rows,m_summary);
  for(int i=0;i < d.size();i++) {
    Place p = Place::fromSearchHit(d[i]);
    addRow(p,d[i].fragment,d[i].ix);
  }
}
int TextSearchWidget::addRow(const Place & p, const QString & text,int pos) {

  QTableWidgetItem * item;
  QLabel * label;

  int row = m_results->rowCount();
  m_results->insertRow(row);

  m_results->setCellWidget(row,SELECT_COLUMN,new CenteredCheckBox);

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
