#ifndef __LOGVIEWER_H__
#define __LOGVIEWER_H__
#include <QListWidget>
#include <QDebug>
#include <QVBoxLayout>
#include <QSettings>
#include <QDialog>
#include <QIcon>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QPushButton>
#include <QHBoxLayout>
class LogViewer : public QWidget {
  Q_OBJECT

public:
  LogViewer(QWidget * parent = 0);
  ~LogViewer();
  QSize sizeHint() const;
  public slots:
    void onRefresh();
    void onClose();
    void onPause();
 private:
  QListWidget * m_list;
  QFile m_log;
  int m_maxlines;
  int m_refreshInterval;
  void addLine(const QString &);
  QPushButton * m_pauseButton;
  QPushButton * m_closeButton;
  QTextStream m_stream;
  QIcon m_warning;
  QIcon m_info;
  QIcon m_error;
  QIcon m_debug;
  QTimer m_timer;
  void readSettings();
  void writeSettings();
};
#endif
