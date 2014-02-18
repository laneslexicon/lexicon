#include "menubar.h"
AppMenu::AppMenu(QWidget * parent) : QMenuBar(parent) {
}
void AppMenu::mousePressEvent(QMouseEvent * event) {
  QAction * action = activeAction();
  if (action) {
    QMenu * menu = action->menu();
    if (menu) {
      QString obj = menu->objectName();
      if (obj == "bookmarkmenu") {
        emit(rebuildBookmarks());
      }
    }
  }
  QMenuBar::mousePressEvent(event);
}
