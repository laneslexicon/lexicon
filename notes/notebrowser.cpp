#include "notebrowser.h"
#ifdef LANE
#include "laneslexicon.h"
#include "graphicsentry.h"
#include "application.h"
#include "externs.h"
#include "definedsettings.h"
#endif
#include "QsLog.h"
#include "notes.h"
#include "xsltsupport.h"
#include "noteview.h"
#include "nodeview.h"
#include "notedialog.h"
#include "place.h"
#include "definedsql.h"
#include "centeredcheckbox.h"
extern NoteMaster * getNotes();
extern QSettings * getSettings();
#define ID_COLUMN 0
#define DELETE_COLUMN 1
#define WORD_COLUMN 2
#define DATE_COLUMN 3
#define SUBJECT_COLUMN 4
#define NOTE_COLUMN 5
#define NODE_COLUMN 6
#define PLACE_COLUMN 7
#define VOLUME_COLUMN 8
NoteBrowser::NoteBrowser(QWidget * parent) : QWidget(parent) {
  setObjectName("notebrowser");
  readSettings();
  QVBoxLayout * layout = new QVBoxLayout;
  m_list = new QTableWidget;
  m_list->setObjectName("notetable");
  m_list->installEventFilter(this);

  m_list->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  QStyle * style = m_list->style();
  style->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus);
  this->setStyle(style);

  QHBoxLayout * btnlayout = new QHBoxLayout;
  m_printButton = new QPushButton(tr("&Print"));
  m_deleteButton = new QPushButton(tr("&Delete"));
  m_viewButton = new QPushButton(tr("&View entry"));
  m_viewNoteButton = new QPushButton(tr("&View note"));
  m_refreshButton = new QPushButton(tr("&Refresh"));

  btnlayout->addWidget(m_printButton);
  btnlayout->addWidget(m_deleteButton);
  btnlayout->addWidget(m_viewNoteButton);
  btnlayout->addWidget(m_refreshButton);
  btnlayout->addSpacing(20);
  btnlayout->addWidget(m_viewButton);
  btnlayout->addStretch();

  layout->addWidget(m_list);
  layout->addLayout(btnlayout);
  m_list->adjustSize();

  setLayout(layout);

  loadNotes();

  m_deleteButton->setEnabled(false);
  m_printButton->setEnabled(false);
  m_viewButton->setEnabled(false);
  m_viewNoteButton->setEnabled(false);

  connect(m_list,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(onCellDoubleClicked(int,int)));
  connect(m_list,SIGNAL(cellClicked(int,int)),this,SLOT(onCellClicked(int,int)));
  connect(m_deleteButton,SIGNAL(clicked()),this,SLOT(onDeleteClicked()));
  connect(m_viewButton,SIGNAL(clicked()),this,SLOT(onViewEntryClicked()));
  connect(m_viewNoteButton,SIGNAL(clicked()),this,SLOT(onViewNoteClicked()));
  connect(m_refreshButton,SIGNAL(clicked()),this,SLOT(loadNotes()));

  if (m_debug) {
    m_list->setColumnWidth(NODE_COLUMN,50);
    m_list->setColumnWidth(ID_COLUMN,50);
    m_list->showColumn(NODE_COLUMN);
    m_list->showColumn(ID_COLUMN);
  }
  else {
    m_list->hideColumn(NODE_COLUMN);
    m_list->hideColumn(ID_COLUMN);
  }
  m_list->hideColumn(PLACE_COLUMN);


  initXslt();
  if (m_list->rowCount() > 0) {
    m_list->selectRow(0);
    m_list->itemDoubleClicked(m_list->item(0,0));
    m_list->setFocusPolicy(Qt::StrongFocus);
    m_list->setFocus();
  }
}
void NoteBrowser::loadNotes() {
  NoteMaster * notes = ::getNotes();
  m_list->setRowCount(0);
  m_list->setColumnCount(9);
  m_list->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  QStringList headers;
  headers << tr("Id") << tr("Delete") << tr("Word") << tr("Date") << tr("Subject") << tr("Note") << tr("Node") << tr("Place") << tr("Volume/Page");
  m_list->setHorizontalHeaderLabels(headers);
  m_list->horizontalHeader()->setStretchLastSection(true);
  //  m_list->setSelectionMode(QAbstractItemView::SingleSelection);

  QTableWidgetItem * item;

  QString sql = QString(SQL_LIST_NOTES).arg(m_substrLength);
  QSqlQuery  * q = notes->getNoteList(sql);
  QLabel * l;
  QString str;
  while(q->next()) {
    int row = m_list->rowCount();
    m_list->insertRow(row);

    QString word = q->value("word").toString();
    word = qobject_cast<Lexicon *>(qApp)->scanAndSpan(word);

    item = new QTableWidgetItem(q->value("id").toString());
    m_list->setItem(row,ID_COLUMN,item);

    CenteredCheckBox * box = new CenteredCheckBox;
    connect(box,SIGNAL(stateChanged(int)),this,SLOT(stateChanged(int)));
    m_list->setCellWidget(row,DELETE_COLUMN,box);

    l = this->createLabel(word);
    l->setAlignment(l->alignment() ^ Qt::AlignHCenter);
    m_list->setCellWidget(row,WORD_COLUMN,l);

    QString when;
    when = q->value("amended").toString();
    if (when.isEmpty())
        when = q->value("created").toString();

    QDateTime d = QDateTime::fromString(when);
    l = this->createLabel(d.toString( Qt::SystemLocaleShortDate));
    m_list->setCellWidget(row,DATE_COLUMN,l);
    l->setAlignment(l->alignment() ^ Qt::AlignHCenter);

    l = this->createLabel(q->value("subject").toString());
    l->setAlignment(l->alignment() ^ Qt::AlignHCenter);
    m_list->setCellWidget(row,SUBJECT_COLUMN,l);

    /// this is the substr of the note
    l = this->createLabel(q->value(5).toString());
    m_list->setCellWidget(row,NOTE_COLUMN,l);

    Place p = Place::fromString(q->value("place").toString());
    item = new QTableWidgetItem(p.getNode());
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_list->setItem(row,NODE_COLUMN,item);

    item = new QTableWidgetItem(p.format("%V/%P"));
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_list->setItem(row,VOLUME_COLUMN,item);


    item = new QTableWidgetItem(q->value("place").toString());
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_list->setItem(row,PLACE_COLUMN,item);
  }
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
    m_list->resizeColumnToContents(NODE_COLUMN);
    m_list->setColumnWidth(WORD_COLUMN,settings.value(SID_NOTES_WORD_COLUMN,200).toInt());
    m_list->setColumnWidth(SUBJECT_COLUMN,settings.value(SID_NOTES_SUBJECT_COLUMN,300).toInt());
  }
  m_list->resizeColumnToContents(DELETE_COLUMN);
  q->finish();
  delete q;
}
void NoteBrowser::onCellDoubleClicked(int row,int /* column */) {
  QLOG_DEBUG() << Q_FUNC_INFO << row;
  /// saving the id with word because we may not include id in release
  QTableWidgetItem * item = m_list->item(row,ID_COLUMN);
  if (item) {
    bool ok;
    int id = item->text().toInt(&ok);
    if (!ok) {
      QLOG_WARN() << QString(tr("Notebrowser warning: row %1 has invalid id:%2")).arg(row).arg(id);
      return;
    }
    NoteMaster * notes = ::getNotes();
    Note * note = notes->findOne(id);
    if (note) {
      NoteDialog * dlg = new  NoteDialog(note,this);
      dlg->show();
    }
  }
  m_deleteButton->setEnabled(true);
  m_printButton->setEnabled(true);
  m_viewButton->setEnabled(true);
  m_viewNoteButton->setEnabled(true);
}
void NoteBrowser::onCellClicked(int row,int /* column */) {
  QLOG_DEBUG() << Q_FUNC_INFO << row;
  m_printButton->setEnabled(true);
  m_viewButton->setEnabled(true);
  m_viewNoteButton->setEnabled(true);
}
void NoteBrowser::onDeleteClicked() {
  QList<int> d;      // list of ids to delete
  QStringList nodes; // list of nodes to with deleted notes
  QList<int> rows;   // rows to delete
  for(int row=0;row < m_list->rowCount();row++) {
    CenteredCheckBox * b = qobject_cast<CenteredCheckBox *>(m_list->cellWidget(row,DELETE_COLUMN));
    if (b && b->isChecked()) {
      d << m_list->item(row,ID_COLUMN)->text().toInt();
      nodes << m_list->item(row,NODE_COLUMN)->text();
      rows << row;
    }
  }
  //  qDebug() << "Ids" << d;
  //  qDebug() << "Rows" << rows;
  //  qDebug() << "Nodes" << nodes;
  if (d.size() > 0) {
    NoteMaster * notes = ::getNotes();
    QList<int> deleted = notes->deleteNotes(d);
    for(int i=rows.size() - 1;i >= 0;i--) {
      m_list->removeRow(rows[i]);
    }
    statusMessage(QString(tr("Deleted %1 %2")).arg(deleted.size()).arg(deleted.size() == 1 ? "note" : "notes"));
    emit(noteDeleted(nodes));
  }
  m_deleteButton->setEnabled(false);
}
bool NoteBrowser::eventFilter(QObject * target,QEvent * event) {
  if ((target == m_list) && (event->type() == QEvent::KeyPress)) {
    QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
    switch (keyEvent->key()) {
    case Qt::Key_Enter :
    case Qt::Key_Space :
    case Qt::Key_Return : {
      this->onCellDoubleClicked(m_list->currentRow(),0);
      return true;
    }
    default:
      return false;
    }
  }
  return false;
}
void NoteBrowser::onViewNoteClicked() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  /// saving the id with word because we may not include id in release
  Place p;
  QList<QTableWidgetItem *> items = m_list->selectedItems();
  if (items.size() == 0) {
    return;
  }
  int row = items[0]->row();


  QTableWidgetItem * item = m_list->item(row,ID_COLUMN);
  if (item) {
    int id = item->text().toInt();
    NoteMaster * notes = ::getNotes();
    Note * note = notes->findOne(id);
    if (note) {
      NoteDialog * dlg = new  NoteDialog(note,this);
      dlg->show();
    }
  }
}
void NoteBrowser::onViewEntryClicked() {
  QLOG_DEBUG() << Q_FUNC_INFO;

  Place p;
  QList<QTableWidgetItem *> items = m_list->selectedItems();
  if (items.size() > 0) {
    int row = items[0]->row();
    p = Place::fromString(m_list->item(row,PLACE_COLUMN)->text());
    if (p.isValid()) {
      showEntry(p);
    }
    else {
      QLOG_WARN() << QString(tr("Invalid place string:")).arg(m_list->item(row,PLACE_COLUMN)->text());
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
      NodeView * v = new NodeView(this);
      connect(v,SIGNAL(openNode(const QString &)),this,SIGNAL(showNode(const QString &)));
      connect(v,SIGNAL(printNode(const QString &)),this,SIGNAL(printNode(const QString &)));
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
    QLOG_WARN() << QString(tr("Error preparing node query:%1")).arg(query.lastError().text());
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

void NoteBrowser::readSettings() {
  SETTINGS

  settings.beginGroup("Notes");
  m_debug = settings.value(SID_NOTES_DEBUG,false).toBool();
  m_style = settings.value(SID_NOTES_CONTEXT_STYLE,QString()).toString();
  m_substrLength = settings.value(SID_NOTES_SUBSTR_LENGTH,50).toInt();

  settings.endGroup();

  settings.beginGroup("Entry");
  QString css = settings.value(SID_ENTRY_CSS,QString("entry.css")).toString();
  readCssFromFile(css);
  settings.endGroup();

  settings.beginGroup("XSLT");
  m_xsltSource = settings.value(SID_XSLT_NODE,QString("node.xslt")).toString();
  m_xsltSource = getLexicon()->getResourceFilePath(Lexicon::XSLT,m_xsltSource);
  settings.endGroup();

}
bool NoteBrowser::readCssFromFile(const QString & name) {
  QString css;
  if (name.isEmpty()) {
    return false;
  }
  QString filename = getLexicon()->getResourceFilePath(Lexicon::Stylesheet,name);
   if (filename.startsWith("Error:")) {
    QStringList errors = filename.split(":");
    QString msg;
    if (errors.size() >= 2) {

      msg = QString(tr("<p>Cannot find file: %1</p> \
                        <p>Directory is:%2</p> \
                        <p>Please review Preferences -> Layout</p>")).arg(errors[2]).arg(errors[1]);
    }
    else {
      msg = QString(tr("Cannot find file: %1")).arg(name);
    }
    QMessageBox::warning(this,tr("Missing Entry Stylesheet"),msg,QMessageBox::Ok);
    QLOG_WARN() << QString(tr("Missing Entry CSS  file: %1")).arg(name);
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
  settings.setValue(SID_NOTES_COLUMNS,colwidths);
}
bool NoteBrowser::startsWithArabic(const QString & t) const {
  for(int i=0;i < t.size();i++) {
    if (t.at(i).direction() == QChar::DirAL) {
      return true;
    }
    if (t.at(i).direction() == QChar::DirL) {
      return false;
    }

  }
  return false;
}
QLabel * NoteBrowser::createLabel(const QString & text,bool removeLineBreaks) const {
  QString str = text;
  if (removeLineBreaks) {
   str = str.replace(QRegularExpression("\\r|\\n")," ");
  }

  QLabel * l = new QLabel(qobject_cast<Lexicon *>(qApp)->scanAndStyle(str,"notebrowser"));
  if (this->startsWithArabic(str)) {
    l->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  }
  else {
    l->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  }
  return l;
}
void NoteBrowser::stateChanged(int /* state */) {
  for(int row=0;row < m_list->rowCount();row++) {
    CenteredCheckBox * b = qobject_cast<CenteredCheckBox *>(m_list->cellWidget(row,DELETE_COLUMN));
    if (b && b->isChecked()) {
      m_deleteButton->setEnabled(true);
      return;
    }
  }
  m_deleteButton->setEnabled(false);
}
