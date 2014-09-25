#include "logviewer.h"
#include "QsLog.h"
#include "place.h"
#include "application.h"
#include "definedsettings.h"
LogViewer::LogViewer(QWidget * parent) : QDialog(parent) {
  readSettings();
  QVBoxLayout * layout = new QVBoxLayout;
  m_list = new QListWidget(this);
  layout->addWidget(m_list);

  m_pauseButton = new QPushButton(tr("Pause auto-update"),this);
  m_closeButton = new QPushButton(tr("Close"),this);

  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addStretch();
  hlayout->addWidget(m_pauseButton);
  hlayout->addWidget(m_closeButton);

  layout->addLayout(hlayout);
  m_warning = new QIcon("/home/andrewsg/qt5projects/mansur/gui/Resources/images/oxygen/16/dialog-warning.png");
  m_error = new QIcon("/home/andrewsg/qt5projects/mansur/gui/Resources/images/oxygen/16/dialog-error.png");
  m_info = new QIcon("/home/andrewsg/qt5projects/mansur/gui/Resources/images/oxygen/16/dialog-information.png");
  m_debug = new QIcon("/home/andrewsg/qt5projects/mansur/gui/Resources/images/oxygen/16/debug-run.png");
  setLayout(layout);
  setModal(false);
  QStringList lines;
  if (m_log.open(QIODevice::ReadOnly)) {
    m_stream.setDevice(&m_log);
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
   QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());
   settings->beginGroup("Logging");
   m_log.setFileName(settings->value(SID_LOGGING_FILE,"../log.txt").toString());
   m_maxlines = settings->value(SID_LOGGING_VIEWER_MAXLINES,100).toInt();
   m_refreshInterval = settings->value(SID_LOGGING_VIEWER_INTERVAL,1000).toInt();
}
LogViewer::~LogViewer() {
  QLOG_DEBUG() << Q_FUNC_INFO;
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
