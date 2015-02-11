#include "optionswidget.h"
#ifndef STANDALONE
#include "application.h"
#include "externs.h"
#endif
OptionsWidget::OptionsWidget(QSettings * settings,QWidget * parent) : QWidget(parent) {
  m_dirty = false;
  m_settings = settings;
  m_btns = 0;
}
OptionsWidget::OptionsWidget(QWidget * parent) : QWidget(parent) {
  m_dirty = false;
  m_settings = 0;
  m_btns = 0;
}
OptionsWidget::OptionsWidget(const QString & theme,QWidget * parent) : QWidget(parent) {
#ifndef STANDALONE
  m_settingsFileName = getLexicon()->settingsFileName(theme);
#else
  m_settingsFileName = QString("%1.ini").arg(theme);
#endif
}
void OptionsWidget::setFileName(const QString & theme) {
  m_settingsFileName = theme;
  m_btns = 0;
}
void OptionsWidget::addButtons() {
  m_btns = new QDialogButtonBox(QDialogButtonBox::Save|QDialogButtonBox::Reset);
  m_btns->button(QDialogButtonBox::Save)->setEnabled(false);
  m_btns->button(QDialogButtonBox::Reset)->setEnabled(false);

  layout()->addWidget(m_btns);
  connect(m_btns->button(QDialogButtonBox::Save),SIGNAL(clicked()),this,SLOT(onSave()));
  connect(m_btns->button(QDialogButtonBox::Reset),SIGNAL(clicked()),this,SLOT(onReset()));
}
void OptionsWidget::onSave() {
  writeSettings();
  setButtons(false);
}
void OptionsWidget::onReset() {
  readSettings();
  setButtons(false);
}
void OptionsWidget::setButtons(bool dirty) {
  if (m_btns != 0) {
  m_btns->button(QDialogButtonBox::Save)->setEnabled(dirty);
  m_btns->button(QDialogButtonBox::Reset)->setEnabled(dirty);
  }
}
/*
void OptionsWidget::readSettings() {
}
void OptionsWidget::writeSettings() {
}
*/
void OptionsWidget::notifyChange() {
  bool v =  isModified();
  setButtons(v);
  emit(modified(v));
  //  emit(valueChanged());
}
OptionsWidget::~OptionsWidget() {
}
void OptionsWidget::stateChanged(int /* state */) {
  bool v =  isModified();
  setButtons(v);
  emit(modified(v));

  //  emit(modified(m_dirty));
}
void OptionsWidget::editingFinished() {
  bool v =  isModified();
  setButtons(v);
  emit(modified(v));

  //  m_dirty = this->isModified();
  //  emit(modified(m_dirty));
}
void OptionsWidget::textChanged(const QString & /* text */) {
  bool v =  isModified();
  setButtons(v);
  emit(modified(v));

  //  m_dirty = this->isModified();
  //  emit(modified(m_dirty));

}
/*
bool OptionsWidget::isModified()  {
  return false;
}
*/
void OptionsWidget::blockAllSignals(bool block) {
  QList<QKeySequenceEdit *> edits = this->findChildren<QKeySequenceEdit *>();
  foreach(QKeySequenceEdit *  widget,edits) {
    widget->blockSignals(block);
  }
  QList<QLineEdit *> lineedits = this->findChildren<QLineEdit *>();
  foreach(QLineEdit *  widget,lineedits) {
    widget->blockSignals(block);
  }
  QList<QCheckBox *> checkboxes = this->findChildren<QCheckBox *>();
  foreach(QCheckBox *  widget,checkboxes) {
    widget->blockSignals(block);
  }
  QList<QComboBox *> comboboxes = this->findChildren<QComboBox *>();
  foreach(QComboBox *  widget,comboboxes) {
    widget->blockSignals(block);
  }
}
void OptionsWidget::setupConnections() {
  QList<QKeySequenceEdit *> edits = this->findChildren<QKeySequenceEdit *>();
  foreach(QKeySequenceEdit *  widget,edits) {
      connect(widget,SIGNAL(editingFinished()),this,SLOT(editingFinished()));
  }
  QList<QLineEdit *> lineedits = this->findChildren<QLineEdit *>();
  foreach(QLineEdit *  widget,lineedits) {
      connect(widget,SIGNAL(textChanged(const QString &)),this,SLOT(textChanged(const QString &)));
  }
  QList<QCheckBox *> checkboxes = this->findChildren<QCheckBox *>();
  foreach(QCheckBox *  widget,checkboxes) {
      connect(widget,SIGNAL(stateChanged(int)),this,SLOT(stateChanged(int)));
  }
  QList<QComboBox *> comboboxes = this->findChildren<QComboBox *>();
  foreach(QComboBox *  widget,comboboxes) {
      connect(widget,SIGNAL(currentIndexChanged(int)),this,SLOT(stateChanged(int)));
  }

  /*
  connect(<checkbox>,SIGNAL(stateChanged(int)),this,SLOT(stateChanged(int)));
  */

  /*
  connect(<lineqedit>,SIGNAL(textChanged(const QString &)),this,SLOT(textChanged(const QString &)));
  */

  /*
  connect(<combobox>,SIGNAL(currentIndexChanged(int)),this,SLOT(stateChanged(int)));
  */

}
