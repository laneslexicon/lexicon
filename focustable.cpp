#include "focustable.h"

FocusTable::FocusTable(QWidget * parent) : QTableWidget(parent) {
}
void FocusTable::focusInEvent(QFocusEvent * event) {
  this->setStyleSheet("");
  QTableWidget::focusInEvent(event);
}
void FocusTable::focusOutEvent(QFocusEvent * event) {
  /// TODO get from INI
  this->setStyleSheet("QTableView { selection-background-color : lightgray}");
  QTableWidget::focusOutEvent(event);
}
