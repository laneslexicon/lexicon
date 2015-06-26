#include "showmapwidget.h"
ShowMapWidget::ShowMapWidget(QWidget * parent) : QDialog(parent) {
  setAttribute(Qt::WA_DeleteOnClose,true);
  setObjectName("showmapwidget");
  QVBoxLayout * layout = new QVBoxLayout;
  m_html = new QTextEdit;
  m_buttons = new QDialogButtonBox(QDialogButtonBox::Close);
  layout->addWidget(m_html);
  layout->addWidget(m_buttons);
  this->resize(QSize(800,600));
  setLayout(layout);
  connect(m_buttons->button(QDialogButtonBox::Close),SIGNAL(clicked()),this,SLOT(accept()));
}
ShowMapWidget::~ShowMapWidget() {
}
void ShowMapWidget::setMapName(const QString & name) {
  m_map = name;
}
QString ShowMapWidget::map() const {
  return m_map;
}
void ShowMapWidget::focusOutEvent(QFocusEvent * /* event */) {
  this->lower();
}
void ShowMapWidget::loadHtml(const QString & fileName) {
  QFile file(fileName);
  if (! file.open(QIODevice::ReadOnly)) {
    return;
  }
  QTextStream in(&file);
  QString html = in.readAll();
  m_html->setHtml(html);
  file.close();
  m_html->setReadOnly(true);
}
