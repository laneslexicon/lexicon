#include "logviewer.h"
#include "QsLog.h"
#include "place.h"
#include "application.h"
#include "externs.h"
#include "definedsettings.h"
LogViewer::LogViewer(QWidget * parent) : QWidget(parent) {
  this->setObjectName("logviewer");
  readSettings();
  QVBoxLayout * layout = new QVBoxLayout;
  m_list = new QListWidget(this);
  m_list->setObjectName("loglist");
  layout->addWidget(m_list);

  m_pauseButton = new QPushButton(tr("&Pause auto-update"),this);
  m_closeButton = new QPushButton(tr("&Close"),this);

  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addStretch();
  hlayout->addWidget(m_pauseButton);
  hlayout->addWidget(m_closeButton);

  layout->addLayout(hlayout);
  setLayout(layout);
  //  setModal(false);
  QStringList lines;
  if (m_log.open(QIODevice::ReadOnly)) {
    m_stream.setDevice(&m_log);
    m_stream.setCodec("UTF-8");
    while(! m_stream.atEnd()) {
      lines << m_stream.readLine();
      if (lines.size() > m_maxlines) {
          lines.takeFirst();
      }
    }
    for(int i=0;i < lines.size();i++) {
      addLine(lines[i]);
    }
  }
  else {
    QString err = QString(tr("LogViewer could not open log file:%1")).arg(m_log.fileName());
    QLOG_WARN() << err;
    m_list->addItem(err);

  }
  m_timer = new QTimer(this);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(onRefresh()));
  connect(m_pauseButton,SIGNAL(clicked()),this,SLOT(onPause()));
  connect(m_closeButton,SIGNAL(clicked()),this,SLOT(onClose()));
  m_timer->start(m_refreshInterval);

}
void LogViewer::readSettings() {
  SETTINGS
  settings.beginGroup("Logging");
  m_log.setFileName(settings.value(SID_LOGGING_FILE,"../log.txt").toString());
  m_maxlines = settings.value(SID_LOGGING_VIEWER_MAXLINES,1000).toInt();
  m_refreshInterval = settings.value(SID_LOGGING_VIEWER_INTERVAL,1000).toInt();
  this->restoreGeometry(settings.value("Geometry").toByteArray());
  settings.endGroup();
  settings.beginGroup("System");
  QString theme = settings.value("Theme",QString()).toString();
  QString section;
  if (theme.isEmpty()) {
    section = "Icons-Default";
  }
  else {
    section = QString("Icons-%1").arg(theme);
  }
  settings.endGroup();
  settings.beginGroup(section);
  QString imgdir = settings.value("Directory","images").toString();
  QFileInfo fi(imgdir);
  if (! fi.exists()) {
    QLOG_WARN() << QString(tr("Theme directory not found:%1")).arg(imgdir);
    return;
  }
  if (! fi.isDir()) {
    QLOG_WARN() << QString(tr("Theme directory is not a directory:%1")).arg(imgdir);
    return;
  }
  QString filename;
  QDir d(imgdir);
  filename = settings.value("Warning",QString()).toString();
  fi.setFile(d,filename);
  if (! fi.exists()) {
    QLOG_WARN() << QString(tr("Icon not found:%1")).arg(fi.absolutePath());
  }
  else {
    m_warning = new QIcon(fi.absoluteFilePath());
  }
  filename = settings.value("Error",QString()).toString();
  fi.setFile(d,filename);
  if (! fi.exists()) {
    QLOG_WARN() << QString(tr("Icon not found:%1")).arg(fi.absolutePath());
  }
  else {
    m_error = new QIcon(fi.absoluteFilePath());
  }
  filename = settings.value("Info",QString()).toString();
  fi.setFile(d,filename);
  if (! fi.exists()) {
    QLOG_WARN() << QString(tr("Icon not found:%1")).arg(fi.absolutePath());
  }
  else {
    m_info = new QIcon(fi.absoluteFilePath());
  }
  filename = settings.value("Debug",QString()).toString();
  fi.setFile(d,filename);
  if (! fi.exists()) {
    QLOG_WARN() << QString(tr("Icon not found:%1")).arg(fi.absolutePath());
  }
  else {
    m_debug = new QIcon(fi.absoluteFilePath());
  }
}
void LogViewer::writeSettings() {
  SETTINGS
  settings.beginGroup("Logging");
  settings.setValue("Geometry", saveGeometry());
  settings.endGroup();
}
QSize LogViewer::sizeHint() const {
  return QSize(400,600);
}
LogViewer::~LogViewer() {
  writeSettings();
  m_list->clear();
  delete m_warning;
  delete m_error;
  delete m_info;
}
void LogViewer::addLine(const QString & line) {
  QString t  = line.trimmed();
  QRegExp rx("^(DEBUG|INFO|TRACE|FATAL|WARN)\\s+([^\\s]+)\\s+(.+)$");
  int row = m_list->currentRow();
  row++;
  if (rx.indexIn(t) != -1) {
    if (rx.cap(1) == "INFO") {
      QListWidgetItem * item = new QListWidgetItem(*m_info,rx.cap(3));
      m_list->addItem(item);
    }
    if ((rx.cap(1) == "DEBUG") || (rx.cap(1) == "DEBUG")) {
      QListWidgetItem * item = new QListWidgetItem(*m_debug,rx.cap(3));
      m_list->addItem(item);
    }
    if (rx.cap(1) == "WARN") {
      QListWidgetItem * item = new QListWidgetItem(*m_warning,rx.cap(3));
      m_list->addItem(item);
    }
    if (rx.cap(1) == "FATAL") {
      QListWidgetItem * item = new QListWidgetItem(*m_error,rx.cap(3));
      m_list->addItem(item);
    }
  }

  while(m_list->count() > m_maxlines) {
    QListWidgetItem * item = m_list->takeItem(0);
    if (item) {
      delete item;
    }
  }
  m_list->scrollToBottom();
}
void LogViewer::onRefresh() {
  while(!m_stream.atEnd()) {
    addLine(m_stream.readLine());
  }
}
void LogViewer::onClose() {
  this->hide();
}
void LogViewer::onPause() {
  m_timer->blockSignals(! m_timer->signalsBlocked());
  if (m_timer->signalsBlocked()) {
    m_pauseButton->setText(tr("Resume auto-update"));
  }
  else {
    m_pauseButton->setText(tr("Pause auto-update"));

  }
}
