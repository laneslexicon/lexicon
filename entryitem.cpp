#include "entryitem.h"
EntryItem::EntryItem(const QString & text, QGraphicsItem * parent) : QGraphicsTextItem(text,parent) {
}
EntryItem::EntryItem(QGraphicsItem * parent) :QGraphicsTextItem(parent) {

}
void EntryItem::contextMenuEvent(QGraphicsSceneContextMenuEvent * event ) {
  //  QGraphicsTextItem::contextMenuEvent(event);

  QMenu menu(tr("Entry Menu"));
  menu.setObjectName("entry");
  QAction *markAction = menu.addAction("Bookmark");
  QAction *searchAction = menu.addAction("Find");
  connect(searchAction,SIGNAL(triggered()),this,SLOT(searchItem()));

  if (this->textCursor().hasSelection()) {
    QAction *copyAction = menu.addAction("Copy");
    connect(copyAction,SIGNAL(triggered()),this,SLOT(copy()));
  }
  QAction * perseusAction = menu.addAction("Show Perseus XML");
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
}
void EntryItem::setPlace(const Place & p) {
 m_place = p;
 setToolTip(m_place.getText());
 }
void EntryItem::searchItem() {
  qDebug() << Q_FUNC_INFO;
}
void EntryItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event) {
  //  QGraphicsTextItem::hoverEnterEvent(event);
  scene()->setFocusItem(this);
}
void EntryItem::focusInEvent(QFocusEvent * event) {
  //  QLOG_DEBUG() << Q_FUNC_INFO << this->getPlace().getText();
  /// this updates m_place in graphicsentry so the current node is saved on exit
  emit(placeChanged(this->getPlace()));
  QGraphicsTextItem::focusInEvent(event);
}
void EntryItem::copy() {
  QString txt = this->textCursor().selectedText();
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(txt);
}
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
QTextCursor EntryItem::highlight(const QString & target) {
  int pos;
  QTextCursor cursor;
  QTextCursor firstPos;
  QTextDocument * doc = this->document();
  QTextCharFormat fmt;
  /// TODO get from QSettings
  fmt.setBackground(Qt::yellow);
  cursor = doc->find(target,QTextDocument::FindWholeWords);
  firstPos = cursor;
  while(! cursor.isNull()) {
    pos =  cursor.position();
    //    qDebug() << Q_FUNC_INFO << "found at" << pos;
    cursor.setPosition(pos - target.size(), QTextCursor::MoveAnchor);
    cursor.setPosition(pos, QTextCursor::KeepAnchor);
    cursor.setCharFormat(fmt);
    cursor = doc->find(target,pos,QTextDocument::FindWholeWords);
  }
  return firstPos;
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
