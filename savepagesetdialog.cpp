#include "savepagesetdialog.h"
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
SavePageSetDialog::SavePageSetDialog(QTabWidget * tabs,QWidget * parent) : QDialog(parent) {

  //  m_action = action;
  m_name = new QLineEdit;
  m_setlist = new QTableWidget;
  m_tablist = new QTableWidget;

  m_overwrite = new QCheckBox(tr("Do not warn when overwriting existing page set"));
  m_selectAll = new QRadioButton(tr("Select all"));
  m_selectNone = new QRadioButton(tr("Clear selections"));
  readSettings();

  connect(m_selectAll,SIGNAL(toggled(bool)),this,SLOT(selectionToggled(bool)));
  setWindowTitle(tr("Save page set"));

  QVBoxLayout * layout = new QVBoxLayout;

  QLabel * intro = new QLabel("<em>Select which tabs to include in the page set and give the page set a title</em><br/>");

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

  flayout->addRow(tr("Page set title"),m_name);

  flayout->addRow("",current);
  flayout->addRow(tr("Existing page sets"),m_setlist);
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
}
SavePageSetDialog::~SavePageSetDialog() {
  qDebug() << Q_FUNC_INFO;
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
  if (name.isEmpty()) {
    QString msg(tr("A page set title is required in order to save the selected pages."));
    QMessageBox::warning(this,tr("Empty page set title"),msg,QMessageBox::Ok);
    return;
  }
  if (! m_overwrite->isChecked()) {
    for(int i=0;i < m_setlist->rowCount();i++) {
      if (name == m_setlist->item(i,SET_TITLE_COLUMN)->text()) {
        QString msg(tr("A page set with the same title already exists.\nEither use a different title or check the overwrite box"));
        QMessageBox::warning(this,tr("Duplicate page set title"),msg,QMessageBox::Ok);
        return;
      }
    }
  }
  QList<int> t = this->requestedTabs();
  if (t.size() == 0) {
    QString msg(tr("No tabs have been selected for saving."));
    QMessageBox::warning(this,tr("No tabs selected"),msg,QMessageBox::Ok);
    return;
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

  QLOG_DEBUG() << Q_FUNC_INFO;
  /*
  HtmlDelegate * d = new HtmlDelegate(m_tablist);
  if ( ! m_spanStyle.isEmpty()) {
    d->setStyleSheet(m_spanStyle);
  }
  m_tablist->setItemDelegateForColumn(TAB_TITLE_COLUMN,d);
  */

  QMap<int,QString> hm;
  hm.insert(TAB_SAVE_COLUMN,tr("Save"));
  hm.insert(TAB_INDEX_COLUMN,tr("Tab"));
  hm.insert(TAB_TITLE_COLUMN,tr("Title"));
  hm.insert(TAB_ROOT_COLUMN,tr("Root"));
  hm.insert(TAB_WORD_COLUMN,tr("Headword"));
  hm.insert(TAB_VOLUME_COLUMN,tr("Volume/Page"));

  m_tablist->clear();
  m_tablist->setColumnCount(hm.size());
  m_tablist->verticalHeader()->setVisible(false);


  QStringList headers;
  headers << hm.values();
  m_tablist->setHorizontalHeaderLabels(headers);
  m_tablist->horizontalHeader()->setStretchLastSection(true);
  m_tablist->setSelectionMode(QAbstractItemView::SingleSelection);
  m_tablist->setSelectionBehavior(QAbstractItemView::SelectRows);
  int row = 0;
  QString html;
  QString str;
  QLabel * label;
  for(int i=0;i < tabs->count();i++) {
    GraphicsEntry * entry = qobject_cast<GraphicsEntry *>(tabs->widget(i));
    if (entry) {
      m_tablist->insertRow(row);

      m_tablist->setCellWidget(row,TAB_SAVE_COLUMN,new CenteredCheckBox);
      QString str = tabs->tabText(i);
      QRegularExpression rx("^\\s*(\\d+)\\s*");
      str = str.replace(rx,"");
      html = qobject_cast<Lexicon *>(qApp)->scanAndStyle(str,"pageset");
      label = new QLabel;
      label->setText(html);
      label->setAlignment(Qt::AlignCenter);
      m_tablist->setCellWidget(row,TAB_TITLE_COLUMN,label);

      Place p = entry->getPlace();

      m_tablist->setItem(row,TAB_INDEX_COLUMN,new QTableWidgetItem(QString("%1").arg(i+1)));

      html = qobject_cast<Lexicon *>(qApp)->scanAndStyle(p.m_root,"pageset");
      label = new QLabel(html);;
      label->setAlignment(Qt::AlignCenter);
      m_tablist->setCellWidget(row,TAB_ROOT_COLUMN,label);

      html = qobject_cast<Lexicon *>(qApp)->scanAndStyle(p.m_word,"pageset");
      label = new QLabel(html);;
      label->setAlignment(Qt::AlignCenter);
      m_tablist->setCellWidget(row,TAB_WORD_COLUMN,label);

      m_tablist->setItem(row,TAB_VOLUME_COLUMN,new QTableWidgetItem(p.format("%V/%P")));

      row++;
    }
  }
  m_tablist->resizeColumnsToContents();
}
void SavePageSetDialog::loadTitles() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  QSqlRecord rec;
  m_setlist->clear();
  m_setlist->verticalHeader()->setVisible(false);
  m_setlist->setColumnCount(4);
  QStringList headers;
  headers << tr("Id") << tr("Title") << tr("Pages") << tr("Created");
  m_setlist->setHorizontalHeaderLabels(headers);
  m_setlist->horizontalHeader()->setStretchLastSection(true);
  m_setlist->setSelectionMode(QAbstractItemView::SingleSelection);
  m_setlist->setSelectionBehavior(QAbstractItemView::SelectRows);
  connect(m_setlist,SIGNAL(itemSelectionChanged()),this,SLOT(setTitleFromTable()));
  QSqlQuery q(QSqlDatabase::database("notesdb"));
  if (! q.prepare(SQL_PAGESET_HEADERS)) {
    /// Report error
    QLOG_WARN() << QString(tr("Prepare failed for SQL_PAGESET_HEADER query:%1")).arg(q.lastError().text());
    return;
  }
  QSqlQuery p(QSqlDatabase::database("notesdb"));
  if (! p.prepare(SQL_PAGESET_PAGE_COUNT)) {
    /// Report error
    QLOG_WARN() << QString(tr("Prepare failed for SQL_PAGESET_PAGE_COUNT query:%1")).arg(p.lastError().text());
    return;
  }
  q.exec();
  int row;
  while(q.next()) {
    row = m_setlist->rowCount();
    m_setlist->insertRow(row);
    rec = q.record();

    m_setlist->setItem(row,SET_TITLE_COLUMN,new QTableWidgetItem(rec.value("title").toString()));
    m_setlist->setItem(row,SET_ID_COLUMN,new QTableWidgetItem(QString("%1").arg(rec.value("id").toInt())));

    QString d = rec.value("accessed").toString();
    m_setlist->setItem(row,SET_ACCESSED_COLUMN,new QTableWidgetItem(QString("%1").arg(d)));
    p.bindValue(0,rec.value("id").toInt());
    p.exec();
    int pages = 0;
    if (p.first()) {
      pages = p.record().value("count(id)").toInt();
    }
    m_setlist->setItem(row,SET_COUNT_COLUMN,new QTableWidgetItem(QString("%1").arg(pages)));


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
  qDebug() << items.size();
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
      QString v = m_tablist->item(i,TAB_INDEX_COLUMN)->text();
      int x = v.toInt(&ok);
      if (ok) {
      tabs << x;
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
