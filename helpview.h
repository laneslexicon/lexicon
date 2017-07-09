#ifndef _HELPVIEW_H_
#define _HELPVIEW_H_
#include <QApplication>
#include <QFormLayout>
#include <QDebug>
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QLabel>
#include <QSettings>
#include <QtGlobal>
#ifdef HELP_WEBKIT
#include <QtWebKitWidgets/QWebView>
#include <QtWebKitWidgets/QWebPage>
#include <QWebHistoryItem>
#endif
#ifdef HELP_WEBENGINE
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineHistory>
#include <QWebEngineHistoryItem>
#endif
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QUrl>
#include <QFile>
#include <QDialogButtonBox>
#include <QProgressDialog>
#include <QPushButton>
#include <QPixmap>
#include <QTimer>
#include <QDesktopServices>
/*
class NoHelp : QWidget {
  Q_OBJECT
 public:
  NoHelp(QWidget * parent = 0);
  void load(const QString &);
};
*/
#ifdef HELP_WEBENGINE
class HelpPage : public QWebEnginePage {
  Q_OBJECT
 public:
  void setLocal(bool v);
  QUrl getLocalUrl() const;
 protected:
  bool acceptNavigationRequest(const QUrl &, NavigationType type, bool isMainFrame);
 private:
  bool m_local;
  QString m_url;
};
#endif
class HelpView : public QWidget {
  Q_OBJECT
 public:
  HelpView(QWidget * parent = 0);
  ~HelpView();
#ifdef HELP_WEBKIT
  QWebView * m_view;
#endif
#ifdef HELP_WEBENGINE
  QWebEngineView * m_view;
#endif
#ifdef HELP_NONE
  QWidget * m_view;
#endif
  bool isLoaded() const;
  bool isOffline() const;
  QUrl lastWishes() const;
  QUrl sectionToUrl(const QString& section);
  
  bool loadHelpSystem(const QString & section = QString());
  public slots:
    void linkclick(const QUrl &);
    void onClose();
    void loadProgress(int);
    void loadFinished(bool);
    void loadStarted();
    void onCancel();
    void onPageBack();
    void onPageForward();
    void showSection(const QString &);
 protected:
    void showEvent(QShowEvent *);
 private:
    QPushButton * m_forwardButton;
    QPushButton * m_backButton;
    QPushButton * m_closeButton;
    void readSettings();
    void writeSettings();
    QString m_localPrefix;
    QString m_localRoot;
    QUrl m_currentLocalPage;
    QList<QUrl> m_stack;
    QString m_onlinePrefix;
    QString m_onlineRoot;
    QUrl m_currentOnlinePage;
    QUrl m_currentUrl;
    bool m_localSource;
    QProgressDialog * m_progress;
    bool m_initialPage;
    QTimer * m_timer;
 signals:
    void finished(bool);
    void helpSystemLoaded(bool);
};
#endif // MAINWINDOW_H
