#include "notebrowser.h"
#ifdef LANE
#include "laneslexicon.h"
#include "graphicsentry.h"
#include "application.h"
#include "externs.h"
#include "definedsettings.h"
#endif
#include "QsLog.h"
#include "htmldelegate.h"
#include "notes.h"
#include "xsltsupport.h"
#include "noteview.h"
#include "notedialog.h"
#include "place.h"
#include "definedsql.h"
extern NoteMaster * getNotes();
extern QSettings * getSettings();
#define ID_COLUMN 0
#define COL_WITH_ID 1
#define WORD_COLUMN 1
#define AMENDED_COLUMN 2
#define DATE_COLUMN 2
#define SUBJECT_COLUMN 3
#define NOTE_COLUMN 4
#define NOTE_SUBSTR_LENGTH 30

NoteBrowser::NoteBrowser(QWidget * parent) : QWidget(parent) {
  setObjectName("notebrowser");
  readSettings();
  QVBoxLayout * layout = new QVBoxLayout;
  m_list = new QTableWidget;
  m_list->installEventFilter(this);
  HtmlDelegate * d = new HtmlDelegate;
  d->setStyleSheet(m_style);
  m_list->setItemDelegateForColumn(COL_WITH_ID,d);
  m_list->setItemDelegateForColumn(SUBJECT_COLUMN,d);

  m_list->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  QStyle * style = m_list->style();
  style->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus);
  this->setStyle(style);

  QHBoxLayout * btnlayout = new QHBoxLayout;
  m_printButton = new QPushButton(tr("&Print"));
  m_deleteButton = new QPushButton(tr("&Delete"));
  m_viewButton = new QPushButton(tr("&View entry"));

  btnlayout->addWidget(m_printButton);
  btnlayout->addWidget(m_deleteButton);
  btnlayout->addWidget(m_viewButton);
  btnlayout->addStretch();

  layout->addWidget(m_list);
  layout->addLayout(btnlayout);
  m_list->adjustSize();//resizeColumnsToContents();

  setLayout(layout);

  loadNotes();

  m_deleteButton->setEnabled(false);
  m_printButton->setEnabled(false);
  m_viewButton->setEnabled(false);

  connect(m_list,SIGNAL(cellClicked(int,int)),this,SLOT(onCellClicked(int,int)));
  connect(m_deleteButton,SIGNAL(clicked()),this,SLOT(onDeleteClicked()));
  connect(m_viewButton,SIGNAL(clicked()),this,SLOT(onViewClicked()));
  initXslt();
}
void NoteBrowser::loadNotes() {
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

  QString sql = QString(SQL_LIST_NOTES).arg(NOTE_SUBSTR_LENGTH);
  QSqlQuery  * q = notes->getNoteList(sql);
  while(q->next()) {
    QString word = q->value("word").toString();
    word = qobject_cast<Lexicon *>(qApp)->scanAndSpan(word);
    int row = m_list->rowCount();
    m_list->insertRow(row);
    item = new QTableWidgetItem(q->value("id").toString());
    m_list->setItem(row,0,item);

    item = new QTableWidgetItem(word);
    item->setData(Qt::UserRole,q->value("id").toInt());
    //    item->setFont(m_arabicFont);
    m_list->setItem(row,COL_WITH_ID,item);

    QString when;
    when = q->value("amended").toString();
    if (when.isEmpty())
        when = q->value("created").toString();

    QDateTime d = QDateTime::fromString(when);
    item = new QTableWidgetItem(d.toString( Qt::SystemLocaleShortDate));
    //      item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_list->setItem(row,2,item);
    QString subject = q->value("subject").toString();
    subject = qobject_cast<Lexicon *>(qApp)->scanAndSpan(subject);

    item = new QTableWidgetItem(subject);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    //      item->setFont(m_resultsFont);
    m_list->setItem(row,SUBJECT_COLUMN,item);

    item = new QTableWidgetItem(q->value(5).toString());
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    //      item->setFont(m_resultsFont);
    m_list->setItem(row,NOTE_COLUMN,item);
  }
  if (m_list->rowCount() > 0) {
    m_list->itemDoubleClicked(m_list->item(0,0));

  }
  /**
   * having to do this because the html elements do not appear to
   * report their size correctly (too large)
   */



  SETTINGS

  settings.beginGroup("Notes");
  if (settings.contains("Columns")) {
    bool ok;
    QList<QVariant> cols = settings.value("Columns").toList();
    if (cols.size() > 0) {
      for(int i=0;i < cols.size();i++) {
        int sz = cols[i].toInt(&ok);
        if (ok) {
          m_list->setColumnWidth(i,sz);
        }
      }
    }
  }
  else {
    m_list->resizeColumnToContents(ID_COLUMN);
    m_list->resizeColumnToContents(DATE_COLUMN);
    m_list->resizeColumnToContents(NOTE_COLUMN);
    m_list->setColumnWidth(WORD_COLUMN,settings.value("Word column",200).toInt());
    m_list->setColumnWidth(SUBJECT_COLUMN,settings.value("Subject column",300).toInt());
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
      NoteDialog * dlg = new  NoteDialog(note,this);

      //      m_subject->setText(note->getSubject());
      //      m_note->setText(note->getNote());
      //      m_type->setCurrentIndex(m_type->findData(note->getType()));
      dlg->exec();
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
  QLOG_DEBUG() << Q_FUNC_INFO;
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
    ok = query.prepare(SQL_GET_NODE_FOR_NOTE);
    if (ok) {
      query.bindValue(0,p.getNode());
      query.exec();
    }
  }
  else if (! p.getWord().isEmpty()) {
    ok = query.prepare(SQL_GET_WORD_FOR_NOTE);
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
                         p.getNode());

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
  SETTINGS

  settings.beginGroup("Notes");
  m_style = settings.value("Context style",QString()).toString();
  settings.endGroup();
  settings.beginGroup("Entry");
  QString css = settings.value(SID_ENTRY_CSS,QString("entry.css")).toString();


  readCssFromFile(css);
  settings.endGroup();
  settings.beginGroup("FullSearch");
  m_xsltSource = settings.value("XSLT",QString("node.xslt")).toString();
  m_debug = settings.value("Debug",false).toBool();
  settings.endGroup();

}
bool NoteBrowser::readCssFromFile(const QString & name) {
  QString css;
  if (name.isEmpty()) {
    return false;
  }
  QString filename = getLexicon()->getResourceFilePath(Lexicon::Stylesheet,name);
  if (filename.isEmpty()) {
    QString err = getLexicon()->takeLastError();
    QLOG_WARN() << QString(tr("Unable to open entry stylesheet: %1")).arg(err);
    return false;
  }

  QFile f(filename);
  if (! f.open(QIODevice::ReadOnly)) {
    QLOG_WARN()  << QString(tr("I/O Error opening CSS file for reading: %1 - %2"))
      .arg(filename)
      .arg(f.errorString());
    return false;
  }
  QTextStream in(&f);
  css = in.readAll();
  f.close();
  m_css = css;
  return true;
}
NoteBrowser::~NoteBrowser() {
  QList<QVariant> colwidths;
  for(int i=0;i < m_list->columnCount();i++) {
    colwidths << QVariant(m_list->columnWidth(i));
  }
  SETTINGS

  settings.beginGroup("Notes");
  settings.setValue("Columns",colwidths);
}
