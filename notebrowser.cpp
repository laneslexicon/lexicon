#include "notebrowser.h"
#include "laneslexicon.h"
extern LanesLexicon * getApp();
NoteBrowser::NoteBrowser(QWidget * parent) : QWidget(parent) {
  //  LanesLexicon * app = getApp();
  //  QSettings * settings = app->getSettings();
  //  settings->beginGroup("Notes");
  //  QString f = settings->value("Font",QString()).toString();
//  if (! f.isEmpty()) {
//    m_resultsFont.fromString(f);
//  }
//  delete settings;

//  NoteMaster * notes = app->notes();
  QVBoxLayout * layout = new QVBoxLayout;
  m_list = new QTableWidget;
  m_list->installEventFilter(this);
  /// id, word,subject,date
  /*
  m_list->setColumnCount(5);
  m_list->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  QStringList headers;
  headers << tr("Mark") << tr("Id") << tr("Word") << tr("Created") << tr("Subject");
  m_list->setHorizontalHeaderLabels(headers);
  m_list->horizontalHeader()->setStretchLastSection(true);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  */
  //  m_list->installEventFilter(this);
  QStyle * style = m_list->style();
  style->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus);
  this->setStyle(style);
  QSplitter * splitter = new QSplitter(Qt::Vertical);
  QWidget * container  = new QWidget(this);
  QFormLayout * containerlayout = new QFormLayout;
  m_subject = new QLineEdit;
  m_note = new QTextEdit;
  m_note->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  QHBoxLayout * btnlayout = new QHBoxLayout;
  m_printButton = new QPushButton(tr("Print"));
  m_deleteButton = new QPushButton(tr("Delete"));
  m_viewButton = new QPushButton(tr("View"));

  btnlayout->addWidget(m_printButton);
  btnlayout->addWidget(m_deleteButton);
  btnlayout->addWidget(m_viewButton);

  containerlayout->addRow(btnlayout);
  containerlayout->addRow("Subject",m_subject);
  containerlayout->addRow("Note",m_note);
  container->setLayout(containerlayout);
  container->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

  splitter->addWidget(m_list);
  splitter->addWidget(container);
  splitter->setStretchFactor(0,0);
  splitter->setStretchFactor(1,1);
  layout->addWidget(splitter);
  m_list->adjustSize();//resizeColumnsToContents();

  setLayout(layout);
  //connect(m_list,SIGNAL(currentItemChanged(QTableWidgetItem * ,QTableWidgetItem * )),
  //      this,SLOT(itemChanged(QTableWidgetItem * ,QTableWidgetItem * )));
//  connect(m_list,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
//          this,SLOT(itemDoubleClicked(QTableWidgetItem * )));
  /// show the first item in the list
  /// TODO decide who gets focus and select the first row
  /// TODO if table loses focus, change the background selection color
  loadTable();
  m_deleteButton->setEnabled(false);
  m_printButton->setEnabled(false);
  m_viewButton->setEnabled(false);

  connect(m_list,SIGNAL(cellClicked(int,int)),this,SLOT(onCellClicked(int,int)));
  connect(m_deleteButton,SIGNAL(clicked()),this,SLOT(onDeleteClicked()));

}
void NoteBrowser::loadTable() {
  LanesLexicon * app = getApp();
  NoteMaster * notes = app->notes();
  m_list->setColumnCount(5);
  m_list->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  QStringList headers;
  headers << tr("Mark") << tr("Id") << tr("Word") << tr("Created") << tr("Subject");
  m_list->setHorizontalHeaderLabels(headers);
  m_list->horizontalHeader()->setStretchLastSection(true);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);

  QTableWidgetItem * item;

  QSqlQuery q = notes->getNoteList("select id,word,subject,created,amended from notes");
  while(q.next()) {
    QString word = q.value("word").toString();
    int row = m_list->rowCount();
    m_list->insertRow(row);
    QCheckBox * check = new QCheckBox(this);
    m_list->setCellWidget(row,0,check);

    connect(check,SIGNAL(stateChanged(int)),this,SLOT(onNoteSelected(int)));
    item = new QTableWidgetItem(q.value("id").toString());
    m_list->setItem(row,1,item);

    item = new QTableWidgetItem(word);
    item->setData(Qt::UserRole,q.value("id").toInt());
    //      item->setFont(m_resultsFont);
    //      item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_list->setItem(row,2,item);

    QString when;
    when = q.value("amended").toString();
    if (when.isEmpty())
        when = q.value("created").toString();

    QDateTime d = QDateTime::fromString(when);
    item = new QTableWidgetItem(d.toString( Qt::SystemLocaleShortDate));
    //      item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_list->setItem(row,3,item);

    item = new QTableWidgetItem(q.value("subject").toString());
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    //      item->setFont(m_resultsFont);
    m_list->setItem(row,4,item);
  }
  if (m_list->rowCount() > 0) {
    m_list->itemDoubleClicked(m_list->item(0,0));
    m_list->resizeColumnsToContents();
  }
}
void NoteBrowser::onNoteSelected(int /* state */) {
  for(int i=0;i < m_list->rowCount();i++) {
    QCheckBox * box = qobject_cast<QCheckBox *>(m_list->cellWidget(i,0));
    if (box->isChecked()) {
      m_deleteButton->setEnabled(true);
      m_printButton->setEnabled(true);
      m_viewButton->setEnabled(true);
      return;
    }
  }
  m_deleteButton->setEnabled(false);
  m_printButton->setEnabled(false);
  m_viewButton->setEnabled(false);
}
void NoteBrowser::onCellClicked(int row,int /* column */) {
  qDebug() << Q_FUNC_INFO << row;
  QTableWidgetItem * item = m_list->item(row,2);
  if (item) {
    int id = item->data(Qt::UserRole).toInt();
    LanesLexicon * app = getApp();
    NoteMaster * notes = app->notes();
    Note * note = notes->findOne(id);
    if (note) {
      m_subject->setText(note->getSubject());
      m_note->setText(note->getNote());
    }
  }


}
void NoteBrowser::onDeleteClicked() {
  QList<int> ids;
  QMap<int,int> rowmap;
  for(int i=0;i < m_list->rowCount();i++) {
    QCheckBox * box = qobject_cast<QCheckBox *>(m_list->cellWidget(i,0));
    if (box->isChecked()) {
      QTableWidgetItem * item = m_list->item(i,2);
      int id =  item->data(Qt::UserRole).toInt();
      ids << id;
      rowmap.insert(id,i);
    }
  }
  if (ids.size() > 0) {
    LanesLexicon * app = getApp();
    NoteMaster * notes = app->notes();
    /**
     * get list of deleted ids, get the corresponding rows
     * from row map, sort them, and call removeRow from the highest to lowest
     */
    QList<int> d = notes->deleteNotes(ids);
    QList<int> rows;
    for(int i=0;i < d.size();i++) {
      rows << rowmap.value(d[i]);
    }
    qSort(rows);
    for(int i=rows.size() - 1;i >= 0;i--) {
      m_list->removeRow(rows[i]);
    }
    this->onNoteSelected(0);
  }

}
bool NoteBrowser::eventFilter(QObject * target,QEvent * event) {
  if ((target == m_list) && (event->type() == QEvent::KeyPress)) {
    QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
    switch (keyEvent->key()) {
    case Qt::Key_Enter :
    case Qt::Key_Space :
    case Qt::Key_Return : {
      this->onCellClicked(m_list->currentRow(),0);
      return true;
    }
    default:
      return false;
    }
  }
  return false;
}
