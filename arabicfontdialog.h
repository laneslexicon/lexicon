#ifndef __ARABICFONTDIALOG_H__
#define __ARABICFONTDIALOG_H__
#include <QWidget>
#include <QFont>
#include <QFontDialog>
#include <QDebug>
#include <QComboBox>
class ArabicFontDialog : public QFontDialog {
  Q_OBJECT

 public:
  ArabicFontDialog(const QFont &, QWidget * parent = 0);
};
#endif
