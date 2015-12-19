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
#include "columnselectdialog.h"
#include "columnartablewidget.h"
extern NoteMaster * getNotes();
extern QSettings * getSettings();
#define ID_COLUMN 0
#define DELETE_COLUMN 1
#define ROOT_COLUMN 2
#define WORD_COLUMN 3
#define DATE_COLUMN 4
#define SUBJECT_COLUMN 5
#define NOTE_COLUMN 6
#define VOLUME_COLUMN 7
#define NODE_COLUMN 8
#define PLACE_COLUMN 9
NoteBrowser::NoteBrowser(QWidget * parent) : QWidget(parent) {
  setObjectName("notebrowser");

  QStringList cols;
  cols << tr("id") << tr("Delete") << tr("Root") << tr("Word") << tr("Created") << tr("Subject") << tr("Note")  << tr("Volume/Page") << tr("Node") << tr("Place");

  readSettings();
  QVBoxLayout * layout = new QVBoxLayout;
  m_list = new ColumnarTableWidget(cols);
  m_list->setObjectName("notetable");
  m_list->installEventFilter(this);
  m_list->hideColumn(NODE_COLUMN);
  m_list->hideColumn(ID_COLUMN);

  m_list->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  QStyle * style = m_list->style();
  style->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus);
  this->setStyle(style);

  QHBoxLayout * btnlayout = new QHBoxLayout;
  m_printButton = new QPushButton(tr("&Print"));
  m_deleteButton = new QPushButton(tr("&Delete selected"));
  m_viewButton = new QPushButton(tr("&View entry"));
  m_viewNoteButton = new QPushButton(tr("&View note"));
  m_refreshButton = new QPushButton(tr("&Refresh"));


  btnlayout->addWidget(m_viewNoteButton);
  btnlayout->addWidget(m_viewButton);
  btnlayout->addWidget(m_printButton);
  btnlayout->addSpacing(20);
  btnlayout->addWidget(m_deleteButton);
  btnlayout->addSpacing(20);
  btnlayout->addWidget(m_refreshButton);
  btnlayout->addStretch();

  layout->addWidget(m_list);
  layout->addLayout(btnlayout);
  m_list->adjustSize();

  setLayout(layout);



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

  m_list->hideColumn(PLACE_COLUMN);


  initXslt();

  SETTINGS
  settings.beginGroup("Notes");
  m_list->readConfiguration(settings);

  loadNotes();

}

void NoteBrowser::loadNotes() {
  NoteMaster * notes = ::getNotes();
  qDebug() << ">>>>>>>>>>>>>>>> count before clear" << m_list->rowCount();
  m_list->clearContents();
  qDebug() << ">>>>>>>>>>>>>>>> count after clear" << m_list->rowCount();
  m_list->setRowCount(0);

  QTableWidgetItem * item;
  QString sql = QString(SQL_LIST_NOTES).arg(m_substrLength);
  QSqlQuery  * q = notes->getNoteList(sql);
  QLabel * l;
  QString str;
  while(q->next()) {
    int row = m_list->rowCount();
    m_list->insertRow(row);
    Place p = Place::fromString(q->value("place").toString());

    QString word = q->value("word").toString();
    word = qobject_cast<Lexicon *>(qApp)->scanAndSpan(word);

    item = new QTableWidgetItem(q->value("id").toString());
    m_list->setItem(row,ID_COLUMN,item);

    CenteredCheckBox * box = new CenteredCheckBox;
    connect(box,SIGNAL(stateChanged(int)),this,SLOT(stateChanged(int)));
    m_list->setCellWidget(row,DELETE_COLUMN,box);

    l = this->createLabel(p.root());
    l->setAlignment(l->alignment() ^ Qt::AlignHCenter);
    m_list->setCellWidget(row,ROOT_COLUMN,l);


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


    l = this->createLabel(p.node());
    l->setAlignment(l->alignment() ^ Qt::AlignHCenter);
    m_list->setCellWidget(row,NODE_COLUMN,l);

    l = this->createLabel(p.format("%V/%P"));
    l->setAlignment(l->alignment() ^ Qt::AlignHCenter);
    m_list->setCellWidget(row,VOLUME_COLUMN,l);


    item = new QTableWidgetItem(q->value("place").toString());
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_list->setItem(row,PLACE_COLUMN,item);
  }
  q->finish();
  delete q;
  this->afterLoad();
}
void NoteBrowser::afterLoad() {
  QLOG_DEBUG() << Q_FUNC_INFO << m_list->rowCount();
  if (m_list->rowCount() > 0) {
    if (m_noNotes) {
      /// first time through set some columns visible
      m_list->horizontalHeader()->showSection(DELETE_COLUMN);
      m_list->horizontalHeader()->showSection(NOTE_COLUMN);
      m_list->horizontalHeader()->showSection(SUBJECT_COLUMN);
      m_list->horizontalHeader()->showSection(NOTE_COLUMN);
    }
    m_list->selectRow(0);
    m_list->itemDoubleClicked(m_list->item(0,0));
    m_deleteButton->setEnabled(true);
    m_printButton->setEnabled(true);
    m_viewButton->setEnabled(true);
    m_viewNoteButton->setEnabled(true);
    m_noNotes = false;
    m_list->setSaveConfiguration(true);
  }
  else {
    m_noNotes = true;
    m_list->setRowCount(1);
    QLabel * l = new QLabel("<em>" + tr("There are no notes.") + "</em>");
    for(int i=0;i < m_list->columnCount();i++) {
      m_list->horizontalHeader()->hideSection(i);
    }
    l->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    m_list->showColumn(NOTE_COLUMN);
    m_list->setCellWidget(0,NOTE_COLUMN,l);
    m_deleteButton->setEnabled(false);
    m_printButton->setEnabled(false);
    m_viewButton->setEnabled(false);
    m_viewNoteButton->setEnabled(false);
    m_list->selectRow(0);
    m_list->setSaveConfiguration(false);
  }
}
void NoteBrowser::onCellDoubleClicked(int row,int /* column */) {
  QLOG_DEBUG() << Q_FUNC_INFO << row;

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
  QLOG_DEBUG() << Q_FUNC_INFO << m_list->rowCount();
  for(int row=0;row < m_list->rowCount();row++) {
    CenteredCheckBox * b = qobject_cast<CenteredCheckBox *>(m_list->cellWidget(row,DELETE_COLUMN));
    if (b && b->isChecked()) {
      d << m_list->item(row,ID_COLUMN)->text().toInt();
      Place p = Place::fromString(m_list->item(row,PLACE_COLUMN)->text());
      nodes << p.node();
      rows << row;
    }
  }
  //  qDebug() << "Ids" << d;
  //  qDebug() << "Rows" << rows;
  // qDebug() << "Nodes with notes" << nodes;
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
  this->afterLoad();
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
  QLOG_DEBUG() << Q_FUNC_INFO  << p.getNode();
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
  QLOG_DEBUG() << Q_FUNC_INFO;
  /*
  if (! m_noNotes) {
    SETTINGS
    settings.beginGroup("Notes");
    settings.setValue(SID_NOTES_COLUMN_STATE,m_list->horizontalHeader()->saveState());
  }
  */
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
  CenteredCheckBox * box = qobject_cast<CenteredCheckBox *>(sender());
  for(int row=0;row < m_list->rowCount();row++) {
    CenteredCheckBox * b = qobject_cast<CenteredCheckBox *>(m_list->cellWidget(row,DELETE_COLUMN));
    if (b == box) {
      m_list->selectRow(row);
    }
    if (b && b->isChecked()) {
      m_deleteButton->setEnabled(true);
      return;
    }
  }
  m_deleteButton->setEnabled(false);
}
