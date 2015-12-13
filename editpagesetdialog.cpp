#include "editpagesetdialog.h"
#include "pageentrydialog.h"
#include "definedsql.h"
#include "QsLog.h"
#include "application.h"
#include "externs.h"
#include "graphicsentry.h"
#include "definedsettings.h"
#include "centeredcheckbox.h"
#define SET_ID_COLUMN 0
#define SET_TITLE_COLUMN 1
#define SET_COUNT_COLUMN 2
#define SET_ACCESSED_COLUMN 3
#define SET_LOAD_COLUMN 4
#define SET_SELECT_COLUMN 5
#define SET_LOAD_COUNT_COLUMN 6


#define PAGE_LOAD_COLUMN 0
#define PAGE_NODE_COLUMN 1
#define PAGE_ROOT_COLUMN 2
#define PAGE_WORD_COLUMN 3
#define PAGE_VOLUME_COLUMN 4
#define PAGE_ID_COLUMN 5

EditPageSetDialog::EditPageSetDialog(QWidget * parent) : QDialog(parent) {
  qDebug() << Q_FUNC_INFO;

  m_setlist = new QTableWidget;

  readSettings();

  setWindowTitle(tr("Edit Tab Set"));

  QVBoxLayout * layout = new QVBoxLayout;


  layout->addWidget(m_setlist);


  int rows = this->loadTitles();


  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel);
  m_loadButton  = new QPushButton(tr("Apply"));


  buttonBox->addButton(m_loadButton,QDialogButtonBox::ActionRole);
  m_loadButton->setVisible(rows > 0);
  m_loadButton->setEnabled(false);


  connect(m_loadButton,SIGNAL(clicked()),this,SLOT(onApply()));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  layout->addWidget(buttonBox);

  setLayout(layout);

  connect(m_setlist,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),this,SLOT(onItemDoubleClicked(QTableWidgetItem *)));
  m_dirty = false;
}
EditPageSetDialog::~EditPageSetDialog() {
  qDebug() << Q_FUNC_INFO;
  writeSettings();
}
void EditPageSetDialog::onItemDoubleClicked(QTableWidgetItem * item) {
  bool ok = false;
  if (item->column() == SET_TITLE_COLUMN) {
     QString text = QInputDialog::getText(this, tr("Set Title"),
                                         tr("Title:"), QLineEdit::Normal,
                                         "", &ok);
     if (ok && !text.isEmpty()){
       item->setText(text);
       m_dirty = true;
       m_loadButton->setEnabled(true);
     }
  }
}
QString EditPageSetDialog::pageSetTitle() const {
  return QString();
}
int EditPageSetDialog::loadTitles() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  QSqlRecord rec;
  //  m_setlist->clear();
  m_setlist->setRowCount(0);
  m_setlist->verticalHeader()->setVisible(false);

  QMap<int,QString> hmap;
  hmap.insert(SET_ID_COLUMN,tr("Id"));
  hmap.insert(SET_TITLE_COLUMN,tr("Title"));
  hmap.insert(SET_COUNT_COLUMN,tr("Tab count"));
  hmap.insert(SET_ACCESSED_COLUMN,tr("Created"));
  hmap.insert(SET_LOAD_COLUMN,tr("Delete"));
  hmap.insert(SET_SELECT_COLUMN,tr("Select tabs"));
  hmap.insert(SET_LOAD_COUNT_COLUMN,tr("Tabs to delete"));
  m_setlist->setColumnCount(hmap.size());

  m_setlist->setHorizontalHeaderLabels(hmap.values());
  //  m_setlist->horizontalHeader()->setStretchLastSection(true);
  //  m_setlist->setSelectionMode(QAbstractItemView::SingleSelection);
  m_setlist->setSelectionBehavior(QAbstractItemView::SelectRows);

  m_setlist->horizontalHeader()->setSectionResizeMode(SET_ACCESSED_COLUMN,QHeaderView::Stretch);
  //  connect(m_setlist,SIGNAL(itemSelectionChanged()),this,SLOT(setTitleFromTable()));
  QSqlQuery q(QSqlDatabase::database("notesdb"));
  if (! q.prepare(SQL_PAGESET_HEADERS)) {
    QLOG_WARN() << QString(tr("Prepare failed for SQL_PAGESET_HEADER query:%1")).arg(q.lastError().text());
    return 0;
  }
  QSqlQuery p(QSqlDatabase::database("notesdb"));
  if (! p.prepare(SQL_PAGESET_PAGE_COUNT)) {
    QLOG_WARN() << QString(tr("Prepare failed for SQL_PAGESET_PAGE_COUNT query:%1")).arg(p.lastError().text());
    return 0;
  }
  q.exec();
  int row;

  while(q.next()) {
    rec = q.record();
    p.bindValue(0,rec.value("id").toInt());
    p.exec();

    if (p.first()) {
      int c = p.record().value("count(id)").toInt();
      if ( c > 0) {
        row = m_setlist->rowCount();
        m_setlist->insertRow(row);
        m_setlist->setItem(row,SET_COUNT_COLUMN,new QTableWidgetItem(p.record().value("count(id)").toString()));
        m_setlist->item(row,SET_COUNT_COLUMN)->setTextAlignment(Qt::AlignCenter);
        m_setlist->setItem(row,SET_TITLE_COLUMN,new QTableWidgetItem(rec.value("title").toString()));
        m_setlist->setItem(row,SET_ID_COLUMN,new QTableWidgetItem(QString("%1").arg(rec.value("id").toInt())));
        CenteredCheckBox * box = new CenteredCheckBox;
        box->setRow(row);
        connect(box,SIGNAL(stateChanged(int)),this,SLOT(onSelectAll(int)));
        m_setlist->setCellWidget(row,SET_LOAD_COLUMN,box);


        QString d = rec.value("accessed").toString();
        m_setlist->setItem(row,SET_ACCESSED_COLUMN,new QTableWidgetItem(QString("%1").arg(d)));
        m_setlist->item(row,SET_ACCESSED_COLUMN)->setTextAlignment(Qt::AlignCenter);

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
  }
  int rows = m_setlist->rowCount();
  if (rows == 0) {
    m_setlist->clear();
    m_setlist->setRowCount(0);
    m_setlist->insertRow(0);
    m_setlist->setColumnCount(1);
    m_setlist->setHorizontalHeaderLabels(QStringList() << "");
    m_setlist->horizontalHeader()->setStretchLastSection(true);
    QLabel * m = new QLabel(tr("<em>No tab sets found</em>"));
    m->setAlignment(Qt::AlignCenter);
    m_setlist->setCellWidget(0,0,m);
  }
  else {
    // set the cells to be read only
    // for the title cell, save copy of the text so we can check whether it has been changed
    // and needs saving
    QTableWidgetItem * item;
    QList<int> cols;
    cols << SET_ID_COLUMN << SET_COUNT_COLUMN << SET_TITLE_COLUMN << SET_LOAD_COUNT_COLUMN << SET_ACCESSED_COLUMN;
    for(int i=0;i < m_setlist->rowCount();i++) {
      for(int j=0;j < cols.size();j++) {
        item = m_setlist->item(i,cols[j]);
        if (! item ) {
          QLOG_DEBUG() << "Missing item row/colum" <<  i << j;
        }
        else {
          if (cols[j] == SET_TITLE_COLUMN) {
            item->setData(Qt::UserRole,item->text());
          }
          item->setFlags(item->flags() ^ Qt::ItemIsEditable);

        }
      }
    }
  }
  m_setlist->hideColumn(SET_ID_COLUMN);
  QList<int> k = m_pageDialogs.keys();
  for(int i = 0;i < k.size();i++) {
    PageEntryDialog * d = qobject_cast<PageEntryDialog *>(m_pageDialogs.take(k[i]));
    if (d) {
      delete d;
    }
  }
  m_pageDialogs.clear();
  //  m_setlist->resizeColumnsToContents();
  return rows;
}
void EditPageSetDialog::readSettings() {
  SETTINGS

  settings.beginGroup("PageSets");
  resize(settings.value(SID_PAGESET_EDIT_DIALOG_SIZE, QSize(600, 400)).toSize());
  move(settings.value(SID_PAGESET_EDIT_DIALOG_POS, QPoint(200, 200)).toPoint());
  m_spanStyle = settings.value(SID_PAGESET_MAIN_CONTEXT,QString()).toString();
  m_spanStyle += " " + settings.value(SID_PAGESET_ARABIC_CONTEXT,QString()).toString();
}
void EditPageSetDialog::writeSettings() {
  SETTINGS

 settings.beginGroup("PageSets");
  settings.setValue(SID_PAGESET_EDIT_DIALOG_SIZE, size());
  settings.setValue(SID_PAGESET_EDIT_DIALOG_POS, pos());
}
void EditPageSetDialog::onSelectPages() {
  QPushButton * btn = qobject_cast<QPushButton *>(QObject::sender());
  if (! btn ) {
    return;
  }
  int pageset = btn->property("pageset").toInt();
  PageEntryDialog * d;
  if (! m_pageDialogs.contains(pageset)) {
    d = new PageEntryDialog(pageset,this);
    d->setWindowTitle(btn->property("pagetitle").toString());
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
void EditPageSetDialog::onSelectAll(int state) {
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
int EditPageSetDialog::loadCount() const {
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
QList<int> EditPageSetDialog::pages() const {
  QList<int> p;
  int pageset;

  bool ok;

  QLOG_DEBUG() << Q_FUNC_INFO;
  QSqlQuery q(QSqlDatabase::database("notesdb"));
  if (! q.prepare(SQL_PAGESET_DETAIL)) {
    return p;
  }
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
  return p;
}
void EditPageSetDialog::onApply() {
  bool ok;
  QString str;
  for(int i=0;i < m_setlist->rowCount();i++) {
    // delete selected pagesets
    // otherwise check if the title has changed
    str = m_setlist->item(i,SET_ID_COLUMN)->text();
    int id = str.toInt(&ok);
    if (ok) {
      CenteredCheckBox * b = qobject_cast<CenteredCheckBox *>(m_setlist->cellWidget(i,SET_LOAD_COLUMN));
      if (b) {
        if (b->isChecked()) {
          this->deletePageSet(id);
        }
      }
      QTableWidgetItem * item = m_setlist->item(i,SET_TITLE_COLUMN);
      if (item->text() != item->data(Qt::UserRole).toString()) {
        QSqlQuery q(QSqlDatabase::database("notesdb"));
        if (q.prepare(SQL_PAGESET_SET_TITLE)) {
          q.bindValue(0,item->text());
          q.bindValue(1,id);
          if (q.exec()) {
            item->setData(Qt::UserRole,item->text());
            QSqlDatabase::database("notesdb").commit();
          }
        }
        else {
          QLOG_WARN() << QString(tr("Prepare failed for SQL_PAGESET_SET_TITLE:%1")).arg(q.lastError().text());
        }

      }
    }
  }
  QSqlQuery p(QSqlDatabase::database("notesdb"));
  if (! p.prepare(SQL_PAGESET_DELETE_PAGE)) {
    QLOG_WARN() << QString(tr("Prepare failed for SQL_PAGESET_DELETE_PAGE query:%1")).arg(p.lastError().text());
    return;
  }

  QList<int> pages = this->pages();
  for(int i=0;i < pages.size();i++) {
    p.bindValue(0,pages[i]);
    if (! p.exec()) {
      QLOG_WARN() << QString(tr("Exec failed for SQL_PAGESET_DELETE_PAGE query:%1")).arg(p.lastError().text());
    }
  }
  QSqlDatabase::database("notesdb").commit();
  QList<int> orphaned;
  //
  //  if we removed all the pages from a set, get rid of the header
  //
  if ( p.prepare(SQL_PAGESET_FIND_ORPHANED)) {
    p.exec();
    while(p.next()) {
      int c = p.value(0).toInt(&ok);
      if (ok) {
        orphaned << c;
      }
    }
  }
  else {
    QLOG_WARN() << QString(tr("Prepare failed for SQL_PAGESET_FIND_ORPHANED:%1")).arg(p.lastError().text());
  }
  for(int i=0;i < orphaned.size();i++) {
    this->deletePageSet(orphaned[i]);
  }
  QSqlDatabase::database("notesdb").commit();
  this->loadTitles();
  m_loadButton->setEnabled(false);
  m_dirty = false;
}
bool EditPageSetDialog::deletePageSet(int id) {
  bool ok = false;
  /// delete the header
  QSqlQuery q(QSqlDatabase::database("notesdb"));
  if (! q.prepare(SQL_PAGESET_DELETE)) {
    QLOG_WARN() << QString(tr("Prepare failed for SQL_PAGESET_DELETE query:%1")).arg(q.lastError().text());
    return ok;
  }
  q.bindValue(0,id);
  if (! q.exec()) {
    QLOG_WARN() << QString(tr("Exec failed for SQL_PAGESET_DELETE query:%1")).arg(q.lastError().text());
    return ok;
  }
  /// delete the pages
  QSqlQuery p(QSqlDatabase::database("notesdb"));
  if (! p.prepare(SQL_PAGESET_DELETE_ALL_PAGES)) {
    QLOG_WARN() << QString(tr("Prepare failed for SQL_PAGESET_DELETE_ALL_PAGES query:%1")).arg(p.lastError().text());
    return ok;
  }

  p.bindValue(0,id);
  if (! p.exec()) {
    QLOG_WARN() << QString(tr("Exec failed for SQL_PAGESET_DELETE_PAGE query:%1")).arg(p.lastError().text());
    return ok;
  }
  return true;
}
