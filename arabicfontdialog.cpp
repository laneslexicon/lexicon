#include "arabicfontdialog.h"

ArabicFontDialog::ArabicFontDialog(const QFont & font, QWidget * parent) :
  QFontDialog(font,parent) {

  QList<QComboBox *> boxes = this->findChildren<QComboBox *>();
  if (boxes.size() == 1) {
    int ix = boxes[0]->findText("Arabic");
    if (ix >= 0) {
      boxes[0]->setCurrentText("Arabic");
      boxes[0]->activated(ix);
    }
    else {

    }
  }

}
