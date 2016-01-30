#include "savepagesetdialog.h"
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
SavePageSetDialog::SavePageSetDialog(QTabWidget * tabs,QWidget * parent) : QDialog(parent) {

  //  m_action = action;
  m_name = new QLineEdit;

  QStringList cols;
  cols << tr("Id") << tr("Title") << tr("Tabs") << tr("Created");
  m_setlist = new ColumnarTableWidget(cols);
  m_setlist->setKey(ColumnarTableWidget::STATE,SID_PAGESET_SETLIST_STATE);

  cols.clear();
  cols << tr("Save") << tr("Tab") << tr("Title") << tr("Root") << tr("Headword") << tr("Volume/Page");


  m_tablist = new ColumnarTableWidget(cols);
  m_tablist->setKey(ColumnarTableWidget::STATE,SID_PAGESET_TABLIST_STATE);

  m_overwrite = new QCheckBox(tr("Overwrite tab set with the same title."));
  m_selectAll = new QRadioButton(tr("Select all"));
  m_selectNone = new QRadioButton(tr("Clear selections"));
  readSettings();

  connect(m_selectAll,SIGNAL(toggled(bool)),this,SLOT(selectionToggled(bool)));
  setWindowTitle(tr("Save tab set"));

  QVBoxLayout * layout = new QVBoxLayout;

  QLabel * intro = new QLabel("<em>Select which tabs to include in the tab set and give the set a title</em><br/>");

  layout->addWidget(intro);
  QFormLayout * flayout = new QFormLayout;

  QGroupBox * current = new QGroupBox(tr("Current tabs"));
  QVBoxLayout * grouplayout = new QVBoxLayout;
  grouplayout->addWidget(m_tablist);

  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addWidget(m_selectAll);
  hlayout->addWidget(m_selectNone);

  grouplayout->addLayout(hlayout);

  current->setLayout(grouplayout);

  flayout->addRow(tr("Tab set title"),m_name);

  flayout->addRow("",current);
  flayout->addRow(tr("Existing tab sets"),m_setlist);
  flayout->addRow("",m_overwrite);
  layout->addLayout(flayout);

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Save
                                     | QDialogButtonBox::Cancel);

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(onSave()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  layout->addWidget(buttonBox);

  setLayout(layout);

  this->loadTabs(tabs);
  this->loadTitles();

  m_setlist->hideColumn(SET_ID_COLUMN);


  this->selectionToggled(m_selectAll->isChecked());
  ///
  SETTINGS
  settings.beginGroup("PageSets");
  m_tablist->readConfiguration(settings);
  m_setlist->readConfiguration(settings);

}
SavePageSetDialog::~SavePageSetDialog() {
  writeSettings();
}
QString SavePageSetDialog::pageSetTitle() const {
  return m_name->text();
}
bool SavePageSetDialog::overwrite() const {
  return m_overwrite->isChecked();
}
void SavePageSetDialog::onSave() {
  QString name = m_name->text();
  QList<int> t = this->requestedTabs();
  if (t.size() == 0) {
    QString msg(tr("No tabs have been selected for saving."));
    QMessageBox::warning(this,tr("No tabs selected"),msg,QMessageBox::Ok);
    return;
  }
  if (name.isEmpty()) {
    QString msg(tr("A tab set title is required in order to save the selected tabs."));
    QMessageBox::warning(this,tr("Empty tab set title"),msg,QMessageBox::Ok);
    return;
  }
  m_overwriteId = -1;
  for(int i=0;i < m_setlist->rowCount();i++) {
    if (name == m_setlist->item(i,SET_TITLE_COLUMN)->text()) {
      if ( m_overwrite->isChecked()) {
        //        QString msg(tr("A tab set with the same title already exists.\nEither use a different title or check the overwrite box"));
        //        QMessageBox::warning(this,tr("Duplicate tab set title"),msg,QMessageBox::Ok);
        //        return;
        /// delete existing i.e this forces unique tab set titles
        m_overwriteId = m_setlist->item(i,SET_ID_COLUMN)->text().toInt();
      }
    }
  }
  this->accept();
}
#define TAB_SAVE_COLUMN 0
#define TAB_INDEX_COLUMN 1
#define TAB_TITLE_COLUMN 2
#define TAB_ROOT_COLUMN  3
#define TAB_WORD_COLUMN  4
#define TAB_VOLUME_COLUMN  5
void SavePageSetDialog::loadTabs(QTabWidget * tabs) {

  int row = 0;
  QString html;
  QString str;
  QLabel * label;
  for(int i=0;i < tabs->count();i++) {
    GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(tabs->widget(i));
    if (entry) {
      Place p = entry->getPlace();

      m_tablist->insertRow(row);

      m_tablist->setCellWidget(row,TAB_SAVE_COLUMN,new CenteredCheckBox);

      QString str = tabs->tabText(i);
      QRegularExpression rx("^\\s*(\\d+)\\s*");
      str = str.replace(rx,"");

      label = m_tablist->createLabel(str,"pageset");
      label->setAlignment(Qt::AlignCenter);
      m_tablist->setCellWidget(row,TAB_TITLE_COLUMN,label);

      /// tab titles start at 1, so increment the tab index

      label = m_tablist->createLabel(QString("%1").arg(i+1));
      label->setAlignment(Qt::AlignCenter);
      m_tablist->setCellWidget(row,TAB_INDEX_COLUMN,label);

      label = m_tablist->createLabel(p.m_root,"pageset");
      label->setAlignment(Qt::AlignCenter);
      m_tablist->setCellWidget(row,TAB_ROOT_COLUMN,label);


      label = m_tablist->createLabel(p.m_word,"pageset");
      label->setAlignment(Qt::AlignCenter);
      m_tablist->setCellWidget(row,TAB_WORD_COLUMN,label);

      label = m_tablist->createLabel(p.format("%V/%P"));
      label->setAlignment(Qt::AlignCenter);
      m_tablist->setCellWidget(row,TAB_VOLUME_COLUMN,label);

      row++;
    }
  }
  m_tablist->resizeColumnsToContents();
}
void SavePageSetDialog::loadTitles() {
  QLOG_DEBUG() << Q_FUNC_INFO;

  QSqlRecord rec;

  connect(m_setlist,SIGNAL(itemSelectionChanged()),this,SLOT(setTitleFromTable()));
  QSqlQuery q(QSqlDatabase::database("notesdb"));
  if (! q.prepare(SQL_PAGESET_HEADERS)) {
    QLOG_WARN() << QString(tr("Prepare failed for SQL_PAGESET_HEADER query:%1")).arg(q.lastError().text());
    return;
  }
  QSqlQuery p(QSqlDatabase::database("notesdb"));
  if (! p.prepare(SQL_PAGESET_PAGE_COUNT)) {
    QLOG_WARN() << QString(tr("Prepare failed for SQL_PAGESET_PAGE_COUNT query:%1")).arg(p.lastError().text());
    return;
  }
  q.exec();
  int row;
  QTableWidgetItem * item;
  while(q.next()) {
    row = m_setlist->rowCount();
    m_setlist->insertRow(row);
    rec = q.record();
    item = new QTableWidgetItem(rec.value("title").toString());
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_setlist->setItem(row,SET_TITLE_COLUMN,item);

    item = new QTableWidgetItem(QString("%1").arg(rec.value("id").toInt()));
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_setlist->setItem(row,SET_ID_COLUMN,item);

    QString d = rec.value("accessed").toString();
    item = new QTableWidgetItem(d);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_setlist->setItem(row,SET_ACCESSED_COLUMN,item);

    p.bindValue(0,rec.value("id").toInt());
    p.exec();
    int pages = 0;
    if (p.first()) {
      pages = p.record().value("count(id)").toInt();
    }
    item = new QTableWidgetItem(QString("%1").arg(pages));
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_setlist->setItem(row,SET_COUNT_COLUMN,item);
  }
  m_setlist->resizeColumnsToContents();
  return;
}
void SavePageSetDialog::readSettings() {
  SETTINGS

  settings.beginGroup("PageSets");
  resize(settings.value(SID_PAGESET_SAVE_DIALOG_SIZE, QSize(600, 400)).toSize());
  move(settings.value(SID_PAGESET_SAVE_DIALOG_POS, QPoint(200, 200)).toPoint());
  m_spanStyle = settings.value(SID_PAGESET_MAIN_CONTEXT,QString()).toString();
  m_spanStyle += " " + settings.value(SID_PAGESET_ARABIC_CONTEXT,QString()).toString();
  m_overwrite->setChecked(settings.value(SID_PAGESET_OVERWRITE_EXISTING,false).toBool());
  m_selectAll->setChecked(settings.value(SID_PAGESET_SELECT_ALL,false).toBool());

}
void SavePageSetDialog::writeSettings() {
  SETTINGS

 settings.beginGroup("PageSets");
  settings.setValue(SID_PAGESET_SAVE_DIALOG_SIZE, size());
  settings.setValue(SID_PAGESET_SAVE_DIALOG_POS, pos());
  settings.setValue(SID_PAGESET_OVERWRITE_EXISTING,m_overwrite->isChecked());
  settings.setValue(SID_PAGESET_SELECT_ALL,m_selectAll->isChecked());
}
void SavePageSetDialog::setTitleFromTable() {
  QList<QTableWidgetItem *> items = m_setlist->selectedItems();
  QString name;

  if (items.size() > 0) {
    for(int i=0;i < items.size();i++) {
      if (items[i]->column() == SET_TITLE_COLUMN) {
        name = items[i]->text();
        break;
      }
    }
  }
  m_name->setText(name);
}
QList<int> SavePageSetDialog::requestedTabs() const {
  bool ok;
  QList<int> tabs;
  for(int i=0;i < m_tablist->rowCount();i++) {
    CenteredCheckBox * b = qobject_cast<CenteredCheckBox *>(m_tablist->cellWidget(i,TAB_SAVE_COLUMN));
    if (b->isChecked()) {
      QLabel * label = qobject_cast<QLabel *>(m_tablist->cellWidget(i,TAB_INDEX_COLUMN));
      if (label) {
        QString v = label->text();
      int x = v.toInt(&ok);
      if (ok) {
        /// decrement the index we incremented
        x--;
        tabs << x;
      }
    }
  }
}
return tabs;
}
void SavePageSetDialog::selectionToggled(bool v) {

  for(int i=0;i < m_tablist->rowCount();i++) {
    CenteredCheckBox * b = qobject_cast<CenteredCheckBox *>(m_tablist->cellWidget(i,TAB_SAVE_COLUMN));
    b->setChecked(v);
  }
}
int SavePageSetDialog::overwriteId() const {
  return m_overwriteId;
}
