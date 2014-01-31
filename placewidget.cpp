#include "placewidget.h"

PlaceWidget::PlaceWidget(QWidget * parent) : QWidget(parent) {
  readSettings();
  QHBoxLayout * layout = new QHBoxLayout;
  /*
  m_root = new QLabel("Root");
  m_root->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
  layout->addWidget(m_root);

  m_word = new QLabel("Word");
  m_word->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
  layout->addWidget(m_word);

  m_node = new QLabel("Node");
  m_node->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
  layout->addWidget(m_node);

  m_page = new QLabel("Page");
  m_page->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
  layout->addWidget(m_page);
  */
  m_text = new QTextBrowser(this);
  m_text->setMaximumHeight(40);
  //  m_text->setMaximumWidth(100);
  //  m_text->setMinimumSize(100,30);
  layout->addWidget(m_text);
  setLayout(layout);
  if (! m_show) {
    this->hide();
  }
}
void PlaceWidget::setPlace(const Place & p) {
  m_text->document()->setDefaultStyleSheet(m_css);
  m_place = p;
  QString html;
  if ( ! m_place.getRoot().isEmpty()) {
    html += QString("<span>Root </span><span class=\"arabic\">%1</span><br/>").arg(m_place.getRoot());
  }
  if ( ! m_place.getWord().isEmpty()) {
   html += QString("<span class=\"arabic\">Word: %1</span><br/>").arg(m_place.getWord());
  }
  if ( m_place.getPage() > 0) {
    html += QString("<span>Page: %1</span><br/>").arg(m_place.getPage());
  }
  if ( ! m_place.getNode().isEmpty()) {
    html += QString("<span>Node: %1</span><br/>").arg(m_place.getNode());
  }
  m_text->setHtml(html);
  m_text->setToolTip(html);
  /*
   m_root->setText(m_place.getRoot());
  m_root->setFont(m_rootFont);

  m_node->setText(m_place.getNode());
  m_word->setText(m_place.getWord());
  m_word->setFont(m_wordFont);
  int page = m_place.getPage();
  if (page != -1) {
    m_page->setText(QString("Page: %1").arg(page));
  }
  else {
    m_page->setText("");
  }
  */
  qDebug() << m_text->toHtml();
}
/*
void PlaceWidget::setRoot(const QString & root) {
  m_root->setText(root);
}
void PlaceWidget::setNode(const QString & node) {
  m_node->setText(node);
}
void PlaceWidget::setWord(const QString & word) {
  m_word->setText(word);
}
*/
/**
 * need this otherwise setStyleSheet will not work
 *
 * @param pe
 */
void PlaceWidget::paintEvent(QPaintEvent *pe)
{
 QStyleOption o;
 o.initFrom(this);
 QPainter p(this);
 style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}
void PlaceWidget::readSettings() {
  QSettings settings;
  settings.setIniCodec("UTF-8");
  settings.beginGroup("Place");
  m_show = settings.value("Show",false).toBool();
  QString f  = settings.value("CSS","place.css").toString();
  readCssFromFile(f);
  /*
  QString f = settings.value("Root font",QString()).toString();
  if (! f.isEmpty()) {
    m_rootFont.fromString(f);
  }
  f = settings.value("Word font",QString()).toString();
  if (! f.isEmpty()) {
    m_wordFont.fromString(f);
  }
  */
}
/**
 *
 *
 */
bool PlaceWidget::readCssFromFile(const QString & name) {
  QFile f(name);
  if (! f.open(QIODevice::ReadOnly)) {
    QLOG_WARN()  << "Cannot open file for reading: " << name
                 << f.errorString();
    return false;

  }
  QTextStream in(&f);
  QString css;
  while( ! in.atEnd()) {
    if (! css.startsWith("-")) {
      css += in.readLine();
    }
  }
  f.close();
  if (! css.isEmpty()) {
    m_css = css;
  }
  return true;
}
