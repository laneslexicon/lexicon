#include "placewidget.h"

PlaceWidget::PlaceWidget(QWidget * parent) : QWidget(parent) {
  readSettings();
  QHBoxLayout * layout = new QHBoxLayout;

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

  setLayout(layout);
}
void PlaceWidget::setPlace(const Place & p) {
  m_place = p;
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
}
void PlaceWidget::setRoot(const QString & root) {
  m_root->setText(root);
}
void PlaceWidget::setNode(const QString & node) {
  m_node->setText(node);
}
void PlaceWidget::setWord(const QString & word) {
  m_word->setText(word);
}
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
  QString f = settings.value("Root font",QString()).toString();
  if (! f.isEmpty()) {
    m_rootFont.fromString(f);
  }
  f = settings.value("Word font",QString()).toString();
  if (! f.isEmpty()) {
    m_wordFont.fromString(f);
  }
}
