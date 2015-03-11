#include "keyboard.h"
/**
 *
 *
 * @param size
 * @param key
 * @param parent
 */
GraphicsButton::GraphicsButton(const QRect & rect,KeyDef * key,QGraphicsItem * parent) : QGraphicsRectItem(parent) {
  /// these are the defaults but should be set through setColor
  m_textColor = Qt::white;
  m_backgroundUpColor = Qt::black;
  m_backgroundDownColor = Qt::gray;
  m_keyboardColor = Qt::darkGray;

  m_marginTop = 1;
  m_marginBottom = 1;
  m_marginLeft = 1;
  m_marginRight = 1;
  setRect(rect);
  m_keydef = key;
}
void GraphicsButton::setCss(const QString & css) {
  m_css = css;
}
void GraphicsButton::setColor(int i, const QColor & color ) {
  switch(i) {
  case GraphicsButton::TextColor :
    m_textColor = color;
    break;
  case GraphicsButton::ButtonUpColor :
    m_backgroundUpColor = color;
    break;
  case GraphicsButton::ButtonDownColor :
    m_backgroundDownColor = color;
    break;
  case GraphicsButton::KeyboardColor :
    m_keyboardColor = color;
    break;
  }
}
void GraphicsButton::setColor(int i, const QString & colorStr ) {
  QColor color(colorStr);
  if (!color.isValid() && ! colorStr.isEmpty()) {
    qWarning() << "Invalid color RGB string passed" << colorStr;
    return;
  }
  switch(i) {
  case GraphicsButton::TextColor :
    m_textColor = color;
    break;
  case GraphicsButton::ButtonUpColor :
    m_backgroundUpColor = color;
    break;
  case GraphicsButton::ButtonDownColor :
    m_backgroundDownColor = color;
    break;
  case GraphicsButton::KeyboardColor :
    m_keyboardColor = color;
    break;
  }
}
void GraphicsButton::setup() {
  QRectF r = this->boundingRect();

  int mx = m_marginLeft;
  /// TODO get from keyboard definition
  int boxWidth  = r.width() - m_marginLeft - m_marginRight;
  int boxHeight = r.height() - m_marginTop - m_marginBottom;
  /**
   * if group count = 2, then we have two columns,one aligned left, the other right
   * if group count = 1, and level count = 2,we have one column, centrally aligned
   * if group count = 1, and level count = 3,we have two column, left,aligned and right aligned
   *
   * Within each column, if one item, align central
   *                     if two items, align higher level top, lower level bottom
   *

   1. one cell
   2. two cells split vertically, left cell group1, right cell group 2
   3. two cells split horizontally, bottom cell level1, top cell level2 (from group1)
   4  three cells, split vertically, left cell split horizontally
                  left lower, group 1 level1
                  left top , group 1 level2
                  right group2 whatever level
   5  four cells , both split horizontally and vertically
                left low, group 1,level 1
                left top, group 1,level 2
                right low, group 2, level 1
                right top, group 2, level 2
   */
  //  KeyDef * key = m_keydef;
  switch(m_keydef->decorationCount()) {
  case 1 : {
    QRectF rect(mx,m_marginTop,boxWidth,boxHeight);
    decorateKey(rect,1,1);
    break;
  }
  case 2 : {
    if (m_keydef->groupCount() == 1) {   // split horizontally
      qreal y = boxHeight/2;
      QRectF top(mx,m_marginTop,boxWidth,y);
      QRectF bottom(mx,m_marginTop + y, boxWidth,y);
      decorateKey(bottom,1,1);
      decorateKey(top,1,2);
    }
    else {
      qreal x = boxWidth/2;
      QRectF left(mx,m_marginTop,x,boxHeight);
      QRectF right(x,m_marginTop,x,boxHeight);
      decorateKey(left,1,1);
      decorateKey(right,2,1);
    }
    break;
  }
  case 3 : {
    qreal y = boxHeight/2;
    qreal x = boxWidth/2;
    QRectF leftTop(mx,m_marginTop,x,y);
    QRectF leftBottom(mx,m_marginTop+y,x,y);
    decorateKey(leftBottom,1,1);
    decorateKey(leftTop,1,2);
    /// if one group, but third level in the bottom right
    if (m_keydef->groupCount() == 1) {
      QRectF right(mx+x,m_marginTop+y,x,y);
      decorateKey(right,1,3);
    }
    else {  // otherwise put it in the middle
      QRectF right(mx+x,m_marginTop,x,boxHeight);
      decorateKey(right,2,1);
    }
    break;
    }
 case 4: {
    qreal y = boxHeight/2;
    qreal x = boxWidth/2;
    QRectF leftTop(mx,m_marginTop,x,y);
    QRectF leftBottom(mx,m_marginTop+y,x,y);
    QRectF rightTop(mx+x,m_marginTop,x,y);
    QRectF rightBottom(mx+x,m_marginTop+y,x,y);
    decorateKey(leftBottom,1,1);
    decorateKey(leftTop,1,2);
    decorateKey(rightBottom,2,1);
    decorateKey(rightTop,2,2);
    break;
    }

 }

  setPen(QPen(m_keyboardColor));
}
QGraphicsTextItem * GraphicsButton::decorateKey(const QRectF & cell,int group,int level) {
  QString text = m_keydef->getDecoration(group,level);
  QGraphicsTextItem * item = new QGraphicsTextItem(this);

  int scriptCount = 0;
  /// Get the script for the text and use it as CSS selector
  /// selectors are case insensitive
  ///
  QString script;
  if (text.size() == 1) {
    script = UcdScripts::getScript(text.at(0).unicode());
    if ( ! script.isEmpty()) {
      scriptCount = 1;
    }
  }
  else {
    script = UcdScripts::getScript(text,&scriptCount);
    if (scriptCount != 1) {
      script = "default";
    }
  }
  //  qDebug() << "Get scripts" << QString(" target [%1] end").arg(text) << script << scriptCount;
  //  qDebug() << QString("target [%1], script").arg(text) << script << scriptCount;
  script = script.toCaseFolded();
  if (! m_css.isEmpty())
    item->document()->setDefaultStyleSheet(m_css);
  /// this stops Arabic being moved to the right
  QTextOption topt;
  topt.setTextDirection(Qt::LeftToRight);
  item->document()->setDefaultTextOption(topt);
  QString html;
  //  QFont f("Amiri",30);
  //  item->setFont(f);
  //  html = QString("<html><body><span class=\"%1\">%2</span></body></html>").arg(script).arg(text);
  html = QString("<span class=\"%1\">%2</span>").arg(script).arg(text);
  item->setHtml(html);//Html(html);

  QRectF wr = item->boundingRect();

  /// align the text centrally in its bounding rect
  qreal dx = cell.width() - wr.width();
  qreal dy = cell.height() - wr.height();


  if (m_keydef->centerText(group,level)) {
    dy = dy/2;
  }
  else {
    int vpos = m_keydef->getVerticalAdjust(group,level);
    dy += vpos;
  }

  item->setPos(cell.topLeft() + QPointF(dx/2,dy));//dy/2));
  item->setDefaultTextColor(m_textColor);
  this->setBrush(QBrush(m_backgroundUpColor));
  this->setPen(QPen(m_textColor));
  return item;
}
void GraphicsButton::toggleKeyPressed(int /* i */) {
  if (m_keydef->getToggleState() == KeyDef::KeyUp) {
    this->setBrush(QBrush(m_backgroundDownColor));
    KeyboardScene * scene = dynamic_cast<KeyboardScene *>(this->scene());
    if (m_keydef->getType() == KeyDef::LevelShift) {
      scene->setLevel(m_keydef->changeTo());
    }
    m_keydef->toggle();
  }
  else {
    this->setBrush(QBrush(m_backgroundUpColor));
    KeyboardScene * scene = dynamic_cast<KeyboardScene *>(this->scene());
    /// toggle buttons return to level 1
    if (m_keydef->getType() == KeyDef::LevelShift) {
      scene->setLevel(1);
    }
    m_keydef->toggle();
  }
}
void GraphicsButton::stickyKeyPressed(int /* i */) {
  if (m_keydef->getStickyState() == KeyDef::KeyUp) {
    this->setBrush(QBrush(m_backgroundDownColor));
    KeyboardScene * scene = dynamic_cast<KeyboardScene *>(this->scene());
    if (m_keydef->getType() == KeyDef::LevelShift) {
      scene->setLevel(m_keydef->changeTo());
    }

  }
  else {
    this->setBrush(QBrush(m_backgroundUpColor));
    KeyboardScene * scene = dynamic_cast<KeyboardScene *>(this->scene());
    /// sticky buttons return to level 1
    if (m_keydef->getType() == KeyDef::LevelShift) {
      scene->setLevel(1);
    }

  }
  m_keydef->sticky();
}
void GraphicsButton::mousePressEvent(QGraphicsSceneMouseEvent * event)  {
  if (m_keydef->isEmpty())
    return;
  if (event->button() == Qt::LeftButton) {
      if (m_keydef->isToggle()) {
        this->toggleKeyPressed(0);
      }
      else if (m_keydef->isSticky()) {
        this->stickyKeyPressed(0);
      }
      else {
          this->setBrush(QBrush(m_backgroundDownColor));
      }
  }
}
void GraphicsButton::keyReleased() {
  this->setBrush(QBrush(m_backgroundUpColor));
}
void GraphicsButton::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)  {
  if (m_keydef->isEmpty())
    return;
  if (m_keydef->isToggle() || m_keydef->isSticky()) {
    return;
  }
  if (event->button() == Qt::LeftButton) {
    this->keyReleased();
    KeyboardScene * scene = dynamic_cast<KeyboardScene *>(this->scene());
    scene->keyPressed(this);
  }
}
/**
 *
 *
 * @param parent
 */
KeyboardScene::KeyboardScene(QObject * parent) : QGraphicsScene(parent) {
}
void KeyboardScene::keyPressed(GraphicsButton * button) {
  //  qDebug() << "Pressed key" << button->getKeyDef()->getName();
  QList<QGraphicsView *> v = this->views();
  for(int i=0;i < v.size();i++) {
    KeyboardView * k = dynamic_cast<KeyboardView *>(v[i]);
    k->keyPressed(button->getKeyDef()->getValue());
  }
  QList<QGraphicsItem *> items = this->items();
  for(int i=0;i < items.size();i++) {
    GraphicsButton * button = dynamic_cast<GraphicsButton *>(items[i]);
    if (button) {
      KeyDef * key = button->getKeyDef();
      if (key->isSticky()) {// && (key->getStickyState() == KeyDef::KeyDown)) {
        button->keyReleased();
      }
    }
  }

}
void KeyboardScene::setLevel(int n) {
  QList<QGraphicsView *> v = this->views();
  for(int i=0;i < v.size();i++) {
    KeyboardView * k = dynamic_cast<KeyboardView *>(v[i]);
    k->setLevel(n);
  }
}
void KeyboardScene::setGroup(int n) {
  QList<QGraphicsView *> v = this->views();
  for(int i=0;i < v.size();i++) {
    KeyboardView * k = dynamic_cast<KeyboardView *>(v[i]);
    k->setGroup(n);
  }
}
/**
 *
 *
 * @param parent
 */
KeyboardView::KeyboardView(QWidget * parent) : QGraphicsView(parent) {
  m_kbd = new KeyboardDef();
  m_scene = new KeyboardScene;
  /// TODO get from ini ?
  m_vspace = 5;
  m_hspace = 5;
  /**
   * need to have a rectangle that covers the entire keyboard
   * so we can set it background color and generally make it look pretty
   *
   * set key top color from the ini file
   * set key pressed color from the ini file
   * set the key pen from the ini file
   *
   */
  setScene(m_scene);
  //  setSceneRect(QRectF());
}
KeyboardView::~KeyboardView() {
  delete m_kbd;
}
/**
 * if variable length buttons are used, then it is the responsibility
 * of the .ini specification to make sure that buttons down't overlay
 *
 * E.g. if button at row 1,col2 has a span = 2.0, then the ini file
 * should not have a button a col = 3
 * @param row
 * @param col
 * @param button
 */
void KeyboardView::addButton(int row,int col,GraphicsButton * button) {
  m_scene->addItem(button);
  qreal x,y;
  x = col*m_buttonWidth  + col*m_hspace; // + button->span()*m_hspace
  y = row*m_buttonHeight + row*m_vspace;
  button->setPos(x,y);
}
void KeyboardView::setLevel(int n) {
  m_kbd->setLevel(n);
}
void KeyboardView::setGroup(int n) {
  m_kbd->setGroup(n);
}

void KeyboardView::keyPressed(int k) {
  emit(virtualKeyPressed(k));
  m_kbd->unstick();
}
void KeyboardView::keyPressed(QList<int> k) {
  emit(virtualKeyPressed(k));
  m_kbd->unstick();
}
void KeyboardView::loadKeyboard(const QString & fileName) {
  /**
   * need to have a rectangle that covers the entire keyboard
   * so we can set it background color and generally make it look pretty
   *
   * set key top color from the ini file
   * set key pressed color from the ini file
   * set the key pen from the ini file
   *
   */
  qDebug() << "Loading keyboard" << fileName;
  delete m_kbd;
  m_kbd = new KeyboardDef();
  m_scene->clear();
  m_kbd->load(fileName);
  //  m_scene->setBackgroundBrush(Qt::darkCyan);
  QSettings settings(fileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup("Header");
  m_buttonWidth = settings.value("button width",80).toInt();
  m_buttonHeight = settings.value("button height",80).toInt();
  m_vspace = settings.value("vertical spacing",5).toInt();
  m_hspace = settings.value("horizontal spacing",5).toInt();
  QString textColor = settings.value("text color","white").toString();
  QString keyUpColor = settings.value("key up color","black").toString();
  QString keyDownColor =  settings.value("key down color","gray").toString();
  QString keyboardColor = settings.value("keyboard color","darkgray").toString();
  QString css = settings.value("css",QString()).toString();


  settings.endGroup();
  for(int i=0;i < m_kbd->keyCount();i++) {
    KeyDef * key = m_kbd->getKey(i);
    int width = m_buttonWidth*key->getSpan() + m_hspace*(key->getSpan() - 1);
    int height = m_buttonHeight;
    QRect r(0,0,width,height);

    GraphicsButton * btn = new GraphicsButton(r,key);
    btn->setCss(css);
    btn->setColor(GraphicsButton::TextColor,textColor);
    btn->setColor(GraphicsButton::ButtonUpColor,keyUpColor);
    btn->setColor(GraphicsButton::ButtonDownColor,keyDownColor);
    btn->setColor(GraphicsButton::KeyboardColor,keyboardColor);

    btn->setup();
    addButton(key->getRow(),key->getCol(),btn);
  }
  //  QSize sz = m_kbd->getButtonSize();
  /**
   * we need to resize the sceneRect after each load. It seems that if the
   * scene rect is updated as it increases, but not as it decreases
   */

  /// NOTE: if the buttons are not square, this does not work correctly
  ///  qDebug() << "dimensions" << m_kbd->rows() << m_kbd->cols();
  //  int width  =  (m_kbd->cols() * m_buttonWidth) + (m_kbd->cols() - 1)*m_hspace;
  //  int height =  (m_kbd->rows() * m_buttonHeight) + (m_kbd->rows() - 1)*m_vspace;
  //  m_aspectRatio = (qreal)width/(qreal)height;
  //  qDebug() << "Keyboard dimension" << width << height << m_aspectRatio;
  //  QRectF r = m_scene->sceneRect();
  //  r.setWidth(width);
  //  r.setHeight(height);
  //  m_scene->setSceneRect(r);
  setScene(m_scene);

  //  qDebug() << Q_FUNC_INFO << "scene rect" << m_scene->sceneRect();
  /// TODO get from ini and poss to KeyboardDef before creating buttons
  m_scene->setBackgroundBrush(QColor(keyboardColor));

}
