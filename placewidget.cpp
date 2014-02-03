#include "placewidget.h"

PlaceWidget::PlaceWidget(QWidget * parent) : QWidget(parent) {
  readSettings();
  QHBoxLayout * layout = new QHBoxLayout;
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
  m_show = settings.value("Show",false).toBool();
  QString f  = settings.value("CSS","place.css").toString();
  readCssFromFile(f);
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
