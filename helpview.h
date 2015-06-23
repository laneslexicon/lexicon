#ifndef _HELPVIEW_H_
#define _HELPVIEW_H_
#include <QApplication>
#include <QFormLayout>
#include <QDebug>
#include <QVBoxLayout>
#include <QSettings>
#include <QtWebKitWidgets/QWebView>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QUrl>
#include <QFile>
#include <QDialogButtonBox>
#include <QProgressDialog>
#include <QTimer>
class HelpView : public QWidget {
  Q_OBJECT
 public:
  HelpView(QWidget * parent = 0);
  ~HelpView();
  QWebView * m_view;
  bool isLoaded();
  public slots:
    void linkclick(const QUrl &);
    void onClose();
    void loadProgress(int);
    void loadFinished(bool);
    void loadStarted();
    void onCancel();
 protected:
    void showEvent(QShowEvent *);
 private:
    void readSettings();
    void writeSettings();
    QString m_localPrefix;
    QString m_localRoot;
    QUrl m_currentLocalPage;

    QString m_onlinePrefix;
    QString m_onlineRoot;
    QUrl m_currentOnlinePage;

    bool m_localSource;
    QProgressDialog * m_progress;
    bool m_initialPage;
    QTimer * m_timer;
};
#endif // MAINWINDOW_H
