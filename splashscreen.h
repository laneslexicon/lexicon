#ifndef __SPLASHSCREEN_H__
#define __SPLASHSCREEN_H__
#include <QSplashScreen>
#include <QDebug>
class LanesLexicon;
class SplashScreen : public QSplashScreen {
  Q_OBJECT

 public:
  SplashScreen(const QPixmap & pixmap, Qt::WindowFlags f = 0);
  void setWidget(QWidget *);
  public slots:
    void pausedFinish();
 private:
    QWidget * m_mainWidget;
};
#endif
