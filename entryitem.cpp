#include "QsLog.h"
#include "xsltsupport.h"
#include "history.h"
#include "place.h"
#include "notedialog.h"
#include "notes.h"
#include "laneslexicon.h"
#include "entryitem.h"
#include "contentswidget.h"
extern LanesLexicon * getApp();
extern NoteMaster * getNotes();
EntryItem::EntryItem(const QString & text, QGraphicsItem * parent) : QGraphicsTextItem(text,parent) {
  m_focusOnHover = false;
  m_note = NULL;
  m_noteWidget = NULL;
  setObjectName("entryitem");
  setFlag(QGraphicsItem::ItemIsSelectable,true);
  setFlag(QGraphicsItem::ItemIsFocusable,true);
  setAcceptDrops(true);
  m_backgroundColorName = "yellow";
}
EntryItem::EntryItem(QGraphicsItem * parent) :QGraphicsTextItem(parent) {
  m_focusOnHover = false;
  m_note = NULL;
  m_noteWidget = NULL;
  setObjectName("entryitem");
  setFlag(QGraphicsItem::ItemIsSelectable,true);
  setFlag(QGraphicsItem::ItemIsFocusable,true);
  setAcceptDrops(true);
  m_backgroundColorName = "yellow";
}
/**
 * the note dialog does not have a QWidget parent so delete it manually
 *
 */
EntryItem::~EntryItem() {
  if (m_note != NULL) {
    m_note->close();
    delete m_note;
    m_note = NULL;
  }
  while(m_notes.size() > 0) {
    Note * n = m_notes.takeFirst();
    delete n;
  }
  /// TODO Check proxywidget are delete automatically
}
void EntryItem::setHighlightColor(const QString & name) {
  m_backgroundColorName = name;
}
void EntryItem::setOutputHtml(const QString & html) {
  m_html = html;
}
QString EntryItem::getNode() {
  return m_place.getNode();
}
QString EntryItem::getRoot() {
  return m_place.getRoot();
}
QString EntryItem::getWord() {
  return m_place.getWord();
}
bool EntryItem::isRoot() {
  return m_place.isRoot();
}
int EntryItem::getSupplement() {
  return m_place.getSupplement();
}
int EntryItem::getPage() {
  return m_place.getPage();
}
void EntryItem::setProxy(QGraphicsWidget * widget ) {
  m_noteWidget = widget;
}
void EntryItem::setNotes() {
  m_notes.clear();
  NoteMaster * m = ::getNotes();
  //  m_notes = m->find(m_place.getWord());
  m_notes = m->findByNode(m_place.getNode());
}
void EntryItem::setXml(const QString & xml) {
  m_xml = xml;
}
QString EntryItem::getXml() const {
  return m_xml;
}
bool EntryItem::hasNotes() const {
  if (m_notes.size() > 0)
    return true;

  return false;
}
/*
This is what a text fragment looks like:
<html><head><meta name="qrichtext" content="1" /><style type="text/css">
p, li { white-space: pre-wrap; }
</style></head><body>
<p dir='rtl' style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;"><!--StartFragment--><a href="127.0.0.0?nolink=11"><span style=" font-family:'Amiri'; font-size:18px; text-decoration: underline; color:#ff0000;">شَآمِيَةٌ</span></a><span style=" font-family:'Droid Sans'; font-size:12px;">‎</span><!--EndFragment--></p></body></html>"
*/
void EntryItem::contextMenuEvent(QGraphicsSceneContextMenuEvent * event ) {
  //  QGraphicsTextItem::contextMenuEvent(event);
  QString href;
  QString anchor;
  QAction *jumpAction;
  QAction *addNoteAction;
  QAction *deleteNoteAction;
  QAction *showNoteAction;

  QTextCursor c = textCursor();
  c.setPosition(document()->documentLayout()->hitTest(event->pos(), Qt::FuzzyHit));
  c.select(QTextCursor::WordUnderCursor);

  QString f = c.selection().toHtml();
  QRegExp rx("<!--StartFragment--><a\\s+href=\"\\d+\\.\\d+\\.\\d+\\.\\d+\\?([^\"]+)\">");
  if (rx.indexIn(f) != -1) {
    href = rx.cap(1);
    anchor = c.selectedText();
  }
  QMenu menu(m_place.getShortText());
  menu.setObjectName("entrycontextmenu");
  menu.addSection(tr("Current entry"));
  if ( href.startsWith("golink")) {
    QString t = QString("Where's this: %1").arg(anchor.remove("↓"));
    jumpAction = menu.addAction(t);
    jumpAction->setData(href);
  }
  if ( href.startsWith("nolink")) {
    QString t = QString("Unknown cross reference: %1").arg(anchor);
    jumpAction = menu.addAction(t);
    jumpAction->setData(href);
  }
  if (m_notesEnabled) {
    if (this->hasNotes()) {
      showNoteAction = menu.addAction(tr("Sho&w note"));
      deleteNoteAction = menu.addAction(tr("&Delete note"));
    }
    else {
      addNoteAction = menu.addAction(tr("&Add note"));
    }
  }
  QAction *markAction = menu.addAction(tr("&Bookmark"));
  //  QAction *searchAction = menu.addAction("Find");
  //  connect(searchAction,SIGNAL(triggered()),this,SLOT(searchItem()));
  QAction *selectAction;// = menu.addAction(tr("&Select"));
  QAction *printAction = menu.addAction(tr("&Print"));

  QAction *copyAction = menu.addAction("&Copy");
  connect(copyAction,SIGNAL(triggered()),this,SIGNAL(copy()));
  copyAction->setEnabled(this->textCursor().hasSelection());

  if (this->textCursor().hasSelection()) {
    //    QAction *copyAction = menu.addAction("&Copy");
    //    connect(copyAction,SIGNAL(triggered()),this,SIGNAL(copy()));
    QAction *clearCurrentAction = menu.addAction(tr("Clear &selection"));
    connect(clearCurrentAction,SIGNAL(triggered()),this,SLOT(clearSelection()));
  }
  else {
    selectAction = menu.addAction(tr("&Select current entry"));
  }
  QAction * perseusAction = 0;
  /// TODO make this debug/advanced ?
  perseusAction = menu.addAction("Show &XML");
  QAction * htmlAction = 0;
  if (! m_xml.isEmpty()) {
     htmlAction = menu.addAction("Show &Html");
  }
  menu.addSection(tr("All entries"));
  QAction *selectAllAction = menu.addAction(tr("S&elect"));
  QAction *printAllAction = menu.addAction(tr("Pri&nt"));
  QAction *localSearchAction = menu.addAction(tr("&Local search"));
  connect(printAllAction,SIGNAL(triggered()),this,SIGNAL(printPage()));
  if (this->textCursor().hasSelection()) {
    QAction *clearAllAction = menu.addAction(tr("C&lear selections"));
    connect(clearAllAction,SIGNAL(triggered()),this,SIGNAL(clearAllItems()));
  }
  Place p = this->getPlace();
  QAction *selectedAction = menu.exec(event->screenPos());
  if (selectedAction == perseusAction) {
    emit(showXml(p));
  }
  if (selectedAction == localSearchAction) {
    emit(searchPage());
  }
  else if (selectedAction == markAction) {
    bool ok;
    QString title = tr("Add bookmark to ") + p.getShortText();
    QString text = QInputDialog::getText(0, title,
                                         tr("Bookmark ID:"), QLineEdit::Normal,
                                         QString(), &ok);
    if (ok && !text.isEmpty()) {
      emit(bookmarkAdd(text,p));
    }

  }
  else if (selectedAction == selectAction) {
    this->selectAll();
  }
  else if (selectedAction == selectAllAction) {
    emit(selectAllItems());
  }
  else if (selectedAction == addNoteAction) {
    this->addNote();

  }
  else if (selectedAction == showNoteAction) {
    this->showNote();
  }
  else if (selectedAction == deleteNoteAction) {
    this->deleteNote();
  }
  else if ((jumpAction != NULL) && (selectedAction == jumpAction)) {
    //    QLOG_DEBUG() << "bookmark goto" << jumpAction->data();
    //    Place p;
    //    p.setNode(jumpAction->data().toString());
    /// TODO check boolean values
    //    emit(gotoNode(p,true,true));
    emit(showLinkDetails(jumpAction->data().toString()));

  }
  else if (htmlAction && (selectedAction == htmlAction)) {
    QLOG_DEBUG() << "Emit html action";
    emit(showHtml());
  }
  else if (selectedAction == printAction) {
    QLOG_DEBUG() << "sending printNode signal" << m_place.getNode();
    emit(printNode(m_place.getNode()));
  }
  this->setFocus();
}
void EntryItem::selectAll() {
    QTextCursor c = this->textCursor();
    c.select(QTextCursor::Document);
    this->setTextCursor(c);
}
void EntryItem::clearSelection() {
  QTextCursor c = this->textCursor();
  c.clearSelection();
  this->setTextCursor(c);
}
void EntryItem::setPlace(const Place & p) {
 m_place = p;
 setToolTip(m_place.getText());
 }
void EntryItem::searchItem() {
  QLOG_DEBUG() << Q_FUNC_INFO;
}
void EntryItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event) {
  if (m_focusOnHover)
    scene()->setFocusItem(this);
  QGraphicsTextItem::hoverEnterEvent(event);
}
void EntryItem::focusInEvent(QFocusEvent * event) {
  //  QLOG_DEBUG() << Q_FUNC_INFO << event->reason();
  /// when GraphicsEntry sets the current focus via
  /// scene->setFocusItem, we don't emit the signal
  ///
  if (event->reason() != Qt::OtherFocusReason) {
    emit(placeChanged(this->getPlace()));
  }
  QGraphicsTextItem::focusInEvent(event);
}
void EntryItem::focusOutEvent(QFocusEvent * event) {
  QGraphicsTextItem::focusOutEvent(event);
}
/**
 * can remove this
 *
 * @param painter
 * @param o
 * @param w
 */
void EntryItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *o, QWidget *w) {
  QGraphicsTextItem::paint(painter, o, w);
}
void EntryItem::showHighlights() {
  QTextCursor c = this->textCursor();
  for(int i=0;i < m_highlights.size();i++) {
    c.setPosition(m_highlights[i]);
    c.select(QTextCursor::WordUnderCursor);
    QTextCharFormat fmt = c.charFormat();

    fmt.setBackground(QColor(m_backgroundColorName));
    c.setCharFormat(fmt);
    this->setTextCursor(c);
  }
}
int EntryItem::showHighlight(int index) {
  QTextCursor c = this->textCursor();
  QString t;
  if ((index >= 0) && (index < m_highlights.size())) {
    c.setPosition(m_highlights[index]);
    t = m_finds.value(m_highlights[index]);
    c.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor,t.size());
    QTextCharFormat fmt = c.charFormat();
    fmt.setBackground(QColor(m_backgroundColorName));
    c.setCharFormat(fmt);
    c.clearSelection();
    this->setTextCursor(c);
    return m_highlights[index];
  }
  return -1;
}
void EntryItem::clearHighlights(bool keepResults) {

  QString t;
  if (! m_html.isEmpty()) {
    this->document()->setHtml(m_html);
  }
  if (! keepResults ) {
    m_highlights.clear();
    m_finds.clear();
  }
}
void EntryItem::setWord(const QString & word) {
  m_place.setWord(word);
}
void EntryItem::setRoot(const QString & root,bool isroot) {
  m_place.setRoot(root);
  //  m_place.setIsRoot(isroot);
}
void EntryItem::setNode(const QString & node) {
  m_place.setNode(node);
}
void EntryItem::setSupplement(int supplement) {
  m_place.setSupplement(supplement);
}
void EntryItem::setPage(const int page) {
  m_place.setPage(page);
}
Place EntryItem::getPlace() {
  return m_place;
}
void EntryItem::addNote() {
  if (m_note == NULL) {
    m_note = new NoteDialog(m_place);
    connect(m_note,SIGNAL(noteSaved(bool)),this,SIGNAL(addButton(bool)));
  }
  m_note->show();
}
/**
 * For the moment we are only doing the first note
 *
 */
void EntryItem::showNote() {
  if ((m_note == NULL) && (m_notes.size() > 0)){
    m_note = new NoteDialog(m_notes[0]);
    m_note->setAutosave(::getNotes()->autosave());
    connect(m_note,SIGNAL(rejected()),this,SLOT(notesRejected()));
    connect(m_note,SIGNAL(accepted()),this,SLOT(notesAccepted()));
  }
  m_note->show();
}
void EntryItem::deleteNote() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  NoteMaster * m = ::getNotes();
  for(int i=0;i < m_notes.size();i++) {
    m->deleteById(m_notes[i]->getId());
  }
  m_notes.clear();
  m_notes = m->find(m_place.getWord());
  if ((m_notes.size() == 0) && (m_noteWidget != NULL)) {
    QGraphicsScene * scene = m_noteWidget->scene();
    scene->removeItem(m_noteWidget);
    delete m_noteWidget;
    m_noteWidget = NULL;
  }
  else {
    QLOG_DEBUG() << "no note widget to delete";
  }
}
QList<Note *> EntryItem::getNotes(bool erase) {
  QList<Note *> notes;
  for(int i=0;i < m_notes.size();i++) {
    notes << m_notes[i];
  }
  if (erase)
    m_notes.clear();

  return notes;
}
void EntryItem::destroyNotes() {
  m_notes.clear();
}
void EntryItem::notesRejected() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  this->setFocus();
}
void EntryItem::notesAccepted() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  this->setFocus();
}
QPair<int,int> EntryItem::find(const QRegExp & rx,int position,bool highlight) {
  //QLOG_DEBUG() << Q_FUNC_INFO << position;
  QTextCursor c = this->document()->find(rx,position);
  if (c.isNull()) {
    return qMakePair(-1,-1);
  }
  QString t = c.selectedText();
  c.movePosition(QTextCursor::PreviousCharacter,QTextCursor::MoveAnchor);
  m_highlights << c.position();
  m_finds.insert(c.position(),t);
  c.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor,t.size());
  this->setTextCursor(c);

  QTextCharFormat fmt = c.charFormat();
  /*
  QLOG_DEBUG() << Q_FUNC_INFO << __LINE__ << fmt.background().color().name();
  if (! m_background.isValid() || (m_background.name() == "#000000")) {
      m_background = fmt.background().color();

  }
  */
  if (highlight) {
    //    c.select(QTextCursor::WordUnderCursor);
    fmt.setBackground(QColor(m_backgroundColorName));
    c.setCharFormat(fmt);
  }
  c.clearSelection();
  this->setTextCursor(c);


  /*
  QTextCharFormat fmt = QTextEdit::ExtraSelection::format;//c.charFormat();
  QLOG_DEBUG() << fmt.background().color().name();
  QLOG_DEBUG() << fmt.foreground().color().name();
  QTextBlockFormat bfmt = c.blockFormat();
  QLOG_DEBUG() << bfmt.background().color().name();
  QLOG_DEBUG() << bfmt.foreground().color().name();
  */
  //  c.movePosition(QTextCursor::NextCharacter,QTextCursor::MoveAnchor);

  //  QLOG_DEBUG() << "linecount" << this->document()->lineCount() << "text width" << this->textWidth() << this->document()->characterCount();
  //  QLOG_DEBUG() << "boundingRect" << this->boundingRect();


  return qMakePair(c.position(),t.size());
}
bool EntryItem::hasHighlights() {
  return (m_highlights.size() > 0);
}
void EntryItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event) {
  QLOG_DEBUG() << Q_FUNC_INFO;
  ContentsWidget * source = qobject_cast<ContentsWidget *>(event->source());
  if (source) {
    event->setDropAction(Qt::LinkAction);
    event->accept();//setAccepted(event->mimeData()->hasFormat("text/plain"));
  }
}
void EntryItem::dragMoveEvent(QGraphicsSceneDragDropEvent *event) {
  ContentsWidget * source = qobject_cast<ContentsWidget *>(event->source());
  if (source) {
    event->setDropAction(Qt::LinkAction);
    event->accept();//setAccepted(event->mimeData()->hasFormat("text/plain"));
  }
}
void EntryItem::dropEvent(QGraphicsSceneDragDropEvent *event) {
  QLOG_DEBUG() << Q_FUNC_INFO << event->mimeData()->text();
  ContentsWidget * source = qobject_cast<ContentsWidget *>(event->source());
  if (! source) {
    return;
  }
  QTextCursor c = textCursor();
  c.setPosition(document()->documentLayout()->hitTest(event->pos(), Qt::FuzzyHit));
  c.select(QTextCursor::WordUnderCursor);

  QString href;
  QString anchor;
  QString f = c.selection().toHtml();
  QRegExp rx("<!--StartFragment--><a\\s+href=\"\\d+\\.\\d+\\.\\d+\\.\\d+\\?([^\"]+)\">");
  if (rx.indexIn(f) != -1) {
    QString t = event->mimeData()->text();
    href = rx.cap(1);
    ///
    /// entry.xslt adds 200e after a link to force a left-to-right
    ///
    QRegularExpression rx("[\u2000-\u202e]");
    t.remove(rx);
    href.remove(rx);
    anchor = c.selectedText();
    event->setDropAction(Qt::LinkAction);
    event->accept();
    /// pass link info i.e either nolink=nn or golink=nn
    /// the link text - this should match the text in the link record
    /// target info in the form <node>:<word>
    /// node that needs fixing
    /// reload page after fixup
    QStringList param;
    param << href << anchor << t << m_place.getNode();
    emit(fixLink(param,true));
  }
}
