#include "entryitem.h"
EntryItem::EntryItem(const QString & text, QGraphicsItem * parent) : QGraphicsTextItem(text,parent) {
  m_focusOnHover = false;
  m_note = NULL;
}
EntryItem::EntryItem(QGraphicsItem * parent) :QGraphicsTextItem(parent) {
  m_focusOnHover = false;
  m_note = NULL;
}
/**
 * the note dialog does not have a QWidget parent so delete it manually
 *
 */
EntryItem::~EntryItem() {
  if (m_note != NULL) {
    m_note->close();
    delete m_note;
  }
}
void EntryItem::contextMenuEvent(QGraphicsSceneContextMenuEvent * event ) {
  //  QGraphicsTextItem::contextMenuEvent(event);
  QString href;
  QString anchor;
  QAction *jumpAction;
  QTextCursor c = textCursor();
  c.setPosition(document()->documentLayout()->hitTest(event->pos(), Qt::FuzzyHit));
  c.select(QTextCursor::WordUnderCursor);
  qDebug() << "selected text:" << c.selectedText();
  if (c.charFormat().isAnchor()) {
    qDebug() << "is anchor" << c.charFormat().isAnchor() << c.charFormat().anchorHref();
    href = c.charFormat().anchorHref();
    if (href.startsWith("#")) {
      href.remove(0,1);
    }
    anchor = c.selectedText();
  }
  QMenu menu(m_place.getShortText());
  menu.setObjectName("entry");
  if ( ! href.isEmpty()) {
    QString t = QString("Goto %1").arg(anchor);
    jumpAction = menu.addAction(t);
    jumpAction->setData(href);
  }
  QAction *addNoteAction = menu.addAction(tr("Add &note"));
  QAction *markAction = menu.addAction(tr("Add &bookmark"));
  //  QAction *searchAction = menu.addAction("Find");
  //  connect(searchAction,SIGNAL(triggered()),this,SLOT(searchItem()));
  QAction *selectAction;// = menu.addAction(tr("Select current &entry"));
  QAction *selectAllAction = menu.addAction(tr("Select &all"));



  if (this->textCursor().hasSelection()) {
    QAction *copyAction = menu.addAction("&Copy");
    connect(copyAction,SIGNAL(triggered()),this,SIGNAL(copy()));
    QAction *clearCurrentAction = menu.addAction(tr("Clear current &entry selection"));
    connect(clearCurrentAction,SIGNAL(triggered()),this,SLOT(clearSelection()));
    QAction *clearAllAction = menu.addAction(tr("Clear all selected &items"));
    connect(clearAllAction,SIGNAL(triggered()),this,SIGNAL(clearAllItems()));

  }
  else {
    selectAction = menu.addAction(tr("&Select current entry"));
  }
  QAction * perseusAction = menu.addAction("Show &Perseus XML");
  Place p = this->getPlace();
  QAction *selectedAction = menu.exec(event->screenPos());
  if (selectedAction == perseusAction) {
    emit(showPerseus(p));
  }
  else if (selectedAction == markAction) {
    bool ok;
    QString text = QInputDialog::getText(0, tr("Bookmark Add)"),
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
  else if ((jumpAction != NULL) && (selectedAction == jumpAction)) {
    qDebug() << "GOTO" << jumpAction->data();
    Place p;
    p.setNode(jumpAction->data().toString());
    emit(gotoNode(p,true));

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
  qDebug() << Q_FUNC_INFO;
}
void EntryItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event) {
  if (m_focusOnHover)
    scene()->setFocusItem(this);
  QGraphicsTextItem::hoverEnterEvent(event);
}
void EntryItem::focusInEvent(QFocusEvent * event) {
  //  QLOG_DEBUG() << Q_FUNC_INFO << this->getPlace().getText();
  /// this updates m_place in graphicsentry so the current node is saved on exit
  emit(placeChanged(this->getPlace()));
  QGraphicsTextItem::focusInEvent(event);
}
/*
void EntryItem::copy() {
  QString txt = this->textCursor().selectedText();
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(txt);
}
*/
void EntryItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *o, QWidget *w) {
  QPen pen = painter->pen();
  QBrush brush = painter->brush();
  painter->setPen(Qt::NoPen);
  if ( ! m_place.isSupplement()) {
    painter->setBrush(Qt::white);
  }
  else {
    painter->setBrush(m_backgroundColor);
  }
  painter->drawRect(boundingRect());
  painter->setPen(pen);
  QGraphicsTextItem::paint(painter, o, w);
}
QTextCursor EntryItem::highlight(const QString & target, Qt::GlobalColor color) {
  int pos;
  QTextCursor cursor;
  QTextCursor firstPos;
  QTextDocument * doc = this->document();

  m_searchText = target;
  m_searchPositions.clear();
  cursor = doc->find(target,QTextDocument::FindWholeWords);
  firstPos = cursor;
  while(! cursor.isNull()) {
    pos =  cursor.position();
    m_searchPositions << pos;
    //    qDebug() << Q_FUNC_INFO << "found at" << pos;
    cursor.setPosition(pos - target.size(), QTextCursor::MoveAnchor);
    cursor.setPosition(pos, QTextCursor::KeepAnchor);
    QTextCharFormat fmt = cursor.charFormat();
    fmt.setBackground(color);
    cursor.setCharFormat(fmt);
    cursor = doc->find(target,pos,QTextDocument::FindWholeWords);
  }
  return firstPos;
}
void EntryItem::clearHighlights() {
  /*
  QTextCursor c = this->textCursor();
  c.select(QTextCursor::Document);

  QTextCharFormat fmt;
  /// TODO get from QSettings
  fmt.setBackground(Qt::white);
  c.setCharFormat(fmt);
  c.clearSelection();
  */
  this->highlight(m_searchText,Qt::white);
}
/**
 * this is the one that is used by default
 *
 * @param target
 *
 * @return
 */
QTextCursor EntryItem::highlightRx(const QString & target) {
  int pos;
  QTextCursor cursor;
  QTextCursor firstPos;
  QTextDocument * doc = this->document();
  QTextCharFormat fmt;
  QRegExp rx;
  /// TODO this needs to be fixed to allow for e.g. full stop
  /// and then made the default
  rx.setPattern("\\b" + target + "\\b");
  /// TODO get from QSettings
  fmt.setBackground(Qt::yellow);
  cursor = doc->find(rx,QTextDocument::FindWholeWords);
  firstPos = cursor;
  while(! cursor.isNull()) {
    pos =  cursor.position();
    cursor.setPosition(pos - target.size(), QTextCursor::MoveAnchor);
    cursor.setPosition(pos, QTextCursor::KeepAnchor);
    cursor.setCharFormat(fmt);
    cursor = doc->find(target,pos,QTextDocument::FindWholeWords);
  }
  return firstPos;
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
  }
  m_note->show();

}
