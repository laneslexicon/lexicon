#include "splashscreen.h"
SplashScreen::SplashScreen(const QPixmap & pixmap, Qt::WindowFlags f) :
  QSplashScreen(pixmap,f) {
  f |= Qt::WindowStaysOnTopHint;
  setWindowFlags(f);
}
void SplashScreen::pausedFinish() {
  qDebug() << Q_FUNC_INFO;
  this->finish(m_mainWidget);
}
void SplashScreen::setWidget(QWidget * w) {
  m_mainWidget = w;

}
