#include "loadpagesetdialog.h"
#include "pageentrydialog.h"
#include "definedsql.h"
#include "QsLog.h"
#include "application.h"
#include "externs.h"
#include "graphicsentry.h"
#include "definedsettings.h"
#include "centeredcheckbox.h"
#include "columnartablewidget.h"
#define SET_ID_COLUMN 0
#define SET_TITLE_COLUMN 1
#define SET_COUNT_COLUMN 2
#define SET_ACCESSED_COLUMN 3
#define SET_LOAD_COLUMN 4
#define SET_SELECT_COLUMN 5
#define SET_LOAD_COUNT_COLUMN 6

LoadPageSetDialog::LoadPageSetDialog(QWidget * parent) : QDialog(parent) {
  QLOG_DEBUG() << Q_FUNC_INFO;
  //  m_action = action;
  QStringList cols;
  cols << tr("Id") << tr("Title") << tr("Tab count") << tr("Created") << tr("Load all") << tr("Select tabs") << tr("Tabs to load");

  m_setlist = new ColumnarTableWidget(cols);
  m_setlist->setKey(ColumnarTableWidget::STATE,SID_PAGESET_LOADSETLIST_STATE);


  m_overwrite = new QCheckBox(tr("Close current tabs"));
  readSettings();

  setWindowTitle(tr("Load Tab Set"));

  QVBoxLayout * layout = new QVBoxLayout;


  //  m_tree = new QTreeWidget;
  //  layout->addWidget(m_tree);
  layout->addWidget(m_setlist);
  layout->addWidget(m_overwrite);

  int rows = this->loadTitles();
  //  this->loadTitlesToTree();

  m_setlist->hideColumn(SET_ID_COLUMN);
  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel);
  m_loadButton  = new QPushButton(tr("Load"));
  buttonBox->addButton(m_loadButton,QDialogButtonBox::AcceptRole);
  m_loadButton->setVisible(rows > 0);
  m_loadButton->setEnabled(false);

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  layout->addWidget(buttonBox);

  setLayout(layout);
 ///
  SETTINGS
  settings.beginGroup("PageSets");
  m_setlist->readConfiguration(settings);
  m_setlist->resizeColumnToContents(SET_SELECT_COLUMN);
  m_setlist->resizeColumnToContents(SET_LOAD_COLUMN);

}
LoadPageSetDialog::~LoadPageSetDialog() {
  qDebug() << Q_FUNC_INFO;
  writeSettings();
}
QString LoadPageSetDialog::pageSetTitle() const {
  return QString();
}
bool LoadPageSetDialog::closeExisting() const {
  return m_overwrite->isChecked();
}
int LoadPageSetDialog::loadTitles() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  QSqlRecord rec;
  m_setlist->clearContents();
  m_setlist->verticalHeader()->setVisible(false);

  //  connect(m_setlist,SIGNAL(itemSelectionChanged()),this,SLOT(setTitleFromTable()));
  QSqlQuery q(QSqlDatabase::database("notesdb"));
  if (! q.prepare(SQL_PAGESET_HEADERS)) {
    /// Report error
    QLOG_WARN() << QString(tr("Prepare failed for SQL_PAGESET_HEADER query:%1")).arg(q.lastError().text());
    return 0;
  }
  QSqlQuery p(QSqlDatabase::database("notesdb"));
  if (! p.prepare(SQL_PAGESET_PAGE_COUNT)) {
    /// Report error
    QLOG_WARN() << QString(tr("Prepare failed for SQL_PAGESET_PAGE_COUNT query:%1")).arg(p.lastError().text());
    return 0;
  }
  q.exec();
  int row;
  QTableWidgetItem * item;
  while(q.next()) {
    rec = q.record();
    p.bindValue(0,rec.value("id").toInt());
    p.exec();
    if (p.first() && (p.record().value("count(id)").toInt() > 0)) {
      row = m_setlist->rowCount();
      m_setlist->insertRow(row);
      item = new QTableWidgetItem(p.record().value("count(id)").toString());
      item->setTextAlignment(Qt::AlignCenter);
      item->setFlags(item->flags() ^ Qt::ItemIsEditable);
      m_setlist->setItem(row,SET_COUNT_COLUMN,item);


      item = new QTableWidgetItem(rec.value("title").toString());
      item->setTextAlignment(Qt::AlignCenter);
      item->setFlags(item->flags() ^ Qt::ItemIsEditable);
      m_setlist->setItem(row,SET_TITLE_COLUMN,item);

      item = new QTableWidgetItem(QString("%1").arg(rec.value("id").toInt()));
      item->setTextAlignment(Qt::AlignCenter);
      item->setFlags(item->flags() ^ Qt::ItemIsEditable);
      m_setlist->setItem(row,SET_ID_COLUMN,item);

      CenteredCheckBox * box = new CenteredCheckBox;
      box->setRow(row);
      connect(box,SIGNAL(stateChanged(int)),this,SLOT(onSelectAll(int)));
      m_setlist->setCellWidget(row,SET_LOAD_COLUMN,box);


      QString d = rec.value("accessed").toString();
      item = new QTableWidgetItem(QString("%1").arg(d));
      item->setTextAlignment(Qt::AlignCenter);
      item->setFlags(item->flags() ^ Qt::ItemIsEditable);
      m_setlist->setItem(row,SET_ACCESSED_COLUMN,item);


      QPushButton * btn = new QPushButton(tr(".."));
      btn->setStyleSheet("margin : 5");
      btn->setProperty("pageset",rec.value("id"));
      btn->setProperty("pagetitle",rec.value("title"));
      btn->setProperty("tablerow",row);
      connect(btn,SIGNAL(clicked()),this,SLOT(onSelectPages()));
      m_setlist->setCellWidget(row,SET_SELECT_COLUMN,btn);
      m_setlist->setItem(row,SET_LOAD_COUNT_COLUMN,new QTableWidgetItem("0"));
      m_setlist->item(row,SET_LOAD_COUNT_COLUMN)->setTextAlignment(Qt::AlignCenter);
      m_pagesets.insert(row,rec.value("id").toInt());
    }
  }
  int rows = m_setlist->rowCount();
  if (rows == 0) {
    m_setlist->clearContents();
    m_setlist->insertRow(0);
    m_setlist->setColumnCount(2);
    m_setlist->setHorizontalHeaderLabels(QStringList() << "");
    m_setlist->horizontalHeader()->setStretchLastSection(true);
    QLabel * m = new QLabel(tr("<em>No tab sets found</em>"));
    m->setAlignment(Qt::AlignCenter);
    m_setlist->setCellWidget(0,1,m);
  }

  //  m_setlist->resizeColumnsToContents();
  ///
  SETTINGS
  settings.beginGroup("PageSets");
  m_setlist->readConfiguration(settings);
  return rows;
}
void LoadPageSetDialog::readSettings() {
  SETTINGS

  settings.beginGroup("PageSets");
  resize(settings.value(SID_PAGESET_LOAD_DIALOG_SIZE, QSize(600, 400)).toSize());
  move(settings.value(SID_PAGESET_LOAD_DIALOG_POS, QPoint(200, 200)).toPoint());
  m_spanStyle = settings.value(SID_PAGESET_MAIN_CONTEXT,QString()).toString();
  m_spanStyle += " " + settings.value(SID_PAGESET_ARABIC_CONTEXT,QString()).toString();
  m_overwrite->setChecked(settings.value(SID_PAGESET_OVERWRITE_EXISTING,false).toBool());
}
void LoadPageSetDialog::writeSettings() {
  SETTINGS

 settings.beginGroup("PageSets");
  settings.setValue(SID_PAGESET_LOAD_DIALOG_SIZE, size());
  settings.setValue(SID_PAGESET_LOAD_DIALOG_POS, pos());
  settings.setValue(SID_PAGESET_OVERWRITE_EXISTING,m_overwrite->isChecked());
}
void LoadPageSetDialog::onSelectPages() {
  QPushButton * btn = qobject_cast<QPushButton *>(QObject::sender());
  if (! btn ) {
    return;
  }
  int pageset = btn->property("pageset").toInt();
  PageEntryDialog * d;
  if (! m_pageDialogs.contains(pageset)) {
    d = new PageEntryDialog(pageset,this);
    d->setWindowTitle(QString(tr("Select tabs to load from %1")).arg(btn->property("pagetitle").toString()));
    m_pageDialogs.insert(pageset,d);
  }
  else {
    d = qobject_cast<PageEntryDialog *>(m_pageDialogs.value(pageset));
  }
  if (! d ) {
    return;
  }
  if (d->exec() == QDialog::Accepted) {
    int x = d->pageCount();
    int row = btn->property("tablerow").toInt();
    m_setlist->item(row,SET_LOAD_COUNT_COLUMN)->setText(QString("%1").arg(x));
    CenteredCheckBox * b = qobject_cast<CenteredCheckBox *>(m_setlist->cellWidget(row,SET_LOAD_COLUMN));
    if (b) {
      b->blockSignals(true);
      b->setChecked(false);
      b->blockSignals(false);
    }
  }
  m_loadButton->setEnabled(this->loadCount() > 0);
}
void LoadPageSetDialog::onSelectAll(int state) {
  CenteredCheckBox * b = qobject_cast<CenteredCheckBox *>(QObject::sender());
  if (! b ) {
    return;
  }
  int row = b->row();
  if (state == Qt::Checked) {
    QString t = m_setlist->item(row,SET_COUNT_COLUMN)->text();
    m_setlist->item(row,SET_LOAD_COUNT_COLUMN)->setText(t);
  }
  else {
    m_setlist->item(row,SET_LOAD_COUNT_COLUMN)->setText("0");
  }
  m_loadButton->setEnabled(this->loadCount() > 0);
}
int LoadPageSetDialog::loadCount() const {
  bool ok;
  int p = 0;
  for(int i=0;i < m_setlist->rowCount();i++) {
    QString t = m_setlist->item(i,SET_LOAD_COUNT_COLUMN)->text();
    int c = t.toInt(&ok);
    if (ok) {
      p += c;
    }
  }
  return p;
}
QList<int> LoadPageSetDialog::pages() const {
  QList<int> p;
  int pageset;

  bool ok;

  QLOG_DEBUG() << Q_FUNC_INFO;
  QSqlQuery q(QSqlDatabase::database("notesdb"));
  if (! q.prepare(SQL_PAGESET_DETAIL)) {
    return p;
  }
  qDebug() << m_pagesets;
  for(int i=0;i < m_setlist->rowCount();i++) {
    QString t = m_setlist->item(i,SET_LOAD_COUNT_COLUMN)->text();
    int c = t.toInt(&ok);
    if (ok && (c > 0)) {
      pageset = m_pagesets.value(i);
      CenteredCheckBox * b = qobject_cast<CenteredCheckBox *>(m_setlist->cellWidget(i,SET_LOAD_COLUMN));
      /// if the load box is checked, read the db to get the page id
      /// otherwise get the from the pageentrydialog
      if (b) {
        if (b->isChecked()) {
          q.bindValue(0,pageset);
          q.exec();
          while(q.next()) {
            p << q.record().value("id").toInt();
          }
        }
        else if (m_pageDialogs.contains(pageset)) {
          PageEntryDialog * d = qobject_cast<PageEntryDialog *>(m_pageDialogs.value(pageset));
          if (d) {
            p << d->pages();
          }
        }
      }
    }
  }
  qDebug() << p.size() << "pages:" << p;
  return p;
}
