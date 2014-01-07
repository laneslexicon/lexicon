#include "graphicsentry.h"
EntryItem::EntryItem(const QString & text, QGraphicsItem * parent) : QGraphicsTextItem(text,parent) {
}
EntryItem::EntryItem(QGraphicsItem * parent) :QGraphicsTextItem(parent) {

}
void EntryItem::contextMenuEvent(QGraphicsSceneContextMenuEvent * event ) {
  //  QGraphicsTextItem::contextMenuEvent(event);

  QMenu menu;
  QAction *removeAction = menu.addAction("Remove");
  QAction *markAction = menu.addAction("Mark");
  QAction *selectedAction = menu.exec(event->screenPos());
}
void EntryItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event) {
  //  QLOG_DEBUG() << "hover" << this->getNode() << this->isRoot();
}
void EntryItem::setRoot(const QString & root,bool isRootEntry) {
  m_root = root;
  m_isRoot = isRootEntry;
}
LaneGraphicsView::LaneGraphicsView(QGraphicsScene * scene,GraphicsEntry * parent) :
  QGraphicsView(scene,parent) {
  //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}
void LaneGraphicsView::scrollContentsBy(int dx,int dy) {
  QScrollBar * b = this->verticalScrollBar();
   //  QLOG_DEBUG() << "scrolling" << dx << dy << b->maximum() << b->value();
  /// lastRoot will emit nextRoot(root)
  //qDebug() << Q_FUNC_INFO << dx << dy << b->value() << b->minimum();
  if (b->value() == b->maximum()) {
    emit(nextPage());
  }
  else if (b->value() == b->minimum()) {
    emit(backPage());
  }
  else {
    QGraphicsView::scrollContentsBy(dx,dy);
  }
    // if (b->value() == b->minimum()) {
  //   QLOG_DEBUG() << "At top";
  // }

}
void LaneGraphicsView::keyPressEvent(QKeyEvent * event) {
  //  QLOG_DEBUG() << "got key";
  QGraphicsView::keyPressEvent(event);
}
GraphicsEntry::GraphicsEntry(QWidget * parent ) : QWidget(parent) {
  QVBoxLayout * layout = new QVBoxLayout;
  m_debug = true;
  m_textOption.setTextDirection(Qt::LeftToRight);
  m_compXsl = 0;
  m_dbname = new QLineEdit;
  m_dbname->setText("lexicon.sqlite");
  /// 0 = paging forward, items are appended
  /// 1 = paging backward, items are prepended
  m_pagingDir = 0;
  m_scale = 1.0;
  m_currentDb = m_dbname->text();

  m_xsl = new QLineEdit;
  m_xsl->setText("./entry.xslt");

  m_cssFile = new QLineEdit("entry.css");

  m_node = new QLineEdit;
  // m_node->setText("n43789");
  m_node->setText("jwb");
  m_findNodeBtn = new QPushButton("Fetch");
  m_nodeXml = new QTextEdit;

  m_root = new QLabel;
  m_word = new QLabel;

  layout->addWidget(m_dbname);

  m_loadCssBtn = new QPushButton("Load CSS");
  m_anchorBtn = new QPushButton("Jump to anchor");
  QHBoxLayout * xsllayout = new QHBoxLayout;
  xsllayout->addWidget(new QLabel("XSL"));
  xsllayout->addWidget(m_xsl);
  xsllayout->addWidget(new QLabel("CSS"));
  xsllayout->addWidget(m_cssFile);
  xsllayout->addWidget(m_loadCssBtn);

  layout->addLayout(xsllayout);
  layout->addWidget(m_node);
  QHBoxLayout * findlayout = new QHBoxLayout;
  findlayout->addWidget(m_findNodeBtn);
  findlayout->addWidget(m_anchorBtn);
  layout->addLayout(findlayout);

  QHBoxLayout * hlayout = new QHBoxLayout;

  hlayout->addWidget(new QLabel("Root:"));
  hlayout->addWidget(m_root);
  hlayout->addWidget(new QLabel("Word:"));
  hlayout->addWidget(m_word);
  hlayout->addStretch();
  layout->addLayout(hlayout);

  QHBoxLayout * btnslayout = new QHBoxLayout;
  m_zoomIn = new QPushButton(tr("+"));
  m_zoomOut = new QPushButton(tr("-"));
  m_clearSceneBtn = new QPushButton(tr("Clear"));
  btnslayout->addWidget(m_zoomIn);
  btnslayout->addWidget(m_zoomOut);
  btnslayout->addWidget(m_clearSceneBtn);
  layout->addLayout(btnslayout);

  connect(m_zoomIn,SIGNAL(clicked()),this,SLOT(onZoomIn()));
  connect(m_zoomOut,SIGNAL(clicked()),this,SLOT(onZoomOut()));
  connect(m_clearSceneBtn,SIGNAL(clicked()),this,SLOT(onClearScene()));

  ///
  ///
  ///

  QSplitter * splitter = new QSplitter;
  m_scene = new QGraphicsScene(this);
  qDebug() << "graphicsentry contstruct" << this;
  m_view = new LaneGraphicsView(m_scene,this);
  connect(m_view,SIGNAL(nextPage()),this,SLOT(nextPageRequested()));
  connect(m_view,SIGNAL(backPage()),this,SLOT(prevPageRequested()));
  qDebug() << "view parent" << m_view->parent();
  //  m_scene->setSceneRect(0,0,300,20000);
  m_view->setInteractive(true);
  m_item = new QGraphicsTextItem("");
  m_item->setTextInteractionFlags(Qt::TextBrowserInteraction);
  m_item->setTextWidth(300);
  m_scene->addItem(m_item);
  // html widget has the html and the text of the node
  QWidget * htmlwidget = new QWidget;
  m_nodeHtml = new QTextBrowser(htmlwidget);
  //   m_nodeText = new QTextEdit(htmlwidget);
  QVBoxLayout * htmllayout = new QVBoxLayout;
  htmllayout->addWidget(m_nodeHtml);
  // add the graphics viwe
  htmllayout->addWidget(m_view);

  htmlwidget->setLayout(htmllayout);

  splitter->addWidget(m_nodeXml);
  splitter->addWidget(htmlwidget);
  m_nodeXml->hide();
  m_nodeHtml->hide();
  layout->addWidget(splitter,1);

  m_nodeQuery = 0;
  setLayout(layout);
  // this doesn't open the db, it just sets up the queries
  //  openDb(m_dbname->text());
  connect(m_findNodeBtn,SIGNAL(clicked()),this,SLOT(on_findNode()));
  connect(m_loadCssBtn,SIGNAL(clicked()),this,SLOT(cssChanged()));
  connect(m_anchorBtn,SIGNAL(clicked()),this,SLOT(anchorTest()));
  //  connect(m_dbname,SIGNAL(textChanged(const QString &)),this,SLOT(dbnameChanged(const QString &)));
  connect(m_dbname,SIGNAL(editingFinished()),this,SLOT(dbnameChanged()));
  connect(m_cssFile,SIGNAL(editingFinished()),this,SLOT(cssChanged()));
  connect(m_nodeHtml,SIGNAL(anchorClicked(const QUrl &)),this,SLOT(anchorClicked(const QUrl &)));
  connect(m_item,SIGNAL(linkActivated(const QString &)),this,SLOT(linkActivated(const QString &)));
  connect(m_item,SIGNAL(linkHovered(const QString &)),this,SLOT(linkHovered(const QString &)));

  connect(m_scene,SIGNAL(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)),
          this,SIGNAL(focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason)));

 m_standardCSS = QString(".arabic { font-family : Droid Sans;font-size : 28px};div { font-family : Droid Sans;font-size : 20px}");
  m_currentCSS = m_standardCSS;
  cssChanged();
  m_xalan = getXalan();
}
GraphicsEntry::~GraphicsEntry() {
  delete m_nodeQuery;
  delete m_rootQuery;
  delete m_nextRootQuery;
  //  if (m_db.isOpen()) {
  //    m_db.close();
  //  }
}
void GraphicsEntry::keyPressEvent(QKeyEvent * event) {
  switch(event->key()) {
  case Qt::Key_Plus: {
    onZoomIn();
    break;
  }
  case Qt::Key_Minus: {
    onZoomOut();
    break;
  }
  default:
    QWidget::keyPressEvent(event);
  }

}
void GraphicsEntry::focusInEvent(QFocusEvent * event) {
  QLOG_DEBUG() << "Got input focus";
  m_view->setFocus();
  QWidget::focusInEvent(event);

}
void GraphicsEntry::onClearScene() {
  for(int i=0;i < m_items.size();i++) {
    m_scene->removeItem(m_items[i]);
  }
  while(m_items.size() > 0) {
    delete m_items.takeFirst();
  }
}
void GraphicsEntry::anchorClicked(const QUrl & link) {
  QLOG_DEBUG() << link.toDisplayString();
  QLOG_DEBUG() << QApplication::keyboardModifiers();
}
void GraphicsEntry::linkActivated(const QString & link) {
  /// turn history on as the user has clicked on something
  getHistory()->on();
  QString node(link);
  node.remove(0,1);
  showNode(node);
}
void GraphicsEntry::linkHovered(const QString & link) {
  QGraphicsTextItem * gi = static_cast<QGraphicsTextItem *>(QObject::sender());

  if (link.isEmpty()) {
    gi->setCursor(QCursor(Qt::ArrowCursor));
  }
  else {
    gi->setCursor(QCursor(Qt::PointingHandCursor));
    QLOG_DEBUG() << "Link hovered" << link;
    QLOG_DEBUG() << QApplication::keyboardModifiers();
  }
}
void GraphicsEntry::anchorTest() {
  QString node = m_node->text();

  //  QList<QGraphicsItem *> items = m_scene->items();
  for(int i=0;i < m_items.size();i++) {
    EntryItem * item = m_items[i];
    if (item) {
      if (item->isNode(node)) {
        m_scene->setFocusItem(item);
        m_view->ensureVisible(item);
        return;
      }
    }
  }
}
bool GraphicsEntry::showNode(const QString & node,bool thisPageOnly) {
  /// check if the node is on this page
  for(int i=0;i < m_items.size();i++) {
    EntryItem * item = m_items[i];
    if (item->isNode(node)) {
      QLOG_DEBUG() << "Found local node" << node;
      m_scene->setFocusItem(item);
      //m_view->ensureVisible(item);
       m_view->centerOn(item);
      //     m_scene->clearFocus();
      return true;
    }
  }
  QLOG_DEBUG() << "Not found on page" << node;
  /// trying out of page jump
  if (! thisPageOnly ) {
    QLOG_DEBUG() << "Out of page jump" << node;
    getXmlForNode(node);
  }
  return false;
}
void GraphicsEntry::dbnameChanged() { //const QString & text) {
  /*
  QLOG_DEBUG() << "db name changed" << m_dbname->text();
  QFile f(m_dbname->text());
  if (f.exists()) {
    if (openDb(m_dbname->text())) {
      m_currentDb = m_dbname->text();
      return;
    }
  }
  QLOG_DEBUG() << "No such file" << m_dbname->text();
  m_dbname->setText(m_currentDb);
  openDb(m_currentDb);
  */
}
void GraphicsEntry::setCSS(const QString & css) {
  QLOG_DEBUG() << "Setting CSS";
  QLOG_DEBUG() << css;
  m_currentCSS = css;
  /*
  QString html = m_nodeHtml->toHtml();
  if (html != m_currentHtml) {
    QLOG_DEBUG() << "Not current html";
  }
  m_nodeHtml->document()->setDefaultStyleSheet(m_currentCSS);
  m_nodeHtml->setHtml(m_currentHtml);
  m_item->document()->setDefaultStyleSheet(m_currentCSS);
  m_item->setHtml(m_currentHtml);
  m_item->setTextInteractionFlags(Qt::TextBrowserInteraction);
  */
  //  m_nodeHtml->clear();
  //  m_nodeHtml->setHtml(html);
  //  on_findNode();

  // this is what transform does
  //
  //  m_nodeHtml->document()->setDefaultStyleSheet(m_currentCSS);
  //  m_nodeHtml->setHtml("<html><body>" + html + "</body></html>");

}
void GraphicsEntry::cssChanged() {
  QString name = m_cssFile->text();
  QFile f(name);
  if (! f.open(QIODevice::ReadOnly)) {
    std::cerr << "Cannot open file for reading: "
              << qPrintable(f.errorString()) << std::endl;
    return;

  }
  QTextStream in(&f);
  QString css;
  while( ! in.atEnd()) {
    css += in.readLine();
  }
  f.close();
  if (! css.isEmpty()) {
    setCSS(css);
  }

}



bool GraphicsEntry::prepareQueries() {
  /*
  QFile dbfile(dbname);
  if (m_db.isOpen()) {
    m_db.close();
  }
  bool ok;
  m_db = QSqlDatabase::addDatabase("QSQLITE");
  m_db.setDatabaseName(dbname);
  ok = m_db.open();
  if (ok) {
    m_db = QSqlDatabase::database();
    QLOG_DEBUG() << "success,  opened DB " << dbname;
  }
  else {
    qWarning() << Q_FUNC_INFO << "db open failed" << ok;
    return ok;
  }
  if (! m_nodeQuery ) {
    delete m_nodeQuery;
  }
  */
  m_nodeQuery = new QSqlQuery;
  bool ok = m_nodeQuery->prepare("select * from entry where nodeId = ?");
  if (! ok ) {
    QLOG_DEBUG() << "node SQL prepare failed";
  }
  m_rootQuery = new QSqlQuery;
  ok = m_rootQuery->prepare("select root,broot,word,bword,xml,page,itype,nodeId from entry where root = ? order by nodenum");
  if (! ok ) {
    QLOG_DEBUG() << "root SQL prepare failed";
  }
  m_nextRootQuery = new QSqlQuery;
  ok = m_nextRootQuery->prepare("select word from root ");
  return ok;
}
/**
 *
 *
 * @param root
 * @param node the id of the entry we want to focus on
 */
void GraphicsEntry::getXmlForRoot(const QString & root,const QString & node) {
  QList<EntryItem *> items;
  QLOG_DEBUG() << "Search for root" << root;
  m_rootQuery->bindValue(0,root);
  m_rootQuery->exec();
  QString arRoot;
  QString startNode = node;
  /// get the position of the last item
  int itemCount = m_items.size();
  /// add the root item
  QString rootxml = QString("<word type=\"root\" ar=\"%1\" />").arg(root);
  EntryItem * rootItem  = createEntry(rootxml);
  /// this will be set to the right word if a node has been supplied
  QString showWord;
  rootItem->setRoot(root,true);
  items << rootItem;
  qDebug() << "Added root item at" << (m_items.size() - 1);
  /// now add all the entries for the root
  while(m_rootQuery->next()) {
    arRoot = m_rootQuery->value(0).toString();
    QLOG_DEBUG() << m_rootQuery->value(3).toString();
    QString t  = QString("<word buck=\"%1\" ar=\"%2\" page=\"%3\" itype=\"%4\">")
      .arg(m_rootQuery->value(3).toString())
      .arg(m_rootQuery->value(2).toString())
      .arg(m_rootQuery->value(5).toString())
      .arg(m_rootQuery->value(6).toString());
    t += m_rootQuery->value(4).toString();
    t += "</word>";
    if (m_debug) {
      QFileInfo fi(QDir::tempPath(),QString("/tmp/%1.xml").arg(m_rootQuery->value(7).toString()));
      QFile f(fi.filePath());
      if (f.open(QIODevice::WriteOnly)) {
        QTextStream out(&f);
        out << t;
      }
    }
    EntryItem * item  = createEntry(t);
    item->setNode(m_rootQuery->value(7).toString());
    /// get the nodeid of the first item at added, so we jump to it later
    item->setRoot(arRoot);
    item->setWord(m_rootQuery->value(2).toString());
    if (startNode.isEmpty()) {
      startNode = m_rootQuery->value(7).toString();
    }
    else if ( startNode == item->getNode()) {
      showWord = item->getWord();
    }
    items << item;
  }
  if (m_pagingDir == 1) {
    while(items.size() > 0) {
      EntryItem * item = items.takeLast();
      m_items.prepend(item);
    }
  }
  else {
    while(items.size() > 0) {
      EntryItem * item = items.takeFirst();
      m_items.append(item);
    }
    addEntries(itemCount);
  }

  m_view->setFocus();
  m_transform = m_view->transform();

  //QLOG_DEBUG() << "Scene rect" << m_scene->sceneRect();
  /// without thus centerOn() does not work properly for
  /// items added to the scene
  m_view->setSceneRect(m_scene->sceneRect());

  emit rootChanged(arRoot,node);
  /**
   * we need to know whether we got here by accessing the history button
   * or not
   *
   */
  if (getHistory()->isOn()) {
    HistoryEvent * event = new HistoryEvent;
    event->setRoot(arRoot);
    event->setNode(node);
    event->setWord(showWord);
    getHistory()->add(event);
  }
  if ( node.isEmpty()) {
      m_scene->setFocusItem(rootItem);
      m_view->centerOn(rootItem);
      //      m_scene->clearFocus();
  }
}
/**
 * create the QTextGraphicsItem by transforming the passed xml
 *
 * @param xml
 *
 * @return
 */
EntryItem * GraphicsEntry::createEntry(const QString & xml) {
    QString html =transform(m_xsl->text(),xml);
    EntryItem * gi = new EntryItem("");
    gi->document()->setDefaultStyleSheet(m_currentCSS);
    gi->setTextWidth(300);
    gi->setHtml(html);
    /// need this otherwise arabic text will be right justified
    gi->document()->setDefaultTextOption(m_textOption);
    gi->setTextInteractionFlags(Qt::TextBrowserInteraction);
    gi->setAcceptHoverEvents(true);
    connect(gi,SIGNAL(linkActivated(const QString &)),this,SLOT(linkActivated(const QString &)));
    connect(gi,SIGNAL(linkHovered(const QString &)),this,SLOT(linkHovered(const QString &)));
    return gi;
}
/**
 * add the graphics items in m_items to the scene starting with the item
 * at startPos
 *
 * @param startPos
 */
void GraphicsEntry::addEntries(int startPos) {
  qreal ypos = 0;
  qreal xpos = 0;
  QRectF r;
  QSizeF sz;
  QLOG_DEBUG() << "addEntries" << startPos;
  /// calculate the y-position of the last item currently in the scene
  if (startPos > 0) {
    QPointF p = m_items[startPos - 1]->pos();
    r = m_items[startPos - 1]->boundingRect();
    ypos =  p.y() + r.height() + 10;
  }
  /// add items updating the ypos as we go
  for(int i=startPos;i < m_items.size();i++) {
    m_items[i]->setPos(xpos,ypos);
    m_scene->addItem(m_items[i]);
    r = m_items[i]->boundingRect();
    QLOG_DEBUG() << "Pos" << m_items[i]->getNode()  << ypos << r.height();
    if (m_debug) {
      QFileInfo fi(QDir::tempPath(),QString("/tmp/%1.html").arg(m_items[i]->getNode()));
      QFile f(fi.filePath());
      if (f.open(QIODevice::WriteOnly)) {
        QTextStream out(&f);
        out << m_items[i]->toHtml();
      }
    }
    sz = m_items[i]->document()->size();
    ypos += sz.height() + 10;
    //    ypos += r.height() + 10;

  }
}
void GraphicsEntry::getXmlForNode(const QString  & node) {
  QLOG_DEBUG() << "Search for node" << node;
  m_nodeQuery->bindValue(0,node);
  m_nodeQuery->exec();
  if (m_nodeQuery->first()) {
    QString xml = m_nodeQuery->value("xml").toString();
    //    QLOG_DEBUG() << "got " << xml;
    m_nodeXml->setText(xml);
    QString root = QString("%1/%2").arg(m_nodeQuery->value("broot").toString()).arg(m_nodeQuery->value("root").toString());
    QString word = QString("%1/%2").arg(m_nodeQuery->value("bword").toString()).arg(m_nodeQuery->value("word").toString());
    m_root->setText(root);
    m_word->setText(word);
    getXmlForRoot(m_nodeQuery->value("root").toString(),node);
    /// focus on the node, set thisPageOnly to true just in case something has gone horribly wrong
    /// otherwise we'll be looping forever
    showNode(node,true);
  }
  else {
    QLOG_DEBUG() << "Error" << m_nodeQuery->lastError().text();
  }

}
void GraphicsEntry::on_findNode()  {
  QRegExp rx("n\\d+");
  QString node = m_node->text();
  if (rx.indexIn(node) != -1) {
    getXmlForNode(node);
  }
  else {
    getXmlForRoot(node);
    showNode(node);
  }
}
QString GraphicsEntry::transform(const QString & xsl,const QString & xml) {
  //  QString header = "<TEI.2><text><body><div1>";
  //  QString footer = "</div1></body></text></TEI.2>";

  //  QString tei  = header + xml + footer;
  if (m_compXsl == 0) {
    std::istringstream iss(xsl.toStdString());
    int r = m_xalan->compileStylesheet("entry.xslt",m_compXsl);
    if (r != 0) {
      QLOG_DEBUG() << "Error compiling stylesheet" << m_xalan->getLastError();
      return QString();
    }
  }
  std::istringstream iss(xml.toStdString());
  std::stringstream ostream;

  m_xalan->transform(iss,m_compXsl,ostream);
  return QString::fromStdString(ostream.str());
}
void GraphicsEntry::addEntry(const QString & html) {
  m_nodeHtml->document()->setDefaultStyleSheet(m_currentCSS);
  // if we do not include the wrappers, the word entries are right justified
  m_currentHtml = QString("<html><body>%1</body></html>").arg(html);
  m_nodeHtml->setHtml(m_currentHtml);

  //  m_nodeHtml->setHtml(html);
  //  m_nodeText->setPlainText(html);

}
void GraphicsEntry::onZoomIn() {
  m_view->setTransform(m_transform);
  m_scale += .1;
  m_view->scale(m_scale,m_scale);
}
void GraphicsEntry::onZoomOut() {
  m_view->setTransform(m_transform);
  m_scale -= .1;
  m_view->scale(m_scale,m_scale);
}
/**
 * search for the given root in the items list
 *
 * @param root         arabic root to search for
 * @param setFocus     center on the root if found
 * @return index in list of item or -1
 */
int GraphicsEntry::hasRoot(const QString & root,bool setFocus) {
  int max = m_items.size();
  int ix = -1;

  for(int i=0;i < max;i++) {
    if (m_items[i]->isRoot() &&
        (m_items[i]->getRoot() == root)) {
          ix = i;
          i = max;
    }
  }
  qDebug() << Q_FUNC_INFO << root << setFocus << ix;
  if ((ix != -1) && setFocus) {
    m_scene->setFocusItem(m_items[ix]);
    m_view->ensureVisible(m_items[ix]);
  }
  return ix;
}
QString GraphicsEntry::lastRoot() {
    /// find the last root item
    int ix = m_items.size() - 1;
    QString root;
    while((ix >= 0) && root.isEmpty()) {
      if (m_items[ix]->isRoot()) {
        root = m_items[ix]->getRoot();
      }
      ix--;
    }
    if (root.isEmpty()) {
      QLOG_DEBUG() << "Cannot find root on current page";
    }
    else {
      qDebug() << "emit nextRoo" << root;
      emit nextRoot(root);
    }
    return root;
}
QString GraphicsEntry::firstRoot() {
  /// find the first root item (should always be the first
  /// element in m_items
  qDebug() << Q_FUNC_INFO;
  int ix = 0;
  int max = m_items.size();
  QString root;
    while((ix < max) && root.isEmpty()) {
      if (m_items[ix]->isRoot()) {
        root = m_items[ix]->getRoot();
      }
      ix++;
    }
    if (root.isEmpty()) {
      QLOG_DEBUG() << "Cannot find root on current page";
    }
    else {
      qDebug() << "emit prevRoot" << root;
      emit prevRoot(root);
    }
    return root;
}
void GraphicsEntry::nextPageRequested() {
  this->lastRoot();
}

void GraphicsEntry::prevPageRequested() {
  qDebug() << Q_FUNC_INFO;
  this->firstRoot();
}
