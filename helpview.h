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
class HelpView : public QWidget {
  Q_OBJECT
 public:
  HelpView(QWidget * parent = 0);
  ~HelpView();
  QWebView * m_view;
  public slots:
    void linkclick(const QUrl &);
    void onClose();
    void loadProgress(int);
 private:
    void readSettings();
    void writeSettings();
    QString m_helpRoot;
    QUrl m_currentPage;
};
#endif // MAINWINDOW_H
