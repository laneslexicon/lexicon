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
#include <QSplitter>
#include <QSettings>
#include <QUrl>
#include <QMap>
#include <QHelpEngine>
#include <QHelpContentWidget>
#include <QHelpIndexModel>
#include <QHelpIndexWidget>
#include "application.h"
class HelpBrowser : public QTextBrowser {
  Q_OBJECT;
 public:
  HelpBrowser(QWidget * parent = 0);
  void setHelpEngine(QHelpEngine * he) {
    m_he = he;
  }
  void toAnchor();
 private:
  QHelpEngine * m_he;
  QString m_firstAnchor;
  private slots:
    void getAnchor(const QUrl &);

 protected:
  QVariant loadResource(int type, const QUrl &name);


};
class HelpViewer : public QWidget {
  Q_OBJECT;

 public:
  HelpViewer(QWidget * parent = 0);
  ~HelpViewer();
  HelpBrowser * browser() { return m_browser;}
  public slots:
    void backwardAvailable(bool);
    void goBack();
 private:
  HelpBrowser * m_browser;
  QPushButton * m_backBtn;
  QHelpEngine * m_he;
};
class HelpWidget : public QWidget {
  Q_OBJECT;

 public:
  HelpWidget(QWidget * parent = 0);
  ~HelpWidget();
  void readSettings();
  void writeSettings();
 public slots:
  void contentsCreated();
  void helpLinkActivated(const QUrl &);
 private:
  QHelpEngine * m_he;
  HelpViewer * m_viewer;
  QString m_helpCollection;
  QString m_helpPage;
  QUrl m_currentUrl;
};
#endif
