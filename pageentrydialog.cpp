#include "pageentrydialog.h"
#include "definedsql.h"
#include "QsLog.h"
#include "application.h"
#include "externs.h"
#include "graphicsentry.h"
#include "definedsettings.h"
#include "centeredcheckbox.h"

#define PAGE_LOAD_COLUMN 0
#define PAGE_NODE_COLUMN 1
#define PAGE_ROOT_COLUMN 2
#define PAGE_WORD_COLUMN 3
#define PAGE_VOLUME_COLUMN 4
#define PAGE_ID_COLUMN 5

PageEntryDialog::PageEntryDialog(int pageset,QWidget * parent) : QDialog(parent) {
  QLOG_DEBUG() << Q_FUNC_INFO;
  QSqlQuery p(QSqlDatabase::database("notesdb"));
  if (! p.prepare(SQL_PAGESET_DETAIL)) {
    QLOG_WARN() << QString(tr("Prepare failed for SQL_PAGESET_DETAIL query:%1")).arg(p.lastError().text());
  }
  /// we shouldn't have sql errors, but this should fail gracefully
  p.bindValue(0,pageset);
  p.exec();
  int page = 0;
  QVBoxLayout * layout = new QVBoxLayout;
  m_pages = new QTableWidget;
  QMap<int,QString> hm;
  hm.insert(PAGE_LOAD_COLUMN,tr("Select"));
  hm.insert(PAGE_NODE_COLUMN,tr("Node"));
  hm.insert(PAGE_ROOT_COLUMN,tr("Root"));
  hm.insert(PAGE_WORD_COLUMN,tr("Headword"));
  hm.insert(PAGE_VOLUME_COLUMN,tr("Volume/Page"));
  hm.insert(PAGE_ID_COLUMN,tr("Id"));

  m_pages->setColumnCount(hm.size());

  m_pages->setHorizontalHeaderLabels(hm.values());
  m_pages->horizontalHeader()->setStretchLastSection(true);
  m_pages->setSelectionMode(QAbstractItemView::SingleSelection);
  m_pages->verticalHeader()->setVisible(false);

  QString html;
  QLabel * label;
  while(p.next()) {
    Place n = Place::fromString(p.record().value("place").toString());
    if (n.isValid()) {
      page = m_pages->rowCount();
      m_pages->insertRow(page);
      m_pages->setItem(page,PAGE_NODE_COLUMN,new QTableWidgetItem(n.m_node));
      m_pages->item(page,PAGE_NODE_COLUMN)->setTextAlignment(Qt::AlignCenter);

      html = qobject_cast<Lexicon *>(qApp)->scanAndStyle(n.m_root,"pageset");
      label = new QLabel(html);;
      label->setAlignment(Qt::AlignCenter);
      m_pages->setCellWidget(page,PAGE_ROOT_COLUMN,label);

      html = qobject_cast<Lexicon *>(qApp)->scanAndStyle(n.m_word,"pageset");
      label = new QLabel(html);;
      label->setAlignment(Qt::AlignCenter);
      m_pages->setCellWidget(page,PAGE_WORD_COLUMN,label);
      m_pages->setCellWidget(page,PAGE_LOAD_COLUMN,new CenteredCheckBox);

      m_pages->setItem(page,PAGE_VOLUME_COLUMN,new QTableWidgetItem(n.format("%V/%P")));
      m_pages->item(page,PAGE_VOLUME_COLUMN)->setTextAlignment(Qt::AlignCenter);
      m_pages->setItem(page,PAGE_ID_COLUMN,new QTableWidgetItem(p.record().value("id").toString()));
    }
  }
  int rows = m_pages->rowCount();
  if (m_pages->rowCount() == 0) {
    m_pages->clear();
    m_pages->insertRow(0);
    m_pages->setColumnCount(1);
    m_pages->setHorizontalHeaderLabels(QStringList() << "");
    m_pages->horizontalHeader()->setStretchLastSection(true);
    QLabel * m = new QLabel(tr("<em>No pages found</em>"));
    m->setAlignment(Qt::AlignCenter);
    m_pages->setCellWidget(0,0,m);
  }
  else {
    QTableWidgetItem * item;
    QList<int> cols;
    cols << PAGE_NODE_COLUMN << PAGE_VOLUME_COLUMN << PAGE_ID_COLUMN;
    for(int i=0;i < m_pages->rowCount();i++) {
      for(int j=0;j < cols.size();j++) {
        item = m_pages->item(i,cols[j]);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
      }
    }
  }
  m_pages->hideColumn(PAGE_ID_COLUMN);
  m_pages->resizeColumnsToContents();
  layout->addWidget(m_pages);
  QDialogButtonBox * buttonBox;
  if (rows > 0) {
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QPushButton * btn1 = new QPushButton(tr("Select all"));
    buttonBox->addButton(btn1,QDialogButtonBox::ActionRole);
    connect(btn1,SIGNAL(clicked()),this,SLOT(onSelectAll()));
    QPushButton * btn2 = new QPushButton(tr("Clear Selections"));
    buttonBox->addButton(btn2,QDialogButtonBox::ActionRole);
    connect(btn2,SIGNAL(clicked()),this,SLOT(onClearSelections()));
  }
  else {
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel);
  }

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  layout->addWidget(buttonBox);
  setLayout(layout);
}
int PageEntryDialog::pageCount() const {
  int p = 0;
  for(int i=0;i < m_pages->rowCount();i++) {
    CenteredCheckBox * b = qobject_cast<CenteredCheckBox *>(m_pages->cellWidget(i,PAGE_LOAD_COLUMN));
    if (b && b->isChecked()) {
      p++;
    }

  }
  return p;
}
QList<int> PageEntryDialog::pages() const {
  QList<int> p;
  for(int i=0;i < m_pages->rowCount();i++) {
    CenteredCheckBox * b = qobject_cast<CenteredCheckBox *>(m_pages->cellWidget(i,PAGE_LOAD_COLUMN));
    if (b && b->isChecked()) {
      p << m_pages->item(i,PAGE_ID_COLUMN)->text().toInt();
    }
  }
  return p;
}
void PageEntryDialog::showEvent(QShowEvent * event) {
  QLOG_DEBUG() << Q_FUNC_INFO;
  readSettings();
  QDialog::showEvent(event);
}

void PageEntryDialog::done(int r) {
  QLOG_DEBUG() << Q_FUNC_INFO;
  writeSettings();
  QDialog::done(r);
}
PageEntryDialog::~PageEntryDialog() {
  qDebug() << Q_FUNC_INFO;
  writeSettings();
}
void PageEntryDialog::readSettings() {
  SETTINGS

  settings.beginGroup("PageSets");
  resize(settings.value(SID_PAGESET_PAGE_DIALOG_SIZE, QSize(600, 400)).toSize());
  move(settings.value(SID_PAGESET_PAGE_DIALOG_POS, QPoint(200, 200)).toPoint());
}
void PageEntryDialog::writeSettings() {
  SETTINGS

 settings.beginGroup("PageSets");
  settings.setValue(SID_PAGESET_PAGE_DIALOG_SIZE, size());
  settings.setValue(SID_PAGESET_PAGE_DIALOG_POS, pos());

}
void PageEntryDialog::onSelectAll() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  for(int i=0;i < m_pages->rowCount();i++) {
    CenteredCheckBox * b = qobject_cast<CenteredCheckBox *>(m_pages->cellWidget(i,PAGE_LOAD_COLUMN));
    if (b) {
      b->setChecked(true);
    }
  }
}
void PageEntryDialog::onClearSelections() {
  for(int i=0;i < m_pages->rowCount();i++) {
    CenteredCheckBox * b = qobject_cast<CenteredCheckBox *>(m_pages->cellWidget(i,PAGE_LOAD_COLUMN));
    if (b) {
      b->setChecked(false);
    }
  }
}
