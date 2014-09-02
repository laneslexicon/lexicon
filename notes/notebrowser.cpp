#include "notebrowser.h"
#ifndef TEST_FRAMEWORK
#include "laneslexicon.h"
#include "graphicsentry.h"
#endif
#include "QsLog.h"
#include "notes.h"
#include "xsltsupport.h"
#include "noteview.h"
extern NoteMaster * getNotes();
extern QSettings * getSettings();
#define COL_WITH_ID 1
#define NOTE_SUBSTR_LENGTH 30

NoteBrowser::NoteBrowser(QWidget * parent) : QWidget(parent) {
  setObjectName("notebrowser");
  readSettings();
  QVBoxLayout * layout = new QVBoxLayout;
  m_list = new QTableWidget;
  m_list->installEventFilter(this);
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

  QSizePolicy policy = m_note->sizePolicy();
  policy.setVerticalStretch(1);
  m_note->setSizePolicy(policy);

  QHBoxLayout * btnlayout = new QHBoxLayout;
  m_printButton = new QPushButton(tr("&Print"));
  m_deleteButton = new QPushButton(tr("&Delete"));
  m_viewButton = new QPushButton(tr("&View entry"));

  btnlayout->addWidget(m_printButton);
  btnlayout->addWidget(m_deleteButton);
  btnlayout->addWidget(m_viewButton);


  containerlayout->addRow("Subject",m_subject);
  containerlayout->addRow("Note",m_note);
  containerlayout->addRow(btnlayout);
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
  connect(m_viewButton,SIGNAL(clicked()),this,SLOT(onViewClicked()));
  initXslt();
}
void NoteBrowser::loadTable() {
  NoteMaster * notes = ::getNotes();
  m_list->setColumnCount(5);
  m_list->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  QStringList headers;
  headers << tr("Id") << tr("Word") << tr("Date") << tr("Subject") << tr("Note");
  m_list->setHorizontalHeaderLabels(headers);
  m_list->horizontalHeader()->setStretchLastSection(true);
  //  m_list->setSelectionMode(QAbstractItemView::SingleSelection);

  QTableWidgetItem * item;

  QString sql = QString("select id,word,subject,created,amended,substr(note,1,%1) from notes").arg(NOTE_SUBSTR_LENGTH);
  QSqlQuery  * q = notes->getNoteList(sql);
  while(q->next()) {
    QString word = q->value("word").toString();
    int row = m_list->rowCount();
    m_list->insertRow(row);
    item = new QTableWidgetItem(q->value("id").toString());
    m_list->setItem(row,0,item);

    item = new QTableWidgetItem(word);
    item->setData(Qt::UserRole,q->value("id").toInt());
    //      item->setFont(m_resultsFont);
    //      item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_list->setItem(row,COL_WITH_ID,item);

    QString when;
    when = q->value("amended").toString();
    if (when.isEmpty())
        when = q->value("created").toString();

    QDateTime d = QDateTime::fromString(when);
    item = new QTableWidgetItem(d.toString( Qt::SystemLocaleShortDate));
    //      item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_list->setItem(row,2,item);

    item = new QTableWidgetItem(q->value("subject").toString());
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    //      item->setFont(m_resultsFont);
    m_list->setItem(row,3,item);

    item = new QTableWidgetItem(q->value(5).toString());
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    //      item->setFont(m_resultsFont);
    m_list->setItem(row,4,item);
  }
  if (m_list->rowCount() > 0) {
    m_list->itemDoubleClicked(m_list->item(0,0));
    m_list->resizeColumnsToContents();
  }
  q->finish();
  delete q;
}
void NoteBrowser::onCellClicked(int row,int /* column */) {
  QLOG_DEBUG() << Q_FUNC_INFO << row;
  /// saving the id with word because we may not include id in release
  QTableWidgetItem * item = m_list->item(row,COL_WITH_ID);
  if (item) {
    int id = item->data(Qt::UserRole).toInt();
    NoteMaster * notes = ::getNotes();
    Note * note = notes->findOne(id);
    if (note) {
      m_subject->setText(note->getSubject());
      m_note->setText(note->getNote());
    }
  }
  m_deleteButton->setEnabled(true);
  m_printButton->setEnabled(true);
  m_viewButton->setEnabled(true);
}
/**
 * returns of map of selected id/row combinations
 *
 *
 * @return
 */
QMap<int,int> NoteBrowser::getRowIdMap() {
  QMap<int,int> rowmap;
  QList<QTableWidgetItem *> items = m_list->selectedItems();
  for(int i=0;i < items.size();i++) {
    int row = items[i]->row();
    QTableWidgetItem * item = m_list->item(row,COL_WITH_ID);
    if (item) {
      int id = item->data(Qt::UserRole).toInt();
      rowmap.insert(id,row);
    }
  }
  return rowmap;
}
void NoteBrowser::onDeleteClicked() {
  QMap<int,int> rowmap = getRowIdMap();

  if (rowmap.size() > 0) {
    NoteMaster * notes = ::getNotes();
    /**
     * get list of deleted ids, get the corresponding rows
     * from row map, sort them, and call removeRow from the highest to lowest
     */
    QList<int> d = notes->deleteNotes(rowmap.keys());
    QList<int> rows;
    for(int i=0;i < d.size();i++) {
      rows << rowmap.value(d[i]);
    }
    qSort(rows);
    for(int i=rows.size() - 1;i >= 0;i--) {
      m_list->removeRow(rows[i]);
    }
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
void NoteBrowser::onViewClicked() {
  qDebug() << Q_FUNC_INFO;
  bool ok = false;
  QMap<int,int> rowmap = getRowIdMap();

  if (rowmap.size() == 0)
    return;

  NoteMaster * notes = ::getNotes();
  QList<int> ids = rowmap.keys();

  for(int i=0;i < ids.size();i++) {
    Note * n = notes->findOne(ids[i]);
    if (n) {
      showEntry(n->getPlace());
    }
  }
}
void NoteBrowser::showEntry(const Place & p) {
  bool ok = false;
  QLOG_DEBUG() << Q_FUNC_INFO  << p.getNode() << "root" << p.getRoot() << "word" << p.getWord();
  QSqlQuery query;
  if (! p.getNode().isEmpty()) {
    ok = query.prepare("select root,word,xml from entry where datasource = 1 and nodeId = ?");
    if (ok) {
      query.bindValue(0,p.getNode());
      query.exec();
    }
  }
  else if (! p.getWord().isEmpty()) {
    ok = query.prepare("select root,word,xml from entry where datasource = 1 and word = ?");
    if (ok) {
      query.bindValue(0,p.getWord());
      query.exec();
    }
  }
  if (ok) {
    query.first();
    QString html = transform(query.value(2).toString());
    if (! html.isEmpty()) {
            NoteView * v = new NoteView(this);
            v->setWindowTitle(p.getShortText());
            v->setHeader(query.value(0).toString(),
                         query.value(1).toString(),
                         query.value(2).toString());

            v->setCSS(m_css);
            v->setHtml(html);
            v->show();
            v->raise();
            v->activateWindow();
    }
  }
  else {
    QLOG_WARN() << "Error preparing node query" << query.lastError().text();
    return;
  }
}

QString NoteBrowser::transform(const QString & xml) {
  int ok = compileStylesheet(1,m_xsltSource);
  if (ok == 0) {
    QString html = xsltTransform(1,xml);
    if (! html.isEmpty()) {
      return html;
    }
  }
  /// could be errors in stylesheet or in the xml
  QStringList errors = getParseErrors();
  if (ok != 0) {
    errors.prepend("Errors when processing stylesheet:");
  }
  else {
    errors.prepend("Errors when processing entry:");
  }
  QMessageBox msgBox;
  msgBox.setText(errors.join("\n"));
  msgBox.exec();
  clearParseErrors();
  return QString();
}
/// TODO change group
void NoteBrowser::readSettings() {
  QSettings * settings = ::getSettings();
  bool ok;
  settings->setIniCodec("UTF-8");
  settings->beginGroup("Notes");
   QString f = settings->value("Font",QString()).toString();
  if (! f.isEmpty()) {
    //    m_resultsFont.fromString(f);
  }
  settings->endGroup();
  settings->beginGroup("Entry");
  QString css = settings->value("CSS",QString("entry.css")).toString();
  readCssFromFile(css);
  settings->endGroup();
  settings->beginGroup("FullSearch");
  m_xsltSource = settings->value("XSLT",QString("node.xslt")).toString();
  m_debug = settings->value("Debug",false).toBool();
  settings->endGroup();
  delete settings;
}
bool NoteBrowser::readCssFromFile(const QString & name) {
  QFile f(name);
  if (! f.open(QIODevice::ReadOnly)) {
    QLOG_WARN()  << "Cannot open CSS file for reading: " << name
                 << f.errorString();
    return false;

  }
  QTextStream in(&f);
  QString css;
  QString t;
  while( ! in.atEnd()) {
    t = in.readLine();
    if (! t.startsWith("-")) {
      css += t;
    }
  }
  f.close();
  if (! css.isEmpty()) {
    m_css = css;
  }
  return true;
}
