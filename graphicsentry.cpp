#include "graphicsentry.h"
EntryItem::EntryItem(const QString & text, QGraphicsItem * parent) : QGraphicsTextItem(text,parent) {
}
EntryItem::EntryItem(QGraphicsItem * parent) :QGraphicsTextItem(parent) {

}
LaneGraphicsView::LaneGraphicsView(QGraphicsScene * scene,QWidget * parent) :
  QGraphicsView(scene,parent) {
}
void LaneGraphicsView::scrollContentsBy(int dx,int dy) {
  QScrollBar * b = this->verticalScrollBar();
  QGraphicsView::scrollContentsBy(dx,dy);
  //  qDebug() << "scrolling" << dx << dy << b->maximum() << b->value();
  if (b->value() == b->maximum()) {
    qDebug() << "At bottom";
  }
  if (b->value() == b->minimum()) {
    qDebug() << "At top";
  }

}
GraphicsEntry::GraphicsEntry(QWidget * parent ) : QWidget(parent) {
  QVBoxLayout * layout = new QVBoxLayout;
  m_textOption.setTextDirection(Qt::LeftToRight);
  m_compXsl = 0;
  m_dbname = new QLineEdit;
  m_dbname->setText("lexicon.sqlite");

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

  QSplitter * splitter = new QSplitter;
  m_scene = new QGraphicsScene;
  m_view = new LaneGraphicsView(m_scene);
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

 m_standardCSS = QString(".arabic { font-family : Sans Droid;font-size : 28px};div { font-family : Sans Droid;font-size : 20px}");
  m_currentCSS = m_standardCSS;
  cssChanged();
  m_xalan = getXalan();
}
GraphicsEntry::~GraphicsEntry() {
  //  if (m_db.isOpen()) {
  //    m_db.close();
  //  }
}
void GraphicsEntry::anchorClicked(const QUrl & link) {
  qDebug() << link.toDisplayString();
  qDebug() << QApplication::keyboardModifiers();
}
void GraphicsEntry::linkActivated(const QString & link) {
  qDebug() << "Link activated" << link;
  qDebug() << QApplication::keyboardModifiers();
  QString node(link);
  node.remove(0,1);
  qDebug() << "Node" << node;
  showNode(node);
}
void GraphicsEntry::linkHovered(const QString & link) {
  QGraphicsTextItem * gi = static_cast<QGraphicsTextItem *>(QObject::sender());

  if (link.isEmpty()) {
    gi->setCursor(QCursor(Qt::ArrowCursor));
  }
  else {
    gi->setCursor(QCursor(Qt::PointingHandCursor));
    qDebug() << "Link hovered" << link;
    qDebug() << QApplication::keyboardModifiers();
  }
}
void GraphicsEntry::anchorTest() {
  QString node = m_node->text();

  QList<QGraphicsItem *> items = m_scene->items();
  for(int i=0;i < items.size();i++) {
    EntryItem * item = dynamic_cast<EntryItem *>(items[i]);
    if (item) {
      if (item->isNode(node)) {
        qDebug() << "Found node" << node;
        m_scene->setFocusItem(item);
        m_view->ensureVisible(item);
        return;
      }
    }
  }
}
bool GraphicsEntry::showNode(const QString & node) {
  QList<QGraphicsItem *> items = m_scene->items();
  for(int i=0;i < items.size();i++) {
    EntryItem * item = dynamic_cast<EntryItem *>(items[i]);
    if (item) {
      if (item->isNode(node)) {
        qDebug() << "Found node" << node;
        m_scene->setFocusItem(item);
        m_view->ensureVisible(item);
        return true;
      }
    }
  }
  return false;
}
void GraphicsEntry::dbnameChanged() { //const QString & text) {
  /*
  qDebug() << "db name changed" << m_dbname->text();
  QFile f(m_dbname->text());
  if (f.exists()) {
    if (openDb(m_dbname->text())) {
      m_currentDb = m_dbname->text();
      return;
    }
  }
  qDebug() << "No such file" << m_dbname->text();
  m_dbname->setText(m_currentDb);
  openDb(m_currentDb);
  */
}
void GraphicsEntry::setCSS(const QString & css) {
  qDebug() << "Setting CSS";
  qDebug() << css;
  m_currentCSS = css;
  /*
  QString html = m_nodeHtml->toHtml();
  if (html != m_currentHtml) {
    qDebug() << "Not current html";
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
    qDebug() << "success,  opened DB " << dbname;
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
    qDebug() << "node SQL prepare failed";
  }
  m_rootQuery = new QSqlQuery;
  ok = m_rootQuery->prepare("select root,broot,word,bword,xml,page,itype,nodeId from entry where root = ? order by nodenum");
  if (! ok ) {
    qDebug() << "root SQL prepare failed";
  }

  return ok;
}
void GraphicsEntry::getXmlForRoot(const QString & root,const QString & node) {
  qDebug() << "Search for root" << root;
  m_rootQuery->bindValue(0,root);
  m_rootQuery->exec();
  QString arRoot;
  QString contents;

  //  if ( ! m_rootQuery->isValid()) {
  //    qDebug() << "Root not found";
  //  }
  QStringList xmlitems;
  QStringList nodes;
  while(m_rootQuery->next()) {
    arRoot = m_rootQuery->value(0).toString();
    qDebug() << m_rootQuery->value(3).toString();
    QString t  = QString("<word buckwalter=\"%1\" ar=\"%2\" page=\"%3\" itype=\"%4\">\n")
      .arg(m_rootQuery->value(3).toString())
      .arg(m_rootQuery->value(2).toString())
      .arg(m_rootQuery->value(5).toString())
      .arg(m_rootQuery->value(6).toString());
    t += m_rootQuery->value(4).toString();
    t += "</word>\n";
    xmlitems << t;
    nodes << m_rootQuery->value(7).toString();
  }

  QString rootxml = QString("<root btext=\"%1\" text=\"%2\">\n").arg(root).arg(arRoot);
  rootxml += contents;
  rootxml += "</root>";
  xmlitems.prepend(rootxml);
  m_nodeXml->setPlainText(xmlitems.join("\n"));
  m_scene->clear();
  int ypos = 0;
  int xpos = 0;
  QString allHtml;
  for(int i=0;i < xmlitems.size();i++) {
    QString html =transform(m_xsl->text(),xmlitems[i]);
    EntryItem * gi = new EntryItem("");
    gi->document()->setDefaultStyleSheet(m_currentCSS);
    gi->setHtml(html);
    gi->setPos(xpos,ypos);
    gi->setTextWidth(300);
    gi->document()->setDefaultTextOption(m_textOption);
    gi->setTextInteractionFlags(Qt::TextBrowserInteraction);
    if (i > 0) {
      gi->setNode(nodes[i-1]);
    }
    connect(gi,SIGNAL(linkActivated(const QString &)),this,SLOT(linkActivated(const QString &)));
    connect(gi,SIGNAL(linkHovered(const QString &)),this,SLOT(linkHovered(const QString &)));
    m_scene->addItem(gi);
    QRectF r = gi->boundingRect();
    ypos += r.toRect().height() + 10;
    allHtml += html;
  }
  //  if (! node.isEmpty()) {
  //    m_nodeHtml->scrollToAnchor(node);
  //  }
  m_nodeHtml->setPlainText(allHtml);
  //  qDebug() << xml;
}
void GraphicsEntry::getXmlForNode(const QString  & node) {
  qDebug() << "Search for node" << node;
  m_nodeQuery->bindValue(0,node);
  m_nodeQuery->exec();
  if (m_nodeQuery->first()) {
    QString xml = m_nodeQuery->value("xml").toString();
    qDebug() << "got " << xml;
    m_nodeXml->setText(xml);
    QString root = QString("%1/%2").arg(m_nodeQuery->value("broot").toString()).arg(m_nodeQuery->value("root").toString());
    QString word = QString("%1/%2").arg(m_nodeQuery->value("bword").toString()).arg(m_nodeQuery->value("word").toString());
    m_root->setText(root);
    m_word->setText(word);
    getXmlForRoot(m_nodeQuery->value("broot").toString(),node);
    //   transform(m_xsl->text(),xml);
  }
  else {
    qDebug() << "Error" << m_nodeQuery->lastError().text();
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
      qDebug() << "Error compiling stylesheet" << m_xalan->getLastError();
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
