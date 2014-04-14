#ifndef __KEYBOARDWIDGET_H__
#define __KEYBOARDWIDGET_H__
#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QDialog>
#include <QVBoxLayout>
#include <QComboBox>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QFontMetrics>
#include <QPushButton>
#include <QTextDocument>
#include <QGridLayout>
#include <QPair>
#include "keydef.h"
#include "scripts.h"
#include "keyboard.h"
class KeyboardWidget : public QDialog {
  Q_OBJECT;

 public:
  KeyboardWidget(QWidget * parent = 0);
  void setTarget(QWidget * w) { m_target = w;};
  void loadDefinitions(const QString & targetScript = QString());
 protected:
  QSize sizeHint() const;
  void resizeEvent(QResizeEvent *);
 public slots:
   void virtualKeyPressed(int);
   void loadKeyboard(int);
   void showKeyboard();
   void attach(QWidget *);
 private:
   void readSettings();
   QString m_keyboardDirectory;
   QString m_defaultKeyboard;
   bool m_keepAspectRatio;

  QComboBox * m_keyboards;
  KeyboardView * m_view;
  qreal m_scale;

  QWidget * m_target;
  QTransform m_transform;
  void autoScale();
  //  public slots:
    //  void zoomIn();
    //  void zoomOut();
};
#endif
