#include "optionswidget.h"
OptionsWidget::OptionsWidget(QSettings * settings,QWidget * parent) : QWidget(parent) {
  m_dirty = false;
  m_settings = settings;
}
void OptionsWidget::readSettings() {
}
void OptionsWidget::writeSettings() {
}
void OptionsWidget::notifyChange() {
  m_dirty = true;
  emit(valueChanged());
}
OptionsWidget::~OptionsWidget() {
  qDebug() << Q_FUNC_INFO;
}
void OptionsWidget::stateChanged(int /* state */) {
  //  m_dirty = this->isModified();
  emit(modified(m_dirty));
}
void OptionsWidget::editingFinished() {
  //  m_dirty = this->isModified();
  emit(modified(m_dirty));
}
void OptionsWidget::textChanged(const QString & /* text */) {
  //  m_dirty = this->isModified();
  qDebug() << Q_FUNC_INFO << m_dirty;
  emit(modified(m_dirty));

}

bool OptionsWidget::isModified()  {
  return false;
}
