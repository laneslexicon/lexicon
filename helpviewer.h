#ifndef __HELPVIEWER_H__
#define __HELPVIEWER_H__
#include <QWidget>
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QDebug>
#include <QPushButton>
#include <QFile>
#include <QHelpEngine>
#include <QByteArray>
#include <QImage>
class HelpBrowser : public QTextBrowser {
  Q_OBJECT;
 public:
  HelpBrowser(QWidget * parent = 0);
  void setHelpEngine(QHelpEngine * he) {
    m_he = he;
  }
 private:
  QHelpEngine * m_he;
 protected:
  QVariant loadResource(int type, const QUrl &name);


};
class HelpViewer : public QWidget {
  Q_OBJECT;

 public:
  HelpViewer(QWidget * parent = 0);
  HelpBrowser * browser() { return m_browser;}
  public slots:
    void backwardAvailable(bool);
    void goBack();
 private:
  HelpBrowser * m_browser;
  QPushButton * m_backBtn;
};
#endif
