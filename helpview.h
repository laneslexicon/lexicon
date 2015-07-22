#ifndef _HELPVIEW_H_
#define _HELPVIEW_H_
#include <QApplication>
#include <QFormLayout>
#include <QDebug>
#include <QVBoxLayout>
#include <QSettings>
#include <QtWebKitWidgets/QWebView>
#include <QtWebKitWidgets/QWebPage>
#include <QWebHistoryItem>
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
class HelpView : public QWidget {
  Q_OBJECT
 public:
  HelpView(QWidget * parent = 0);
  ~HelpView();
  QWebView * m_view;
  bool isLoaded() const;
  bool isOffline() const;
  QUrl lastWishes() const;
  bool loadHelpSystem();
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
